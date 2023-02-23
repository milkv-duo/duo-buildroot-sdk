# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import unicode_literals

import enum


@enum.unique
class Align(enum.Enum):
    AUTO = (1 << 0, "auto")
    LEFT = (1 << 1, "left")
    RIGHT = (1 << 2, "right")
    CENTER = (1 << 3, "center")

    @property
    def align_code(self):
        return self.__align_code

    @property
    def align_string(self):
        return self.__align_string

    def __init__(self, code, string):
        self.__align_code = code
        self.__align_string = string
