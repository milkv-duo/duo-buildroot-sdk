# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from ._checker import (
    TypeChecker,
    TypeCheckerStrictLevel,
)


class NoneTypeCheckerStrictLevel0(TypeCheckerStrictLevel):

    def is_instance(self):
        return self._value is None

    def is_valid_after_convert(self, converted_value):
        return self._value is None


class NoneTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(NoneTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: NoneTypeCheckerStrictLevel0,
            },
            strict_level=strict_level)
