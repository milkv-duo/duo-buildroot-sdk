# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import re

import bs4
from pytablereader import InvalidDataError
from tabledata import TableData
import typepy

from .._constant import TableNameTemplate as tnt
from ..formatter import TableFormatter


class HtmlTableFormatter(TableFormatter):

    @property
    def table_id(self):
        return self.__table_id

    def __init__(self, source_data):
        super(HtmlTableFormatter, self).__init__(source_data)

        self.__table_id = None

        if typepy.is_null_string(source_data):
            raise InvalidDataError

        try:
            self.__soup = bs4.BeautifulSoup(self._source_data, "lxml")
        except bs4.FeatureNotFound:
            self.__soup = bs4.BeautifulSoup(self._source_data, "html.parser")

    def to_table_data(self):
        for table in self.__soup.find_all("table"):
            try:
                table_data = self.__parse_html(table)
            except ValueError:
                continue

            if table_data.is_empty_record():
                continue

            yield table_data

    def _make_table_name(self):
        from collections import OrderedDict

        key = self.table_id
        if typepy.is_null_string(key):
            key = self._loader.get_format_key()

        try:
            title = self.__soup.title.text
        except AttributeError:
            title = ""

        kv_mapping = self._loader._get_basic_tablename_keyvalue_mapping()
        kv_mapping.update(OrderedDict([
            (tnt.KEY, key),
            (tnt.TITLE, title),
        ]))

        return self._loader._expand_table_name_format(kv_mapping)

    def __parse_tag_id(self, table):
        self.__table_id = table.get("id")

        if self.__table_id is None:
            caption = table.find("caption")
            if caption is not None:
                caption = caption.text.strip()
                if typepy.is_not_null_string(caption):
                    self.__table_id = caption

    def __parse_html(self, table):
        header_list = []
        data_matrix = []

        self.__parse_tag_id(table)

        row_list = table.find_all("tr")
        re_table_val = re.compile("td|th")
        for row in row_list:
            td_list = row.find_all("td")
            if typepy.is_empty_sequence(td_list):
                if typepy.is_not_empty_sequence(header_list):
                    continue

                th_list = row.find_all("th")
                if typepy.is_empty_sequence(th_list):
                    continue

                header_list = [row.text.strip() for row in th_list]
                continue

            data_matrix.append([
                value.get_text().strip()
                for value in row.find_all(re_table_val)
            ])

        if typepy.is_empty_sequence(data_matrix):
            raise ValueError("data matrix is empty")

        self._loader.inc_table_count()

        return TableData(
            self._make_table_name(), header_list, data_matrix,
            quoting_flags=self._loader.quoting_flags)
