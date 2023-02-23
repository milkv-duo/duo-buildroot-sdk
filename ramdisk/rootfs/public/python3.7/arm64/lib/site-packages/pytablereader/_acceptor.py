# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

import abc

import six


@six.add_metaclass(abc.ABCMeta)
class LoaderAcceptorInterface(object):
    """
    An interface class of table loader acceptor.
    """

    @abc.abstractmethod
    def accept(self, loader):  # pragma: no cover
        pass


class LoaderAcceptor(LoaderAcceptorInterface):
    """
    An abstract class of table loader acceptor.
    """

    def __init__(self):
        self._loader = None

    def accept(self, loader):
        self._loader = loader
