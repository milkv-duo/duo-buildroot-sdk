# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import io

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
from .formatter import HtmlTableFormatter


class HtmlTableLoader(TableLoader):
    """
    An abstract class of HTML table loaders.
    """

    @property
    def format_name(self):
        return "html"

    def _get_default_table_name_template(self):
        return "{:s}_{:s}".format(tnt.TITLE, tnt.KEY)


class HtmlTableFileLoader(HtmlTableLoader):
    """
    A file loader class to extract tabular data from HTML files.

    :param str file_path: Path to the loading HTML file.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(title)s_%(key)s``.

    .. py:attribute:: encoding

        HTML file encoding. Defaults to ``"utf-8"``.
    """

    def __init__(self, file_path=None):
        super(HtmlTableFileLoader, self).__init__(file_path)

        self.encoding = Default.ENCODING

        self._validator = FileValidator(file_path)
        self._logger = FileSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from HTML table tags in
        a HTML file.
        |load_source_desc_file|

        :return:
            Loaded table data iterator.
            |load_table_name_desc|

            ===================  ==============================================
            Format specifier     Value after the replacement
            ===================  ==============================================
            ``%(filename)s``     |filename_desc|
            ``%(title)s``        ``<title>`` tag value of the HTML.
            ``%(key)s``          | This replaced to:
                                 | **(1)** ``id`` attribute of the table tag
                                 | **(2)** ``%(format_name)s%(format_id)s``
                                 | if ``id`` attribute not present in the
                                 | table tag.
            ``%(format_name)s``  ``"html"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ==============================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the HTML data is invalid or empty.

        .. note::

            Table tag attributes ignored with loaded |TableData|.
        """

        self._validate()
        self._logger.logging_load()

        with io.open(self.source, "r", encoding=self.encoding) as fp:
            formatter = HtmlTableFormatter(fp.read())
        formatter.accept(self)

        return formatter.to_table_data()


class HtmlTableTextLoader(HtmlTableLoader):
    """
    A text loader class to extract tabular data from HTML text data.

    :param str text: HTML text to load.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(title)s_%(key)s``.
    """

    def __init__(self, text):
        super(HtmlTableTextLoader, self).__init__(text)

        self._validator = TextValidator(text)
        self._logger = TextSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from HTML table tags in
        a HTML text object.
        |load_source_desc_text|

        :return:
            Loaded table data iterator.
            |load_table_name_desc|

            ===================  ==============================================
            Format specifier     Value after the replacement
            ===================  ==============================================
            ``%(filename)s``     ``""``
            ``%(title)s``        ``<title>`` tag value of the HTML.
            ``%(key)s``          | This replaced to:
                                 | **(1)** ``id`` attribute of the table tag
                                 | **(2)** ``%(format_name)s%(format_id)s``
                                 | if ``id`` attribute is not included
                                 | in the table tag.
            ``%(format_name)s``  ``"html"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ==============================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the HTML data is invalid or empty.
        """

        self._validate()
        self._logger.logging_load()

        formatter = HtmlTableFormatter(self.source)
        formatter.accept(self)

        return formatter.to_table_data()
