# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from .._common import get_extension
from .._logger import logger
from ..csv.core import CsvTableFileLoader
from ..html.core import HtmlTableFileLoader
from ..json.core import JsonTableFileLoader
from ..ltsv.core import LtsvTableFileLoader
from ..markdown.core import MarkdownTableFileLoader
from ..mediawiki.core import MediaWikiTableFileLoader
from ..spreadsheet.excelloader import ExcelTableFileLoader
from ..sqlite.core import SqliteFileLoader
from ..tsv.core import TsvTableFileLoader
from ._base import BaseTableLoaderFactory


class TableFileLoaderFactory(BaseTableLoaderFactory):
    """
    :param str file_path: Path to the loading file.
    :raises pytablereader.InvalidFilePathError:
        If the ``file_path`` is an empty path.
    """

    @property
    def file_extension(self):
        """
        :return: File extension of the :py:attr:`.source` (without period).
        :rtype: str
        """

        return get_extension(self.source)

    def create_from_path(self):
        """
        Create a file loader from the file extension to loading file.
        Supported file extensions are as follows:

            ==========================  =====================================
            Format name                 Loader
            ==========================  =====================================
            ``"csv"``                   :py:class:`~.CsvTableFileLoader`
            ``"xls"``/``"xlsx"``        :py:class:`~.ExcelTableFileLoader`
            ``"htm"``/``"html"``        :py:class:`~.HtmlTableFileLoader`
            ``"json"``                  :py:class:`~.JsonTableFileLoader`
            ``"ltsv"``                  :py:class:`~.LtsvTableFileLoader`
            ``"md"``                    :py:class:`~.MarkdownTableFileLoader`
            ``"sqlite"``/``"sqlite3"``  :py:class:`~.SqliteFileLoader`
            ``"tsv"``                   :py:class:`~.TsvTableFileLoader`
            ==========================  =====================================

        :return:
            Loader that coincides with the file extension of the
            :py:attr:`.file_extension`.
        :raises pytablereader.LoaderNotFoundError:
            |LoaderNotFoundError_desc| loading the file.
        """

        logger.debug(
            "TableFileLoaderFactory.create_from_path: extension={}".format(
                self.file_extension))

        return self._create_from_extension(self.file_extension)

    def create_from_format_name(self, format_name):
        """
        Create a file loader from a format name.
        Supported file formats are as follows:

            ===============  ======================================
            Format name               Loader
            ===============  ======================================
            ``"csv"``        :py:class:`~.CsvTableFileLoader`
            ``"excel"``      :py:class:`~.ExcelTableFileLoader`
            ``"html"``       :py:class:`~.HtmlTableFileLoader`
            ``"json"``       :py:class:`~.JsonTableFileLoader`
            ``"ltsv"``       :py:class:`~.LtsvTableFileLoader`
            ``"markdown"``   :py:class:`~.MarkdownTableFileLoader`
            ``"mediawiki"``  :py:class:`~.MediaWikiTableFileLoader`
            ``"sqlite"``     :py:class:`~.SqliteFileLoader`
            ``"tsv"``        :py:class:`~.TsvTableFileLoader`
            ===============  ======================================

        :param str format_name: Format name string (case insensitive).
        :return: Loader that coincides with the ``format_name``:
        :raises pytablereader.LoaderNotFoundError:
            |LoaderNotFoundError_desc| the format.
        """

        logger.debug(
            "TableFileLoaderFactory.create_from_format_name: name={}".format(
                self.file_extension))

        return self._create_from_format_name(format_name)

    @staticmethod
    def _get_common_loader_mapping():
        return {
            "csv": CsvTableFileLoader,
            "html": HtmlTableFileLoader,
            "json": JsonTableFileLoader,
            "ltsv": LtsvTableFileLoader,
            "sqlite": SqliteFileLoader,
            "tsv": TsvTableFileLoader,
        }

    def _get_extension_loader_mapping(self):
        """
        :return: Mappings of format extension and loader class.
        :rtype: dict
        """

        loader_table = self._get_common_loader_mapping()
        loader_table.update({
            "htm": HtmlTableFileLoader,
            "md": MarkdownTableFileLoader,
            "sqlite3": SqliteFileLoader,
            "xlsx": ExcelTableFileLoader,
            "xls": ExcelTableFileLoader,
        })

        return loader_table

    def _get_format_name_loader_mapping(self):
        """
        :return: Mappings of format name and loader class.
        :rtype: dict
        """

        loader_table = self._get_common_loader_mapping()
        loader_table.update({
            "excel": ExcelTableFileLoader,
            "markdown": MarkdownTableFileLoader,
            "mediawiki": MediaWikiTableFileLoader,
        })

        return loader_table
