# -*- coding: utf-8 -*-
#
# Copyright (C) 2009 Christopher Lenz
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

"""Thin abstraction layer over the different available modules for decoding
and encoding JSON data.

This module currently supports the following JSON modules:
 - ``simplejson``: https://github.com/simplejson/simplejson
 - ``cjson``: http://pypi.python.org/pypi/python-cjson
 - ``json``: This is the version of ``simplejson`` that is bundled with the
   Python standard library since version 2.6
   (see http://docs.python.org/library/json.html)

The default behavior is to use ``simplejson`` if installed, and otherwise
fallback to the standard library module. To explicitly tell CouchDB-Python
which module to use, invoke the `use()` function with the module name::

    from couchdb import json
    json.use('cjson')

In addition to choosing one of the above modules, you can also configure
CouchDB-Python to use custom decoding and encoding functions::

    from couchdb import json
    json.use(decode=my_decode, encode=my_encode)

"""

__all__ = ['decode', 'encode', 'use']

from couchdb import util
import warnings
import os

_initialized = False
_using = os.environ.get('COUCHDB_PYTHON_JSON')
_decode = None
_encode = None


def decode(string):
    """Decode the given JSON string.
    
    :param string: the JSON string to decode
    :type string: basestring
    :return: the corresponding Python data structure
    :rtype: object
    """
    if not _initialized:
        _initialize()
    return _decode(string)


def encode(obj):
    """Encode the given object as a JSON string.
    
    :param obj: the Python data structure to encode
    :type obj: object
    :return: the corresponding JSON string
    :rtype: basestring
    """
    if not _initialized:
        _initialize()
    return _encode(obj)


def use(module=None, decode=None, encode=None):
    """Set the JSON library that should be used, either by specifying a known
    module name, or by providing a decode and encode function.
    
    The modules "simplejson" and "json" are currently supported for the
    ``module`` parameter.
    
    If provided, the ``decode`` parameter must be a callable that accepts a
    JSON string and returns a corresponding Python data structure. The
    ``encode`` callable must accept a Python data structure and return the
    corresponding JSON string. Exceptions raised by decoding and encoding
    should be propagated up unaltered.
    
    :param module: the name of the JSON library module to use, or the module
                   object itself
    :type module: str or module
    :param decode: a function for decoding JSON strings
    :type decode: callable
    :param encode: a function for encoding objects as JSON strings
    :type encode: callable
    """
    global _decode, _encode, _initialized, _using
    if module is not None:
        if not isinstance(module, util.strbase):
            module = module.__name__
        if module not in ('cjson', 'json', 'simplejson'):
            raise ValueError('Unsupported JSON module %s' % module)
        _using = module
        _initialized = False
    else:
        assert decode is not None and encode is not None
        _using = 'custom'
        _decode = decode
        _encode = encode
        _initialized = True


def _initialize():
    global _initialized

    def _init_simplejson():
        global _decode, _encode
        import simplejson
        _decode = lambda string, loads=simplejson.loads: loads(string)
        _encode = lambda obj, dumps=simplejson.dumps: \
            dumps(obj, allow_nan=False, ensure_ascii=False)

    def _init_cjson():
        global _decode, _encode
        import cjson
        _decode = lambda string, decode=cjson.decode: decode(string)
        _encode = lambda obj, encode=cjson.encode: encode(obj)

    def _init_stdlib():
        global _decode, _encode
        json = __import__('json', {}, {})

        def _decode(string_, loads=json.loads):
            if isinstance(string_, util.btype):
                string_ = string_.decode("utf-8")
            return loads(string_)

        _encode = lambda obj, dumps=json.dumps: \
            dumps(obj, allow_nan=False, ensure_ascii=False)

    if _using == 'simplejson':
        _init_simplejson()
    elif _using == 'cjson':
        warnings.warn("Builtin cjson support is deprecated. Please use the "
                      "default or provide custom decode/encode functions "
                      "[2011-11-09].",
                      DeprecationWarning, stacklevel=1)
        _init_cjson()
    elif _using == 'json':
        _init_stdlib()
    elif _using != 'custom':
        try:
            _init_simplejson()
        except ImportError:
            _init_stdlib()
    _initialized = True
