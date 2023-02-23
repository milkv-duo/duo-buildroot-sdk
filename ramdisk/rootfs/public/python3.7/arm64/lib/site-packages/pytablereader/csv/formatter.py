# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from pytablereader import InvalidDataError
from tabledata import TableData
import typepy

from ..formatter import TableFormatter


class CsvTableFormatter(TableFormatter):

    def to_table_data(self):
        if typepy.is_empty_sequence(self._loader.header_list):
            header_list = self._source_data[0]

            if any([
                    typepy.is_null_string(header) for header in header_list
            ]):
                raise InvalidDataError(
                    "the first line includes empty string item."
                    "all of the items should contain header name."
                    "actual={}".format(header_list))

            data_matrix = self._source_data[1:]
        else:
            header_list = self._loader.header_list
            data_matrix = self._source_data

        if not data_matrix:
            raise InvalidDataError(
                "data row must be greater or equal than one")

        self._loader.inc_table_count()

        yield TableData(
            self._loader.make_table_name(), header_list, data_matrix,
            quoting_flags=self._loader.quoting_flags)
