# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import io

from pytablereader import (
    InvalidHeaderNameError,
    InvalidDataError
)
import typepy

import pathvalidate as pv

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
from ..json.formatter import SingleJsonTableConverterA


class LtsvTableLoader(TableLoader):
    """
    Abstract class of
    `Labeled Tab-separated Values (LTSV) <http://ltsv.org/>`__
    format table loaders.

    .. py:attribute:: encoding

        Encoding of the LTSV data.
    """

    @property
    def format_name(self):
        return "ltsv"

    def __init__(self, source):
        super(LtsvTableLoader, self).__init__(source)

        self._ltsv_input_stream = None

    def _to_data_matrix(self):
        from collections import OrderedDict

        data_matrix = []

        for row_idx, row in enumerate(self._ltsv_input_stream):
            if typepy.is_empty_sequence(row):
                continue

            ltsv_record = OrderedDict()
            for col_idx, ltsv_item in enumerate(row.strip().split("\t")):
                try:
                    label, value = ltsv_item.split(":")
                except ValueError:
                    raise InvalidDataError(
                        "invalid lstv item found: line={}, col={}, item='{}'".format(
                            row_idx, col_idx, ltsv_item))

                label = label.strip('"')

                try:
                    pv.validate_ltsv_label(label)
                except (pv.NullNameError, pv.InvalidCharError):
                    raise InvalidHeaderNameError(
                        "invalid label found (acceptable chars are [0-9A-Za-z_.-]): "
                        "line={}, col={}, label='{}'".format(
                            row_idx, col_idx, label))

                ltsv_record[label] = value

            data_matrix.append(ltsv_record)

        # using generator to prepare for future enhancement to support
        # iterative load.
        yield data_matrix


class LtsvTableFileLoader(LtsvTableLoader):
    """
    `Labeled Tab-separated Values (LTSV) <http://ltsv.org/>`__
    format file loader class.

    :param str file_path: Path to the loading LTSV file.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(filename)s``.
    """

    def __init__(self, file_path):
        super(LtsvTableFileLoader, self).__init__(file_path)

        self.encoding = Default.ENCODING

        self._validator = FileValidator(file_path)
        self._logger = FileSourceLogger(self)

        self.__file = None

    def load(self):
        """
        Extract tabular data as |TableData| instances from a LTSV file.
        |load_source_desc_file|

        :return:
            Loaded table data.
            |load_table_name_desc|

            ===================  ========================================
            Format specifier     Value after the replacement
            ===================  ========================================
            ``%(filename)s``     |filename_desc|
            ``%(format_name)s``  ``"ltsv"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ========================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidHeaderNameError:
            If an invalid label name is included in the LTSV file.
        :raises pytablereader.InvalidDataError:
            If the LTSV data is invalid.
        """

        self._validate()
        self._logger.logging_load()

        self._ltsv_input_stream = io.open(
            self.source, "r", encoding=self.encoding)

        for data_matrix in self._to_data_matrix():
            formatter = SingleJsonTableConverterA(data_matrix)
            formatter.accept(self)

            return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return tnt.FILENAME


class LtsvTableTextLoader(LtsvTableLoader):
    """
    `Labeled Tab-separated Values (LTSV) <http://ltsv.org/>`__
    format text loader class.

    :param str text: LTSV text to load.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(format_name)s%(format_id)s``.
    """

    def __init__(self, text):
        super(LtsvTableTextLoader, self).__init__(text)

        self._validator = TextValidator(text)
        self._logger = TextSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a LTSV text object.
        |load_source_desc_text|

        :return:
            Loaded table data.
            |load_table_name_desc|

            ===================  ========================================
            Format specifier     Value after the replacement
            ===================  ========================================
            ``%(filename)s``     ``""``
            ``%(format_name)s``  ``"ltsv"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ========================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidHeaderNameError:
            If an invalid label name is included in the LTSV file.
        :raises pytablereader.InvalidDataError:
            If the LTSV data is invalid.
        """

        self._validate()
        self._logger.logging_load()

        self._ltsv_input_stream = self.source.splitlines()

        for data_matrix in self._to_data_matrix():
            formatter = SingleJsonTableConverterA(data_matrix)
            formatter.accept(self)

            return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return "{:s}{:s}".format(tnt.FORMAT_NAME, tnt.FORMAT_ID)
