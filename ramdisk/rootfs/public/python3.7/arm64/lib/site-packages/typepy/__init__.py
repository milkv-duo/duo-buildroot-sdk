# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

import typepy.type

from ._const import StrictLevel
from ._error import TypeConversionError
from ._function import (
    is_hex,
    is_list_or_tuple,
    is_null_string,
    is_not_null_string,
    is_empty_string,
    is_not_empty_string,
    is_empty_sequence,
    is_not_empty_sequence,
)
from ._typecode import Typecode
