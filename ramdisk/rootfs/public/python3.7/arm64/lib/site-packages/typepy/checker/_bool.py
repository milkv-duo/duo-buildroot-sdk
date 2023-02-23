# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from .._const import StrictLevel
from ..type._integer import Integer
from ._checker import (
    TypeChecker,
    TypeCheckerStrictLevel,
)
from ._common import isstring


class BoolTypeCheckerStrictLevel0(TypeCheckerStrictLevel):

    def is_instance(self):
        return isinstance(self._value, bool)

    def is_valid_after_convert(self, converted_value):
        return isinstance(converted_value, bool)


class BoolTypeCheckerStrictLevel1(BoolTypeCheckerStrictLevel0):

    def is_exclude_instance(self):
        return Integer(self._value, strict_level=StrictLevel.MAX).is_type()


class BoolTypeCheckerStrictLevel2(BoolTypeCheckerStrictLevel1):

    def is_exclude_instance(self):
        return (
            super(BoolTypeCheckerStrictLevel2, self).is_exclude_instance() or
            isstring(self._value)
        )


class BoolTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(BoolTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: BoolTypeCheckerStrictLevel0,
                1: BoolTypeCheckerStrictLevel1,
                2: BoolTypeCheckerStrictLevel2,
            },
            strict_level=strict_level)
