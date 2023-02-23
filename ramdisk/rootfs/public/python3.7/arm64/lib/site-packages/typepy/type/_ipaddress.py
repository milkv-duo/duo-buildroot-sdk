# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from .._typecode import Typecode
from ..checker import IpAddressTypeChecker
from ..converter import IpAddressConverter
from ._base import AbstractType


class IpAddress(AbstractType):
    """
    |result_matrix_desc|

    .. include:: matrix_ipaddress_type.txt

    :py:attr:`.strict_level`
        |strict_level|
    """

    @property
    def typecode(self):
        return Typecode.IP_ADDRESS

    def __init__(self, value, strict_level=1, **kwargs):
        super(IpAddress, self).__init__(value, strict_level, kwargs)

    def _create_type_checker(self):
        return IpAddressTypeChecker(self._data, self._strict_level)

    def _create_type_converter(self):
        return IpAddressConverter(self._data)
