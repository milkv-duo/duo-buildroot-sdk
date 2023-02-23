
__all__ = [
    'StringIO', 'urlsplit', 'urlunsplit', 'urlquote', 'urlunquote',
    'urlencode', 'utype', 'btype', 'ltype', 'strbase', 'funcode', 'urlparse',
]

utype = unicode
btype = str
ltype = long
strbase = str, bytes, unicode

from io import BytesIO as StringIO
from urlparse import urlparse, urlsplit, urlunsplit
from urllib import quote as urlquote
from urllib import unquote as urlunquote
from urllib import urlencode


def funcode(fun):
    return fun.func_code
