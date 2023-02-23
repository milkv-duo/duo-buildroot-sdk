# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from .._typecode import Typecode
from ..checker import (
    StringTypeChecker,
    NullStringTypeChecker,
)
from ..converter import (
    StringConverter,
    NullStringConverter,
)
from ._base import AbstractType


class String(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_string_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.STRING

    def __init__(self, value, strict_level=1, **kwargs):
        super(String, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return StringTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return StringConverter(self._data)


class NullString(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_nullstring_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.NULL_STRING

    def __init__(self, value, strict_level=1, **kwargs):
        super(NullString, self).__init__(value, strict_level, kwargs)

    def force_convert(self):
        return ""

    def _create_type_checker(self):
        return NullStringTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return NullStringConverter(self._data)
