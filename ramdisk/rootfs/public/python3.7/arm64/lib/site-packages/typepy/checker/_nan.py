# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from ._checker import (
    TypeChecker,
    TypeCheckerStrictLevel,
)
from ._common import (
    isstring,
    isnan,
)


class NanTypeCheckerStrictLevel0(TypeCheckerStrictLevel):

    def is_instance(self):
        return isnan(self._value)

    def is_valid_after_convert(self, converted_value):
        return isnan(converted_value)


class NanTypeCheckerStrictLevel1(NanTypeCheckerStrictLevel0):

    def is_exclude_instance(self):
        return isstring(self._value)


class NanTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(NanTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: NanTypeCheckerStrictLevel0,
                1: NanTypeCheckerStrictLevel1,
            },
            strict_level=strict_level)
