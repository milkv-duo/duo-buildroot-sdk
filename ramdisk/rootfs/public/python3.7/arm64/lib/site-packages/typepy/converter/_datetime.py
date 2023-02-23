# encoding: utf-8

"""
.. codeauthor:: Tsuyoshi Hombashi <tsuyoshi.hombashi@gmail.com>
"""

from __future__ import absolute_import
from __future__ import unicode_literals

from datetime import datetime
import re

from .._error import TypeConversionError
from ._interface import AbstractValueConverter


class DateTimeConverter(AbstractValueConverter):

    __DAYS_TO_SECONDS_COEF = 60 ** 2 * 24
    __MICROSECONDS_TO_SECONDS_COEF = 1000 ** 2
    __COMMON_DST_TIMEZONE_TABLE = {
        -36000: "America/Adak",  # -1000
        -32400: "US/Alaska",  # -0900
        -28800: "US/Pacific",  # -0800
        -25200: "US/Mountain",  # -0700
        -21600: "US/Central",  # -0600
        -18000: "US/Eastern",  # -0500
        -14400: "Canada/Atlantic",  # -0400
        -12600: "America/St_Johns",  # -0330
        -10800: "America/Miquelon",  # -0300
        7200: "Africa/Tripoli",  # 0200
    }

    __RE_VERSION_STR = re.compile("\d+\.\d+\.\d")

    def __init__(self, value, timezone=None):
        super(DateTimeConverter, self).__init__(value)

        self.__datetime = None
        self.__timezone = timezone

    def force_convert(self):
        self.__datetime = self.__from_datetime()
        if self.__datetime:
            return self.__datetime

        self.__datetime = self.__from_timestamp()
        if self.__datetime:
            return self.__datetime

        return self.__from_datetime_string()

    def __from_datetime(self):
        if not isinstance(self._value, datetime):
            return None

        self.__datetime = self._value
        if self.__timezone:
            self.__datetime = self.__timezone.localize(self.__datetime)

        return self.__datetime

    def __from_timestamp(self):
        from ..type._integer import Integer

        timestamp = Integer(self._value, strict_level=1).try_convert()
        if timestamp is None:
            return None

        try:
            self.__datetime = datetime.fromtimestamp(
                timestamp, self.__timezone)
        except (ValueError, OSError, OverflowError):
            raise TypeConversionError(
                "timestamp is out of the range of values supported by the platform")

        return self.__datetime

    def __from_datetime_string(self):
        import dateutil.parser
        import pytz

        self.__validate_datetime_string()

        try:
            self.__datetime = dateutil.parser.parse(self._value)
        except (AttributeError, ValueError, OverflowError):
            raise TypeConversionError(
                "failed to parse as a datetime: type={}".format(
                    type(self._value)))

        if self.__timezone:
            pytz_timezone = self.__timezone
        else:
            try:
                dst_timezone_name = self.__get_dst_timezone_name(
                    self.__get_timedelta_sec())
            except (AttributeError, KeyError):
                return self.__datetime

            pytz_timezone = pytz.timezone(dst_timezone_name)

        self.__datetime = self.__datetime.replace(tzinfo=None)
        self.__datetime = pytz_timezone.localize(self.__datetime)

        return self.__datetime

    def __get_timedelta_sec(self):
        dt = self.__datetime.utcoffset()

        return int(
            (
                dt.days *
                self.__DAYS_TO_SECONDS_COEF +
                float(dt.seconds)
            ) +
            dt.microseconds / self.__MICROSECONDS_TO_SECONDS_COEF
        )

    def __get_dst_timezone_name(self, offset):
        return self.__COMMON_DST_TIMEZONE_TABLE[offset]

    def __validate_datetime_string(self):
        """
        This will require validating version string (such as "3.3.5").
        A version string could be converted to a datetime value if this
        validation is not executed.
        """

        try:
            if self.__RE_VERSION_STR.search(self._value) is not None:
                raise TypeConversionError(
                    "invalid datetime string: version string found " +
                    self._value)
        except TypeError:
            raise TypeConversionError(
                "invalid datetime string: type={}".format(type(self._value)))
