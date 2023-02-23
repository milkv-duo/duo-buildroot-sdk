# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from .._typecode import Typecode
from ..checker import NanTypeChecker
from ..converter import FloatConverter
from ._base import AbstractType


class Nan(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_nan_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.NAN

    def __init__(self, value, strict_level=1, **kwargs):
        super(Nan, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return NanTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return FloatConverter(self._data)
