# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from ..interface import TableLoaderInterface


class TableLoaderManager(TableLoaderInterface):

    def __init__(self, loader):
        self.__loader = loader

    @property
    def format_name(self):
        return self.__loader.format_name

    @property
    def source_type(self):
        return self.__loader.source_type

    @property
    def encoding(self):
        try:
            return self.__loader.encoding
        except AttributeError:
            return None

    @encoding.setter
    def encoding(self, codec_name):
        self.__loader.encoding = codec_name

    def load(self):
        return self.__loader.load()

    def inc_table_count(self):
        self.__loader.inc_table_count()
