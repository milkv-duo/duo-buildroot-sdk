# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import decimal

from .._const import DefaultValue
from .._error import TypeConversionError
from ._interface import AbstractValueConverter


class FloatConverter(AbstractValueConverter):

    def __init__(self, value):
        super(FloatConverter, self).__init__(value)

        self.float_class = DefaultValue.FLOAT_TYPE

    def force_convert(self):
        if isinstance(self._value, float):
            return self.float_class(str(self._value))

        try:
            return self.float_class(self._value)
        except (TypeError, ValueError, decimal.InvalidOperation):
            raise TypeConversionError(
                "failed to force_convert to float: type={}".format(
                    type(self._value)))
