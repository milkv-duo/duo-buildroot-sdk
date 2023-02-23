# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

import datetime

from ..type._integer import Integer
from ._checker import (
    TypeChecker,
    TypeCheckerStrictLevel,
)
from ._common import isstring


class DateTimeTypeCheckerStrictLevel0(TypeCheckerStrictLevel):

    def is_instance(self):
        return isinstance(self._value, datetime.datetime)


class DateTimeTypeCheckerStrictLevel1(DateTimeTypeCheckerStrictLevel0):

    def is_exclude_instance(self):
        # exclude timestamp
        return Integer(self._value, strict_level=1).is_type()


class DateTimeTypeCheckerStrictLevel2(DateTimeTypeCheckerStrictLevel1):

    def is_exclude_instance(self):
        return (
            isstring(self._value) or
            super(DateTimeTypeCheckerStrictLevel2, self).is_exclude_instance()
        )


class DateTimeTypeChecker(TypeChecker):

    def __init__(self, value, strict_level):
        super(DateTimeTypeChecker, self).__init__(
            value=value,
            checker_mapping={
                0: DateTimeTypeCheckerStrictLevel0,
                1: DateTimeTypeCheckerStrictLevel1,
                2: DateTimeTypeCheckerStrictLevel2,
            },
            strict_level=strict_level)
