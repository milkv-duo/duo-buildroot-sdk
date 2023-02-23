# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from pytablereader import InvalidDataError
import typepy

from ..html.formatter import HtmlTableFormatter


class MarkdownTableFormatter(HtmlTableFormatter):

    def __init__(self, source_data):
        import markdown2

        if typepy.is_null_string(source_data):
            raise InvalidDataError

        super(MarkdownTableFormatter, self).__init__(
            markdown2.markdown(source_data, extras=["tables"]))
