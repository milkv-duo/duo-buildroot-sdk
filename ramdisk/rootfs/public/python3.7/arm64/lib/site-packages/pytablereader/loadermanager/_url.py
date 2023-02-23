# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

import typepy

from ..factory import TableUrlLoaderFactory
from ._base import TableLoaderManager


class TableUrlLoader(TableLoaderManager):
    """
    Loader class to loading tables from URL.

    :param str url: URL to load.
    :param str format_name: Data format name to load.
        Supported formats are:
        ``"csv"``, ``"excel"``, ``"html"``, ``"json"``, ``"ltsv"``,
        ``"markdown"``, ``"mediawiki"``, ``"sqlite"``, ``"tsv"``.
        If the value is |None|, automatically detect file format from
        the ``url``.
    :param dict proxies: http/https proxy information.

        .. seealso::
            - `requests proxies <http://requests-docs-ja.readthedocs.io/en/latest/user/advanced/#proxies>`__

    :raises pytablereader.LoaderNotFoundError:
        |LoaderNotFoundError_desc| loading the URL.
    :raises pytablereader.HTTPError:
        If loader received an HTTP error when access to the URL.

    :Example:
        :ref:`example-url-table-loader`

    .. py:method:: load

        Load tables from URL as ``format_name`` format.

        :return: Loaded table data iterator.
        :rtype: |TableData| iterator

        .. seealso::
            * :py:meth:`pytablereader.factory.TableUrlLoaderFactory.create_from_format_name`
            * :py:meth:`pytablereader.factory.TableUrlLoaderFactory.create_from_path`
    """

    def __init__(self, url, format_name=None, encoding=None, proxies=None):
        loader_factory = TableUrlLoaderFactory(url, encoding, proxies)

        if typepy.is_not_null_string(format_name):
            loader = loader_factory.create_from_format_name(format_name)
        else:
            loader = loader_factory.create_from_path()

        super(TableUrlLoader, self).__init__(loader)

    @classmethod
    def get_format_name_list(cls):
        """
        :return:
            Available format names. These names can use by
            :py:class:`.TableUrlLoader` class constructor.
        :rtype: list

        :Example:
            .. code:: python

                >>> pytablereader.TableUrlLoaderFactory.get_format_name_list()
                ['csv', 'excel', 'html', 'json', 'ltsv', 'markdown', 'mediawiki', 'sqlite', 'tsv']
        """

        return TableUrlLoaderFactory("http://dummy.com/").get_format_name_list()
