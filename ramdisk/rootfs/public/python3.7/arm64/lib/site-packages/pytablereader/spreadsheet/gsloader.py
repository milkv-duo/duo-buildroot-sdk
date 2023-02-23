# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from tabledata import TableData
import typepy

from .._constant import TableNameTemplate as tnt
from .._validator import TextValidator
from ..error import OpenError
from .core import SpreadSheetLoader


class GoogleSheetsTableLoader(SpreadSheetLoader):
    """
    Concrete class of Google Spreadsheet loader.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(sheet)s``.

    :param str file_path: Path to the Google Sheets credential JSON file.

    :Dependency Packages:
        - `gspread <https://github.com/burnash/gspread>`_
        - `SimpleSQLite <https://github.com/thombashi/SimpleSQLite>`_
        - `oauth2client <https://pypi.python.org/pypi/oauth2client>`_
        - `pyOpenSSL <https://pypi.python.org/pypi/pyOpenSSL>`_

    :Examples:
        :ref:`example-gs-table-loader`
    """

    @property
    def _sheet_name(self):
        return self._worksheet.title

    @property
    def _row_count(self):
        return self._worksheet.row_count

    @property
    def _col_count(self):
        return self._worksheet.col_count

    def __init__(self, file_path=None):
        super(GoogleSheetsTableLoader, self).__init__(file_path)

        self.title = None
        self.start_row = 0

        self._validator = TextValidator(file_path)

        self.__all_values = None

    def load(self):
        """
        Load table data from a Google Spreadsheet.

        This method consider :py:attr:`.source` as a path to the
        credential JSON file to access Google Sheets API.

        The method automatically search the header row start from
        :py:attr:`.start_row`. The condition of the header row is that
        all of the columns have value (except empty columns).

        :return:
            Loaded table data. Return one |TableData| for each sheet in
            the workbook. The table name for data will be determined by
            :py:meth:`~.GoogleSheetsTableLoader.make_table_name`.
        :rtype: iterator of |TableData|
        :raises pytablereader.InvalidDataError:
            If the header row is not found.
        :raises pytablereader.OpenError:
            If the spread sheet not found.
        """

        import gspread
        from oauth2client.service_account import ServiceAccountCredentials

        self._validate_table_name()
        self._validate_title()

        scope = ['https://spreadsheets.google.com/feeds']
        credentials = ServiceAccountCredentials.from_json_keyfile_name(
            self.source, scope)

        gc = gspread.authorize(credentials)
        try:
            for worksheet in gc.open(self.title).worksheets():
                self._worksheet = worksheet
                self.__all_values = [row for row in worksheet.get_all_values()]

                if self._is_empty_sheet():
                    continue

                try:
                    self.__strip_empty_col()
                except ValueError:
                    continue

                value_matrix = self.__all_values[self._get_start_row_idx():]
                try:
                    header_list = value_matrix[0]
                    record_list = value_matrix[1:]
                except IndexError:
                    continue

                self.inc_table_count()

                yield TableData(
                    self.make_table_name(), header_list, record_list,
                    quoting_flags=self.quoting_flags)
        except gspread.exceptions.SpreadsheetNotFound:
            raise OpenError("spreadsheet '{}' not found".format(self.title))

    def _is_empty_sheet(self):
        return len(self.__all_values) <= 1

    def _get_start_row_idx(self):
        row_idx = 0
        for row_value_list in self.__all_values:
            if all([
                    typepy.is_not_null_string(value)
                    for value in row_value_list
            ]):
                break

            row_idx += 1

        return self.start_row + row_idx

    def _validate_title(self):
        if typepy.is_null_string(self.title):
            raise ValueError("spreadsheet title is empty")

    def _make_table_name(self):
        self._validate_title()

        kv_mapping = self._get_basic_tablename_keyvalue_mapping()
        kv_mapping[tnt.TITLE] = self.title
        try:
            kv_mapping[tnt.SHEET] = self._sheet_name
        except AttributeError:
            kv_mapping[tnt.SHEET] = ""

        return self._expand_table_name_format(kv_mapping)

    def __strip_empty_col(self):
        from simplesqlite import connect_sqlite_memdb
        from simplesqlite.sqlquery import SqlQuery

        con = connect_sqlite_memdb()

        tmp_table_name = "tmp"
        header_list = [
            "a{:d}".format(i)
            for i in range(len(self.__all_values[0]))
        ]
        con.create_table_from_data_matrix(
            table_name=tmp_table_name,
            attr_name_list=header_list,
            data_matrix=self.__all_values)
        for col_idx, header in enumerate(header_list):
            result = con.select(
                select=SqlQuery.to_attr_str(header), table_name=tmp_table_name)
            if any([
                    typepy.is_not_null_string(record[0])
                    for record in result.fetchall()
            ]):
                break

        strip_header_list = header_list[col_idx:]
        if typepy.is_empty_sequence(strip_header_list):
            raise ValueError()

        result = con.select(
            select=",".join(SqlQuery.to_attr_str_list(strip_header_list)),
            table_name=tmp_table_name)
        self.__all_values = result.fetchall()
