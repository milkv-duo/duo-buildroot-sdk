# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc

from ._common import _validate_null_string
from ._six import add_metaclass


@add_metaclass(abc.ABCMeta)
class NameSanitizer(object):

    @abc.abstractproperty
    def reserved_keywords(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def validate(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def sanitize(self, replacement_text=""):  # pragma: no cover
        pass

    @property
    def _str(self):
        return self._value

    def __init__(self, value):
        self._validate_null_string(value)

        self._value = value.strip()

    def _is_reserved_keyword(self, value):
        return value in self.reserved_keywords

    @staticmethod
    def _validate_null_string(text, error_msg="null name"):
        _validate_null_string(text)
