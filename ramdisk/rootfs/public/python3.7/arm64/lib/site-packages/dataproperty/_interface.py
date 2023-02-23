# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
import abc

import six


@six.add_metaclass(abc.ABCMeta)
class DataPeropertyInterface(object):
    __slots__ = ()

    @abc.abstractproperty
    def align(self):  # pragma: no cover
        pass

    @abc.abstractproperty
    def decimal_places(self):  # pragma: no cover
        pass

    @abc.abstractproperty
    def typecode(self):  # pragma: no cover
        pass

    @abc.abstractproperty
    def typename(self):  # pragma: no cover
        pass
