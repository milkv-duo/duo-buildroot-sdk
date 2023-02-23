# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import warnings

from ._const import StrictLevel


def is_hex(value):
    try:
        int(value, 16)
    except (TypeError, ValueError):
        return False

    return True


def _is_list(value):
    return isinstance(value, list)


def _is_tuple(value):
    return isinstance(value, tuple)


def is_list_or_tuple(value):
    """
    .. warning::

        This function will be deleted in the future.
    """

    return any([_is_list(value), _is_tuple(value)])


def is_null_string(value):
    from .type import NullString

    return NullString(value, strict_level=StrictLevel.MIN).is_type()


def is_not_null_string(value):
    from .type import String

    return String(value, strict_level=StrictLevel.MAX).is_type()


def is_empty_string(value):
    warnings.warn(
        "is_empty_string() will be deleted in the future, "
        "use is_null_string instead.",
        DeprecationWarning)

    return is_null_string(value)


def is_not_empty_string(value):
    warnings.warn(
        "is_not_empty_string() will be deleted in the future, "
        "use is_not_null_string instead.",
        DeprecationWarning)

    return is_not_null_string(value)


def is_empty_sequence(value):
    try:
        return value is None or len(value) == 0
    except TypeError:
        return False


def is_not_empty_sequence(value):
    try:
        return len(value) > 0
    except TypeError:
        return False
