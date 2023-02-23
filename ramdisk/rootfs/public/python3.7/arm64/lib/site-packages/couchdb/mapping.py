# -*- coding: utf-8 -*-
#
# Copyright (C) 2007-2009 Christopher Lenz
# All rights reserved.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution.

"""Mapping from raw JSON data structures to Python objects and vice versa.

>>> from couchdb import Server
>>> server = Server()
>>> db = server.create('python-tests')

To define a document mapping, you declare a Python class inherited from
`Document`, and add any number of `Field` attributes:

>>> from datetime import datetime
>>> from couchdb.mapping import Document, TextField, IntegerField, DateTimeField
>>> class Person(Document):
...     name = TextField()
...     age = IntegerField()
...     added = DateTimeField(default=datetime.now)
>>> person = Person(name='John Doe', age=42)
>>> person.store(db) #doctest: +ELLIPSIS
<Person ...>
>>> person.age
42

You can then load the data from the CouchDB server through your `Document`
subclass, and conveniently access all attributes:

>>> person = Person.load(db, person.id)
>>> old_rev = person.rev
>>> person.name
u'John Doe'
>>> person.age
42
>>> person.added                #doctest: +ELLIPSIS
datetime.datetime(...)

To update a document, simply set the attributes, and then call the ``store()``
method:

>>> person.name = 'John R. Doe'
>>> person.store(db)            #doctest: +ELLIPSIS
<Person ...>

If you retrieve the document from the server again, you should be getting the
updated data:

>>> person = Person.load(db, person.id)
>>> person.name
u'John R. Doe'
>>> person.rev != old_rev
True

>>> del server['python-tests']
"""

import copy

from calendar import timegm
from datetime import date, datetime, time
from decimal import Decimal
from time import strptime, struct_time

from couchdb.design import ViewDefinition
from couchdb import util

__all__ = ['Mapping', 'Document', 'Field', 'TextField', 'FloatField',
           'IntegerField', 'LongField', 'BooleanField', 'DecimalField',
           'DateField', 'DateTimeField', 'TimeField', 'DictField', 'ListField',
           'ViewField']
__docformat__ = 'restructuredtext en'

DEFAULT = object()


class Field(object):
    """Basic unit for mapping a piece of data between Python and JSON.
    
    Instances of this class can be added to subclasses of `Document` to describe
    the mapping of a document.
    """

    def __init__(self, name=None, default=None):
        self.name = name
        self.default = default

    def __get__(self, instance, owner):
        if instance is None:
            return self
        value = instance._data.get(self.name)
        if value is not None:
            value = self._to_python(value)
        elif self.default is not None:
            default = self.default
            if callable(default):
                default = default()
            value = default
        return value

    def __set__(self, instance, value):
        if value is not None:
            value = self._to_json(value)
        instance._data[self.name] = value

    def _to_python(self, value):
        return util.utype(value)

    def _to_json(self, value):
        return self._to_python(value)


class MappingMeta(type):

    def __new__(cls, name, bases, d):
        fields = {}
        for base in bases:
            if hasattr(base, '_fields'):
                fields.update(base._fields)
        for attrname, attrval in d.items():
            if isinstance(attrval, Field):
                if not attrval.name:
                    attrval.name = attrname
                fields[attrname] = attrval
        d['_fields'] = fields
        return type.__new__(cls, name, bases, d)

MappingMetaClass = MappingMeta('MappingMetaClass', (object,), {})


class Mapping(MappingMetaClass):

    def __init__(self, **values):
        self._data = {}
        for attrname, field in self._fields.items():
            if attrname in values:
                setattr(self, attrname, values.pop(attrname))
            else:
                setattr(self, attrname, getattr(self, attrname))

    def __iter__(self):
        return iter(self._data)

    def __len__(self):
        return len(self._data or ())

    def __delitem__(self, name):
        del self._data[name]

    def __getitem__(self, name):
        return self._data[name]

    def __setitem__(self, name, value):
        self._data[name] = value

    def get(self, name, default=None):
        return self._data.get(name, default)

    def setdefault(self, name, default):
        return self._data.setdefault(name, default)

    def unwrap(self):
        return self._data

    @classmethod
    def build(cls, **d):
        fields = {}
        for attrname, attrval in d.items():
            if not attrval.name:
                attrval.name = attrname
            fields[attrname] = attrval
        d['_fields'] = fields
        return type('AnonymousStruct', (cls,), d)

    @classmethod
    def wrap(cls, data):
        instance = cls()
        instance._data = data
        return instance

    def _to_python(self, value):
        return self.wrap(value)

    def _to_json(self, value):
        return self.unwrap()


class ViewField(object):
    r"""Descriptor that can be used to bind a view definition to a property of
    a `Document` class.
    
    >>> class Person(Document):
    ...     name = TextField()
    ...     age = IntegerField()
    ...     by_name = ViewField('people', '''\
    ...         function(doc) {
    ...             emit(doc.name, doc);
    ...         }''')
    >>> Person.by_name
    <ViewDefinition '_design/people/_view/by_name'>
    
    >>> print(Person.by_name.map_fun)
    function(doc) {
        emit(doc.name, doc);
    }
    
    That property can be used as a function, which will execute the view.
    
    >>> from couchdb import Database
    >>> db = Database('python-tests')
    
    >>> Person.by_name(db, count=3)
    <ViewResults <PermanentView '_design/people/_view/by_name'> {'count': 3}>
    
    The results produced by the view are automatically wrapped in the
    `Document` subclass the descriptor is bound to. In this example, it would
    return instances of the `Person` class. But please note that this requires
    the values of the view results to be dictionaries that can be mapped to the
    mapping defined by the containing `Document` class. Alternatively, the
    ``include_docs`` query option can be used to inline the actual documents in
    the view results, which will then be used instead of the values.
    
    If you use Python view functions, this class can also be used as a
    decorator:
    
    >>> class Person(Document):
    ...     name = TextField()
    ...     age = IntegerField()
    ...
    ...     @ViewField.define('people')
    ...     def by_name(doc):
    ...         yield doc['name'], doc
    
    >>> Person.by_name
    <ViewDefinition '_design/people/_view/by_name'>

    >>> print(Person.by_name.map_fun)
    def by_name(doc):
        yield doc['name'], doc
    """

    def __init__(self, design, map_fun, reduce_fun=None, name=None,
                 language='javascript', wrapper=DEFAULT, **defaults):
        """Initialize the view descriptor.
        
        :param design: the name of the design document
        :param map_fun: the map function code
        :param reduce_fun: the reduce function code (optional)
        :param name: the actual name of the view in the design document, if
                     it differs from the name the descriptor is assigned to
        :param language: the name of the language used
        :param wrapper: an optional callable that should be used to wrap the
                        result rows
        :param defaults: default query string parameters to apply
        """
        self.design = design
        self.name = name
        self.map_fun = map_fun
        self.reduce_fun = reduce_fun
        self.language = language
        self.wrapper = wrapper
        self.defaults = defaults

    @classmethod
    def define(cls, design, name=None, language='python', wrapper=DEFAULT,
               **defaults):
        """Factory method for use as a decorator (only suitable for Python
        view code).
        """
        def view_wrapped(fun):
            return cls(design, fun, language=language, wrapper=wrapper,
                       **defaults)
        return view_wrapped

    def __get__(self, instance, cls=None):
        if self.wrapper is DEFAULT:
            wrapper = cls._wrap_row
        else:
            wrapper = self.wrapper
        return ViewDefinition(self.design, self.name, self.map_fun,
                              self.reduce_fun, language=self.language,
                              wrapper=wrapper, **self.defaults)


class DocumentMeta(MappingMeta):

    def __new__(cls, name, bases, d):
        for attrname, attrval in d.items():
            if isinstance(attrval, ViewField):
                if not attrval.name:
                    attrval.name = attrname
        return MappingMeta.__new__(cls, name, bases, d)

DocumentMetaClass = DocumentMeta('DocumentMetaClass', (object,), {})


class Document(DocumentMetaClass, Mapping):

    def __init__(self, id=None, **values):
        Mapping.__init__(self, **values)
        if id is not None:
            self.id = id

    def __repr__(self):
        return '<%s %r@%r %r>' % (type(self).__name__, self.id, self.rev,
                                  dict([(k, v) for k, v in self._data.items()
                                        if k not in ('_id', '_rev')]))

    def _get_id(self):
        if hasattr(self._data, 'id'): # When data is client.Document
            return self._data.id
        return self._data.get('_id')
    def _set_id(self, value):
        if self.id is not None:
            raise AttributeError('id can only be set on new documents')
        self._data['_id'] = value
    id = property(_get_id, _set_id, doc='The document ID')

    @property
    def rev(self):
        """The document revision.
        
        :rtype: basestring
        """
        if hasattr(self._data, 'rev'): # When data is client.Document
            return self._data.rev
        return self._data.get('_rev')

    def items(self):
        """Return the fields as a list of ``(name, value)`` tuples.
        
        This method is provided to enable easy conversion to native dictionary
        objects, for example to allow use of `mapping.Document` instances with
        `client.Database.update`.
        
        >>> class Post(Document):
        ...     title = TextField()
        ...     author = TextField()
        >>> post = Post(id='foo-bar', title='Foo bar', author='Joe')
        >>> sorted(post.items())
        [('_id', 'foo-bar'), ('author', u'Joe'), ('title', u'Foo bar')]
        
        :return: a list of ``(name, value)`` tuples
        """
        retval = []
        if self.id is not None:
            retval.append(('_id', self.id))
            if self.rev is not None:
                retval.append(('_rev', self.rev))
        for name, value in self._data.items():
            if name not in ('_id', '_rev'):
                retval.append((name, value))
        return retval

    @classmethod
    def load(cls, db, id):
        """Load a specific document from the given database.
        
        :param db: the `Database` object to retrieve the document from
        :param id: the document ID
        :return: the `Document` instance, or `None` if no document with the
                 given ID was found
        """
        doc = db.get(id)
        if doc is None:
            return None
        return cls.wrap(doc)

    def store(self, db):
        """Store the document in the given database."""
        db.save(self._data)
        return self

    @classmethod
    def query(cls, db, map_fun, reduce_fun, language='javascript', **options):
        """Execute a CouchDB temporary view and map the result values back to
        objects of this mapping.
        
        Note that by default, any properties of the document that are not
        included in the values of the view will be treated as if they were
        missing from the document. If you want to load the full document for
        every row, set the ``include_docs`` option to ``True``.
        """
        return db.query(map_fun, reduce_fun=reduce_fun, language=language,
                        wrapper=cls._wrap_row, **options)

    @classmethod
    def view(cls, db, viewname, **options):
        """Execute a CouchDB named view and map the result values back to
        objects of this mapping.
        
        Note that by default, any properties of the document that are not
        included in the values of the view will be treated as if they were
        missing from the document. If you want to load the full document for
        every row, set the ``include_docs`` option to ``True``.
        """
        return db.view(viewname, wrapper=cls._wrap_row, **options)

    @classmethod
    def _wrap_row(cls, row):
        doc = row.get('doc')
        if doc is not None:
            return cls.wrap(doc)
        data = row['value']
        data['_id'] = row['id']
        if 'rev' in data:  # When data is client.Document
            data['_rev'] = data['rev']
        return cls.wrap(data)


class TextField(Field):
    """Mapping field for string values."""
    _to_python = util.utype


class FloatField(Field):
    """Mapping field for float values."""
    _to_python = float


class IntegerField(Field):
    """Mapping field for integer values."""
    _to_python = int


class LongField(Field):
    """Mapping field for long integer values."""
    _to_python = util.ltype


class BooleanField(Field):
    """Mapping field for boolean values."""
    _to_python = bool


class DecimalField(Field):
    """Mapping field for decimal values."""

    def _to_python(self, value):
        return Decimal(value)

    def _to_json(self, value):
        return util.utype(value)


class DateField(Field):
    """Mapping field for storing dates.
    
    >>> field = DateField()
    >>> field._to_python('2007-04-01')
    datetime.date(2007, 4, 1)
    >>> field._to_json(date(2007, 4, 1))
    '2007-04-01'
    >>> field._to_json(datetime(2007, 4, 1, 15, 30))
    '2007-04-01'
    """

    def _to_python(self, value):
        if isinstance(value, util.strbase):
            try:
                value = date(*strptime(value, '%Y-%m-%d')[:3])
            except ValueError:
                raise ValueError('Invalid ISO date %r' % value)
        return value

    def _to_json(self, value):
        if isinstance(value, datetime):
            value = value.date()
        return value.isoformat()


class DateTimeField(Field):
    """Mapping field for storing date/time values.

    >>> field = DateTimeField()
    >>> field._to_python('2007-04-01T15:30:00Z')
    datetime.datetime(2007, 4, 1, 15, 30)
    >>> field._to_python('2007-04-01T15:30:00.009876Z')
    datetime.datetime(2007, 4, 1, 15, 30, 0, 9876)
    >>> field._to_json(datetime(2007, 4, 1, 15, 30, 0))
    '2007-04-01T15:30:00Z'
    >>> field._to_json(datetime(2007, 4, 1, 15, 30, 0, 9876))
    '2007-04-01T15:30:00.009876Z'
    >>> field._to_json(date(2007, 4, 1))
    '2007-04-01T00:00:00Z'
    """

    def _to_python(self, value):
        if isinstance(value, util.strbase):
            try:
                split_value = value.split('.') # strip out microseconds
                if len(split_value) == 1:   # No microseconds provided
                    value = split_value[0]
                    value = value.rstrip('Z')  #remove timezone separator
                    value = datetime.strptime(value, '%Y-%m-%dT%H:%M:%S')
                else:
                    value = value.rstrip('Z')
                    value = datetime.strptime(value, '%Y-%m-%dT%H:%M:%S.%f')

            except ValueError:
                raise ValueError('Invalid ISO date/time %r' % value)
        return value

    def _to_json(self, value):
        if isinstance(value, struct_time):
            value = datetime.utcfromtimestamp(timegm(value))
        elif not isinstance(value, datetime):
            value = datetime.combine(value, time(0))
        return value.isoformat() + 'Z'


class TimeField(Field):
    """Mapping field for storing times.

    >>> field = TimeField()
    >>> field._to_python('15:30:00')
    datetime.time(15, 30)
    >>> field._to_json(time(15, 30))
    '15:30:00'
    >>> field._to_json(datetime(2007, 4, 1, 15, 30))
    '15:30:00'
    """

    def _to_python(self, value):
        if isinstance(value, util.strbase):
            try:
                value = value.split('.', 1)[0] # strip out microseconds
                value = time(*strptime(value, '%H:%M:%S')[3:6])
            except ValueError:
                raise ValueError('Invalid ISO time %r' % value)
        return value

    def _to_json(self, value):
        if isinstance(value, datetime):
            value = value.time()
        return value.replace(microsecond=0).isoformat()


class DictField(Field):
    """Field type for nested dictionaries.
    
    >>> from couchdb import Server
    >>> server = Server()
    >>> db = server.create('python-tests')

    >>> class Post(Document):
    ...     title = TextField()
    ...     content = TextField()
    ...     author = DictField(Mapping.build(
    ...         name = TextField(),
    ...         email = TextField()
    ...     ))
    ...     extra = DictField()

    >>> post = Post(
    ...     title='Foo bar',
    ...     author=dict(name='John Doe',
    ...                 email='john@doe.com'),
    ...     extra=dict(foo='bar'),
    ... )
    >>> post.store(db) #doctest: +ELLIPSIS
    <Post ...>
    >>> post = Post.load(db, post.id)
    >>> post.author.name
    u'John Doe'
    >>> post.author.email
    u'john@doe.com'
    >>> post.extra
    {u'foo': u'bar'}

    >>> del server['python-tests']
    """
    def __init__(self, mapping=None, name=None, default=None):
        default = default or {}
        Field.__init__(self, name=name, default=lambda: default.copy())
        self.mapping = mapping

    def _to_python(self, value):
        if self.mapping is None:
            return value
        else:
            return self.mapping.wrap(value)

    def _to_json(self, value):
        if self.mapping is None:
            return value
        if not isinstance(value, Mapping):
            value = self.mapping(**value)
        return value.unwrap()


class ListField(Field):
    """Field type for sequences of other fields.

    >>> from couchdb import Server
    >>> server = Server()
    >>> db = server.create('python-tests')

    >>> class Post(Document):
    ...     title = TextField()
    ...     content = TextField()
    ...     pubdate = DateTimeField(default=datetime.now)
    ...     comments = ListField(DictField(Mapping.build(
    ...         author = TextField(),
    ...         content = TextField(),
    ...         time = DateTimeField()
    ...     )))

    >>> post = Post(title='Foo bar')
    >>> post.comments.append(author='myself', content='Bla bla',
    ...                      time=datetime.now())
    >>> len(post.comments)
    1
    >>> post.store(db) #doctest: +ELLIPSIS
    <Post ...>
    >>> post = Post.load(db, post.id)
    >>> comment = post.comments[0]
    >>> comment['author']
    u'myself'
    >>> comment['content']
    u'Bla bla'
    >>> comment['time'] #doctest: +ELLIPSIS
    u'...T...Z'

    >>> del server['python-tests']
    """

    def __init__(self, field, name=None, default=None):
        default = default or []
        Field.__init__(self, name=name, default=lambda: copy.copy(default))
        if type(field) is type:
            if issubclass(field, Field):
                field = field()
            elif issubclass(field, Mapping):
                field = DictField(field)
        self.field = field

    def _to_python(self, value):
        return self.Proxy(value, self.field)

    def _to_json(self, value):
        return [self.field._to_json(item) for item in value]


    class Proxy(list):

        def __init__(self, list, field):
            self.list = list
            self.field = field

        def __lt__(self, other):
            return self.list < other

        def __le__(self, other):
            return self.list <= other

        def __eq__(self, other):
            return self.list == other

        def __ne__(self, other):
            return self.list != other

        def __gt__(self, other):
            return self.list > other

        def __ge__(self, other):
            return self.list >= other

        def __repr__(self):
            return repr(self.list)

        def __str__(self):
            return str(self.list)

        def __unicode__(self):
            return util.utype(self.list)

        def __delitem__(self, index):
            if isinstance(index, slice):
                self.__delslice__(index.start, index.stop)
            else:
                del self.list[index]

        def __getitem__(self, index):
            if isinstance(index, slice):
                return self.__getslice__(index.start, index.stop)
            return self.field._to_python(self.list[index])

        def __setitem__(self, index, value):
            if isinstance(index, slice):
                self.__setslice__(index.start, index.stop, value)
            else:
                self.list[index] = self.field._to_json(value)

        def __delslice__(self, i, j):
            del self.list[i:j]

        def __getslice__(self, i, j):
            return ListField.Proxy(self.list[i:j], self.field)

        def __setslice__(self, i, j, seq):
            self.list[i:j] = (self.field._to_json(v) for v in seq)

        def __contains__(self, value):
            for item in self.list:
                if self.field._to_python(item) == value:
                    return True
            return False

        def __iter__(self):
            for index in range(len(self)):
                yield self[index]

        def __len__(self):
            return len(self.list)

        def __nonzero__(self):
            return bool(self.list)

        def append(self, *args, **kwargs):
            if args or not isinstance(self.field, DictField):
                if len(args) != 1:
                    raise TypeError('append() takes exactly one argument '
                                    '(%s given)' % len(args))
                value = args[0]
            else:
                value = kwargs
            self.list.append(self.field._to_json(value))

        def count(self, value):
            return [i for i in self].count(value)

        def extend(self, list):
            for item in list:
                self.append(item)

        def index(self, value):
            return self.list.index(self.field._to_json(value))

        def insert(self, idx, *args, **kwargs):
            if args or not isinstance(self.field, DictField):
                if len(args) != 1:
                    raise TypeError('insert() takes exactly 2 arguments '
                                    '(%s given)' % len(args))
                value = args[0]
            else:
                value = kwargs
            self.list.insert(idx, self.field._to_json(value))

        def remove(self, value):
            return self.list.remove(self.field._to_json(value))

        def pop(self, *args):
            return self.field._to_python(self.list.pop(*args))
