# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from ._error import NullNameError


def _validate_null_string(text, error_msg="null name"):
    if is_not_null_string(text):
        return

    if is_null_string(text):
        raise NullNameError(error_msg)

    raise TypeError("text must be a string")


def _preprocess(name):
    return name.strip()


def is_null_string(value):
    if value is None:
        return True

    try:
        return len(value.strip()) == 0
    except AttributeError:
        return False


def is_not_null_string(value):
    try:
        return len(value.strip()) > 0
    except AttributeError:
        return False
