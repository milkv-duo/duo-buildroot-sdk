# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""


def convert_idx_to_alphabet(column_idx):
    if column_idx < 26:
        return chr(65 + column_idx)

    return (
        convert_idx_to_alphabet(int(column_idx / 26 - 1)) +
        convert_idx_to_alphabet(column_idx % 26))
