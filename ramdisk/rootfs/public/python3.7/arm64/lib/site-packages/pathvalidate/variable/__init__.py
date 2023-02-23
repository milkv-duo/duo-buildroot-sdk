# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import

from ._elasticsearch import ElasticsearchIndexNameSanitizer
from ._javascript import (
    validate_js_var_name,
    sanitize_js_var_name,
    JavaScriptVarNameSanitizer,
)
from ._python import (
    validate_python_var_name,
    sanitize_python_var_name,
    PythonVarNameSanitizer,
)
