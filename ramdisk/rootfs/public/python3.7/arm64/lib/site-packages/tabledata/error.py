# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import


class InvalidNameError(Exception):
    """
    Base name error class.
    """


class InvalidTableNameError(InvalidNameError):
    """
    Exception raised when the table name is invalid.
    """


class InvalidHeaderNameError(InvalidNameError):
    """
    Exception raised when a table header name is invalid.
    """


class InvalidDataError(ValueError):
    """
    Exception raised when data is invalid as tabular data.
    """


class EmptyDataError(InvalidDataError):
    """
    Exception raised when data does not include valid table data.
    """
