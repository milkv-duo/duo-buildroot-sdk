# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from .._constant import TableNameTemplate as tnt
from .._validator import FileValidator
from ..interface import TableLoader
from .formatter import SqliteTableFormatter


class SqliteFileLoader(TableLoader):
    """
    A file loader class to extract tabular data from SQLite database files.

    :param str file_path: Path to the loading SQLite database file.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(filename)s_%(key)s``.

    :Dependency Packages:
        - `SimpleSQLite <https://github.com/thombashi/SimpleSQLite>`__
    """

    @property
    def format_name(self):
        return "sqlite"

    def __init__(self, file_path=None):
        super(SqliteFileLoader, self).__init__(file_path)

        self._validator = FileValidator(file_path)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a SQLite database
        file. |load_source_desc_file|

        :return:
            Loaded table data iterator.
            |load_table_name_desc|

            ===================  ==============================================
            Format specifier     Value after the replacement
            ===================  ==============================================
            ``%(filename)s``     |filename_desc|
            ``%(key)s``          ``%(format_name)s%(format_id)s``
            ``%(format_name)s``  ``"sqlite"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ==============================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the SQLite database file data is invalid or empty.
        """

        self._validate()

        formatter = SqliteTableFormatter(self.source)
        formatter.accept(self)

        return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return "{:s}{:s}".format(tnt.FORMAT_NAME, tnt.FORMAT_ID)
