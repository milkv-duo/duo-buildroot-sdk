# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import csv
import io
import platform

from mbstrdecoder import MultiByteStrDecoder
from pytablereader import InvalidDataError
import six
import typepy

from .._constant import (
    Default,
    TableNameTemplate as tnt,
)
from .._logger import (
    FileSourceLogger,
    TextSourceLogger,
)
from .._validator import (
    FileValidator,
    TextValidator
)
from ..interface import TableLoader
from .formatter import CsvTableFormatter


class CsvTableLoader(TableLoader):
    """
    The abstract class of CSV table loaders.

    .. py:attribute:: header_list

        Attribute names of the table. Use the first line of
        the CSV file as attribute list if header_list is empty.

    .. py:attribute:: delimiter

        A one-character string used to separate fields.
        Defaults to ``","``.

    .. py:attribute:: quotechar

        A one-character string used to quote fields containing
        special characters, such as the ``delimiter`` or ``quotechar``,
        or which contain new-line characters.
        Defaults to ``'"'``.

    .. py:attribute:: encoding

        Encoding of the CSV data.
    """

    @property
    def format_name(self):
        return "csv"

    @property
    def delimiter(self):
        # "delimiter" must be a string, not an unicode
        return str(MultiByteStrDecoder(self.__delimiter).unicode_str)

    @delimiter.setter
    def delimiter(self, value):
        self.__delimiter = value

    @property
    def quotechar(self):
        # "quotechar" must be a string, not an unicode
        return str(MultiByteStrDecoder(self.__quotechar).unicode_str)

    @quotechar.setter
    def quotechar(self, value):
        self.__quotechar = value

    def __init__(self, source):
        super(CsvTableLoader, self).__init__(source)

        self._csv_reader = None

        self.header_list = ()
        self.delimiter = ","
        self.quotechar = '"'
        self.encoding = Default.ENCODING

    def _to_data_matrix(self):
        try:
            return [
                [self.__modify_item(data) for data in row]
                for row in self._csv_reader
                if typepy.is_not_empty_sequence(row)
            ]
        except csv.Error as e:
            raise InvalidDataError(e)

    @staticmethod
    def __modify_item(data):
        try:
            return typepy.type.Integer(data).convert()
        except typepy.TypeConversionError:
            pass

        try:
            return typepy.type.RealNumber(data).convert()
        except typepy.TypeConversionError:
            pass

        return MultiByteStrDecoder(data).unicode_str


class CsvTableFileLoader(CsvTableLoader):
    """
    A file loader class to extract tabular data from CSV files.

    :param str file_path: Path to the loading CSV file.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(filename)s``.

    :Examples:
        :ref:`example-csv-table-loader`
    """

    def __init__(self, file_path):
        super(CsvTableFileLoader, self).__init__(file_path)

        self._validator = FileValidator(file_path)
        self._logger = FileSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a CSV file.
        |load_source_desc_file|

        :return:
            Loaded table data.
            |load_table_name_desc|

            ===================  ========================================
            Format specifier     Value after the replacement
            ===================  ========================================
            ``%(filename)s``     |filename_desc|
            ``%(format_name)s``  ``"csv"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ========================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the CSV data is invalid.

        .. seealso::
            :py:func:`csv.reader`
        """

        self._validate()
        self._logger.logging_load()

        if all([platform.system() == "Windows", six.PY3]):
            self._csv_reader = csv.reader(
                io.open(self.source, "r", encoding=self.encoding),
                delimiter=self.delimiter, quotechar=self.quotechar,
                strict=True, skipinitialspace=True)
        else:
            self._csv_reader = csv.reader(
                open(self.source, "r"),
                delimiter=self.delimiter, quotechar=self.quotechar,
                strict=True, skipinitialspace=True)

        formatter = CsvTableFormatter(self._to_data_matrix())
        formatter.accept(self)

        return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return tnt.FILENAME


class CsvTableTextLoader(CsvTableLoader):
    """
    A text loader class to extract tabular data from CSV text data.

    :param str text: CSV text to load.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(format_name)s%(format_id)s``.

    :Examples:
        :ref:`example-csv-table-loader`
    """

    def __init__(self, text):
        super(CsvTableTextLoader, self).__init__(text)

        self._validator = TextValidator(text)
        self._logger = TextSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a CSV text object.
        |load_source_desc_text|

        :return:
            Loaded table data.
            |load_table_name_desc|

            ===================  ========================================
            Format specifier     Value after the replacement
            ===================  ========================================
            ``%(filename)s``     ``""``
            ``%(format_name)s``  ``"csv"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ========================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the CSV data is invalid.

        .. seealso::
            :py:func:`csv.reader`
        """

        self._validate()
        self._logger.logging_load()

        self._csv_reader = csv.reader(
            six.StringIO(self.source.strip()),
            delimiter=self.delimiter, quotechar=self.quotechar,
            strict=True, skipinitialspace=True)
        formatter = CsvTableFormatter(self._to_data_matrix())
        formatter.accept(self)

        return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return "{:s}{:s}".format(tnt.FORMAT_NAME, tnt.FORMAT_ID)
