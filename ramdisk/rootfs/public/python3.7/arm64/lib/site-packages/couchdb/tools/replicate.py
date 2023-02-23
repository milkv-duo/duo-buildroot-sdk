#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2009 Maximillian Dornseif <md@hudora.de>
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

"""
This script replicates databases from one CouchDB server to an other.

This is mainly for backup purposes or "priming" a new server before
setting up trigger based replication. But you can also use the
'--continuous' option to set up automatic replication on newer
CouchDB versions.

Use 'python replicate.py --help' to get more detailed usage instructions.
"""

from couchdb import http, client, util
import optparse
import sys
import time
import fnmatch

def findpath(parser, s):
    '''returns (server url, path component)'''

    if s == '.':
        return client.DEFAULT_BASE_URL, ''
    if not s.startswith('http'):
        return client.DEFAULT_BASE_URL, s

    bits = util.urlparse(s)
    res = http.Resource('%s://%s/' % (bits.scheme, bits.netloc), None)
    parts = bits.path.split('/')[1:]
    if parts and not parts[-1]:
        parts = parts[:-1]

    cut = None
    for i in range(0, len(parts) + 1):
        try:
            data = res.get_json('/'.join(parts[:i]))[2]
        except Exception:
            data = None
        if data and 'couchdb' in data:
            cut = i

    if cut is None:
        raise parser.error("'%s' does not appear to be a CouchDB" % s)

    base = res.url + (parts[:cut] and '/'.join(parts[:cut]) or '')
    return base, '/'.join(parts[cut:])

def main():

    usage = '%prog [options] <source> <target>'
    parser = optparse.OptionParser(usage=usage)
    parser.add_option('--continuous',
        action='store_true',
        dest='continuous',
        help='trigger continuous replication in cochdb')
    parser.add_option('--compact',
        action='store_true',
        dest='compact',
        help='compact target database after replication')

    options, args = parser.parse_args()
    if len(args) != 2:
        raise parser.error('need source and target arguments')

    # set up server objects

    src, tgt = args
    sbase, spath = findpath(parser, src)
    source = client.Server(sbase)
    tbase, tpath = findpath(parser, tgt)
    target = client.Server(tbase)

    # check database name specs

    if '*' in tpath:
        raise parser.error('invalid target path: must be single db or empty')

    all = sorted(i for i in source if i[0] != '_') # Skip reserved names.
    if not spath:
        raise parser.error('source database must be specified')

    sources = [i for i in all if fnmatch.fnmatchcase(i, spath)]
    if not sources:
        raise parser.error("no source databases match glob '%s'" % spath)

    if len(sources) > 1 and tpath:
        raise parser.error('target path must be empty with multiple sources')
    elif len(sources) == 1:
        databases = [(sources[0], tpath)]
    else:
        databases = [(i, i) for i in sources]

    # do the actual replication

    for sdb, tdb in databases:

        start = time.time()
        print(sdb, '->', tdb)
        sys.stdout.flush()

        if tdb not in target:
            target.create(tdb)
            sys.stdout.write("created")
            sys.stdout.flush()

        sdb = '%s%s' % (sbase, util.urlquote(sdb, ''))
        if options.continuous:
            target.replicate(sdb, tdb, continuous=options.continuous)
        else:
            target.replicate(sdb, tdb)
        print('%.1fs' % (time.time() - start))
        sys.stdout.flush()

    if options.compact:
        for (sdb, tdb) in databases:
            print('compact', tdb)
            target[tdb].compact()

if __name__ == '__main__':
    main()
