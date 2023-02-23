# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import re

from ._common import (
    _validate_null_string,
    _preprocess,
)
from ._error import (
    InvalidCharError,
    ValidReservedNameError,
    InvalidReservedNameError
)


__SQLITE_VALID_RESERVED_KEYWORDS = [
    'ABORT', 'ACTION', 'AFTER', 'ANALYZE', 'ASC', 'ATTACH',
    'BEFORE', 'BEGIN', 'BY',
    'CASCADE', 'CAST', 'COLUMN', 'CONFLICT', 'CROSS', 'CURRENT_DATE',
    'CURRENT_TIME', 'CURRENT_TIMESTAMP',
    'DATABASE', 'DEFERRED', 'DESC', 'DETACH',
    'EACH', 'END', 'EXCLUSIVE', 'EXPLAIN',
    'FAIL', 'FOR', 'FULL', 'GLOB',
    'IGNORE', 'IMMEDIATE', 'INDEXED', 'INITIALLY', 'INNER', 'INSTEAD',
    'KEY',
    'LEFT', 'LIKE',
    'MATCH',
    'NATURAL',
    'NO',
    'OF', 'OFFSET', 'OUTER',
    'PLAN', 'PRAGMA',
    'QUERY',
    'RAISE', 'RECURSIVE', 'REGEXP', 'REINDEX', 'RELEASE', 'RENAME', 'REPLACE',
    'RESTRICT', 'RIGHT', 'ROLLBACK', 'ROW',
    'SAVEPOINT',
    'TEMP', 'TEMPORARY', 'TRIGGER',
    'VACUUM', 'VIEW', 'VIRTUAL',
    'WITH', 'WITHOUT',
]
__SQLITE_INVALID_RESERVED_KEYWORDS = [
    'ADD', 'ALL', 'ALTER', 'AND', 'AS', 'AUTOINCREMENT',
    'BETWEEN',
    'CASE', 'CHECK', 'COLLATE', 'COMMIT', 'CONSTRAINT', 'CREATE',
    'DEFAULT', 'DEFERRABLE', 'DELETE', 'DISTINCT', 'DROP',
    'ELSE', 'ESCAPE', 'EXCEPT', 'EXISTS', 'FOREIGN',
    'FROM',
    'GROUP',
    'HAVING',
    'IN', 'INDEX', 'INSERT', 'INTERSECT', 'INTO', 'IS', 'ISNULL',
    'JOIN',
    'LIMIT',
    'NOT', 'NOTNULL', 'NULL',
    'ON', 'OR', 'ORDER',
    'PRIMARY',
    'REFERENCES',
    'SELECT', 'SET',
    'TABLE', 'THEN', 'TO', 'TRANSACTION',
    'UNION', 'UNIQUE', 'UPDATE', 'USING',
    'VALUES',
    'WHEN', 'WHERE',
]

__SQLITE_VALID_RESERVED_KEYWORDS_TABLE = (
    __SQLITE_VALID_RESERVED_KEYWORDS)
__SQLITE_INVALID_RESERVED_KEYWORDS_TABLE = (
    __SQLITE_INVALID_RESERVED_KEYWORDS + ['IF'])

__SQLITE_VALID_RESERVED_KEYWORDS_ATTR = (
    __SQLITE_VALID_RESERVED_KEYWORDS + ['IF'])
__SQLITE_INVALID_RESERVED_KEYWORDS_ATTR = (
    __SQLITE_INVALID_RESERVED_KEYWORDS)

__RE_INVALID_SQLITE_NAME_HEAD = re.compile("^[^a-zA-Z]+", re.UNICODE)


def validate_sqlite_table_name(name):
    """
    :param str name: Name to validate.
    :raises pathvalidate.NullNameError: If the ``name`` is empty.
    :raises pathvalidate.InvalidCharError:
        If the first character of the ``name`` is invalid
        (not an alphabetic character).
    :raises pathvalidate.InvalidReservedNameError:
        |raises_sqlite_keywords|
        And invalid as a table name.
    :raises pathvalidate.ValidReservedNameError:
        |raises_sqlite_keywords|
        However, valid as a table name.
    """

    _validate_null_string(name)

    if name.upper() in __SQLITE_INVALID_RESERVED_KEYWORDS_TABLE:
        raise InvalidReservedNameError(
            "'{:s}' is a reserved keyword by sqlite".format(name))

    if name.upper() in __SQLITE_VALID_RESERVED_KEYWORDS_TABLE:
        raise ValidReservedNameError(
            "'{:s}' is a reserved keyword by sqlite".format(name))

    match = __RE_INVALID_SQLITE_NAME_HEAD.search(_preprocess(name))
    if match is not None:
        name = match.group()

        try:
            name.encode("ascii")
        except UnicodeEncodeError:
            try:
                name.encode("utf8")
            except Exception:
                raise
            else:
                return

        raise InvalidCharError(
            "the first character of the sqlite name is invalid: '{}'".format(
                re.escape(name)))


def validate_sqlite_attr_name(name):
    """
    :param str name: Name to validate.
    :raises pathvalidate.NullNameError: If the ``name`` is empty.
    :raises pathvalidate.InvalidCharError:
        If the first character of the ``name`` is invalid
        (not an alphabetic character).
    :raises pathvalidate.InvalidReservedNameError:
        |raises_sqlite_keywords|
        And invalid as a attribute name.
    :raises pathvalidate.ValidReservedNameError:
        |raises_sqlite_keywords|
        However, valid as a attribute name.
    """

    _validate_null_string(name)

    if name.upper() in __SQLITE_INVALID_RESERVED_KEYWORDS_ATTR:
        raise InvalidReservedNameError(
            "'{}' is a reserved keyword by sqlite".format(name))

    if name.upper() in __SQLITE_VALID_RESERVED_KEYWORDS_ATTR:
        raise ValidReservedNameError(
            "'{}' is a reserved keyword by sqlite".format(name))

    match = __RE_INVALID_SQLITE_NAME_HEAD.search(_preprocess(name))
    if match is not None:
        name = match.group()

        try:
            name.encode("ascii")
        except UnicodeEncodeError:
            try:
                name.encode("utf8")
            except Exception:
                raise
            else:
                return

        raise InvalidCharError(
            "the first character of the sqlite name is invalid: '{}'".format(
                re.escape(match.group())))
