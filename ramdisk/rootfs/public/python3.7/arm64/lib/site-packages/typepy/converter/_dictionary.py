# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from .._error import TypeConversionError
from ._interface import AbstractValueConverter


class DictionaryConverter(AbstractValueConverter):

    def force_convert(self):
        try:
            return dict(self._value)
        except (TypeError, ValueError):
            raise TypeConversionError(
                "failed to force_convert to dictionary: type={}".format(
                    type(self._value)))
