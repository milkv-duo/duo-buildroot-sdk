# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from ._checker import (
    TypeChecker,
    TypeCheckerStrictLevel,
)
from ._common import isstring


class StringTypeCheckerStrictLevel0(TypeCheckerStrictLevel):

    def is_instance(self):
        return isstring(self._value)

    def is_valid_after_convert(self, converted_value):
        return isstring(converted_value)

    def _is_null_string(self, value):
        try:
            value = value.strip()
        except AttributeError:
            return False

        return len(value) == 0


class StringTypeCheckerStrictLevel1(StringTypeCheckerStrictLevel0):

    def is_exclude_instance(self):
        return (
            not isstring(self._value) or
            self._is_null_string(self._value)
        )


class StringTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(StringTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: StringTypeCheckerStrictLevel0,
                1: StringTypeCheckerStrictLevel1,
            },
            strict_level=strict_level)


class NullStringTypeCheckerStrictLevel0(StringTypeCheckerStrictLevel0):

    def is_instance(self):
        return self._value is None

    def is_valid_after_convert(self, converted_value):
        return self._is_null_string(converted_value)


class NullStringTypeCheckerStrictLevel1(NullStringTypeCheckerStrictLevel0):

    def is_instance(self):
        return False


class NullStringTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(NullStringTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: NullStringTypeCheckerStrictLevel0,
                1: NullStringTypeCheckerStrictLevel1,
            },
            strict_level=strict_level)
