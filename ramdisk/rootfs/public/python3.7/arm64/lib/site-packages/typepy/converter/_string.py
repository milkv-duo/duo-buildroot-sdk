# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from mbstrdecoder import MultiByteStrDecoder
import six

from ._interface import AbstractValueConverter


class StringConverter(AbstractValueConverter):

    def force_convert(self):
        try:
            return MultiByteStrDecoder(self._value).unicode_str
        except ValueError:
            return six.text_type(self._value)


class NullStringConverter(StringConverter):

    def force_convert(self):
        return super(NullStringConverter, self).force_convert().strip()
