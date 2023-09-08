# -*- coding: utf-8 -*-
"""
 Copyright (c) 2015-2017 Alan Yorinks All rights reserved.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE
 Version 3 as published by the Free Software Foundation; either
 or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE
 along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""


class PinData:
    """
    Each analog and digital input pin is described by an instance of
    this class. It contains both the last data value received and a potential
    callback reference. It may also contain a callback differential that if met
    will cause a callback to occur. The differential pertains to non-digital
    inputs.
    """

    def __init__(self, data_lock):
        self.data_lock = data_lock
        # current data value
        self._current_value = 0
        # time stamp of last change event
        self._event_time = 0
        # callback reference
        self._cb = None
        # analog differential
        self._differential = 1
        # digital pin was set as a pullup pin
        self._pull_up = False
#修饰符学习
    @property
    def current_value(self):
        with self.data_lock:
            return self._current_value

    @current_value.setter
    def current_value(self, value):
        with self.data_lock:
            self._current_value = value

    @property
    def event_time(self):
        with self.data_lock:
            return self._event_time

    @event_time.setter
    def event_time(self, value):
        with self.data_lock:
            self._event_time = value

    @property
    def cb(self):
        with self.data_lock:
            return self._cb

    @cb.setter
    def cb(self, value):
        with self.data_lock:
            self._cb = value

    @property
    def differential(self):
        with self.data_lock:
            return self._differential

    @differential.setter
    def differential(self, value):
        with self.data_lock:
            self._differential = value

    @property
    def pull_up(self):
        with self.data_lock:
            return self._pull_up

    @pull_up.setter
    def pull_up(self, value):
        with self.data_lock:
            self._pull_up = value


