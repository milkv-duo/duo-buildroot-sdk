# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc

import six


@six.add_metaclass(abc.ABCMeta)
class TypeCheckerInterface(object):

    @abc.abstractmethod
    def is_type(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def validate(self):  # pragma: no cover
        pass
