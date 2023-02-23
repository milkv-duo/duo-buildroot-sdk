# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from decimal import (
    Decimal,
    InvalidOperation,
)

from .._error import TypeConversionError
from ._interface import AbstractValueConverter


class IntegerConverter(AbstractValueConverter):

    def force_convert(self):
        try:
            return int(Decimal(self._value))
        except (TypeError, OverflowError, ValueError, InvalidOperation):
            raise TypeConversionError(
                "failed to force_convert to int: type={}".format(
                    type(self._value)))
