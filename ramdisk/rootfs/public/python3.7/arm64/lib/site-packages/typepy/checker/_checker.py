# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc

from ._interface import TypeCheckerInterface


class CheckerCreator(object):
    __slots__ = (
        "__min_strict_level",
        "__max_strict_level",
        "__value",
        "__checker_mapping",
    )

    @property
    def min_strict_level(self):
        return self.__min_strict_level

    @property
    def max_strict_level(self):
        return self.__max_strict_level

    def __init__(self, value, checker_mapping):
        self.__value = value
        self.__checker_mapping = checker_mapping

        self.__min_strict_level = min(checker_mapping)
        self.__max_strict_level = max(checker_mapping)

    def create(self, strict_level=None):
        if strict_level is None:
            strict_level = self.max_strict_level
        elif strict_level < self.min_strict_level:
            strict_level = self.min_strict_level
        elif strict_level > self.max_strict_level:
            strict_level = self.max_strict_level

        return self.__checker_mapping.get(strict_level)(self.__value)


class TypeCheckerStrictLevel(TypeCheckerInterface):
    __slots__ = (
        "_value",
    )

    def __init__(self, value):
        self._value = value

    @abc.abstractmethod
    def is_instance(self):
        pass

    def is_type(self):
        return (
            self.is_instance() and
            not self.is_exclude_instance()
        )

    def validate(self):
        """
        :raises TypeError:
            If the value is not matched the type to be expected.
        """

        if self.is_type():
            return

        raise TypeError(
            "invalid value type: actual={}".format(type(self._value)))

    def is_exclude_instance(self):
        return False

    def is_valid_after_convert(self, converted_value):
        return True


class TypeChecker(TypeCheckerInterface):
    __slots__ = (
        "__checker",
    )

    def __init__(self, value, checker_mapping, strict_level):
        self.__checker = CheckerCreator(
            value=value,
            checker_mapping=checker_mapping).create(strict_level)

    def is_type(self):
        return self.__checker.is_type()

    def is_valid_after_convert(self, value):
        return self.__checker.is_valid_after_convert(value)

    def is_instance(self):
        return self.__checker.is_instance()

    def is_exclude_instance(self):
        return self.__checker.is_exclude_instance()

    def validate(self):
        self.__checker.validate()
