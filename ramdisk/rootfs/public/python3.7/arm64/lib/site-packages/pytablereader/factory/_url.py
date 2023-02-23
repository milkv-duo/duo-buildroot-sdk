# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import os
import tempfile

import requests
import typepy

from six.moves.urllib.parse import urlparse

from .._common import (
    get_extension,
    make_temp_file_path_from_url,
)
from .._constant import SourceType
from .._logger import logger
from .._validator import UrlValidator
from ..csv.core import CsvTableTextLoader
from ..error import (
    InvalidFilePathError,
    InvalidUrlError,
    HTTPError,
    ProxyError,
)
from ..html.core import HtmlTableTextLoader
from ..json.core import JsonTableTextLoader
from ..ltsv.core import LtsvTableTextLoader
from ..markdown.core import MarkdownTableTextLoader
from ..mediawiki.core import MediaWikiTableTextLoader
from ..spreadsheet.excelloader import ExcelTableFileLoader
from ..sqlite.core import SqliteFileLoader
from ..tsv.core import TsvTableTextLoader
from ._base import BaseTableLoaderFactory


class TableUrlLoaderFactory(BaseTableLoaderFactory):

    def __init__(self, url, encoding=None, proxies=None):
        super(TableUrlLoaderFactory, self).__init__(None)

        self.__url = url
        self.__proxies = proxies
        self.__temp_dir_path = None

        self._encoding = encoding

        UrlValidator(url).validate()

    def __del__(self):
        if typepy.is_null_string(self.__temp_dir_path):
            return

        os.removedirs(self.__temp_dir_path)
        self.__temp_dir_path = None

    def create_from_path(self):
        """
        Create a file loader from the file extension to loading file.
        Supported file extensions are as follows:

            =========================================  =====================================
            Format name                                 Loader
            =========================================  =====================================
            ``"csv"``                                  :py:class:`~.CsvTableTextLoader`
            ``"xls"``/``"xlsx"``                       :py:class:`~.ExcelTableFileLoader`
            ``"htm"``/``"html"``/``"asp"``/``"aspx"``  :py:class:`~.HtmlTableTextLoader`
            ``"json"``                                 :py:class:`~.JsonTableTextLoader`
            ``"ltsv"``                                 :py:class:`~.LtsvTableTextLoader`
            ``"md"``                                   :py:class:`~.MarkdownTableTextLoader`
            ``"sqlite"``/``"sqlite3"``                 :py:class:`~.SqliteFileLoader`
            ``"tsv"``                                  :py:class:`~.TsvTableTextLoader`
            =========================================  =====================================

        :return:
            Loader that coincides with the file extension of the URL.
        :raises pytablereader.InvalidUrlError: If unacceptable URL format.
        :raises pytablereader.LoaderNotFoundError:
            |LoaderNotFoundError_desc| loading the URL.
        """

        url_path = urlparse(self.__url).path
        try:
            url_extension = get_extension(url_path.rstrip("/"))
        except InvalidFilePathError:
            raise InvalidUrlError("url must include path")

        logger.debug(
            "TableUrlLoaderFactory.create_from_path: extension={}".format(
                url_extension))

        loader_class = self._get_loader_class(
            self._get_extension_loader_mapping(), url_extension)

        try:
            self._fetch_source(loader_class)
        except requests.exceptions.ProxyError as e:
            raise ProxyError(e)

        return self._create_from_extension(url_extension)

    def create_from_format_name(self, format_name):
        """
        Create a file loader from a format name.
        Supported file formats are as follows:

            ==========================  ======================================
            Format name                 Loader
            ==========================  ======================================
            ``"csv"``                   :py:class:`~.CsvTableTextLoader`
            ``"excel"``                 :py:class:`~.ExcelTableFileLoader`
            ``"html"``                  :py:class:`~.HtmlTableTextLoader`
            ``"json"``                  :py:class:`~.JsonTableTextLoader`
            ``"ltsv"``                  :py:class:`~.LtsvTableTextLoader`
            ``"markdown"``              :py:class:`~.MarkdownTableTextLoader`
            ``"mediawiki"``             :py:class:`~.MediaWikiTableTextLoader`
            ``"sqlite"``                :py:class:`~.SqliteFileLoader`
            ``"tsv"``                   :py:class:`~.TsvTableTextLoader`
            ==========================  ======================================

        :param str format_name: Format name string (case insensitive).
        :return: Loader that coincide with the ``format_name``:
        :raises pytablereader.LoaderNotFoundError:
            |LoaderNotFoundError_desc| the format.
        :raises TypeError: If ``format_name`` is not a string.
        """

        logger.debug(
            "TableUrlLoaderFactory.create_from_format_name: name={}".format(
                format_name))

        loader_class = self._get_loader_class(
            self._get_format_name_loader_mapping(), format_name)

        try:
            self._fetch_source(loader_class)
        except requests.exceptions.ProxyError as e:
            raise ProxyError(e)

        return self._create_from_format_name(format_name)

    def _fetch_source(self, loader_class):
        loader_source_type = loader_class("").source_type

        if loader_source_type not in [SourceType.TEXT, SourceType.FILE]:
            raise ValueError(
                "unknown loader source: type={}".format(loader_source_type))

        r = requests.get(self.__url, proxies=self.__proxies)

        try:
            r.raise_for_status()
        except requests.HTTPError as e:
            raise HTTPError(e)

        if typepy.is_null_string(self._encoding):
            self._encoding = r.encoding

        logger.debug("\n".join([
            "_fetch_source: ",
            "  source-type={}".format(loader_source_type),
            "  content-type={}".format(r.headers["Content-Type"]),
            "  encoding={}".format(self._encoding),
            "  status-code={}".format(r.status_code),
        ]))

        if loader_source_type == SourceType.TEXT:
            self._source = r.text
        elif loader_source_type == SourceType.FILE:
            self.__temp_dir_path = tempfile.mkdtemp()
            self._source = "{:s}.xlsx".format(
                make_temp_file_path_from_url(self.__temp_dir_path, self.__url))
            with open(self._source, "wb") as f:
                f.write(r.content)

    def _get_common_loader_mapping(self):
        return {
            "csv": CsvTableTextLoader,
            "html": HtmlTableTextLoader,
            "json": JsonTableTextLoader,
            "ltsv": LtsvTableTextLoader,
            "sqlite": SqliteFileLoader,
            "tsv": TsvTableTextLoader,
        }

    def _get_extension_loader_mapping(self):
        """
        :return: Mappings of format-extension and loader class.
        :rtype: dict
        """

        loader_table = self._get_common_loader_mapping()
        loader_table.update({
            "asp": HtmlTableTextLoader,
            "aspx": HtmlTableTextLoader,
            "htm": HtmlTableTextLoader,
            "md": MarkdownTableTextLoader,
            "sqlite3": SqliteFileLoader,
            "xls": ExcelTableFileLoader,
            "xlsx": ExcelTableFileLoader,
        })

        return loader_table

    def _get_format_name_loader_mapping(self):
        """
        :return: Mappings of format-name and loader class.
        :rtype: dict
        """

        loader_table = self._get_common_loader_mapping()
        loader_table.update({
            "excel": ExcelTableFileLoader,
            "markdown": MarkdownTableTextLoader,
            "mediawiki": MediaWikiTableTextLoader,
        })

        return loader_table
