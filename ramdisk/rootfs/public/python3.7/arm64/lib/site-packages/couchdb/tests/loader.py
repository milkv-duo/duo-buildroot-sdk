# -*- coding: utf-8 -*-
#
# Copyright (C) 2016 Daniel Holth
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

import unittest
import os.path

from couchdb import loader
from couchdb.tests import testutil

expected = {
 '_id': u'_design/loader',
 'filters': {'filter': u'function(doc, req) { return true; }'},
 'language': u'javascript',
 'views': {'a': {'map': u'function(doc) {\n  emit(doc.property_to_index);\n}'}}}

class LoaderTestCase(unittest.TestCase):

    directory = os.path.join(os.path.dirname(__file__), '_loader')

    def test_loader(self):
        doc = loader.load_design_doc(self.directory,
                                     strip=True,
                                     predicate=lambda x: \
                                        not x.endswith('.xml'))
        self.assertEqual(doc, expected)

    def test_bad_directory(self):
        def bad_directory():
            doc = loader.load_design_doc('directory_does_not_exist')

        self.assertRaises(OSError, bad_directory)

    def test_clobber_1(self):
        def clobber():
            doc = loader.load_design_doc(self.directory,
                                     strip=True,
                                     predicate=lambda x: \
                                     not x.endswith('filters.xml'))

        self.assertRaises(loader.DuplicateKeyError, clobber)

    def test_clobber_2(self):
        def clobber():
            doc = loader.load_design_doc(self.directory,
                                     strip=True,
                                     predicate=lambda x: \
                                     not x.endswith('language.xml'))

        self.assertRaises(loader.DuplicateKeyError, clobber)


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(LoaderTestCase))
    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='suite')
