# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import copy
import multiprocessing

import enum
from typepy import is_empty_sequence
from typepy.type import String

from six.moves import zip

from ._common import (
    NOT_STRICT_TYPE_MAPPING,
    DefaultValue,
)
from ._dataproperty import (
    DataProperty,
    ColumnDataProperty,
)
from ._dataproperty_converter import DataPropertyConverter
from ._logger import logger


@enum.unique
class MatrixFormatting(enum.Enum):
    # raise exception if the matrix is not properly formatted
    EXCEPTION = 1 << 1

    # trim to the minimum size column
    TRIM = 1 << 2

    # Append None values to columns so that it is the same as the maximum
    # column size.
    FILL_NONE = 1 << 3

    HEADER_ALIGNED = 1 << 4


class DataPropertyExtractor(object):
    """
    .. py:attribute:: quoting_flags

        Configurations to add double quote to for each items in a matrix,
        where |Typecode| of table-value is |True| in the ``quote_flag_table``
        mapping table. ``quote_flag_table`` should be a dictionary.
        And is ``{ Typecode : bool }``. Defaults to:

        .. code-block:: json
            :caption: The default values

            {
                Typecode.BOOL: False,
                Typecode.DATETIME: False,
                Typecode.DICTIONARY: False,
                Typecode.INFINITY: False,
                Typecode.INTEGER: False,
                Typecode.IP_ADDRESS: False,
                Typecode.LIST: False,
                Typecode.NAN: False,
                Typecode.NULL_STRING: False,
                Typecode.NONE: False,
                Typecode.REAL_NUMBER: False,
                Typecode.STRING: False,
            }
    """

    @property
    def header_list(self):
        return self.__header_list

    @header_list.setter
    def header_list(self, value):
        if self.__header_list == value:
            return

        self.__header_list = value
        self.__clear_cache()

    @property
    def default_type_hint(self):
        return self.__default_type_hint

    @default_type_hint.setter
    def default_type_hint(self, value):
        if self.__default_type_hint == value:
            return

        self.__default_type_hint = value
        self.__clear_cache()

    @property
    def column_type_hint_list(self):
        return self.__col_type_hint_list

    @column_type_hint_list.setter
    def column_type_hint_list(self, value):
        if self.__col_type_hint_list == value:
            return

        self.__col_type_hint_list = value
        self.__clear_cache()

    @property
    def is_formatting_float(self):
        return self.__is_formatting_float

    @is_formatting_float.setter
    def is_formatting_float(self, value):
        self.__is_formatting_float = value

    @property
    def strip_str_header(self):
        return self.__strip_str_header

    @strip_str_header.setter
    def strip_str_header(self, value):
        if self.__strip_str_header == value:
            return

        self.__strip_str_header = value
        self.__clear_cache()

    @property
    def strip_str_value(self):
        return self.__strip_str_value

    @strip_str_value.setter
    def strip_str_value(self, value):
        if self.__strip_str_value == value:
            return

        self.__strip_str_value = value
        self.__clear_cache()

    @property
    def min_column_width(self):
        return self.__min_col_ascii_char_width

    @min_column_width.setter
    def min_column_width(self, value):
        if self.__min_col_ascii_char_width == value:
            return

        self.__min_col_ascii_char_width = value
        self.__clear_cache()

    @property
    def float_type(self):
        return self.__float_type

    @float_type.setter
    def float_type(self, value):
        if self.__float_type == value:
            return

        self.__float_type = value
        self.__clear_cache()

    @property
    def datetime_format_str(self):
        return self.__datetime_format_str

    @datetime_format_str.setter
    def datetime_format_str(self, value):
        if self.__datetime_format_str == value:
            return

        self.__datetime_format_str = value
        self.__clear_cache()

    @property
    def strict_type_mapping(self):
        return self.__strict_type_mapping

    @strict_type_mapping.setter
    def strict_type_mapping(self, value):
        if self.__strict_type_mapping == value:
            return

        self.__strict_type_mapping = value
        self.__clear_cache()

    @property
    def east_asian_ambiguous_width(self):
        return self.__east_asian_ambiguous_width

    @east_asian_ambiguous_width.setter
    def east_asian_ambiguous_width(self, value):
        if self.__east_asian_ambiguous_width == value:
            return

        self.__east_asian_ambiguous_width = value
        self.__clear_cache()

    @property
    def type_value_mapping(self):
        return self.__type_value_mapping

    @type_value_mapping.setter
    def type_value_mapping(self, value):
        if self.__type_value_mapping == value:
            return

        self.__type_value_mapping = value
        self.__clear_cache()

    @property
    def const_value_mapping(self):
        return self.__const_value_mapping

    @const_value_mapping.setter
    def const_value_mapping(self, value):
        if self.__const_value_mapping == value:
            return

        self.__const_value_mapping = value
        self.__clear_cache()

    @property
    def quoting_flags(self):
        return self.__quoting_flags

    @quoting_flags.setter
    def quoting_flags(self, value):
        if self.__quoting_flags == value:
            return

        self.__quoting_flags = value
        self.__clear_cache()

    @property
    def datetime_formatter(self):
        return self.__datetime_formatter

    @datetime_formatter.setter
    def datetime_formatter(self, value):
        if self.__datetime_formatter == value:
            return

        self.__datetime_formatter = value
        self.__clear_cache()

    @property
    def matrix_formatting(self):
        return self.__matrix_formatting

    @matrix_formatting.setter
    def matrix_formatting(self, value):
        if self.__matrix_formatting == value:
            return

        self.__matrix_formatting = value
        self.__clear_cache()

    def __init__(self):
        self.max_workers = multiprocessing.cpu_count()

        self.__header_list = []
        self.__default_type_hint = None
        self.__col_type_hint_list = None

        self.__strip_str_header = None
        self.__strip_str_value = None
        self.__is_formatting_float = True
        self.__min_col_ascii_char_width = 0
        self.__float_type = None
        self.__datetime_format_str = DefaultValue.DATETIME_FORMAT
        self.__strict_type_mapping = copy.deepcopy(
            DefaultValue.STRICT_LEVEL_MAPPING)
        self.__east_asian_ambiguous_width = 1

        self.__type_value_mapping = copy.deepcopy(
            DefaultValue.TYPE_VALUE_MAPPING)
        self.__const_value_mapping = copy.deepcopy(
            DefaultValue.CONST_VALUE_MAPPING)
        self.__quoting_flags = copy.deepcopy(DefaultValue.QUOTING_FLAGS)
        self.__datetime_formatter = None
        self.__matrix_formatting = MatrixFormatting.TRIM

        self.__clear_cache()

    def __clear_cache(self):
        self.__update_dp_converter()
        self.__dp_cache_zero = self.__to_dp_raw(0)
        self.__dp_cache_one = self.__to_dp_raw(1)
        self.__dp_cache_true = self.__to_dp_raw(True)
        self.__dp_cache_false = self.__to_dp_raw(False)
        self.__dp_cache_mapping = {
            None: self.__to_dp_raw(None),
            "": self.__to_dp_raw(""),
        }

    def to_dp(self, value):
        self.__update_dp_converter()

        return self.__to_dp(value, strip_str=self.strip_str_value)

    def to_dp_list(self, value_list):
        if is_empty_sequence(value_list):
            return []

        self.__update_dp_converter()

        return self._to_dp_list(value_list, strip_str=self.strip_str_value)

    def to_column_dp_list(
            self, value_dp_matrix, previous_column_dp_list=None):
        logger.debug("prev_col_count={}, mismatch_process={}".format(
            len(previous_column_dp_list) if previous_column_dp_list else None,
            self.matrix_formatting))

        col_dp_list = self.__get_col_dp_list_base()

        logger.debug("converting to column dataproperty:")
        for col_idx, value_dp_list in enumerate(zip(*value_dp_matrix)):
            try:
                col_dp_list[col_idx]
            except IndexError:
                col_dp_list.append(ColumnDataProperty(
                    column_index=col_idx, min_width=self.min_column_width,
                    is_formatting_float=self.is_formatting_float,
                    datetime_format_str=self.datetime_format_str,
                    east_asian_ambiguous_width=self.east_asian_ambiguous_width
                ))

            col_dp = col_dp_list[col_idx]
            col_dp.begin_update()

            try:
                col_dp.merge(previous_column_dp_list[col_idx])
            except (TypeError, IndexError):
                pass

            for value_dp in value_dp_list:
                col_dp.update_body(value_dp)

            col_dp.end_update()

            logger.debug("  {:s}".format(str(col_dp)))

        return col_dp_list

    def to_dp_matrix(self, value_matrix):
        self.__update_dp_converter()
        logger.debug("max_workers = {}".format(self.max_workers))

        value_matrix = self.__strip_data_matrix(value_matrix)

        if self.__is_dp_matrix(value_matrix):
            logger.debug("already a dataproperty matrix")
            return value_matrix

        if not self.max_workers:
            self.max_workers = multiprocessing.cpu_count()

        if self.max_workers <= 1:
            return self.__to_dp_matrix_st(value_matrix)

        return self.__to_dp_matrix_mt(value_matrix)

    def to_header_dp_list(self):
        self.__update_dp_converter()

        return self._to_dp_list(
            self.header_list, type_hint=String,
            strip_str=self.strip_str_header,
            strict_type_mapping=NOT_STRICT_TYPE_MAPPING)

    @staticmethod
    def __is_dp_matrix(value):
        if not value:
            return False

        try:
            return isinstance(value[0][0], DataProperty)
        except (TypeError, IndexError):
            return False

    def __get_col_type_hint(self, col_idx):
        try:
            return self.col_type_hint_list[col_idx]
        except (TypeError, IndexError):
            return self.default_type_hint

    def __to_dp(
            self, data, type_hint=None, strip_str=None,
            strict_type_mapping=None):
        try:
            if data in self.__dp_cache_mapping:
                return self.__dp_cache_mapping.get(data)
        except TypeError:
            # unhashable type
            pass

        if data == 0:
            if str(data) != "False":
                return self.__dp_cache_zero

            return self.__dp_cache_false
        if data == 1:
            if str(data) != "True":
                return self.__dp_cache_one

            return self.__dp_cache_true

        return self.__to_dp_raw(
            data,
            type_hint=type_hint,
            strip_str=strip_str,
            strict_type_mapping=strict_type_mapping)

    def __to_dp_raw(
            self, data, type_hint=None, strip_str=None,
            strict_type_mapping=None):
        value_dp = DataProperty(
            data,
            type_hint=(
                type_hint if type_hint is not None else self.default_type_hint
            ),
            strip_str=strip_str,
            float_type=self.float_type,
            datetime_format_str=self.datetime_format_str,
            strict_type_mapping=(
                strict_type_mapping
                if type_hint is not None else self.strict_type_mapping),
            east_asian_ambiguous_width=self.east_asian_ambiguous_width
        )

        return self.__dp_converter.convert(value_dp)

    def __to_dp_matrix_st(self, value_matrix):
        return list(zip(*[
            _to_dp_list_helper(
                self, col_idx,
                data_list, self.__get_col_type_hint(col_idx),
                self.strip_str_value
            )[1]
            for col_idx, data_list
            in enumerate(zip(*value_matrix))
        ]))

    def __to_dp_matrix_mt(self, value_matrix):
        from concurrent import futures

        col_data_mapping = {}
        try:
            with futures.ProcessPoolExecutor(self.max_workers) as executor:
                future_list = [
                    executor.submit(
                        _to_dp_list_helper, self, col_idx,
                        data_list, self.__get_col_type_hint(col_idx),
                        self.strip_str_value
                    )
                    for col_idx, data_list
                    in enumerate(zip(*value_matrix))
                ]

                for future in futures.as_completed(future_list):
                    col_idx, value_dp_list = future.result()
                    col_data_mapping[col_idx] = value_dp_list
        finally:
            logger.debug("shutdown ProcessPoolExecutor")
            executor.shutdown()

        return list(zip(*[
            col_data_mapping[col_idx] for col_idx in sorted(col_data_mapping)
        ]))

    def _to_dp_list(
            self, data_list, type_hint=None, strip_str=None,
            strict_type_mapping=None):
        from collections import Counter
        from typepy import StrictLevel

        if is_empty_sequence(data_list):
            return []

        type_counter = Counter()

        dp_list = []
        for data in data_list:
            expect_type_hist = type_hint
            if type_hint is None:
                try:
                    expect_type_hist, _count = type_counter.most_common(1)[0]
                    if not expect_type_hist(
                            data, strict_level=StrictLevel.MAX).is_type():
                        expect_type_hist = None
                except IndexError:
                    pass

            dataprop = self.__to_dp(
                data=data, type_hint=expect_type_hist, strip_str=strip_str,
                strict_type_mapping=strict_type_mapping)
            type_counter[dataprop.type_class] += 1

            dp_list.append(dataprop)

        return dp_list

    def __strip_data_matrix(self, data_matrix):
        header_col_size = len(self.header_list) if self.header_list else 0
        try:
            col_size_list = [
                len(data_list) for data_list in data_matrix
            ]
        except TypeError:
            return []

        if self.header_list:
            min_col_size = min([header_col_size] + col_size_list)
            max_col_size = max([header_col_size] + col_size_list)
        elif col_size_list:
            min_col_size = min(col_size_list)
            max_col_size = max(col_size_list)
        else:
            min_col_size = 0
            max_col_size = 0

        if self.matrix_formatting == MatrixFormatting.EXCEPTION:
            if min_col_size != max_col_size:
                raise ValueError(
                    "nonuniform column size: min={}, max={}".format(
                        min_col_size, max_col_size))

            return data_matrix

        if self.matrix_formatting == MatrixFormatting.HEADER_ALIGNED:
            if header_col_size > 0:
                format_col_size = header_col_size
            else:
                format_col_size = max_col_size
        elif self.matrix_formatting == MatrixFormatting.TRIM:
            format_col_size = min_col_size
        elif self.matrix_formatting == MatrixFormatting.FILL_NONE:
            format_col_size = max_col_size
        else:
            raise ValueError(
                "unknown matrix formatting: {}".format(self.matrix_formatting))

        return [
            list(data_matrix[row_idx][:format_col_size]) +
            [None] * (format_col_size - col_size)
            for row_idx, col_size in enumerate(col_size_list)
        ]

    def __get_col_dp_list_base(self):
        header_dp_list = self.to_header_dp_list()
        col_dp_list = []

        for col_idx, header_dp in enumerate(header_dp_list):
            col_dp = ColumnDataProperty(
                column_index=col_idx, min_width=self.min_column_width,
                is_formatting_float=self.is_formatting_float,
                datetime_format_str=self.datetime_format_str,
                east_asian_ambiguous_width=self.east_asian_ambiguous_width)
            col_dp.update_header(header_dp)
            col_dp_list.append(col_dp)

        return col_dp_list

    def __update_dp_converter(self):
        self.__dp_converter = DataPropertyConverter(
            type_value_mapping=self.type_value_mapping,
            const_value_mapping=self.const_value_mapping,
            quoting_flags=self.quoting_flags,
            datetime_formatter=self.datetime_formatter,
            datetime_format_str=self.datetime_format_str,
            float_type=self.float_type,
            strict_type_mapping=self.strict_type_mapping)

    @property
    def col_type_hint_list(self):
        # mark as delete

        return self.column_type_hint_list

    @col_type_hint_list.setter
    def col_type_hint_list(self, value):
        # mark as delete

        self.column_type_hint_list = value

    def to_dataproperty(self, data):
        # mark as delete

        return self.to_dp(data)

    def to_dataproperty_list(self, data_list):
        # mark as delete

        return self.to_dp_list(data_list)

    def to_col_dataproperty_list(
            self, value_dp_matrix, previous_column_dp_list=None):
        # mark as delete

        return self.to_column_dp_list(value_dp_matrix, previous_column_dp_list)

    def to_dataproperty_matrix(self, value_matrix):
        # mark as delete

        return self.to_dp_matrix(value_matrix)

    def to_header_dataproperty_list(self):
        # mark as delete

        return self.to_header_dp_list()


def _to_dp_list_helper(
        extractor, col_idx, data_list, type_hint, strip_str):
    return (col_idx,
            extractor._to_dp_list(
                data_list, type_hint=type_hint, strip_str=strip_str))
