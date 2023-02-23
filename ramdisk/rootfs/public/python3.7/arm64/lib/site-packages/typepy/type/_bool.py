# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from .._typecode import Typecode
from ..checker._bool import BoolTypeChecker
from ..converter._bool import BoolConverter
from ._base import AbstractType


class Bool(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_bool_type.txt

    .. py:attribute:: strict_level

        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.BOOL

    def __init__(self, value, strict_level=2, **kwargs):
        super(Bool, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return BoolTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return BoolConverter(self._data)
