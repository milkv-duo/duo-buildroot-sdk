# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from .._typecode import Typecode
from ..checker import DictionaryTypeChecker
from ..converter import DictionaryConverter
from ._base import AbstractType


class Dictionary(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_dictionary_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.DICTIONARY

    def __init__(self, value, strict_level=1, **kwargs):
        super(Dictionary, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return DictionaryTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return DictionaryConverter(self._data)
