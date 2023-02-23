# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from decimal import Decimal
import math

from mbstrdecoder import MultiByteStrDecoder
import six
from typepy import (
    TypeConversionError,
    Typecode,
    StrictLevel,
)
from typepy.type import (
    Bool,
    DateTime,
    Dictionary,
    Infinity,
    Integer,
    IpAddress,
    List,
    Nan,
    NoneType,
    RealNumber,
    String,
    NullString,
)

from ._align_getter import align_getter
from ._common import DefaultValue
from ._container import (
    MinMaxContainer,
    ListContainer,
)
from ._function import (
    get_number_of_digit,
    get_ascii_char_width,
)
from ._interface import DataPeropertyInterface


class DataPeropertyBase(DataPeropertyInterface):
    __slots__ = (
        "__datetime_format_str",
        "__format_str",
    )

    __TYPE_CLASS_TABLE = {
        Typecode.BOOL: Bool,
        Typecode.DATETIME: DateTime,
        Typecode.DICTIONARY: Dictionary,
        Typecode.INTEGER: Integer,
        Typecode.INFINITY: Infinity,
        Typecode.IP_ADDRESS: IpAddress,
        Typecode.LIST: List,
        Typecode.NAN: Nan,
        Typecode.NONE: NoneType,
        Typecode.NULL_STRING: NullString,
        Typecode.REAL_NUMBER: RealNumber,
        Typecode.STRING: String,
    }

    @property
    def type_class(self):
        return self.__TYPE_CLASS_TABLE.get(self.typecode)

    @property
    def typename(self):
        return self.typecode.name

    @property
    def format_str(self):
        if self.__format_str:
            return self.__format_str

        self.__format_str = self.__get_format_str()

        return self.__format_str

    def __get_format_str(self):
        format_str = {
            Typecode.NONE: "{}",
            Typecode.INTEGER: "{:d}",
            Typecode.IP_ADDRESS: "{}",
            Typecode.BOOL: "{}",
            Typecode.DATETIME: "{:" + self.__datetime_format_str + "}",
            Typecode.DICTIONARY: "{}",
            Typecode.LIST: "{}",
        }.get(self.typecode)

        if format_str is not None:
            return format_str

        if self.typecode in (
                Typecode.REAL_NUMBER, Typecode.INFINITY, Typecode.NAN):
            if Nan(self.decimal_places).is_type():
                return "{:f}"

            return self._get_realnumber_format()

        return "{:s}"

    def _get_realnumber_format(self):
        try:
            return "{:" + ".{:d}f".format(self.decimal_places) + "}"
        except ValueError:
            return "{:f}"

    def __init__(self, datetime_format_str):
        self.__datetime_format_str = datetime_format_str
        self.__format_str = None


class DataProperty(DataPeropertyBase):
    __slots__ = (
        "__data",
        "__typecode",
        "__align",
        "__integer_digits",
        "__decimal_places",
        "__east_asian_ambiguous_width",
        "__additional_format_len",
        "__length",
        "__ascii_char_width",
    )

    __type_class_list = [
        NoneType,
        Integer,
        Infinity,
        Nan,
        IpAddress,
        RealNumber,
        Bool,
        List,
        Dictionary,
        DateTime,
        NullString,
        String,
    ]

    @property
    def align(self):
        if not self.__align:
            self.__align = align_getter.get_align_from_typecode(self.typecode)

        return self.__align

    @property
    def decimal_places(self):
        """
        :return:
            Decimal places if the ``data`` is ``float``.
            Returns ``0`` if the ``data`` is ``int``.
            Otherwise, returns ``float("nan")``.
        :rtype: int
        """

        if not self.__decimal_places:
            self.__set_digit()

        return self.__decimal_places

    @property
    def typecode(self):
        """
        ``typepy.Typecode`` that corresponds to the type of the ``data``.

        :return:
            One of the Enum value that are defined ``typepy.Typecode``.
        :rtype: typepy.Typecode
        """

        return self.__typecode

    @property
    def data(self):
        """
        :return: Original data.
        :rtype: Original data type.
        """

        return self.__data

    @property
    def length(self):
        """
        :return: Length of the ``data``.
        :rtype: int
        """

        if not self.__length:
            self.__length = self.__get_length()

        return self.__length

    @property
    def ascii_char_width(self):
        if not self.__ascii_char_width:
            self.__ascii_char_width = self.__get_ascii_char_width(
                self.__east_asian_ambiguous_width)

        return self.__ascii_char_width

    @property
    def integer_digits(self):
        """
        :return:
            Integer digits if the ``data`` is ``int``/``float``.
            Otherwise, returns ``float("nan")``.
        :rtype: int
        """

        if not self.__integer_digits:
            self.__set_digit()

        return self.__integer_digits

    @property
    def additional_format_len(self):
        if not self.__additional_format_len:
            self.__additional_format_len = self.__get_additional_format_len()

        return self.__additional_format_len

    def __init__(
            self, data,
            type_hint=None,
            strip_str=None,
            float_type=None,
            datetime_format_str=DefaultValue.DATETIME_FORMAT,
            strict_type_mapping=None,
            replace_tabs_with_spaces=True, tab_length=2,
            east_asian_ambiguous_width=1):
        super(DataProperty, self).__init__(datetime_format_str)
        self.__additional_format_len = None
        self.__align = None
        self.__ascii_char_width = None
        self.__decimal_places = None
        self.__east_asian_ambiguous_width = east_asian_ambiguous_width
        self.__integer_digits = None
        self.__length = None
        self.__typecode = None

        data = self.__preprocess_data(data, strip_str)
        self.__set_data(data, type_hint, float_type, strict_type_mapping)
        self.__replace_tabs(replace_tabs_with_spaces, tab_length)

    def __eq__(self, other):
        if self.typecode != other.typecode:
            return False

        if self.typecode == Typecode.NAN:
            return True

        return self.data == other.data

    def __ne__(self, other):
        if self.typecode != other.typecode:
            return True

        if self.typecode == Typecode.NAN:
            return False

        return self.data != other.data

    def __repr__(self):
        element_list = []

        if self.typecode == Typecode.DATETIME:
            element_list.append(
                "data={:s}".format(six.text_type(self.data)))
        else:
            try:
                element_list.append("data=" + self.to_str())
            except UnicodeEncodeError:
                element_list.append("data={}".format(
                    MultiByteStrDecoder(self.data).unicode_str))

        element_list.extend([
            "typename={:s}".format(self.typename),
            "align={}".format(self.align.align_string),
            "ascii_char_width={:d}".format(self.ascii_char_width),
        ])

        if Integer(self.length).is_type():
            element_list.append(
                "length={}".format(self.length))

        if Integer(self.integer_digits).is_type():
            element_list.append(
                "integer_digits={}".format(self.integer_digits))

        if Integer(self.decimal_places).is_type():
            element_list.append(
                "decimal_places={}".format(self.decimal_places))

        if Integer(self.additional_format_len).is_type():
            element_list.append(
                "additional_format_len={}".format(self.additional_format_len))

        return ", ".join(element_list)

    def get_padding_len(self, ascii_char_width):
        try:
            return ascii_char_width - (self.ascii_char_width - self.length)
        except TypeError:
            return ascii_char_width

    def to_str(self):
        return self.format_str.format(self.data)

    def __get_additional_format_len(self):
        if not RealNumber(self.data).is_type():
            return 0

        format_len = 0

        if float(self.data) < 0:
            # for minus character
            format_len += 1

        return format_len

    def __get_base_float_len(self):
        if any([self.integer_digits < 0, self.decimal_places < 0]):
            raise ValueError(
                "integer digits and decimal places must be greater or equals to zero")

        float_len = self.integer_digits + self.decimal_places
        if self.decimal_places > 0:
            # for dot
            float_len += 1

        return float_len

    def __get_length(self):
        if self.typecode in (
                Typecode.DICTIONARY, Typecode.LIST, Typecode.STRING):
            return len(self.data)

        return None

    def __get_ascii_char_width(self, east_asian_ambiguous_width):
        if self.typecode == Typecode.INTEGER:
            return self.integer_digits + self.additional_format_len

        if self.typecode == Typecode.REAL_NUMBER:
            return (
                self.__get_base_float_len() + self.additional_format_len)

        if self.typecode == Typecode.DATETIME:
            try:
                return len(self.to_str())
            except ValueError:
                # reach to this line if the year <1900.
                # the datetime strftime() methods require year >= 1900.
                return len(six.text_type(self.data))

        try:
            unicode_str = MultiByteStrDecoder(self.data).unicode_str
        except ValueError:
            unicode_str = self.to_str()

        return get_ascii_char_width(unicode_str, east_asian_ambiguous_width)

    @staticmethod
    def __preprocess_data(data, strip_str):
        if strip_str is None:
            return data

        try:
            return data.strip(strip_str)
        except AttributeError:
            return data
        except UnicodeDecodeError:
            return MultiByteStrDecoder(data).unicode_str.strip(strip_str)

    def __set_data(self, data, type_hint, float_type, strict_type_mapping):
        if float_type is None:
            float_type = DefaultValue.FLOAT_TYPE

        if strict_type_mapping is None:
            strict_type_mapping = DefaultValue.STRICT_LEVEL_MAPPING

        if type_hint:
            type_obj = type_hint(
                data, strict_level=StrictLevel.MIN, float_type=float_type)
            self.__typecode = type_obj.typecode
            self.__data = type_obj.try_convert()

            if type_hint(
                    self.__data, strict_level=StrictLevel.MAX,
                    float_type=float_type).is_type():
                return

        for type_class in self.__type_class_list:
            strict_level = strict_type_mapping.get(
                type_class(None).typecode, False)

            if self.__try_convert_type(
                    data, type_class, strict_level, float_type):
                return

        raise TypeConversionError(
            "failed to convert: data={}, strict_level={}".format(
                data, strict_type_mapping))

    def __set_digit(self):
        integer_digits, decimal_places = get_number_of_digit(self.__data)
        self.__integer_digits = integer_digits
        self.__decimal_places = decimal_places

    def __try_convert_type(self, data, type_class, strict_level, float_type):
        type_obj = type_class(data, strict_level, float_type=float_type)

        try:
            self.__data = type_obj.convert()
        except TypeConversionError:
            return False

        self.__typecode = type_obj.typecode

        return True

    def __replace_tabs(self, replace_tabs_with_spaces, tab_length):
        if not replace_tabs_with_spaces:
            return

        try:
            self.__data = self.__data.replace("\t", " " * tab_length)
        except (TypeError, AttributeError):
            pass


class ColumnDataProperty(DataPeropertyBase):
    __slots__ = (
        "__ascii_char_width",
        "__column_index",
        "__decimal_places",
        "__dp_list",
        "__east_asian_ambiguous_width",
        "__is_calculate",
        "__is_formatting_float",
        "__length",
        "__minmax_integer_digits",
        "__minmax_decimal_places",
        "__minmax_additional_format_len",
        "__typecode",
        "__typecode_bitmap",
    )

    @property
    def align(self):
        return align_getter.get_align_from_typecode(self.typecode)

    @property
    def bit_length(self):
        if self.typecode != Typecode.INTEGER:
            return None

        bit_length = 0
        for value_dp in self.__dp_list:
            try:
                bit_length = max(
                    bit_length, int.bit_length(value_dp.data))
            except TypeError:
                pass

        return bit_length

    @property
    def column_index(self):
        return self.__column_index

    @property
    def decimal_places(self):
        return self.__decimal_places

    @property
    def typecode(self):
        return self.__typecode

    @property
    def padding_len(self):
        import warnings

        warnings.warn(
            "padding_len will be deleted in the future, "
            "use ascii_char_width instead.",
            DeprecationWarning)

        return self.ascii_char_width

    @property
    def ascii_char_width(self):
        return self.__ascii_char_width

    @property
    def minmax_integer_digits(self):
        return self.__minmax_integer_digits

    @property
    def minmax_decimal_places(self):
        return self.__minmax_decimal_places

    @property
    def minmax_additional_format_len(self):
        return self.__minmax_additional_format_len

    def __init__(
            self, column_index=None, min_width=0,
            is_formatting_float=True,
            datetime_format_str=DefaultValue.DATETIME_FORMAT,
            east_asian_ambiguous_width=1):
        super(ColumnDataProperty, self).__init__(datetime_format_str)

        self.__ascii_char_width = min_width
        self.__column_index = column_index
        self.__east_asian_ambiguous_width = east_asian_ambiguous_width

        self.__is_calculate = True
        self.__is_formatting_float = is_formatting_float
        self.__dp_list = []
        self.__decimal_places = float("nan")
        self.__minmax_integer_digits = MinMaxContainer()
        self.__minmax_decimal_places = ListContainer()
        self.__minmax_additional_format_len = MinMaxContainer()

        self.__typecode_bitmap = Typecode.NONE.value
        self.__calc_typecode_from_bitmap()

    def __repr__(self):
        element_list = []

        if self.column_index is not None:
            element_list.append("column={}".format(self.column_index))

        element_list.extend([
            "typename={}".format(self.typename),
            "align={}".format(self.align.align_string),
            "ascii_char_width={}".format(six.text_type(self.ascii_char_width)),
        ])

        if Integer(self.bit_length).is_type():
            element_list.append("bit_length={:d}".format(self.bit_length))

        if self.minmax_integer_digits.has_value():
            element_list.append(
                "integer_digits=({})".format(
                    six.text_type(self.minmax_integer_digits)))

        if self.minmax_decimal_places.has_value():
            element_list.append(
                "decimal_places=({})".format(
                    six.text_type(self.minmax_decimal_places)))

        if not self.minmax_additional_format_len.is_zero():
            element_list.append(
                "additional_format_len=({})".format(
                    six.text_type(self.minmax_additional_format_len)))

        return ", ".join(element_list)

    def dp_to_str(self, value_dp):
        to_string_format_str = self.__get_tostring_format(value_dp)

        if any([
                self.typecode in [Typecode.BOOL, Typecode.DATETIME],
                all([self.typecode == Typecode.STRING,
                     value_dp.typecode == Typecode.REAL_NUMBER]),
        ]):
            return to_string_format_str.format(value_dp.data)

        try:
            value = self.type_class(
                value_dp.data, strict_level=StrictLevel.MIN
            ).convert()
        except TypeConversionError:
            value = value_dp.data

        try:
            item = to_string_format_str.format(value)
        except ValueError:
            item = MultiByteStrDecoder(value).unicode_str

        return item

    def extend_width(self, dwidth):
        self.__ascii_char_width += dwidth

    def update_header(self, dataprop):
        self.__ascii_char_width = max(
            self.__ascii_char_width, dataprop.ascii_char_width)

    def update_body(self, dataprop):
        self.__typecode_bitmap |= dataprop.typecode.value
        self.__calc_typecode_from_bitmap()

        if dataprop.typecode in (Typecode.REAL_NUMBER, Typecode.INTEGER):
            self.__minmax_integer_digits.update(dataprop.integer_digits)
            self.__minmax_decimal_places.update(dataprop.decimal_places)
            self.__calc_decimal_places()

        self.__minmax_additional_format_len.update(
            dataprop.additional_format_len)

        self.__dp_list.append(dataprop)
        self.__ascii_char_width = max(
            self.__ascii_char_width, dataprop.ascii_char_width)
        self.__calc_ascii_char_width()

    def merge(self, col_dataprop):
        self.__typecode_bitmap |= col_dataprop.typecode.value
        self.__calc_typecode_from_bitmap()

        self.__minmax_integer_digits.merge(col_dataprop.minmax_integer_digits)
        self.__minmax_decimal_places.update(col_dataprop.minmax_decimal_places)
        self.__calc_decimal_places()

        self.__minmax_additional_format_len.merge(
            col_dataprop.minmax_additional_format_len)

        self.__ascii_char_width = max(
            self.__ascii_char_width, col_dataprop.ascii_char_width)
        self.__calc_ascii_char_width()

    def begin_update(self):
        self.__is_calculate = False

    def end_update(self):
        self.__is_calculate = True

        self.__calc_typecode_from_bitmap()
        self.__calc_decimal_places()
        self.__calc_ascii_char_width()

    def __is_not_single_typecode(self, typecode_bitmap):
        return (
            self.__typecode_bitmap & typecode_bitmap and
            self.__typecode_bitmap & ~typecode_bitmap
        )

    def __is_float_typecode(self):
        FLOAT_TYPECODE_BMP = (
            Typecode.REAL_NUMBER.value
            | Typecode.INFINITY.value
            | Typecode.NAN.value)
        NUMBER_TYPECODE_BMP = FLOAT_TYPECODE_BMP | Typecode.INTEGER.value

        if self.__is_not_single_typecode(
                NUMBER_TYPECODE_BMP | Typecode.NULL_STRING.value):
            return False

        if bin(
                self.__typecode_bitmap &
                (FLOAT_TYPECODE_BMP | Typecode.NULL_STRING.value)
        ).count("1") >= 2:
            return True

        if bin(self.__typecode_bitmap & NUMBER_TYPECODE_BMP).count("1") >= 2:
            return True

        return False

    def __get_ascii_char_width(self):
        if not self.__typecode_bitmap & Typecode.REAL_NUMBER.value:
            return self.__ascii_char_width

        max_width = self.__ascii_char_width

        for value_dp in self.__dp_list:
            if value_dp.typecode in [Typecode.INFINITY, Typecode.NAN]:
                continue

            max_width = max(
                max_width,
                get_ascii_char_width(
                    self.dp_to_str(value_dp),
                    self.__east_asian_ambiguous_width))

        return max_width

    def __get_decimal_places(self):
        try:
            avg = self.minmax_decimal_places.mean()
        except TypeError:
            return float("nan")

        if Nan(avg).is_type():
            return float("nan")

        return int(min(
            math.ceil(avg + Decimal("1.0")),
            self.minmax_decimal_places.max_value))

    def __get_tostring_format(self, value_dp):
        if all([value_dp.typecode == Typecode.REAL_NUMBER,
                self.__is_formatting_float]):
            return self._get_realnumber_format()

        if any([
                all([
                    self.typecode == Typecode.REAL_NUMBER,
                    not self.__is_formatting_float,
                    value_dp.typecode in [
                        Typecode.INTEGER, Typecode.REAL_NUMBER],
                ]),
                value_dp.typecode == Typecode.NONE,
        ]):
            return "{}"

        try:
            self.format_str.format(value_dp.data)
        except (TypeError, ValueError):
            return "{}"

        return self.format_str

    def __get_typecode_from_bitmap(self):
        if self.__is_float_typecode():
            return Typecode.REAL_NUMBER

        if any([
                self.__is_not_single_typecode(Typecode.BOOL.value),
                self.__is_not_single_typecode(Typecode.DATETIME.value),
        ]):
            return Typecode.STRING

        typecode_list = [
            Typecode.STRING,
            Typecode.REAL_NUMBER,
            Typecode.INTEGER,
            Typecode.DATETIME,
            Typecode.DICTIONARY,
            Typecode.IP_ADDRESS,
            Typecode.LIST,
            Typecode.BOOL,
            Typecode.INFINITY,
            Typecode.NAN,
            Typecode.NULL_STRING,
        ]

        for typecode in typecode_list:
            if self.__typecode_bitmap & typecode.value:
                return typecode

        if self.__typecode_bitmap == Typecode.NONE.value:
            return Typecode.NONE

        return Typecode.STRING

    def __calc_ascii_char_width(self):
        if not self.__is_calculate:
            return

        self.__ascii_char_width = self.__get_ascii_char_width()

    def __calc_decimal_places(self):
        if not self.__is_calculate:
            return

        self.__decimal_places = self.__get_decimal_places()

    def __calc_typecode_from_bitmap(self):
        if not self.__is_calculate:
            return

        self.__typecode = self.__get_typecode_from_bitmap()
