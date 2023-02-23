"""Base class for all tests"""

import logging
import os
import sys
import io
import unittest
import urllib.request, urllib.error, urllib.parse
from email import message_from_string, message_from_file

# add src to PYTHONPATH
sys.path.append(os.path.join(os.path.abspath('.'), '..'))

import cssutils


PY2x = sys.version_info < (3,0)

def msg3x(msg):
    """msg might contain unicode repr `u'...'` which in py3 is `u'...`
    needed by tests using ``assertRaisesMsg``"""
    if not PY2x and msg.find("u'"):
        msg = msg.replace("u'", "'")
    return msg


class BaseTestCase(unittest.TestCase):

    def _tempSer(self):
        "Replace default ser with temp ser."
        self._ser = cssutils.ser
        cssutils.ser = cssutils.serialize.CSSSerializer()

    def _restoreSer(self):
        "Restore the default ser."
        cssutils.ser = self._ser

    def setUp(self):
        # a raising parser!!!
        cssutils.log.raiseExceptions = True
        cssutils.log.setLevel(logging.FATAL)
        self.p = cssutils.CSSParser(raiseExceptions=True)

    def tearDown(self):
        if hasattr(self, '_ser'):
            self._restoreSer()

    def assertRaisesEx(self, exception, callable, *args, **kwargs):
        """
        from
        http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/307970
        """
        if "exc_args" in kwargs:
            exc_args = kwargs["exc_args"]
            del kwargs["exc_args"]
        else:
            exc_args = None
        if "exc_pattern" in kwargs:
            exc_pattern = kwargs["exc_pattern"]
            del kwargs["exc_pattern"]
        else:
            exc_pattern = None

        argv = [repr(a) for a in args]\
               + ["%s=%r" % (k,v)  for k,v in list(kwargs.items())]
        callsig = "%s(%s)" % (callable.__name__, ", ".join(argv))

        try:
            callable(*args, **kwargs)
        except exception as exc:
            if exc_args is not None:
                self.assertFalse(exc.args != exc_args,
                            "%s raised %s with unexpected args: "\
                            "expected=%r, actual=%r"\
                            % (callsig, exc.__class__, exc_args, exc.args))
            if exc_pattern is not None:
                self.assertTrue(exc_pattern.search(str(exc)),
                                "%s raised %s, but the exception "\
                                "does not match '%s': %r"\
                                % (callsig, exc.__class__, exc_pattern.pattern,
                                   str(exc)))
        except:
            exc_info = sys.exc_info()
            print(exc_info)
            self.fail("%s raised an unexpected exception type: "\
                      "expected=%s, actual=%s"\
                      % (callsig, exception, exc_info[0]))
        else:
            self.fail("%s did not raise %s" % (callsig, exception))

    def assertRaisesMsg(self, excClass, msg, callableObj, *args, **kwargs):
        """
        Just like unittest.TestCase.assertRaises,
        but checks that the message is right too.

        Usage::

            self.assertRaisesMsg(
                MyException, "Exception message",
                my_function, (arg1, arg2)
                )

        from
        http://www.nedbatchelder.com/blog/200609.html#e20060905T064418
        """
        try:
            callableObj(*args, **kwargs)
        except excClass as exc:
            excMsg = str(exc)
            if not msg:
                # No message provided: any message is fine.
                return
            elif excMsg == msg:
                # Message provided, and we got the right message: passes.
                return
            else:
                # Message provided, and it didn't match: fail!
                raise self.failureException(
                    "Right exception, wrong message: got '%s' instead of '%s'" %
                    (excMsg, msg))
        else:
            if hasattr(excClass, '__name__'):
                excName = excClass.__name__
            else:
                excName = str(excClass)
            raise self.failureException(
                "Expected to raise %s, didn't get an exception at all" %
                excName
                )

    def do_equal_p(self, tests, att='cssText', debug=False, raising=True):
        """
        if raising self.p is used for parsing, else self.pf
        """
        p = cssutils.CSSParser(raiseExceptions=raising)
        # parses with self.p and checks att of result
        for test, expected in list(tests.items()):
            if debug:
                print('"%s"' % test)
            s = p.parseString(test)
            if expected is None:
                expected = test
            self.assertEqual(expected, str(s.__getattribute__(att), 'utf-8'))

    def do_raise_p(self, tests, debug=False, raising=True):
        # parses with self.p and expects raise
        p = cssutils.CSSParser(raiseExceptions=raising)
        for test, expected in list(tests.items()):
            if debug:
                print('"%s"' % test)
            self.assertRaises(expected, p.parseString, test)

    def do_equal_r(self, tests, att='cssText', debug=False):
        # sets attribute att of self.r and asserts Equal
        for test, expected in list(tests.items()):
            if debug:
                print('"%s"' % test)
            self.r.__setattr__(att, test)
            if expected is None:
                expected = test
            self.assertEqual(expected, self.r.__getattribute__(att))

    def do_raise_r(self, tests, att='_setCssText', debug=False):
        # sets self.r and asserts raise
        for test, expected in list(tests.items()):
            if debug:
                print('"%s"' % test)
            self.assertRaises(expected, self.r.__getattribute__(att), test)

    def do_raise_r_list(self, tests, err, att='_setCssText', debug=False):
        # sets self.r and asserts raise
        for test in tests:
            if debug:
                print('"%s"' % test)
            self.assertRaises(err, self.r.__getattribute__(att), test)
