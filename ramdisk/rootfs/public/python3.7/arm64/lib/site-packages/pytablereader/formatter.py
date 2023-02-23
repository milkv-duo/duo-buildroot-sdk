# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

import abc

from pytablereader import InvalidDataError
import six

from ._acceptor import LoaderAcceptor


@six.add_metaclass(abc.ABCMeta)
class TableFormatterInterface(object):
    """
    The abstract class of table data validator.
    """

    @abc.abstractmethod
    def to_table_data(self):  # pragma: no cover
        pass


class TableFormatter(LoaderAcceptor, TableFormatterInterface):
    """
    The abstract class of |TableData| formatter.
    """

    def _validate_source_data(self):
        if not self._source_data:
            raise InvalidDataError("source data is empty")

    def __init__(self, source_data):
        self._source_data = source_data

        self._validate_source_data()
