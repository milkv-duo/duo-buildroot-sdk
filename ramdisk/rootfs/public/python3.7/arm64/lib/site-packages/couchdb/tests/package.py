# -*- coding: utf-8 -*-

import unittest
import couchdb

class PackageTestCase(unittest.TestCase):

    def test_exports(self):
        expected = set([
            # couchdb.client
            'Server', 'Database', 'Document',
            # couchdb.http
            'HTTPError', 'PreconditionFailed', 'ResourceNotFound',
            'ResourceConflict', 'ServerError', 'Unauthorized',
            'Resource', 'Session'
        ])
        exported = set(e for e in dir(couchdb) if not e.startswith('_'))
        self.assertTrue(expected <= exported)


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(PackageTestCase, 'test'))
    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='suite')
