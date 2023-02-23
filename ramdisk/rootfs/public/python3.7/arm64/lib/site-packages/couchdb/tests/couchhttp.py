# -*- coding: utf-8 -*-
#
# Copyright (C) 2009 Christopher Lenz
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

import socket
import time
import unittest

from couchdb import http, util
from couchdb.tests import testutil


class SessionTestCase(testutil.TempDatabaseMixin, unittest.TestCase):

    def test_timeout(self):
        dbname, db = self.temp_db()
        timeout = 1
        session = http.Session(timeout=timeout)
        start = time.time()
        status, headers, body = session.request('GET', db.resource.url + '/_changes?feed=longpoll&since=1000&timeout=%s' % (timeout*2*1000,))
        self.assertRaises(socket.timeout, body.read)
        self.assertTrue(time.time() - start < timeout * 1.3)

    def test_timeout_retry(self):
        dbname, db = self.temp_db()
        timeout = 1e-12
        session = http.Session(timeout=timeout, retryable_errors=["timed out"])
        self.assertRaises(socket.timeout, session.request, 'GET', db.resource.url)


class ResponseBodyTestCase(unittest.TestCase):
    def test_close(self):
        class TestStream(util.StringIO):
            def isclosed(self):
                return len(self.getvalue()) == self.tell()

        class ConnPool(object):
            def __init__(self):
                self.value = 0
            def release(self, url, conn):
                self.value += 1

        conn_pool = ConnPool()
        stream = TestStream(b'foobar')
        stream.msg = {}
        response = http.ResponseBody(stream, conn_pool, 'a', 'b')

        response.read(10) # read more than stream has. close() is called
        response.read() # steam ended. another close() call

        self.assertEqual(conn_pool.value, 1)

    def test_double_iteration_over_same_response_body(self):
        class TestHttpResp(object):
            msg = {'transfer-encoding': 'chunked'}
            def __init__(self, fp):
                self.fp = fp
            def close(self):
                pass
            def isclosed(self):
                return len(self.fp.getvalue()) == self.fp.tell()

        data = b'foobarbaz\n'
        data = b'\n'.join([hex(len(data))[2:].encode('utf-8'), data])
        response = http.ResponseBody(TestHttpResp(util.StringIO(data)),
                                     None, None, None)
        self.assertEqual(list(response.iterchunks()), [b'foobarbaz\n'])
        self.assertEqual(list(response.iterchunks()), [])


class CacheTestCase(testutil.TempDatabaseMixin, unittest.TestCase):

    def test_remove_miss(self):
        """Check that a cache remove miss is handled gracefully."""
        url = 'http://localhost:5984/foo'
        cache = http.Cache()
        cache.put(url, (None, None, None))
        cache.remove(url)
        cache.remove(url)

    def test_cache_clean(self):
        cache = http.Cache()
        cache.put('foo', (None, {'Date': 'Sat, 14 Feb 2009 02:31:28 -0000'}, None))
        cache.put('bar', (None, {'Date': 'Sat, 14 Feb 2009 02:31:29 -0000'}, None))
        cache.put('baz', (None, {'Date': 'Sat, 14 Feb 2009 02:31:30 -0000'}, None))
        cache.keep_size = 1
        cache._clean()
        self.assertEqual(len(cache.by_url), 1)
        self.assertTrue('baz' in cache.by_url)


def suite():
    suite = unittest.TestSuite()
    suite.addTest(testutil.doctest_suite(http))
    suite.addTest(unittest.makeSuite(SessionTestCase, 'test'))
    suite.addTest(unittest.makeSuite(ResponseBodyTestCase, 'test'))
    suite.addTest(unittest.makeSuite(CacheTestCase, 'test'))
    return suite


if __name__ == '__main__':
    unittest.main(defaultTest='suite')
