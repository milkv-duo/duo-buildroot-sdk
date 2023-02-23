# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from typepy import Typecode

from ._align import Align


class AlignGetter(object):

    @property
    def typecode_align_table(self):
        raise NotImplementedError()

    @typecode_align_table.setter
    def typecode_align_table(self, x):
        self.__typecode_align_table = x

    def get_align_from_typecode(self, typecode):
        return self.__typecode_align_table.get(typecode, self.default_align)

    def __init__(self):
        self.typecode_align_table = {
            Typecode.STRING: Align.LEFT,
            Typecode.INTEGER: Align.RIGHT,
            Typecode.REAL_NUMBER: Align.RIGHT,
        }
        self.default_align = Align.LEFT


align_getter = AlignGetter()
