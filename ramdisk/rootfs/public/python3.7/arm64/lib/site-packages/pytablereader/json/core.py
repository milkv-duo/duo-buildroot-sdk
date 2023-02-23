# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

import io
import json

from .._constant import (
    Default,
    SourceType,
    TableNameTemplate as tnt
)
from .._logger import (
    FileSourceLogger,
    TextSourceLogger,
)
from .._validator import (
    FileValidator,
    TextValidator
)
from ..error import ValidationError
from ..interface import TableLoader
from .formatter import JsonTableFormatter


class JsonTableLoader(TableLoader):
    """
    An abstract class of JSON table loaders.
    """

    @property
    def format_name(self):
        return "json"


class JsonTableFileLoader(JsonTableLoader):
    """
    A file loader class to extract tabular data from JSON files.

    :param str file_path: Path to the loading JSON file.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(filename)s_%(key)s``.
    """

    def __init__(self, file_path=None):
        super(JsonTableFileLoader, self).__init__(file_path)

        self.encoding = Default.ENCODING

        self._validator = FileValidator(file_path)
        self._logger = FileSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a JSON file.
        |load_source_desc_file|

        This method can be loading four types of JSON formats:
        **(1)** Single table data in a file,
        acceptable JSON Schema is as follows:

            .. code-block:: json
                :caption: JSON Schema (1): accept single table

                {
                    "type": "array",
                    "items": {
                        "type": "object",
                        "additionalProperties": {
                            "anyOf": [
                                {"type": "string"},
                                {"type": "number"},
                                {"type": "null"},
                            ],
                        },
                    },
                }

            .. code-block:: json
                :caption: JSON example for the JSON schema (1)

                [
                    {"attr_b": 4, "attr_c": "a", "attr_a": 1},
                    {"attr_b": 2.1, "attr_c": "bb", "attr_a": 2},
                    {"attr_b": 120.9, "attr_c": "ccc", "attr_a": 3}
                ]

        **(2)** Single table data in a file,
        acceptable JSON Schema is as follows:

            .. code-block:: json
                :caption: JSON Schema (2): accept single table

                {
                    "type": "object",
                    "additionalProperties": {
                        "anyOf": [
                            {"type": "string"},
                            {"type": "number"},
                            {"type": "null"},
                        ],
                    },
                }

            .. code-block:: json
                :caption: JSON example for the JSON schema (2)

                {
                    "attr_a": [1, 2, 3],
                    "attr_b": [4, 2.1, 120.9],
                    "attr_c": ["a", "bb", "ccc"]
                }

        **(3)** Multiple table data in a file,
        acceptable JSON Schema is as follows:

            .. code-block:: json
                :caption: JSON Schema (3): accept multiple table

                {
                    "type": "object",
                    "additionalProperties": {
                        "type": "array",
                        "items": {
                            "type": "object",
                            "additionalProperties": {
                                "anyOf": [
                                    {"type": "string"},
                                    {"type": "number"},
                                    {"type": "null"}
                                ]
                            }
                        }
                    }
                }

            .. code-block:: json
                :caption: JSON example for the JSON schema (3)

                {
                    "table_a" : [
                        {"attr_b": 4, "attr_c": "a", "attr_a": 1},
                        {"attr_b": 2.1, "attr_c": "bb", "attr_a": 2},
                        {"attr_b": 120.9, "attr_c": "ccc", "attr_a": 3}
                    ],
                    "table_b" : [
                        {"a": 1, "b": 4},
                        {"a": 2 },
                        {"a": 3, "b": 120.9}
                    ]
                }

        **(4)** Multiple table data in a file,
        acceptable JSON Schema is as follows:

            .. code-block:: json
                :caption: JSON Schema (4): accept multiple table

                {
                    "type": "object",
                    "additionalProperties": {
                        "type": "object",
                        "additionalProperties": {
                            "type": "array",
                            "items": {
                                "anyOf": [
                                    {"type": "string"},
                                    {"type": "number"},
                                    {"type": "null"},
                                ],
                            },
                        },
                    },
                }

            .. code-block:: json
                :caption: JSON example for the JSON schema (4)

                {
                    "table_a" : {
                        "attr_a": [1, 2, 3],
                        "attr_b": [4, 2.1, 120.9],
                        "attr_c": ["a", "bb", "ccc"]
                    },
                    "table_b" : {
                        "a": [1, 3],
                        "b": [4, 120.9]
                    }
                }

        :return:
            Loaded table data iterator.
            |load_table_name_desc|

            ===================  ==============================================
            Format specifier     Value after the replacement
            ===================  ==============================================
            ``%(filename)s``     |filename_desc|
            ``%(key)s``          | This replaced the different value
                                 | for each single/multiple JSON tables:
                                 | [single JSON table]
                                 | ``%(format_name)s%(format_id)s``
                                 | [multiple JSON table] Table data key.
            ``%(format_name)s``  ``"json"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ==============================================
        :rtype: |TableData| iterator
        :raises pytablereader.InvalidDataError:
            If the data is invalid JSON.
        :raises pytablereader.error.ValidationError:
            If the data is not acceptable JSON format.
        """

        self._validate()
        self._logger.logging_load()

        with io.open(self.source, "r", encoding=self.encoding) as fp:
            try:
                json_buffer = json.load(fp)
            except ValueError as e:
                raise ValidationError(e)

        formatter = JsonTableFormatter(json_buffer)
        formatter.accept(self)

        return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return "{:s}_{:s}".format(tnt.FILENAME, tnt.KEY)


class JsonTableTextLoader(JsonTableLoader):
    """
    A text loader class to extract tabular data from JSON text data.

    :param str text: JSON text to load.

    .. py:attribute:: table_name

        Table name string. Defaults to ``%(key)s``.
    """

    @property
    def source_type(self):
        return SourceType.TEXT

    def __init__(self, text):
        super(JsonTableTextLoader, self).__init__(text)

        self._validator = TextValidator(text)
        self._logger = TextSourceLogger(self)

    def load(self):
        """
        Extract tabular data as |TableData| instances from a JSON text object.
        |load_source_desc_text|

        :return:
            Loaded table data iterator.
            |load_table_name_desc|

            ===================  ==============================================
            Format specifier     Value after the replacement
            ===================  ==============================================
            ``%(filename)s``     ``""``
            ``%(key)s``          | This replaced the different value
                                 | for each single/multiple JSON tables:
                                 | [single JSON table]
                                 | ``%(format_name)s%(format_id)s``
                                 | [multiple JSON table] Table data key.
            ``%(format_name)s``  ``"json"``
            ``%(format_id)s``    |format_id_desc|
            ``%(global_id)s``    |global_id|
            ===================  ==============================================
        :rtype: |TableData| iterator

        .. seealso::

            :py:meth:`.JsonTableFileLoader.load()`
        """

        self._validate()
        self._logger.logging_load()

        json_buffer = json.loads(self.source)

        formatter = JsonTableFormatter(json_buffer)
        formatter.accept(self)

        return formatter.to_table_data()

    def _get_default_table_name_template(self):
        return "{:s}".format(tnt.KEY)
