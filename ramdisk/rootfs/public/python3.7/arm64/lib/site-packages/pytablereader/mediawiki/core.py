# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import io

from .._constant import (
    Default,
    SourceType,
    TableNameTemplate as tnt
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
from .formatter import MediaWikiTableFormatter


class MediaWikiTableLoader(TableLoader):
    """
    The abstract class of MediaWiki table loaders.
    """

    @property
    def format_name(self):
        return "mediawiki"


class MediaWikiTableFileLoader(MediaWikiTableLoader):
    """
    A file loader class to extract tabular data from MediaWiki files.

    :param str file_path: Path to the loading file.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(filename)s_%(key)s``.
    """

    def __init__(self, file_path=None):
        super(MediaWikiTableFileLoader, self).__init__(file_path)

        self.encoding = Default.ENCODING

        self._validator = FileValidator(file_path)
        self._logger = FileSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a MediaWiki file.
        |load_source_desc_file|

        :return:
            Loaded table data iterator.
            |load_table_name_desc|

            ===================  ==============================================
            Format specifier     Value after the replacement
            ===================  ==============================================
            ``%(filename)s``     |filename_desc|
            ``%(key)s``          | This replaced to:
                                 | **(1)** ``caption`` mark of the table
                                 | **(2)** ``%(format_name)s%(format_id)s``
                                 | if ``caption`` mark not included
                                 | in the table.
            ``%(format_name)s``  ``"mediawiki"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ==============================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the MediaWiki data is invalid or empty.
        """

        self._validate()
        self._logger.logging_load()

        with io.open(self.source, "r", encoding=self.encoding) as fp:
            formatter = MediaWikiTableFormatter(fp.read())
        formatter.accept(self)

        return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return "{:s}_{:s}".format(tnt.FILENAME, tnt.KEY)


class MediaWikiTableTextLoader(MediaWikiTableLoader):
    """
    A text loader class to extract tabular data from MediaWiki text data.

    :param str text: MediaWiki text to load.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(key)s``.
    """

    @property
    def source_type(self):
        return SourceType.TEXT

    def __init__(self, text):
        super(MediaWikiTableTextLoader, self).__init__(text)

        self._validator = TextValidator(text)
        self._logger = TextSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a MediaWiki text
        object.
        |load_source_desc_text|

        :return:
            Loaded table data iterator.
            |load_table_name_desc|

            ===================  ==============================================
            Format specifier     Value after the replacement
            ===================  ==============================================
            ``%(filename)s``     ``""``
            ``%(key)s``          | This replaced to:
                                 | **(1)** ``caption`` mark of the table
                                 | **(2)** ``%(format_name)s%(format_id)s``
                                 | if ``caption`` mark not included
                                 | in the table.
            ``%(format_name)s``  ``"mediawiki"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ==============================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the MediaWiki data is invalid or empty.
        """

        self._validate()
        self._logger.logging_load()

        formatter = MediaWikiTableFormatter(self.source)
        formatter.accept(self)

        return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return "{:s}".format(tnt.KEY)
