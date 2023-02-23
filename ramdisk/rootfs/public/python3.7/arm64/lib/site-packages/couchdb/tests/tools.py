# -*- coding: utf-8 -*-
#
# Copyright (C) 2012 Alexander Shorin
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.
#


import unittest

from couchdb.util import StringIO
from couchdb import Unauthorized
from couchdb.tools import load, dump
from couchdb.tests import testutil


class ToolLoadTestCase(testutil.TempDatabaseMixin, unittest.TestCase):

    def test_handle_credentials(self):
        # Issue 194: couchdb-load attribute error: 'Resource' object has no attribute 'http'
        # http://code.google.com/p/couchdb-python/issues/detail?id=194
        load.load_db(StringIO(b''), self.db.resource.url, 'foo', 'bar')


class ToolDumpTestCase(testutil.TempDatabaseMixin, unittest.TestCase):

    def test_handle_credentials(self):
        # Similar to issue 194
        # Fixing: AttributeError: 'Resource' object has no attribute 'http'
        try:
            dump.dump_db(self.db.resource.url, 'foo', 'bar', output=StringIO())
        except Unauthorized:
            # This is ok, since we provided dummy credentials.
            pass


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(ToolLoadTestCase, 'test'))
    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='suite')

