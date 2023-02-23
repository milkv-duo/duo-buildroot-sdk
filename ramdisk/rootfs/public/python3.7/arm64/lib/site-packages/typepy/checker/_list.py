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


class ListTypeCheckerStrictLevel0(TypeCheckerStrictLevel):

    def is_instance(self):
        return isinstance(self._value, list)

    def is_valid_after_convert(self, converted_value):
        return isinstance(converted_value, list) and converted_value

    def is_exclude_instance(self):
        return isstring(self._value)


class ListTypeCheckerStrictLevel1(ListTypeCheckerStrictLevel0):

    def is_exclude_instance(self):
        return (
            super(ListTypeCheckerStrictLevel1, self).is_exclude_instance() or
            not isinstance(self._value, list)
        )


class ListTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(ListTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: ListTypeCheckerStrictLevel0,
                1: ListTypeCheckerStrictLevel1,
            },
            strict_level=strict_level)
