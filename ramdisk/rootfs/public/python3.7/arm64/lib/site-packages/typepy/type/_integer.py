# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from .._typecode import Typecode
from ..checker._integer import IntegerTypeChecker
from ..converter._integer import IntegerConverter
from ._base import AbstractType


class Integer(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_integer_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.INTEGER

    def __init__(self, value, strict_level=1, **kwargs):
        super(Integer, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return IntegerTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return IntegerConverter(self._data)
