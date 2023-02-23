# -*- coding: utf-8 -*-
#
# Copyright (C) 2007 Christopher Lenz
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

import unittest

from couchdb.tests import client, couch_tests, design, couchhttp, \
                          multipart, mapping, view, package, tools, \
                          loader


def suite():
    suite = unittest.TestSuite()
    suite.addTest(client.suite())
    suite.addTest(design.suite())
    suite.addTest(couchhttp.suite())
    suite.addTest(multipart.suite())
    suite.addTest(mapping.suite())
    suite.addTest(view.suite())
    suite.addTest(couch_tests.suite())
    suite.addTest(package.suite())
    suite.addTest(tools.suite())
    suite.addTest(loader.suite())
    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='suite')
