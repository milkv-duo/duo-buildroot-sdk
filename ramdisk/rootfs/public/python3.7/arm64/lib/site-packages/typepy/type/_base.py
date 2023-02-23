# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc

from .._error import TypeConversionError
from ..checker._interface import TypeCheckerInterface
from ..converter import ValueConverterInterface


class AbstractType(TypeCheckerInterface, ValueConverterInterface):
    __slots__ = (
        "_data", "_strict_level", "_params",
        "__checker", "__converter",
        "__is_type_result",
    )

    @abc.abstractproperty
    def typecode(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def _create_type_checker(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def _create_type_converter(self):  # pragma: no cover
        pass

    @property
    def typename(self):
        return self.typecode.name

    def __init__(self, value, strict_level, params):
        self._data = value
        self._strict_level = strict_level

        if not params:
            self._params = {}
        else:
            self._params = params

        self.__checker = self._create_type_checker()
        self.__converter = self._create_type_converter()

        self.__is_type_result = None

    def __repr__(self):
        return ", ".join([
            "value={}".format(self._data),
            "typename={}".format(self.typename),
            "strict_level={}".format(self._strict_level),
            "is_type={}".format(self.is_type()),
            "try_convert={}".format(self.try_convert()),
        ])

    def is_type(self):
        """
        :return:
        :rtype: bool
        """

        if self.__is_type_result is not None:
            return self.__is_type_result

        self.__is_type_result = self.__is_type()

        return self.__is_type_result

    def __is_type(self):
        if self.__checker.is_type():
            return True

        if self.__checker.is_exclude_instance():
            return False

        try:
            self._converted_value = self.__converter.force_convert()
        except TypeConversionError:
            return False

        if not self.__checker.is_valid_after_convert(self._converted_value):
            return False

        return True

    def validate(self):
        """
        :raises TypeError:
            If the value is not matched the type that the class represented.
        """

        if self.is_type():
            return

        raise TypeError(
            "invalid value type: expected={}, actual={}".format(
                self.typecode, type(self._data)))

    def convert(self):
        """
        :return: Converted value.
        :raises typepy.TypeConversionError:
            If the value cannot convert.
        """

        if self.is_type():
            return self.force_convert()

        raise TypeConversionError(
            "failed to convert from {} to {}".format(
                type(self._data).__name__, self.typename))

    def force_convert(self):
        """
        :return: Converted value.
        :raises typepy.TypeConversionError:
            If the value cannot convert.
        """

        return self.__converter.force_convert()

    def try_convert(self):
        """
        :return: Converted value. |None| if failed to convert.
        """

        try:
            return self.convert()
        except TypeConversionError:
            return None
