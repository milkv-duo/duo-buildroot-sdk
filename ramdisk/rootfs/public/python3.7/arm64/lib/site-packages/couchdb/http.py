#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2009 Christopher Lenz
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

"""Simple HTTP client implementation based on the ``httplib`` module in the
standard library.
"""

from base64 import b64encode
from datetime import datetime
import errno
import socket
import time
import sys
import ssl

try:
    from threading import Lock
except ImportError:
    from dummy_threading import Lock

try:
    from http.client import BadStatusLine, HTTPConnection, HTTPSConnection
except ImportError:
    from httplib import BadStatusLine, HTTPConnection, HTTPSConnection

try:
    from email.Utils import parsedate
except ImportError:
    from email.utils import parsedate

from couchdb import json
from couchdb import util

__all__ = ['HTTPError', 'PreconditionFailed', 'ResourceNotFound',
           'ResourceConflict', 'ServerError', 'Unauthorized', 'RedirectLimit',
           'Session', 'Resource']
__docformat__ = 'restructuredtext en'


if sys.version < '2.7':

    from httplib import CannotSendHeader, _CS_REQ_STARTED, _CS_REQ_SENT

    class NagleMixin:
        """
        Mixin to upgrade httplib connection types so headers and body can be
        sent at the same time to avoid triggering Nagle's algorithm.

        Based on code originally copied from Python 2.7's httplib module.
        """

        def endheaders(self, message_body=None):
            if self.__dict__['_HTTPConnection__state'] == _CS_REQ_STARTED:
                self.__dict__['_HTTPConnection__state'] = _CS_REQ_SENT
            else:
                raise CannotSendHeader()
            self._send_output(message_body)

        def _send_output(self, message_body=None):
            self._buffer.extend(("", ""))
            msg = "\r\n".join(self._buffer)
            del self._buffer[:]
            if isinstance(message_body, str):
                msg += message_body
                message_body = None
            self.send(msg)
            if message_body is not None:
                self.send(message_body)

    class HTTPConnection(NagleMixin, HTTPConnection):
        pass

    class HTTPSConnection(NagleMixin, HTTPSConnection):
        pass


class HTTPError(Exception):
    """Base class for errors based on HTTP status codes >= 400."""


class PreconditionFailed(HTTPError):
    """Exception raised when a 412 HTTP error is received in response to a
    request.
    """


class ResourceNotFound(HTTPError):
    """Exception raised when a 404 HTTP error is received in response to a
    request.
    """


class ResourceConflict(HTTPError):
    """Exception raised when a 409 HTTP error is received in response to a
    request.
    """


class ServerError(HTTPError):
    """Exception raised when an unexpected HTTP error is received in response
    to a request.
    """


class Unauthorized(HTTPError):
    """Exception raised when the server requires authentication credentials
    but either none are provided, or they are incorrect.
    """


class RedirectLimit(Exception):
    """Exception raised when a request is redirected more often than allowed
    by the maximum number of redirections.
    """


CHUNK_SIZE = 1024 * 8

class ResponseBody(object):

    def __init__(self, resp, conn_pool, url, conn):
        self.resp = resp
        self.chunked = self.resp.msg.get('transfer-encoding') == 'chunked'
        self.conn_pool = conn_pool
        self.url = url
        self.conn = conn

    def __del__(self):
        if not self.chunked:
            self.close()
        else:
            self.resp.close()
            if self.conn:
                # Since chunked responses can be infinite (i.e. for
                # feed=continuous), and we want to avoid leaking sockets
                # (even if just to prevent ResourceWarnings when running
                # the test suite on Python 3), we'll close this connection
                # eagerly. We can't get it into the clean state required to
                # put it back into the ConnectionPool (since we don't know
                # when it ends and we can only do blocking reads). Finding
                # out whether it might in fact end would be relatively onerous
                # and require a layering violation.
                self.conn.close()

    def read(self, size=None):
        bytes = self.resp.read(size)
        if size is None or len(bytes) < size:
            self.close()
        return bytes

    def _release_conn(self):
        self.conn_pool.release(self.url, self.conn)
        self.conn_pool, self.url, self.conn = None, None, None

    def close(self):
        while not self.resp.isclosed():
            chunk = self.resp.read(CHUNK_SIZE)
            if not chunk:
                self.resp.close()
        if self.conn:
            self._release_conn()

    def iterchunks(self):
        assert self.chunked
        buffer = []
        while True:

            if self.resp.isclosed():
                break

            chunksz = int(self.resp.fp.readline().strip(), 16)
            if not chunksz:
                self.resp.fp.read(2) #crlf
                self.resp.close()
                self._release_conn()
                break

            chunk = self.resp.fp.read(chunksz)
            for ln in chunk.splitlines(True):

                end = ln == b'\n' and not buffer # end of response
                if not ln or end:
                    break

                buffer.append(ln)
                if ln.endswith(b'\n'):
                    yield b''.join(buffer)
                    buffer = []

            self.resp.fp.read(2) #crlf


RETRYABLE_ERRORS = frozenset([
    errno.EPIPE, errno.ETIMEDOUT,
    errno.ECONNRESET, errno.ECONNREFUSED, errno.ECONNABORTED,
    errno.EHOSTDOWN, errno.EHOSTUNREACH,
    errno.ENETRESET, errno.ENETUNREACH, errno.ENETDOWN
])


class Session(object):

    def __init__(self, cache=None, timeout=None, max_redirects=5,
                 retry_delays=[0], retryable_errors=RETRYABLE_ERRORS):
        """Initialize an HTTP client session.

        :param cache: an instance with a dict-like interface or None to allow
                      Session to create a dict for caching.
        :param timeout: socket timeout in number of seconds, or `None` for no
                        timeout (the default)
        :param retry_delays: list of request retry delays.
        """
        from couchdb import __version__ as VERSION
        self.user_agent = 'CouchDB-Python/%s' % VERSION
        # XXX We accept a `cache` dict arg, but the ref gets overwritten later
        # during cache cleanup. Do we remove the cache arg (does using a shared
        # Session instance cover the same use cases?) or fix the cache cleanup?
        # For now, let's just assign the dict to the Cache instance to retain
        # current behaviour.
        if cache is not None:
            cache_by_url = cache
            cache = Cache()
            cache.by_url = cache_by_url
        else:
            cache = Cache()
        self.cache = cache
        self.max_redirects = max_redirects
        self.perm_redirects = {}

        self._disable_ssl_verification = False
        self._timeout = timeout
        self.connection_pool = ConnectionPool(
            self._timeout,
            disable_ssl_verification=self._disable_ssl_verification)

        self.retry_delays = list(retry_delays) # We don't want this changing on us.
        self.retryable_errors = set(retryable_errors)

    def disable_ssl_verification(self):
        """Disable verification of SSL certificates and re-initialize the
        ConnectionPool. Only applicable on Python 2.7.9+ as previous versions
        of Python don't verify SSL certs."""
        self._disable_ssl_verification = True
        self.connection_pool = ConnectionPool(self._timeout,
            disable_ssl_verification=self._disable_ssl_verification)

    def request(self, method, url, body=None, headers=None, credentials=None,
                num_redirects=0):
        if url in self.perm_redirects:
            url = self.perm_redirects[url]
        method = method.upper()

        if headers is None:
            headers = {}
        headers.setdefault('Accept', 'application/json')
        headers['User-Agent'] = self.user_agent

        cached_resp = None
        if method in ('GET', 'HEAD'):
            cached_resp = self.cache.get(url)
            if cached_resp is not None:
                etag = cached_resp[1].get('etag')
                if etag:
                    headers['If-None-Match'] = etag

        if (body is not None and not isinstance(body, util.strbase) and
                not hasattr(body, 'read')):
            body = json.encode(body).encode('utf-8')
            headers.setdefault('Content-Type', 'application/json')

        if body is None:
            headers.setdefault('Content-Length', '0')
        elif isinstance(body, util.strbase):
            headers.setdefault('Content-Length', str(len(body)))
        else:
            headers['Transfer-Encoding'] = 'chunked'

        authorization = basic_auth(credentials)
        if authorization:
            headers['Authorization'] = authorization

        path_query = util.urlunsplit(('', '') + util.urlsplit(url)[2:4] + ('',))
        conn = self.connection_pool.get(url)

        def _try_request_with_retries(retries):
            while True:
                try:
                    return _try_request()
                except socket.error as e:
                    ecode = e.args[0]
                    if ecode not in self.retryable_errors:
                        raise
                    try:
                        delay = next(retries)
                    except StopIteration:
                        # No more retries, raise last socket error.
                        raise e
                    finally:
                        time.sleep(delay)
                        conn.close()

        def _try_request():
            try:
                conn.putrequest(method, path_query, skip_accept_encoding=True)
                for header in headers:
                    conn.putheader(header, headers[header])
                if body is None:
                    conn.endheaders()
                else:
                    if isinstance(body, util.strbase):
                        if isinstance(body, util.utype):
                            conn.endheaders(body.encode('utf-8'))
                        else:
                            conn.endheaders(body)
                    else: # assume a file-like object and send in chunks
                        conn.endheaders()
                        while 1:
                            chunk = body.read(CHUNK_SIZE)
                            if not chunk:
                                break
                            if isinstance(chunk, util.utype):
                                chunk = chunk.encode('utf-8')
                            status = ('%x\r\n' % len(chunk)).encode('utf-8')
                            conn.send(status + chunk + b'\r\n')
                        conn.send(b'0\r\n\r\n')
                return conn.getresponse()
            except BadStatusLine as e:
                # httplib raises a BadStatusLine when it cannot read the status
                # line saying, "Presumably, the server closed the connection
                # before sending a valid response."
                # Raise as ECONNRESET to simplify retry logic.
                if e.line == '' or e.line == "''":
                    raise socket.error(errno.ECONNRESET)
                else:
                    raise

        resp = _try_request_with_retries(iter(self.retry_delays))
        status = resp.status

        # Handle conditional response
        if status == 304 and method in ('GET', 'HEAD'):
            resp.read()
            self.connection_pool.release(url, conn)
            status, msg, data = cached_resp
            if data is not None:
                data = util.StringIO(data)
            return status, msg, data
        elif cached_resp:
            self.cache.remove(url)

        # Handle redirects
        if status == 303 or \
                method in ('GET', 'HEAD') and status in (301, 302, 307):
            resp.read()
            self.connection_pool.release(url, conn)
            if num_redirects > self.max_redirects:
                raise RedirectLimit('Redirection limit exceeded')
            location = resp.getheader('location')
            
            # in case of relative location: add scheme and host to the location
            location_split = util.urlsplit(location)

            if not location_split[0]:
                orig_url_split = util.urlsplit(url)
                location = util.urlunsplit(orig_url_split[:2] + location_split[2:])

            if status == 301:
                self.perm_redirects[url] = location
            elif status == 303:
                method = 'GET'
            return self.request(method, location, body, headers,
                                num_redirects=num_redirects + 1)

        data = None
        streamed = False

        # Read the full response for empty responses so that the connection is
        # in good state for the next request
        if method == 'HEAD' or resp.getheader('content-length') == '0' or \
                status < 200 or status in (204, 304):
            resp.read()
            self.connection_pool.release(url, conn)

        # Buffer small non-JSON response bodies
        elif int(resp.getheader('content-length', sys.maxsize)) < CHUNK_SIZE:
            data = resp.read()
            self.connection_pool.release(url, conn)

        # For large or chunked response bodies, do not buffer the full body,
        # and instead return a minimal file-like object
        else:
            data = ResponseBody(resp, self.connection_pool, url, conn)
            streamed = True

        # Handle errors
        if status >= 400:
            ctype = resp.getheader('content-type')
            if data is not None and 'application/json' in ctype:
                data = json.decode(data.decode('utf-8'))
                error = data.get('error'), data.get('reason')
            elif method != 'HEAD':
                error = resp.read()
                self.connection_pool.release(url, conn)
            else:
                error = ''
            if status == 401:
                raise Unauthorized(error)
            elif status == 404:
                raise ResourceNotFound(error)
            elif status == 409:
                raise ResourceConflict(error)
            elif status == 412:
                raise PreconditionFailed(error)
            else:
                raise ServerError((status, error))

        # Store cachable responses
        if not streamed and method == 'GET' and 'etag' in resp.msg:
            self.cache.put(url, (status, resp.msg, data))

        if not streamed and data is not None:
            data = util.StringIO(data)

        return status, resp.msg, data


def cache_sort(i):
    return datetime.fromtimestamp(time.mktime(parsedate(i[1][1]['Date'])))

class Cache(object):
    """Content cache."""

    # Some random values to limit memory use
    keep_size, max_size = 10, 75

    def __init__(self):
        self.by_url = {}

    def get(self, url):
        return self.by_url.get(url)

    def put(self, url, response):
        self.by_url[url] = response
        if len(self.by_url) > self.max_size:
            self._clean()

    def remove(self, url):
        self.by_url.pop(url, None)

    def _clean(self):
        ls = sorted(self.by_url.items(), key=cache_sort)
        self.by_url = dict(ls[-self.keep_size:])


class InsecureHTTPSConnection(HTTPSConnection):
    """Wrapper class to create an HTTPSConnection without SSL verification
    (the default behavior in Python < 2.7.9). See:
    https://docs.python.org/2/library/httplib.html#httplib.HTTPSConnection"""
    if sys.version_info >= (2, 7, 9):
        def __init__(self, *a, **k):
            k['context'] = ssl._create_unverified_context()
            HTTPSConnection.__init__(self, *a, **k)


class ConnectionPool(object):
    """HTTP connection pool."""

    def __init__(self, timeout, disable_ssl_verification=False):
        self.timeout = timeout
        self.disable_ssl_verification = disable_ssl_verification
        self.conns = {} # HTTP connections keyed by (scheme, host)
        self.lock = Lock()

    def get(self, url):

        scheme, host = util.urlsplit(url, 'http', False)[:2]

        # Try to reuse an existing connection.
        self.lock.acquire()
        try:
            conns = self.conns.setdefault((scheme, host), [])
            if conns:
                conn = conns.pop(-1)
            else:
                conn = None
        finally:
            self.lock.release()

        # Create a new connection if nothing was available.
        if conn is None:
            if scheme == 'http':
                cls = HTTPConnection
            elif scheme == 'https':
                if self.disable_ssl_verification:
                    cls = InsecureHTTPSConnection
                else:
                    cls = HTTPSConnection
            else:
                raise ValueError('%s is not a supported scheme' % scheme)
            conn = cls(host, timeout=self.timeout)
            conn.connect()

        return conn

    def release(self, url, conn):
        scheme, host = util.urlsplit(url, 'http', False)[:2]
        self.lock.acquire()
        try:
            self.conns.setdefault((scheme, host), []).append(conn)
        finally:
            self.lock.release()

    def __del__(self):
        for key, conns in list(self.conns.items()):
            for conn in conns:
                conn.close()


class Resource(object):

    def __init__(self, url, session, headers=None):
        if sys.version_info[0] == 2 and isinstance(url, util.utype):
            url = url.encode('utf-8') # kind of an ugly hack for issue 235
        self.url, self.credentials = extract_credentials(url)
        if session is None:
            session = Session()
        self.session = session
        self.headers = headers or {}

    def __call__(self, *path):
        obj = type(self)(urljoin(self.url, *path), self.session)
        obj.credentials = self.credentials
        obj.headers = self.headers.copy()
        return obj

    def delete(self, path=None, headers=None, **params):
        return self._request('DELETE', path, headers=headers, **params)

    def get(self, path=None, headers=None, **params):
        return self._request('GET', path, headers=headers, **params)

    def head(self, path=None, headers=None, **params):
        return self._request('HEAD', path, headers=headers, **params)

    def post(self, path=None, body=None, headers=None, **params):
        return self._request('POST', path, body=body, headers=headers,
                             **params)

    def put(self, path=None, body=None, headers=None, **params):
        return self._request('PUT', path, body=body, headers=headers, **params)

    def delete_json(self, path=None, headers=None, **params):
        return self._request_json('DELETE', path, headers=headers, **params)

    def get_json(self, path=None, headers=None, **params):
        return self._request_json('GET', path, headers=headers, **params)

    def post_json(self, path=None, body=None, headers=None, **params):
        return self._request_json('POST', path, body=body, headers=headers,
                                  **params)

    def put_json(self, path=None, body=None, headers=None, **params):
        return self._request_json('PUT', path, body=body, headers=headers,
                                  **params)

    def _request(self, method, path=None, body=None, headers=None, **params):
        all_headers = self.headers.copy()
        all_headers.update(headers or {})
        if path is not None:
            url = urljoin(self.url, path, **params)
        else:
            url = urljoin(self.url, **params)
        return self.session.request(method, url, body=body,
                                    headers=all_headers,
                                    credentials=self.credentials)

    def _request_json(self, method, path=None, body=None, headers=None, **params):
        status, headers, data = self._request(method, path, body=body,
                                              headers=headers, **params)
        if 'application/json' in headers.get('content-type', ''):
            data = json.decode(data.read().decode('utf-8'))
        return status, headers, data



def extract_credentials(url):
    """Extract authentication (user name and password) credentials from the
    given URL.
    
    >>> extract_credentials('http://localhost:5984/_config/')
    ('http://localhost:5984/_config/', None)
    >>> extract_credentials('http://joe:secret@localhost:5984/_config/')
    ('http://localhost:5984/_config/', ('joe', 'secret'))
    >>> extract_credentials('http://joe%40example.com:secret@localhost:5984/_config/')
    ('http://localhost:5984/_config/', ('joe@example.com', 'secret'))
    """
    parts = util.urlsplit(url)
    netloc = parts[1]
    if '@' in netloc:
        creds, netloc = netloc.split('@')
        credentials = tuple(util.urlunquote(i) for i in creds.split(':'))
        parts = list(parts)
        parts[1] = netloc
    else:
        credentials = None
    return util.urlunsplit(parts), credentials


def basic_auth(credentials):
    """Generates authorization header value for given credentials.
    >>> basic_auth(('root', 'relax'))
    b'Basic cm9vdDpyZWxheA=='
    >>> basic_auth(None)
    >>> basic_auth(())
    """
    if credentials:
        token = b64encode(('%s:%s' % credentials).encode('latin1'))
        return ('Basic %s' % token.strip().decode('latin1')).encode('ascii')


def quote(string, safe=''):
    if isinstance(string, util.utype):
        string = string.encode('utf-8')
    return util.urlquote(string, safe)


def urlencode(data):
    if isinstance(data, dict):
        data = data.items()
    params = []
    for name, value in data:
        if isinstance(value, util.utype):
            value = value.encode('utf-8')
        params.append((name, value))
    return util.urlencode(params)


def urljoin(base, *path, **query):
    """Assemble a uri based on a base, any number of path segments, and query
    string parameters.

    >>> urljoin('http://example.org', '_all_dbs')
    'http://example.org/_all_dbs'

    A trailing slash on the uri base is handled gracefully:

    >>> urljoin('http://example.org/', '_all_dbs')
    'http://example.org/_all_dbs'

    And multiple positional arguments become path parts:

    >>> urljoin('http://example.org/', 'foo', 'bar')
    'http://example.org/foo/bar'

    All slashes within a path part are escaped:

    >>> urljoin('http://example.org/', 'foo/bar')
    'http://example.org/foo%2Fbar'
    >>> urljoin('http://example.org/', 'foo', '/bar/')
    'http://example.org/foo/%2Fbar%2F'

    >>> urljoin('http://example.org/', None) #doctest:+IGNORE_EXCEPTION_DETAIL
    Traceback (most recent call last):
        ...
    TypeError: argument 2 to map() must support iteration
    """
    if base and base.endswith('/'):
        base = base[:-1]
    retval = [base]

    # build the path
    path = '/'.join([''] + [quote(s) for s in path])
    if path:
        retval.append(path)

    # build the query string
    params = []
    for name, value in query.items():
        if type(value) in (list, tuple):
            params.extend([(name, i) for i in value if i is not None])
        elif value is not None:
            if value is True:
                value = 'true'
            elif value is False:
                value = 'false'
            params.append((name, value))
    if params:
        retval.extend(['?', urlencode(params)])

    return ''.join(retval)

