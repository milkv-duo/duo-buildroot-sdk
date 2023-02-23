# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from .._typecode import Typecode
from ..checker import DateTimeTypeChecker
from ..converter import DateTimeConverter
from ._base import AbstractType


class DateTime(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_datetime_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    _TIMEZONE_KEY = "timezone"

    @property
    def typecode(self):
        return Typecode.DATETIME

    def __init__(self, value, strict_level=2, **kwargs):
        super(DateTime, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return DateTimeTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return DateTimeConverter(
            self._data, self._params.get(self._TIMEZONE_KEY))
