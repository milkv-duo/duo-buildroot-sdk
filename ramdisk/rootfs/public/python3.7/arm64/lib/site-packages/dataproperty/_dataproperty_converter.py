# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import re

from typepy import (
    TypeConversionError,
    Typecode,
)

from ._common import (
    STRICT_TYPE_MAPPING,
    DefaultValue,
)
from ._dataproperty import DataProperty


class DataPropertyConverter(object):
    __RE_QUOTE = re.compile("^[\s]*[\"].*[\"][\s]*$")

    def __init__(
            self, type_value_mapping=None, const_value_mapping=None,
            quoting_flags=None,
            datetime_formatter=None,
            datetime_format_str=None,
            float_type=None, strict_type_mapping=None):
        self.__type_value_mapping = (
            type_value_mapping
            if type_value_mapping else DefaultValue.TYPE_VALUE_MAPPING)
        self.__const_value_mapping = (
            const_value_mapping
            if const_value_mapping else DefaultValue.CONST_VALUE_MAPPING)
        self.__quoting_flags = (
            quoting_flags if quoting_flags else DefaultValue.QUOTING_FLAGS)

        self.__datetime_formatter = datetime_formatter
        self.__datetime_format_str = datetime_format_str
        self.__float_type = float_type
        self.__strict_type_mapping = strict_type_mapping

    def convert(self, dp_value):
        try:
            return self.__create_dataproperty(self.__convert_value(dp_value))
        except TypeConversionError:
            if not self.__quoting_flags.get(dp_value.typecode):
                return dp_value

            return self.__create_dataproperty(
                self.__apply_quote(dp_value.typecode, dp_value.to_str()))

    def __create_dataproperty(self, value):
        return DataProperty(
            value,
            float_type=self.__float_type,
            datetime_format_str=self.__datetime_format_str,
            strict_type_mapping=STRICT_TYPE_MAPPING)

    def __apply_quote(self, typecode, data):
        if not self.__quoting_flags.get(typecode):
            return data

        try:
            if self.__RE_QUOTE.search(data):
                return data
        except TypeError:
            return data

        return '"{}"'.format(data)

    def __convert_value(self, dp_value):
        if dp_value.typecode in (Typecode.BOOL, Typecode.STRING):
            try:
                if dp_value.data in self.__const_value_mapping:
                    return self.__apply_quote(
                        dp_value.typecode,
                        self.__const_value_mapping.get(dp_value.data))
            except TypeError:
                # unhashable type will be reached this line
                raise TypeConversionError

        if dp_value.typecode in self.__type_value_mapping:
            return self.__apply_quote(
                dp_value.typecode,
                self.__type_value_mapping.get(dp_value.typecode))

        if dp_value.typecode == Typecode.DATETIME:
            try:
                return self.__apply_quote(
                    dp_value.typecode,
                    self.__datetime_formatter(dp_value.data))
            except TypeError:
                raise TypeConversionError

        raise TypeConversionError("no need to convert")
