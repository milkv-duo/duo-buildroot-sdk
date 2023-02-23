# -*- coding: utf-8 -*-
#
# Copyright (C) 2008-2009 Christopher Lenz
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

"""Support for streamed reading and writing of multipart MIME content."""

from base64 import b64encode
from cgi import parse_header
from email import header

try:
    from hashlib import md5
except ImportError:
    from md5 import new as md5

import uuid

from couchdb import util

__all__ = ['read_multipart', 'write_multipart']
__docformat__ = 'restructuredtext en'


CRLF = b'\r\n'


def read_multipart(fileobj, boundary=None):
    """Simple streaming MIME multipart parser.
    
    This function takes a file-like object reading a MIME envelope, and yields
    a ``(headers, is_multipart, payload)`` tuple for every part found, where
    ``headers`` is a dictionary containing the MIME headers of that part (with
    names lower-cased), ``is_multipart`` is a boolean indicating whether the
    part is itself multipart, and ``payload`` is either a string (if
    ``is_multipart`` is false), or an iterator over the nested parts.
    
    Note that the iterator produced for nested multipart payloads MUST be fully
    consumed, even if you wish to skip over the content.
    
    :param fileobj: a file-like object
    :param boundary: the part boundary string, will generally be determined
                     automatically from the headers of the outermost multipart
                     envelope
    :return: an iterator over the parts
    :since: 0.5
    """
    headers = {}
    buf = []
    outer = in_headers = boundary is None

    next_boundary = boundary and ('--' + boundary + '\n').encode('ascii') or None
    last_boundary = boundary and ('--' + boundary + '--\n').encode('ascii') or None

    def _current_part():
        payload = b''.join(buf)
        if payload.endswith(b'\r\n'):
            payload = payload[:-2]
        elif payload.endswith(b'\n'):
            payload = payload[:-1]
        content_md5 = headers.get(b'content-md5')
        if content_md5:
            h = b64encode(md5(payload).digest())
            if content_md5 != h:
                raise ValueError('data integrity check failed')
        return headers, False, payload

    for line in fileobj:
        if in_headers:
            line = line.replace(CRLF, b'\n')
            if line != b'\n':
                name, value = [item.strip() for item in line.split(b':', 1)]
                name = name.lower().decode('ascii')
                value, charset = header.decode_header(value.decode('utf-8'))[0]
                if charset is None:
                    headers[name] = value
                else:
                    headers[name] = value.decode(charset)
            else:
                in_headers = False
                mimetype, params = parse_header(headers.get('content-type'))
                if mimetype.startswith('multipart/'):
                    sub_boundary = params['boundary']
                    sub_parts = read_multipart(fileobj, boundary=sub_boundary)
                    if boundary is not None:
                        yield headers, True, sub_parts
                        headers.clear()
                        del buf[:]
                    else:
                        for part in sub_parts:
                            yield part
                        return

        elif line.replace(CRLF, b'\n') == next_boundary:
            # We've reached the start of a new part, as indicated by the
            # boundary
            if headers:
                if not outer:
                    yield _current_part()
                else:
                    outer = False
                headers.clear()
                del buf[:]
            in_headers = True

        elif line.replace(CRLF, b'\n') == last_boundary:
            # We're done with this multipart envelope
            break

        else:
            buf.append(line)

    if not outer and headers:
        yield _current_part()


class MultipartWriter(object):

    def __init__(self, fileobj, headers=None, subtype='mixed', boundary=None):
        self.fileobj = fileobj
        if boundary is None:
            boundary = '==' + uuid.uuid4().hex + '=='
        self.boundary = boundary
        if headers is None:
            headers = {}
        headers['Content-Type'] = 'multipart/%s; boundary="%s"' % (
            subtype, self.boundary
        )
        self._write_headers(headers)

    def open(self, headers=None, subtype='mixed', boundary=None):
        self.fileobj.write(b'--')
        self.fileobj.write(self.boundary.encode('utf-8'))
        self.fileobj.write(CRLF)
        return MultipartWriter(self.fileobj, headers=headers, subtype=subtype,
                               boundary=boundary)

    def add(self, mimetype, content, headers=None):
        self.fileobj.write(b'--')
        self.fileobj.write(self.boundary.encode('utf-8'))
        self.fileobj.write(CRLF)
        if headers is None:
            headers = {}

        ctype, params = parse_header(mimetype)
        if isinstance(content, util.utype):
            if 'charset' in params:
                content = content.encode(params['charset'])
            else:
                content = content.encode('utf-8')
                mimetype = mimetype + ';charset=utf-8'

        headers['Content-Type'] = mimetype
        if content:
            headers['Content-Length'] = str(len(content))
            hash = b64encode(md5(content).digest()).decode('ascii')
            headers['Content-MD5'] = hash
        self._write_headers(headers)
        if content:
            # XXX: throw an exception if a boundary appears in the content??
            self.fileobj.write(content)
            self.fileobj.write(CRLF)

    def close(self):
        self.fileobj.write(b'--')
        self.fileobj.write(self.boundary.encode('ascii'))
        self.fileobj.write(b'--')
        self.fileobj.write(CRLF)

    def _write_headers(self, headers):
        if headers:
            for name in sorted(headers.keys()):
                value = headers[name]
                if value.encode('ascii', 'ignore') != value.encode('utf-8'):
                    value = header.make_header([(value, 'utf-8')]).encode()
                self.fileobj.write(name.encode('utf-8'))
                self.fileobj.write(b': ')
                self.fileobj.write(value.encode('utf-8'))
                self.fileobj.write(CRLF)
        self.fileobj.write(CRLF)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()


def write_multipart(fileobj, subtype='mixed', boundary=None):
    r"""Simple streaming MIME multipart writer.

    This function returns a `MultipartWriter` object that has a few methods to
    control the nested MIME parts. For example, to write a flat multipart
    envelope you call the ``add(mimetype, content, [headers])`` method for
    every part, and finally call the ``close()`` method.

    >>> from couchdb.util import StringIO

    >>> buf = StringIO()
    >>> envelope = write_multipart(buf, boundary='==123456789==')
    >>> envelope.add('text/plain', b'Just testing')
    >>> envelope.close()
    >>> print(buf.getvalue().replace(b'\r\n', b'\n').decode('utf-8'))
    Content-Type: multipart/mixed; boundary="==123456789=="
    <BLANKLINE>
    --==123456789==
    Content-Length: 12
    Content-MD5: nHmX4a6el41B06x2uCpglQ==
    Content-Type: text/plain
    <BLANKLINE>
    Just testing
    --==123456789==--
    <BLANKLINE>

    Note that an explicit boundary is only specified for testing purposes. If
    the `boundary` parameter is omitted, the multipart writer will generate a
    random string for the boundary.

    To write nested structures, call the ``open([headers])`` method on the
    respective envelope, and finish each envelope using the ``close()`` method:

    >>> buf = StringIO()
    >>> envelope = write_multipart(buf, boundary='==123456789==')
    >>> part = envelope.open(boundary='==abcdefghi==')
    >>> part.add('text/plain', u'Just testing')
    >>> part.close()
    >>> envelope.close()
    >>> print(buf.getvalue().replace(b'\r\n', b'\n').decode('utf-8')) #:doctest +ELLIPSIS
    Content-Type: multipart/mixed; boundary="==123456789=="
    <BLANKLINE>
    --==123456789==
    Content-Type: multipart/mixed; boundary="==abcdefghi=="
    <BLANKLINE>
    --==abcdefghi==
    Content-Length: 12
    Content-MD5: nHmX4a6el41B06x2uCpglQ==
    Content-Type: text/plain;charset=utf-8
    <BLANKLINE>
    Just testing
    --==abcdefghi==--
    --==123456789==--
    <BLANKLINE>
    
    :param fileobj: a writable file-like object that the output should get
                    written to
    :param subtype: the subtype of the multipart MIME type (e.g. "mixed")
    :param boundary: the boundary to use to separate the different parts
    :since: 0.6
    """
    return MultipartWriter(fileobj, subtype=subtype, boundary=boundary)
