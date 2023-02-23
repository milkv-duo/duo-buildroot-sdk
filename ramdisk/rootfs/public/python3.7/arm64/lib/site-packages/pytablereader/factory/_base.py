# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc

import six

from .._constant import Default
from ..error import LoaderNotFoundError


@six.add_metaclass(abc.ABCMeta)
class BaseTableLoaderFactory(object):

    @property
    def source(self):
        """
        :return: Data source to load.
        :rtype: str
        """

        return self._source

    def __init__(self, source, encoding=Default.ENCODING):
        self._source = source

        if not encoding:
            self._encoding = Default.ENCODING
        else:
            self._encoding = encoding

    @abc.abstractmethod
    def create_from_path(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def create_from_format_name(self, format_name):  # pragma: no cover
        pass

    @abc.abstractmethod
    def _get_extension_loader_mapping(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def _get_format_name_loader_mapping(self):  # pragma: no cover
        pass

    def get_format_name_list(self):
        """
        :return: Available format names.
        :rtype: list
        """

        return sorted(self._get_format_name_loader_mapping())

    def get_extension_list(self):
        """
        :return: Available format file extensions.
        :rtype: list
        """

        return sorted(self._get_extension_loader_mapping())

    def _get_loader_class(self, loader_mapping, format_name):
        try:
            format_name = format_name.lower()
        except AttributeError:
            raise TypeError("format name must be a string")

        try:
            return loader_mapping[format_name]
        except KeyError:
            raise LoaderNotFoundError(", ".join([
                "loader not found: format='{}'".format(format_name),
                "source='{}'".format(self.source),
            ]))

    def _create_from_extension(self, extension):
        try:
            loader = self._get_loader_class(
                self._get_extension_loader_mapping(), extension)(self.source)
            loader.encoding = self._encoding

            return loader
        except LoaderNotFoundError as e:
            raise LoaderNotFoundError("\n".join([
                "{:s} (unknown extension).".format(e.args[0]),
                "",
                "acceptable extensions are: {}.".format(
                    ", ".join(self.get_extension_list())),
                "actual: '{}'".format(extension)
            ]))

    def _create_from_format_name(self, format_name):
        try:
            loader = self._get_loader_class(
                self._get_format_name_loader_mapping(),
                format_name)(self.source)
            loader.encoding = self._encoding

            return loader
        except LoaderNotFoundError as e:
            raise LoaderNotFoundError("\n".join([
                "{:s} (unknown format name).".format(e.args[0]),
                "acceptable format names are: {}.".format(
                    ", ".join(self.get_format_name_list())),
            ]))
