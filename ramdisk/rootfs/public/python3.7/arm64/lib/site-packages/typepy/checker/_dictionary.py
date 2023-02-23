# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from ._checker import (
    TypeChecker,
    TypeCheckerStrictLevel,
)


class DictionaryTypeCheckerStrictLevel0(TypeCheckerStrictLevel):

    def is_instance(self):
        return isinstance(self._value, dict)

    def is_valid_after_convert(self, converted_value):
        return isinstance(converted_value, dict) and converted_value


class DictionaryTypeCheckerStrictLevel1(DictionaryTypeCheckerStrictLevel0):

    def is_exclude_instance(self):
        return not isinstance(self._value, dict)


class DictionaryTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(DictionaryTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: DictionaryTypeCheckerStrictLevel0,
                1: DictionaryTypeCheckerStrictLevel1,
            },
            strict_level=strict_level)
