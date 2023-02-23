# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals


class InvalidNameError(ValueError):
    """
    Base exception class that indicates invalid name errors.
    """


class NullNameError(InvalidNameError):
    """
    Exception raised when a name is empty.
    """


class InvalidCharError(InvalidNameError):
    """
    Exception raised when includes invalid character(s) within a string.
    """


class InvalidCharWindowsError(InvalidCharError):
    """
    Exception raised when includes Windows specific invalid character(s)
    within a string.
    """


class InvalidLengthError(InvalidNameError):
    """
    Exception raised when a string too long/short.
    """


class ReservedNameError(InvalidNameError):
    """
    Exception raised when a string is matched a reserved name.
    """


class ValidReservedNameError(ReservedNameError):
    """
    Exception raised when a string is matched a reserved name.
    However, it can be used as a name.
    """


class InvalidReservedNameError(ReservedNameError):
    """
    Exception raised when a string is matched a reserved name.
    And the reserved name is invalid as a name.
    """
