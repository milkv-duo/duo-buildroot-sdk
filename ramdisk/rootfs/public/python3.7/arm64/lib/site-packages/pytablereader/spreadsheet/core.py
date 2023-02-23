# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import abc

from .._constant import TableNameTemplate as tnt
from ..interface import TableLoader


class SpreadSheetLoader(TableLoader):
    """
    An abstract class of table data.
    Especially spreadsheets that consists multiple rows.

    .. py:attribute:: start_row

        The first row to search header row.
    """

    def __init__(self, source):
        super(SpreadSheetLoader, self).__init__(source)

        self.start_row = 0
        self._worksheet = None
        self._start_col_idx = None
        self._end_col_idx = None

    @abc.abstractproperty
    def _sheet_name(self):  # pragma: no cover
        pass

    @abc.abstractproperty
    def _row_count(self):  # pragma: no cover
        pass

    @abc.abstractproperty
    def _col_count(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def _is_empty_sheet(self):  # pragma: no cover
        pass

    @abc.abstractmethod
    def _get_start_row_idx(self):  # pragma: no cover
        pass

    @property
    def format_name(self):
        return "spreadsheet"

    def _make_table_name(self):
        kv_mapping = self._get_basic_tablename_keyvalue_mapping()

        try:
            kv_mapping[tnt.SHEET] = self._sheet_name
        except AttributeError:
            kv_mapping[tnt.SHEET] = ""

        return self._expand_table_name_format(kv_mapping)

    def _get_default_table_name_template(self):
        return "{:s}".format(tnt.SHEET)
