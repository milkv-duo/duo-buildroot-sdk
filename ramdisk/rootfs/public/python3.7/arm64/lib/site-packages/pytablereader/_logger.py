# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc

import dataproperty
import logbook
import six


logger = logbook.Logger("pytablereader")
logger.disable()


def set_logger(is_enable):
    if is_enable != logger.disabled:
        return

    if is_enable:
        logger.enable()
    else:
        logger.disable()

    dataproperty.set_logger(is_enable)

    try:
        import simplesqlite

        simplesqlite.set_logger(is_enable)
    except ImportError:
        pass


def set_log_level(log_level):
    """
    Set logging level of this module. Using
    `logbook <http://logbook.readthedocs.io/en/stable/>`__ module for logging.

    :param int log_level:
        One of the log level of
        `logbook <http://logbook.readthedocs.io/en/stable/api/base.html>`__.
        Disabled logging if ``log_level`` is ``logbook.NOTSET``.
    :raises LookupError: If ``log_level`` is an invalid value.
    """

    # validate log level
    logbook.get_level_name(log_level)

    if log_level == logger.level:
        return

    if log_level == logbook.NOTSET:
        set_logger(is_enable=False)
    else:
        set_logger(is_enable=True)

    logger.level = log_level
    dataproperty.set_log_level(log_level)

    try:
        import simplesqlite

        simplesqlite.set_log_level(log_level)
    except ImportError:
        pass


@six.add_metaclass(abc.ABCMeta)
class LoggerInterface(object):

    @abc.abstractmethod
    def logging_load(self):  # pragma: no cover
        pass


class BaseLogger(LoggerInterface):

    def __init__(self, loader):
        self._loader = loader


class FileSourceLogger(BaseLogger):

    def logging_load(self):
        message = "loading {:s}: format={:s}, path={}".format(
            self._loader.source_type, self._loader.format_name,
            self._loader.source)

        try:
            message += ", encoding={}".format(self._loader.encoding)
        except AttributeError:
            pass

        logger.debug(message)


class TextSourceLogger(BaseLogger):

    def logging_load(self):
        message = "loading {:s} {:s}".format(
            self._loader.format_name, self._loader.source_type)

        try:
            message += ", len={}".format(len(self._loader.source))
        except TypeError:
            pass

        try:
            message += ", encoding={}".format(self._loader.encoding)
        except AttributeError:
            pass

        logger.debug(message)
