 # ====================================================================
 # Copyright (c) 2004-2010 Open Source Applications Foundation.
 #
 # Permission is hereby granted, free of charge, to any person obtaining a
 # copy of this software and associated documentation files (the "Software"),
 # to deal in the Software without restriction, including without limitation
 # the rights to use, copy, modify, merge, publish, distribute, sublicense,
 # and/or sell copies of the Software, and to permit persons to whom the
 # Software is furnished to do so, subject to the following conditions: 
 #
 # The above copyright notice and this permission notice shall be included
 # in all copies or substantial portions of the Software. 
 #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 # OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 # FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 # DEALINGS IN THE SOFTWARE.
 # ====================================================================

from _icu import *
from _icu import _install__doc__


_install__doc__(BreakIterator, '''
The BreakIterator class implements methods for finding the location of
boundaries in text.

BreakIterator is an abstract base class. Instances of BreakIterator maintain
a current position and scan over text returning the index of characters
where boundaries occur.

Line boundary analysis determines where a text string can be broken when
line-wrapping. The mechanism correctly handles punctuation and hyphenated
words.

Sentence boundary analysis allows selection with correct interpretation of
periods within numbers and abbreviations, and trailing punctuation marks
such as quotation marks and parentheses.

Word boundary analysis is used by search and replace functions, as well as
within text editing applications that allow the user to select words with a
double click. Word selection provides correct interpretation of punctuation
marks within and following words. Characters that are not part of a word,
such as symbols or punctuation marks, have word-breaks on both sides.

Character boundary analysis allows users to interact with characters as they
expect to, for example, when moving the cursor through a text
string. Character boundary analysis provides correct navigation of through
character strings, regardless of how the character is stored. For example,
an accented character might be stored as a base character and a diacritical
mark. What users consider to be a character can differ between languages.

The text boundary positions are found according to the rules described in
Unicode Standard Annex #29, Text Boundaries, and Unicode Standard Annex #14,
Line Breaking Properties. These are available at
http://www.unicode.org/reports/tr14/ and http://www.unicode.org/reports/tr29/.
''')

_install__doc__(BreakIterator.__init__, '''
BreakIterator is abstract. Use one of the class instantiators:
createWordInstance, createLineInstance, createCharacterInstance,
createSentenceInstance or createTitleInstance.
''')

_install__doc__(BreakIterator.getText, '''
Return the string over the text being analyzed.
''')

_install__doc__(BreakIterator.createLineInstance, '''
Returns an instance of a BreakIterator implementing line breaks. Line breaks
are logically possible line breaks, actual line breaks are usually
determined based on display width. LineBreak is useful for word wrapping
text.
''')
