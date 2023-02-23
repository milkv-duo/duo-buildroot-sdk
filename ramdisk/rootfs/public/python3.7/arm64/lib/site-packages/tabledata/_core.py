# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from collections import OrderedDict
from decimal import Decimal
import hashlib
import multiprocessing
import re
import warnings

import six
import typepy

import dataproperty as dp
from six.moves import zip

from ._constant import PatternMatch
from ._logger import logger
from .error import InvalidDataError


class TableData(object):
    """
    Class to represent a table data structure.

    :param str table_name: Name of the table.
    :param list header_list: Table header names.
    :param list record_list: Table data records.
    """

    @property
    def table_name(self):
        """
        :return: Name of the table.
        :rtype: str
        """

        return self.__table_name

    @property
    def header_list(self):
        """
        :return: Table header names.
        :rtype: list
        """

        return self.__dp_extractor.header_list

    @property
    def value_matrix(self):
        """
        :return: Table data records.
        :rtype: list
        """

        if self.__value_matrix:
            return self.__value_matrix

        self.__value_matrix = [
            [value_dp.data for value_dp in value_dp_list]
            for value_dp_list in self.__value_dp_matrix
        ]

        return self.__value_matrix

    @property
    def value_dp_matrix(self):
        """
        :return: DataProperty for table data.
        :rtype: list
        """

        return self.__value_dp_matrix

    @property
    def to_header_dp_list(self):
        return self.__dp_extractor.to_header_dp_list()

    @property
    def record_list(self):
        # alias property of value_matrix. this method will be deleted in the
        # future

        return self.value_matrix

    def __init__(
            self, table_name, header_list, record_list, is_strip_quote=False,
            quoting_flags=None, max_workers=None):

        self.__dp_extractor = dp.DataPropertyExtractor()
        if quoting_flags:
            self.__dp_extractor.quoting_flags = quoting_flags
        self.__dp_extractor.strip_str_header = '"'
        if is_strip_quote:
            self.__dp_extractor.strip_str_value = '"'

        if max_workers:
            self.max_workers = max_workers
        else:
            if six.PY2:
                # avoid unit test execution hang up at Python 2 environment
                self.max_workers = 1
                self.__dp_extractor.max_workers = 1
            else:
                self.max_workers = multiprocessing.cpu_count()

        self.__table_name = table_name
        self.__value_matrix = None

        if not header_list:
            self.__dp_extractor.header_list = []
        else:
            self.__dp_extractor.header_list = header_list

        if not record_list:
            self.__value_dp_matrix = []
        else:
            self.__value_dp_matrix = self.__dp_extractor.to_dp_matrix(
                self.__preprocess_value_matrix(record_list))

    def __repr__(self):
        element_list = [
            "table_name={}".format(self.table_name),
        ]

        try:
            element_list.append("header_list=[{}]".format(
                ", ".join(self.header_list)))
        except TypeError:
            element_list.append("header_list=None")

        element_list.append("rows={}".format(len(self.value_dp_matrix)))

        return ", ".join(element_list)

    def __eq__(self, other):
        return all([
            self.table_name == other.table_name,
            self.header_list == other.header_list,
            all([
                all([
                    lhs == rhs
                    for lhs, rhs in zip(lhs_list, rhs_list)
                ])
                for lhs_list, rhs_list
                in zip(self.value_dp_matrix, other.value_dp_matrix)
            ]),
        ])

    def __ne__(self, other):
        return any([
            self.table_name != other.table_name,
            self.header_list != other.header_list,
            any([
                any([
                    lhs != rhs
                    for lhs, rhs in zip(lhs_list, rhs_list)
                ])
                for lhs_list, rhs_list
                in zip(self.value_dp_matrix, other.value_dp_matrix)
            ]),
        ])

    def __hash__(self):
        body = (
            self.table_name +
            six.text_type(self.header_list) +
            six.text_type(self.value_dp_matrix)
        )
        return hashlib.sha1(body.encode("utf-8")).hexdigest()

    def is_empty_header(self):
        """
        :return: |True| if the data :py:attr:`.header_list` is empty.
        :rtype: bool
        """

        return typepy.is_empty_sequence(self.header_list)

    def is_empty_record(self):
        """
        :return: |True| if the tabular data is not an empty nested list.
        :rtype: bool
        """

        try:
            return not typepy.is_not_empty_sequence(self.value_dp_matrix[0])
        except (TypeError, IndexError):
            return True

    def is_empty(self):
        """
        :return:
            |True| if the data :py:attr:`.header_list` or
            :py:attr:`.value_matrix` is empty.
        :rtype: bool
        """

        return any([self.is_empty_header(), self.is_empty_record()])

    def as_dict(self):
        """
        :return: Table data as a |dict| instance.
        :rtype: dict

        :Sample Code:
            .. code:: python

                from tabledata import TableData

                TableData(
                    table_name="sample",
                    header_list=["a", "b"],
                    record_list=[[1, 2], [3.3, 4.4]]
                ).as_dict()

        :Output:
            .. code:: json

                {'sample': [OrderedDict([('a', 1), ('b', 2)]),
                  OrderedDict([('a', 3.3), ('b', 4.4)])]}
        """

        from typepy import Typecode

        self.__dp_extractor.float_type = float

        dict_body = []
        for value_dp_list in self.value_dp_matrix:
            if typepy.is_empty_sequence(value_dp_list):
                continue

            dict_record = [
                (header, value_dp.data)
                for header, value_dp in zip(self.header_list, value_dp_list)
                if value_dp.typecode != Typecode.NONE
            ]

            if typepy.is_empty_sequence(dict_record):
                continue

            dict_body.append(OrderedDict(dict_record))

        return {self.table_name: dict_body}

    def asdict(self):
        warnings.warn(
            "asdict() will be deleted in the future, use as_dict instead.",
            DeprecationWarning)

        return self.as_dict()

    def as_dataframe(self):
        """
        :return: Table data as a ``pandas.DataFrame`` instance.
        :rtype: pandas.DataFrame

        :Sample Code:
            .. code-block:: python

                from tabledata import TableData

                TableData(
                    table_name="sample",
                    header_list=["a", "b"],
                    record_list=[[1, 2], [3.3, 4.4]]
                ).as_dict()

        :Output:
            .. code-block:: none

                     a    b
                0    1    2
                1  3.3  4.4

        :Dependency Packages:
            - `pandas <http://pandas.pydata.org/>`__
        """

        import pandas

        dataframe = pandas.DataFrame(self.value_matrix)
        if not self.is_empty_header():
            dataframe.columns = self.header_list

        return dataframe

    def filter_column(
            self, pattern_list=None, is_invert_match=False,
            is_re_match=False, pattern_match=PatternMatch.OR):
        logger.debug(
            "filter_column: pattern_list={}, is_invert_match={}, "
            "is_re_match={}, pattern_match={}".format(
                pattern_list, is_invert_match, is_re_match, pattern_match))

        if not pattern_list:
            return TableData(
                table_name=self.table_name, header_list=self.header_list,
                record_list=self.value_dp_matrix)

        match_header_list = []
        match_column_matrix = []

        if pattern_match == PatternMatch.OR:
            match_method = any
        elif pattern_match == PatternMatch.AND:
            match_method = all
        else:
            raise ValueError("unknown matching: {}".format(pattern_match))

        for header, column_value_dp_list in zip(
                self.header_list, zip(*self.value_dp_matrix)):
            is_match_list = []
            for pattern in pattern_list:
                is_match = self.__is_match(header, pattern, is_re_match)

                is_match_list.append(any([
                    is_match and not is_invert_match,
                    not is_match and is_invert_match,
                ]))

            if match_method(is_match_list):
                match_header_list.append(header)
                match_column_matrix.append(column_value_dp_list)

        logger.debug(
            "filter_column: table={}, match_header_list={}".format(
                self.table_name, match_header_list))

        return TableData(
            table_name=self.table_name, header_list=match_header_list,
            record_list=list(zip(*match_column_matrix)))

    @staticmethod
    def from_dataframe(dataframe, table_name=""):
        """
        Initialize TableData instance from a pandas.DataFrame instance.

        :param pandas.DataFrame dataframe:
        :param str table_name: Table name to create.
        """

        return TableData(
            table_name=table_name,
            header_list=list(dataframe.columns.values),
            record_list=dataframe.values.tolist())

    @staticmethod
    def __is_match(header, pattern, is_re_match):
        if is_re_match:
            return re.search(pattern, header) is not None

        return header == pattern

    def __to_record(self, values):
        """
        Convert values to a record.

        :param values: Value to be converted.
        :type values: |dict|/|namedtuple|/|list|/|tuple|
        :raises ValueError: If the ``value`` is invalid.
        """

        try:
            # dictionary to list
            return [
                dp.data
                for dp in self.__dp_extractor.to_dp_list([
                    values.get(header) for header in self.header_list])
            ]
        except AttributeError:
            pass

        try:
            # namedtuple to list
            dict_value = values._asdict()
            return [
                dp.data
                for dp in self.__dp_extractor.to_dp_list([
                    dict_value.get(header) for header in self.header_list])
            ]
        except AttributeError:
            pass

        try:
            return [
                dp.data
                for dp in self.__dp_extractor.to_dp_list(values)
            ]
        except TypeError:
            raise InvalidDataError(
                "record must be a list or tuple: actual={}".format(values))

    def __preprocess_value_matrix(self, value_matrix):
        return [
            _preprocess_value_list(
                self.header_list, value_list, record_idx)[1]
            for record_idx, value_list in enumerate(value_matrix)
        ]

    def __to_value_matrix(self, value_matrix):
        """
        Convert matrix to records
        """

        self.__dp_extractor.float_type = Decimal

        if typepy.is_empty_sequence(self.header_list):
            return value_matrix

        if self.max_workers <= 1:
            return self.__to_value_matrix_st(value_matrix)

        return self.__to_value_matrix_mt(value_matrix)

    def __to_value_matrix_st(self, value_matrix):
        return [
            _to_record_helper(
                self.__dp_extractor, self.header_list, value_list,
                record_idx)[1]
            for record_idx, value_list in enumerate(value_matrix)
        ]

    def __to_value_matrix_mt(self, value_matrix):
        from concurrent import futures

        record_mapping = {}
        try:
            with futures.ProcessPoolExecutor(self.max_workers) as executor:
                future_list = [
                    executor.submit(
                        _to_record_helper, self.__dp_extractor,
                        self.header_list, value_list, record_idx)
                    for record_idx, value_list in enumerate(value_matrix)
                ]

                for future in futures.as_completed(future_list):
                    record_idx, record = future.result()
                    record_mapping[record_idx] = record
        finally:
            logger.debug("shutdown ProcessPoolExecutor")
            executor.shutdown()

        return [
            record_mapping[record_idx] for record_idx in sorted(record_mapping)
        ]


def _preprocess_value_list(header_list, values, record_idx):
    if header_list:
        try:
            # dictionary to list
            return (
                record_idx,
                [values.get(header) for header in header_list])
        except (TypeError, AttributeError):
            pass

        try:
            # namedtuple to list
            dict_value = values._asdict()
            return (
                record_idx,
                [dict_value.get(header) for header in header_list])
        except (TypeError, AttributeError):
            pass

    if not isinstance(values, (tuple, list)):
        raise InvalidDataError(
            "record must be a list or tuple: actual={}".format(values))

    return (record_idx, values)


def _to_record_helper(extractor, header_list, values, record_idx):
    try:
        # dictionary to list
        return (
            record_idx,
            [
                dp.data
                for dp in extractor.to_dp_list([
                    values.get(header) for header in header_list])
            ])
    except AttributeError:
        pass

    try:
        # namedtuple to list
        dict_value = values._asdict()
        return (
            record_idx,
            [
                dp.data
                for dp in extractor.to_dp_list([
                    dict_value.get(header) for header in header_list])
            ])
    except AttributeError:
        pass

    try:
        return (
            record_idx,
            [
                dp.data
                for dp in extractor.to_dp_list(values)
            ])
    except TypeError:
        raise InvalidDataError(
            "record must be a list or tuple: actual={}".format(values))
