#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Load design documents from the filesystem into a dict.
Subset of couchdbkit/couchapp functionality.

Description
-----------

Convert a target directory into an object (dict).

Each filename (without extension) or subdirectory name is a key in this object.

For files, the utf-8-decoded contents are the value, except for .json files
which are first decoded as json.

Subdirectories are converted into objects using the same procedure and then
added to the parent object.

Typically used for design documents. This directory tree::

  .
    ├── filters
    │   └── forms_only.js
    ├── _id
    ├── language
    ├── lib
    │   └── validate.js
    └── views
        ├── view_a
        │   └── map.js
        ├── view_b
        │   └── map.js
        └── view_c
            └── map.js

Becomes this object::

    {
      "views": {
        "view_a": {
          "map": "function(doc) { ... }"
        },
        "view_b": {
          "map": "function(doc) { ... }"
        },
        "view_c": {
          "map": "function(doc) { ... }"
        }
      },
      "_id": "_design/name_of_design_document",
      "filters": {
        "forms_only": "function(doc, req) { ... }"
      },
      "language": "javascript",
      "lib": {
        "validate": "// A library for validations ..."
      }
    }

"""

from __future__ import unicode_literals, absolute_import

import os.path
import pprint
import codecs
import json

class DuplicateKeyError(ValueError):
    pass

def load_design_doc(directory, strip=False, predicate=lambda x: True):
    """
    Load a design document from the filesystem.

    strip: remove leading and trailing whitespace from file contents,
        like couchdbkit.

    predicate: function that is passed the full path to each file or directory.
        Each entry is only added to the document if predicate returns True.
        Can be used to ignore backup files etc.
    """
    objects = {}

    if not os.path.isdir(directory):
        raise OSError("No directory: '{0}'".format(directory))

    for (dirpath, dirnames, filenames) in os.walk(directory, topdown=False):
        key = os.path.split(dirpath)[-1]
        ob = {}
        objects[dirpath] = (key, ob)

        for name in filenames:
            fkey = os.path.splitext(name)[0]
            fullname = os.path.join(dirpath, name)
            if not predicate(fullname): continue
            if fkey in ob:
                raise DuplicateKeyError("file '{0}' clobbers key '{1}'"
                                        .format(fullname, fkey))
            with codecs.open(fullname, 'r', 'utf-8') as f:
                contents = f.read()
                if name.endswith('.json'):
                    contents = json.loads(contents)
                elif strip:
                    contents = contents.strip()
                ob[fkey] = contents

        for name in dirnames:
            if name == '_attachments':
                raise NotImplementedError("_attachments are not supported")
            fullpath = os.path.join(dirpath, name)
            if not predicate(fullpath): continue
            subkey, subthing = objects[fullpath]
            if subkey in ob:
                raise DuplicateKeyError("directory '{0}' clobbers key '{1}'"
                                        .format(fullpath,subkey))
            ob[subkey] = subthing

    return ob


def main():
    import sys
    try:
        directory = sys.argv[1]
    except IndexError:
        sys.stderr.write("Usage:\n\t{0} [directory]\n".format(sys.argv[0]))
        sys.exit(1)
    obj = load_design_doc(directory)
    sys.stdout.write(json.dumps(obj, indent=2))


if __name__ == "__main__":
    main()
