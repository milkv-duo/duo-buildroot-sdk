"""Testcases for cssutils.css.DOMImplementation"""

import xml.dom
import unittest
import cssutils

class DOMImplementationTestCase(unittest.TestCase):

    def setUp(self):
        self.domimpl = xml.dom.getDOMImplementation()

    def test_createCSSStyleSheet(self):
        "DOMImplementationCSS.createCSSStyleSheet()"
        title, media = 'Test Title', cssutils.stylesheets.MediaList('all')
        sheet = self.domimpl.createCSSStyleSheet(title, media)
        self.assertEqual(True, isinstance(sheet, cssutils.css.CSSStyleSheet))
        self.assertEqual(title, sheet.title)
        self.assertEqual(media, sheet.media)

    def test_createDocument(self):
        "DOMImplementationCSS.createDocument()"
        self.assertRaises(NotImplementedError, self.domimpl.createDocument)
        self.assertRaises(NotImplementedError, self.domimpl.createDocument)

    def test_createDocumentType(self):
        "DOMImplementationCSS.createDocumentType()"
        self.assertRaises(NotImplementedError, self.domimpl.createDocumentType)

    def test_hasFeature(self):
        "DOMImplementationCSS.hasFeature()"
        tests = [
            ('css', '1.0'),
            ('css', '2.0'),
            ('stylesheets', '1.0'),
            ('stylesheets', '2.0')
        ]
        for name, version in tests:
            self.assertEqual(True, self.domimpl.hasFeature(name, version))


if __name__ == '__main__':
    unittest.main()