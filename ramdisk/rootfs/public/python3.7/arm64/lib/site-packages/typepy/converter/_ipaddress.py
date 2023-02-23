# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import six

from .._error import TypeConversionError
from ._interface import AbstractValueConverter


class IpAddressConverter(AbstractValueConverter):

    def force_convert(self):
        import ipaddress

        try:
            return ipaddress.ip_address(six.text_type(self._value))
        except ValueError:
            raise TypeConversionError(
                "failed to force_convert to dictionary: type={}".format(
                    type(self._value)))
