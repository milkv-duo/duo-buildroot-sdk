# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from .._typecode import Typecode
from ..checker import ListTypeChecker
from ..converter import ListConverter
from ._base import AbstractType


class List(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_list_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.LIST

    def __init__(self, value, strict_level=1, **kwargs):
        super(List, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return ListTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return ListConverter(self._data)
