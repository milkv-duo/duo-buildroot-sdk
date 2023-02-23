# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc
import re

import dataproperty
import six
import typepy

import pathvalidate as pv
from six.moves import range

from ._common import convert_idx_to_alphabet
from ._core import TableData
from .error import (
    InvalidTableNameError,
    InvalidHeaderNameError,
    EmptyDataError
)


@six.add_metaclass(abc.ABCMeta)
class TableDataSanitizerInterface(object):
    """
    Interface class to validate and sanitize data of |TableData|.
    """

    @abc.abstractmethod
    def validate(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def sanitize(self):  # pragma: no cover
        pass


class AbstractTableDataSanitizer(TableDataSanitizerInterface):

    def __init__(self, tabledata):
        self._tabledata = tabledata

    def validate(self):
        self._validate_table_name(self._tabledata)
        self.__validate_header_list()

    def sanitize(self):
        """
        :return: Sanitized table data.
        :rtype: tabledata.TableData
        """

        return TableData(
            self.__sanitize_table_name(),
            self._sanitize_header_list(),
            self._tabledata.value_dp_matrix)

    @abc.abstractmethod
    def _preprocess_table_name(self):
        """
        Always called before table name validation.
        You must return preprocessed table name.
        """

    @abc.abstractmethod
    def _validate_table_name(self, table_name):
        """
        Must raise :py:class:`~.InvalidTableNameError`
        when you consider the table name invalid.

        :param str header: Table name to validate.
        :raises pytablereader.InvalidTableNameError:
            If the table name is invalid.
            |raises_validate_table_name|
        """

    @abc.abstractmethod
    def _sanitize_table_name(self, table_name):
        """
        Must return a valid table name.
        The table name must be a valid name with
        :py:meth:`~._validate_table_name` method.

        This method called when :py:meth:`~._validate_table_name` method raise
        :py:class:`~.InvalidTableNameError`.

        :param str table_name: Table name to sanitize.
        :return: Sanitized table name.
        :rtype: str
        """

    @abc.abstractmethod
    def _preprocess_header(self, col_idx, header):
        """
        Always called before a header validation.
        You must return preprocessed header.
        """

    @abc.abstractmethod
    def _validate_header(self, header):
        """
        No operation.

        This method called for each table header. Override this method
        in subclass if you want to detect invalid table header element.
        Raise :py:class:`~.InvalidHeaderNameError` if an invalid
        header element found.

        :param str header: Table header name.
        :raises pytablereader.InvalidHeaderNameError:
            If the ``header`` is invalid.
        """

    @abc.abstractmethod
    def _sanitize_header(self, header):
        """
        Must return a valid header name.
        This method called when :py:meth:`~._validate_header` method raise
        :py:class:`~.InvalidHeaderNameError`.
        Override this method in subclass if you want to rename invalid
        table header element.

        :param str header: Header name to sanitize.
        :return: Renamed header name.
        :rtype: str
        """

    def __validate_header_list(self):
        for header in self._tabledata.header_list:
            self._validate_header(header)

    def __sanitize_table_name(self):
        preprocessed_table_name = self._preprocess_table_name()

        try:
            self._validate_table_name(preprocessed_table_name)
            new_table_name = preprocessed_table_name
        except InvalidTableNameError:
            new_table_name = self._sanitize_table_name(
                preprocessed_table_name)
            self._validate_table_name(new_table_name)
        except pv.NullNameError as e:
            raise InvalidTableNameError(e)

        return new_table_name

    def _sanitize_header_list(self):
        new_header_list = []

        for col_idx, header in enumerate(self._tabledata.header_list):
            header = self._preprocess_header(col_idx, header)

            try:
                self._validate_header(header)
                new_header = header
            except InvalidHeaderNameError:
                new_header = self._sanitize_header(header)
                self._validate_header(new_header)
            except pv.NullNameError as e:
                raise InvalidHeaderNameError(e)

            new_header_list.append(new_header)

        return new_header_list


class TableDataSanitizer(AbstractTableDataSanitizer):

    def _preprocess_table_name(self):
        return self._tabledata.table_name

    def _validate_table_name(self, table_name):
        try:
            typepy.type.String(table_name).validate()
        except TypeError as e:
            raise InvalidTableNameError(e)

    def _sanitize_table_name(self, table_name):
        return typepy.type.String(table_name).force_convert()

    def _preprocess_header(self, col_idx, header):
        return header

    def _validate_header(self, header):
        try:
            typepy.type.String(header).validate()
        except TypeError as e:
            raise InvalidHeaderNameError(e)

    def _sanitize_header(self, header):
        return typepy.type.String(header).force_convert()


class SQLiteTableDataSanitizer(AbstractTableDataSanitizer):

    __RE_PREPROCESS = re.compile("[^a-zA-Z0-9_]+")
    __RENAME_TEMPLATE = "rename_{:s}"

    def __init__(self, tabledata):
        super(SQLiteTableDataSanitizer, self).__init__(tabledata)

        self.__upper_header_list = [
            header.upper() for header in self._tabledata.header_list if header
        ]

    def _preprocess_table_name(self):
        try:
            new_name = self.__RE_PREPROCESS.sub(
                "_", self._tabledata.table_name)
            return new_name.strip("_")
        except TypeError:
            raise InvalidTableNameError(
                "table name must be a string: value='{}'".format(
                    self._tabledata.table_name))

    def _validate_table_name(self, table_name):
        try:
            pv.validate_sqlite_table_name(table_name)
        except pv.ValidReservedNameError:
            pass
        except (pv.InvalidReservedNameError, pv.InvalidCharError) as e:
            raise InvalidTableNameError(e)

    def _sanitize_table_name(self, table_name):
        return self.__RENAME_TEMPLATE.format(table_name)

    def _preprocess_header(self, col_idx, header):
        if typepy.is_null_string(header):
            return self.__get_default_header(col_idx)

        if dataproperty.is_multibyte_str(header):
            return header

        try:
            return self.__RE_PREPROCESS.sub("", header)
        except TypeError:
            raise InvalidHeaderNameError(
                "header must be a string: value='{}'".format(header))

    def _validate_header(self, header):
        try:
            pv.validate_sqlite_attr_name(header)
        except (pv.NullNameError, pv.ReservedNameError):
            pass
        except pv.InvalidCharError as e:
            raise InvalidHeaderNameError(e)

    def _sanitize_header(self, header):
        return self.__RENAME_TEMPLATE.format(header)

    def _sanitize_header_list(self):
        if typepy.is_empty_sequence(self._tabledata.header_list):
            try:
                return [
                    self.__get_default_header(col_idx)
                    for col_idx
                    in range(len(self._tabledata.value_dp_matrix[0]))
                ]
            except IndexError:
                raise EmptyDataError("header list and data body are empty")

        return super(SQLiteTableDataSanitizer, self)._sanitize_header_list()

    def __get_default_header(self, col_idx):
        i = 0
        while True:
            header = convert_idx_to_alphabet(col_idx + i)
            if header not in self.__upper_header_list:
                return header

            i += 1
