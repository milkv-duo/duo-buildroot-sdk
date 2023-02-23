# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

import math

import six


def isstring(value):
    return isinstance(
        value, six.string_types + (six.text_type, six.binary_type))


def isinf(value):
    try:
        return math.isinf(value)
    except TypeError:
        return False


def isnan(value):
    try:
        return math.isnan(value)
    except TypeError:
        return False
