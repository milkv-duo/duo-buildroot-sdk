#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2007-2008 Christopher Lenz
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

import unittest

from couchdb.http import ResourceConflict, ResourceNotFound
from couchdb.tests import testutil


class CouchTests(testutil.TempDatabaseMixin, unittest.TestCase):

    def _create_test_docs(self, num):
        for i in range(num):
            self.db[str(i)] = {'a': i + 1, 'b': (i + 1) ** 2}

    def test_basics(self):
        self.assertEqual(0, len(self.db))

        # create a document
        data = {'a': 1, 'b': 1}
        self.db['0'] = data
        self.assertEqual('0', data['_id'])
        assert '_rev' in data
        doc = self.db['0']
        self.assertEqual('0', doc.id)
        self.assertEqual(data['_rev'], doc.rev)
        self.assertEqual(1, len(self.db))

        # delete a document
        del self.db['0']
        self.assertRaises(ResourceNotFound, self.db.__getitem__, '0')

        # test _all_docs
        self._create_test_docs(4)
        self.assertEqual(4, len(self.db))
        for doc_id in self.db:
            assert int(doc_id) in range(4)

        # test a simple query
        query = """function(doc) {
            if (doc.a==4)
                emit(null, doc.b);
        }"""
        result = list(self.db.query(query))
        self.assertEqual(1, len(result))
        self.assertEqual('3', result[0].id)
        self.assertEqual(16, result[0].value)

        # modify a document, and redo the query
        doc = self.db['0']
        doc['a'] = 4
        self.db['0'] = doc
        result = list(self.db.query(query))
        self.assertEqual(2, len(result))

        # add more documents, and redo the query again
        self.db.save({'a': 3, 'b': 9})
        self.db.save({'a': 4, 'b': 16})
        result = list(self.db.query(query))
        self.assertEqual(3, len(result))
        self.assertEqual(6, len(self.db))

        # delete a document, and redo the query once more
        del self.db['0']
        result = list(self.db.query(query))
        self.assertEqual(2, len(result))
        self.assertEqual(5, len(self.db))

    def test_conflict_detection(self):
        doc1 = {'a': 1, 'b': 1}
        self.db['foo'] = doc1
        doc2 = self.db['foo']
        self.assertEqual(doc1['_id'], doc2.id)
        self.assertEqual(doc1['_rev'], doc2.rev)

        # make conflicting modifications
        doc1['a'] = 2
        doc2['a'] = 3
        self.db['foo'] = doc1
        self.assertRaises(ResourceConflict, self.db.__setitem__, 'foo', doc2)

        # try submitting without the revision info
        data = {'_id': 'foo', 'a': 3, 'b': 1}
        self.assertRaises(ResourceConflict, self.db.__setitem__, 'foo', data)

        del self.db['foo']
        self.db['foo'] = data

    def test_lots_of_docs(self):
        num = 100 # Crank up manually to really test
        for i in range(num): 
            self.db[str(i)] = {'integer': i, 'string': str(i)}
        self.assertEqual(num, len(self.db))

        query = """function(doc) {
            emit(doc.integer, null);
        }"""
        results = list(self.db.query(query))
        self.assertEqual(num, len(results))
        for idx, row in enumerate(results):
            self.assertEqual(idx, row.key)

        results = list(self.db.query(query, descending=True))
        self.assertEqual(num, len(results))
        for idx, row in enumerate(results):
            self.assertEqual(num - idx - 1, row.key)

    def test_multiple_rows(self):
        self.db['NC'] = {'cities': ["Charlotte", "Raleigh"]}
        self.db['MA'] = {'cities': ["Boston", "Lowell", "Worcester",
                                    "Cambridge", "Springfield"]}
        self.db['FL'] = {'cities': ["Miami", "Tampa", "Orlando",
                                    "Springfield"]}

        query = """function(doc){
            for (var i = 0; i < doc.cities.length; i++) {
                emit(doc.cities[i] + ", " + doc._id, null);
            }
        }"""
        results = list(self.db.query(query))
        self.assertEqual(11, len(results))
        self.assertEqual("Boston, MA", results[0].key);
        self.assertEqual("Cambridge, MA", results[1].key);
        self.assertEqual("Charlotte, NC", results[2].key);
        self.assertEqual("Lowell, MA", results[3].key);
        self.assertEqual("Miami, FL", results[4].key);
        self.assertEqual("Orlando, FL", results[5].key);
        self.assertEqual("Raleigh, NC", results[6].key);
        self.assertEqual("Springfield, FL", results[7].key);
        self.assertEqual("Springfield, MA", results[8].key);
        self.assertEqual("Tampa, FL", results[9].key);
        self.assertEqual("Worcester, MA", results[10].key);

        # Add a city and rerun the query
        doc = self.db['NC']
        doc['cities'].append("Wilmington")
        self.db['NC'] = doc
        results = list(self.db.query(query))
        self.assertEqual(12, len(results))
        self.assertEqual("Wilmington, NC", results[10].key)

        # Remove a document and redo the query again
        del self.db['MA']
        results = list(self.db.query(query))
        self.assertEqual(7, len(results))
        self.assertEqual("Charlotte, NC", results[0].key);
        self.assertEqual("Miami, FL", results[1].key);
        self.assertEqual("Orlando, FL", results[2].key);
        self.assertEqual("Raleigh, NC", results[3].key);
        self.assertEqual("Springfield, FL", results[4].key);
        self.assertEqual("Tampa, FL", results[5].key);
        self.assertEqual("Wilmington, NC", results[6].key)

    def test_large_docs(self):
        size = 100
        longtext = '0123456789\n' * size
        self.db.save({'longtext': longtext})
        self.db.save({'longtext': longtext})
        self.db.save({'longtext': longtext})
        self.db.save({'longtext': longtext})

        query = """function(doc) {
            emit(null, doc.longtext);
        }"""
        results = list(self.db.query(query))
        self.assertEqual(4, len(results))

    def test_utf8_encoding(self):
        texts = [
            u"1. Ascii: hello",
            u"2. Russian: На берегу пустынных волн",
            u"3. Math: ∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i),",
            u"4. Geek: STARGΛ̊TE SG-1",
            u"5. Braille: ⡌⠁⠧⠑ ⠼⠁⠒  ⡍⠜⠇⠑⠹⠰⠎ ⡣⠕⠌"
        ]
        for idx, text in enumerate(texts):
            self.db[str(idx)] = {'text': text}
        for idx, text in enumerate(texts):
            doc = self.db[str(idx)]
            self.assertEqual(text, doc['text'])

        query = """function(doc) {
            emit(doc.text, null);
        }"""
        for idx, row in enumerate(self.db.query(query)):
            self.assertEqual(texts[idx], row.key)

    def test_design_docs(self):
        for i in range(50): 
            self.db[str(i)] = {'integer': i, 'string': str(i)}
        self.db['_design/test'] = {'views': {
            'all_docs': {'map': 'function(doc) { emit(doc.integer, null) }'},
            'no_docs': {'map': 'function(doc) {}'},
            'single_doc': {'map': 'function(doc) { if (doc._id == "1") emit(null, 1) }'}
        }}
        for idx, row in enumerate(self.db.view('test/all_docs')):
            self.assertEqual(idx, row.key)
        self.assertEqual(0, len(list(self.db.view('test/no_docs'))))
        self.assertEqual(1, len(list(self.db.view('test/single_doc'))))

    def test_collation(self):
        values = [
            None, False, True,
            1, 2, 3.0, 4,
            'a', 'A', 'aa', 'b', 'B', 'ba', 'bb',
            ['a'], ['b'], ['b', 'c'], ['b', 'c', 'a'], ['b', 'd'],
            ['b', 'd', 'e'],
            {'a': 1}, {'a': 2}, {'b': 1}, {'b': 2}, {'b': 2, 'c': 2},
        ]
        self.db['0'] = {'bar': 0}
        for idx, value in enumerate(values):
            self.db[str(idx + 1)] = {'foo': value}

        query = """function(doc) {
            if(doc.foo !== undefined) {
                emit(doc.foo, null);
            }
        }"""
        rows = iter(self.db.query(query))
        self.assertEqual(None, next(rows).value)
        for idx, row in enumerate(rows):
            self.assertEqual(values[idx + 1], row.key)

        rows = self.db.query(query, descending=True)
        for idx, row in enumerate(rows):
            if idx < len(values):
                self.assertEqual(values[len(values) - 1- idx], row.key)
            else:
                self.assertEqual(None, row.value)

        for value in values:
            rows = list(self.db.query(query, key=value))
            self.assertEqual(1, len(rows))
            self.assertEqual(value, rows[0].key)


def suite():
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(CouchTests, 'test'))
    return suite

if __name__ == '__main__':
    unittest.main(defaultTest='suite')
