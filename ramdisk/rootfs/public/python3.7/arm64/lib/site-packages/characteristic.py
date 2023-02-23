"""
Python attributes without boilerplate.
"""

from __future__ import absolute_import, division, print_function

import hashlib
import linecache
import sys
import warnings


__version__ = "14.3.0"
__author__ = "Hynek Schlawack"
__license__ = "MIT"
__copyright__ = "Copyright 2014 Hynek Schlawack"

__all__ = [
    "Attribute",
    "NOTHING",
    "attributes",
    "immutable",
    "strip_leading_underscores",
    "with_cmp",
    "with_init",
    "with_repr",
]

PY26 = sys.version_info[0:2] == (2, 6)

# I'm sorry. :(
if sys.version_info[0] == 2:
    def exec_(code, locals_, globals_):
        exec("exec code in locals_, globals_")
else:  # pragma: no cover
    def exec_(code, locals_, globals_):
        exec(code, locals_, globals_)


class _Nothing(object):
    """
    Sentinel class to indicate the lack of a value when ``None`` is ambiguous.

    .. versionadded:: 14.0
    """
    def __repr__(self):
        return "NOTHING"


NOTHING = _Nothing()
"""
Sentinel to indicate the lack of a value when ``None`` is ambiguous.

.. versionadded:: 14.0
"""


def strip_leading_underscores(attribute_name):
    """
    Strip leading underscores from *attribute_name*.

    Used by default by the ``init_aliaser`` argument of :class:`Attribute`.

    :param attribute_name: The original attribute name to mangle.
    :type attribute_name: str

    :rtype: str
    """
    return attribute_name.lstrip("_")


class Attribute(object):
    """
    A representation of an attribute.

    In the simplest case, it only consists of a name but more advanced
    properties like default values are possible too.

    All attributes on the Attribute class are *read-only*.

    :param name: Name of the attribute.
    :type name: str

    :param exclude_from_cmp: Ignore attribute in :func:`with_cmp`.
    :type exclude_from_cmp: bool

    :param exclude_from_init: Ignore attribute in :func:`with_init`.
    :type exclude_from_init: bool

    :param exclude_from_repr: Ignore attribute in :func:`with_repr`.
    :type exclude_from_repr: bool

    :param exclude_from_immutable: Ignore attribute in :func:`immutable`.
    :type exclude_from_immutable: bool

    :param default_value: A value that is used whenever this attribute isn't
        passed as an keyword argument to a class that is decorated using
        :func:`with_init` (or :func:`attributes` with
        ``apply_with_init=True``).

        Therefore, setting this makes an attribute *optional*.

        Since a default value of `None` would be ambiguous, a special sentinel
        :data:`NOTHING` is used.  Passing it means the lack of a default value.

    :param default_factory: A factory that is used for generating default
        values whenever this attribute isn't passed as an keyword
        argument to a class that is decorated using :func:`with_init` (or
        :func:`attributes` with ``apply_with_init=True``).

        Therefore, setting this makes an attribute *optional*.
    :type default_factory: callable

    :param instance_of: If used together with :func:`with_init` (or
        :func:`attributes` with ``apply_with_init=True``), the passed value is
        checked whether it's an instance of the type passed here.  The
        initializer then raises :exc:`TypeError` on mismatch.
    :type instance_of: type

    :param init_aliaser: A callable that is invoked with the name of the
        attribute and whose return value is used as the keyword argument name
        for the ``__init__`` created by :func:`with_init` (or
        :func:`attributes` with ``apply_with_init=True``).  Uses
        :func:`strip_leading_underscores` by default to change ``_foo`` to
        ``foo``.  Set to ``None`` to disable aliasing.
    :type init_aliaser: callable

    :raises ValueError: If both ``default_value`` and ``default_factory`` have
        been passed.

    .. versionadded:: 14.0
    """
    __slots__ = [
        "name", "exclude_from_cmp", "exclude_from_init", "exclude_from_repr",
        "exclude_from_immutable", "default_value", "default_factory",
        "instance_of", "init_aliaser", "_kw_name",
    ]

    def __init__(self,
                 name,
                 exclude_from_cmp=False,
                 exclude_from_init=False,
                 exclude_from_repr=False,
                 exclude_from_immutable=False,
                 default_value=NOTHING,
                 default_factory=None,
                 instance_of=None,
                 init_aliaser=strip_leading_underscores):
        if (
                default_value is not NOTHING
                and default_factory is not None
        ):
            raise ValueError(
                "Passing both default_value and default_factory is "
                "ambiguous."
            )

        self.name = name
        self.exclude_from_cmp = exclude_from_cmp
        self.exclude_from_init = exclude_from_init
        self.exclude_from_repr = exclude_from_repr
        self.exclude_from_immutable = exclude_from_immutable

        self.default_value = default_value
        self.default_factory = default_factory
        self.instance_of = instance_of

        self.init_aliaser = init_aliaser
        if init_aliaser is not None:
            self._kw_name = init_aliaser(name)
        else:
            self._kw_name = name

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return (
            self.name == other.name and
            self.exclude_from_cmp == other.exclude_from_cmp and
            self.exclude_from_init == other.exclude_from_init and
            self.exclude_from_repr == other.exclude_from_repr and
            self.exclude_from_immutable == other.exclude_from_immutable and
            self.default_value == other.default_value and
            self.default_factory == other.default_factory and
            self.instance_of == other.instance_of
        )

    def __ne__(self, other):
        return not self == other

    def __repr__(self):
        return (
            "<Attribute(name={name!r}, exclude_from_cmp={exclude_from_cmp!r}, "
            "exclude_from_init={exclude_from_init!r}, exclude_from_repr="
            "{exclude_from_repr!r}, exclude_from_immutable="
            "{exclude_from_immutable!r}, default_value={default_value!r}, "
            "default_factory={default_factory!r}, instance_of={instance_of!r},"
            " init_aliaser={init_aliaser!r})>"
        ).format(
            name=self.name, exclude_from_cmp=self.exclude_from_cmp,
            exclude_from_init=self.exclude_from_init,
            exclude_from_repr=self.exclude_from_repr,
            exclude_from_immutable=self.exclude_from_immutable,
            default_value=self.default_value,
            default_factory=self.default_factory, instance_of=self.instance_of,
            init_aliaser=self.init_aliaser,
        )


def _ensure_attributes(attrs, defaults):
    """
    Return a list of :class:`Attribute` generated by creating new instances for
    all non-Attributes.
    """
    if defaults is not NOTHING:
        defaults = defaults or {}
        warnings.warn(
            "`defaults` has been deprecated in 14.0, please use the "
            "`Attribute` class instead.",
            DeprecationWarning,
            stacklevel=3,
        )
    else:
        defaults = {}

    rv = []
    for attr in attrs:
        if isinstance(attr, Attribute):
            if defaults != {}:
                raise ValueError(
                    "Mixing of the 'defaults' keyword argument and passing "
                    "instances of Attribute for 'attrs' is prohibited.  "
                    "Please don't use 'defaults' anymore, it has been "
                    "deprecated in 14.0."
                )
            else:
                rv.append(attr)
        else:
            rv.append(
                Attribute(
                    attr,
                    init_aliaser=None,
                    default_value=defaults.get(attr, NOTHING)
                )
            )

    return rv


def with_cmp(attrs):
    """
    A class decorator that adds comparison methods based on *attrs*.

    For that, each class is treated like a ``tuple`` of the values of *attrs*.
    But only instances of *identical* classes are compared!

    :param attrs: Attributes to work with.
    :type attrs: :class:`list` of :class:`str` or :class:`Attribute`\ s.
    """
    def attrs_to_tuple(obj):
        """
        Create a tuple of all values of *obj*'s *attrs*.
        """
        return tuple(getattr(obj, a.name) for a in attrs)

    def eq(self, other):
        """
        Automatically created by characteristic.
        """
        if other.__class__ is self.__class__:
            return attrs_to_tuple(self) == attrs_to_tuple(other)
        else:
            return NotImplemented

    def ne(self, other):
        """
        Automatically created by characteristic.
        """
        result = eq(self, other)
        if result is NotImplemented:
            return NotImplemented
        else:
            return not result

    def lt(self, other):
        """
        Automatically created by characteristic.
        """
        if other.__class__ is self.__class__:
            return attrs_to_tuple(self) < attrs_to_tuple(other)
        else:
            return NotImplemented

    def le(self, other):
        """
        Automatically created by characteristic.
        """
        if other.__class__ is self.__class__:
            return attrs_to_tuple(self) <= attrs_to_tuple(other)
        else:
            return NotImplemented

    def gt(self, other):
        """
        Automatically created by characteristic.
        """
        if other.__class__ is self.__class__:
            return attrs_to_tuple(self) > attrs_to_tuple(other)
        else:
            return NotImplemented

    def ge(self, other):
        """
        Automatically created by characteristic.
        """
        if other.__class__ is self.__class__:
            return attrs_to_tuple(self) >= attrs_to_tuple(other)
        else:
            return NotImplemented

    def hash_(self):
        """
        Automatically created by characteristic.
        """
        return hash(attrs_to_tuple(self))

    def wrap(cl):
        cl.__eq__ = eq
        cl.__ne__ = ne
        cl.__lt__ = lt
        cl.__le__ = le
        cl.__gt__ = gt
        cl.__ge__ = ge
        cl.__hash__ = hash_

        return cl

    attrs = [a
             for a in _ensure_attributes(attrs, NOTHING)
             if a.exclude_from_cmp is False]
    return wrap


def with_repr(attrs):
    """
    A class decorator that adds a human readable ``__repr__`` method to your
    class using *attrs*.

    :param attrs: Attributes to work with.
    :type attrs: ``list`` of :class:`str` or :class:`Attribute`\ s.
    """
    def repr_(self):
        """
        Automatically created by characteristic.
        """
        return "<{0}({1})>".format(
            self.__class__.__name__,
            ", ".join(a.name + "=" + repr(getattr(self, a.name))
                      for a in attrs)
        )

    def wrap(cl):
        cl.__repr__ = repr_
        return cl

    attrs = [a
             for a in _ensure_attributes(attrs, NOTHING)
             if a.exclude_from_repr is False]
    return wrap


def with_init(attrs, **kw):
    """
    A class decorator that wraps the ``__init__`` method of a class and sets
    *attrs* using passed *keyword arguments* before calling the original
    ``__init__``.

    Those keyword arguments that are used, are removed from the `kwargs` that
    is passed into your original ``__init__``.  Optionally, a dictionary of
    default values for some of *attrs* can be passed too.

    Attributes that are defined using :class:`Attribute` and start with
    underscores will get them stripped for the initializer arguments by default
    (this behavior is changeable on per-attribute basis when instantiating
    :class:`Attribute`.

    :param attrs: Attributes to work with.
    :type attrs: ``list`` of :class:`str` or :class:`Attribute`\ s.

    :raises ValueError: If the value for a non-optional attribute hasn't been
        passed as a keyword argument.
    :raises ValueError: If both *defaults* and an instance of
        :class:`Attribute` has been passed.

    .. deprecated:: 14.0
        Use :class:`Attribute` instead of ``defaults``.

    :param defaults: Default values if attributes are omitted on instantiation.
    :type defaults: ``dict`` or ``None``
    """
    attrs = [attr
             for attr in _ensure_attributes(attrs,
                                            defaults=kw.get("defaults",
                                                            NOTHING))
             if attr.exclude_from_init is False]

    # We cache the generated init methods for the same kinds of attributes.
    sha1 = hashlib.sha1()
    sha1.update(repr(attrs).encode("utf-8"))
    unique_filename = "<characteristic generated init {0}>".format(
        sha1.hexdigest()
    )

    script = _attrs_to_script(attrs)
    locs = {}
    bytecode = compile(script, unique_filename, "exec")
    exec_(bytecode, {"NOTHING": NOTHING, "attrs": attrs}, locs)
    init = locs["characteristic_init"]

    def wrap(cl):
        cl.__original_init__ = cl.__init__
        # In order of debuggers like PDB being able to step through the code,
        # we add a fake linecache entry.
        linecache.cache[unique_filename] = (
            len(script),
            None,
            script.splitlines(True),
            unique_filename
        )
        cl.__init__ = init
        return cl

    return wrap


_VALID_INITS = frozenset(["characteristic_init", "__init__"])


def immutable(attrs):
    """
    Class decorator that makes *attrs* of a class immutable.

    That means that *attrs* can only be set from an initializer.  If anyone
    else tries to set one of them, an :exc:`AttributeError` is raised.

    .. versionadded:: 14.0
    """
    # In this case, we just want to compare (native) strings.
    attrs = frozenset(attr.name if isinstance(attr, Attribute) else attr
                      for attr in _ensure_attributes(attrs, NOTHING)
                      if attr.exclude_from_immutable is False)

    def characteristic_immutability_sentry(self, attr, value):
        """
        Immutability sentry automatically created by characteristic.

        If an attribute is attempted to be set from any other place than an
        initializer, a TypeError is raised.  Else the original __setattr__ is
        called.
        """
        prev = sys._getframe().f_back
        if (
            attr not in attrs
            or
            prev is not None and prev.f_code.co_name in _VALID_INITS
        ):
            self.__original_setattr__(attr, value)
        else:
            raise AttributeError(
                "Attribute '{0}' of class '{1}' is immutable."
                .format(attr, self.__class__.__name__)
            )

    def wrap(cl):
        cl.__original_setattr__ = cl.__setattr__
        cl.__setattr__ = characteristic_immutability_sentry
        return cl

    return wrap


def _default_store_attributes(cls, attrs):
    """
    Store attributes in :attr:`characteristic_attributes` on the class.
    """
    cls.characteristic_attributes = attrs


def attributes(attrs, apply_with_cmp=True, apply_with_init=True,
               apply_with_repr=True, apply_immutable=False,
               store_attributes=_default_store_attributes, **kw):
    """
    A convenience class decorator that allows to *selectively* apply
    :func:`with_cmp`, :func:`with_repr`, :func:`with_init`, and
    :func:`immutable` to avoid code duplication.

    :param attrs: Attributes to work with.
    :type attrs: ``list`` of :class:`str` or :class:`Attribute`\ s.

    :param apply_with_cmp: Apply :func:`with_cmp`.
    :type apply_with_cmp: bool

    :param apply_with_init: Apply :func:`with_init`.
    :type apply_with_init: bool

    :param apply_with_repr: Apply :func:`with_repr`.
    :type apply_with_repr: bool

    :param apply_immutable: Apply :func:`immutable`.  The only one that is off
        by default.
    :type apply_immutable: bool

    :param store_attributes: Store the given ``attr``\ s on the class.
        Should accept two arguments, the class and the attributes, in that
        order. Note that attributes passed in will always be instances of
        :class:`Attribute`\ , (so simple string attributes will already have
        been converted). By default if unprovided, attributes are stored in
        a ``characteristic_attributes`` attribute on the class.
    :type store_attributes: callable

    :raises ValueError: If both *defaults* and an instance of
        :class:`Attribute` has been passed.

    .. versionadded:: 14.0
        Added possibility to pass instances of :class:`Attribute` in ``attrs``.

    .. versionadded:: 14.0
        Added ``apply_*``.

    .. versionadded:: 14.2
        Added ``store_attributes``.

    .. deprecated:: 14.0
        Use :class:`Attribute` instead of ``defaults``.

    :param defaults: Default values if attributes are omitted on instantiation.
    :type defaults: ``dict`` or ``None``

    .. deprecated:: 14.0
        Use ``apply_with_init`` instead of ``create_init``.  Until removal, if
        *either* if `False`, ``with_init`` is not applied.

    :param create_init: Apply :func:`with_init`.
    :type create_init: bool
    """

    create_init = kw.pop("create_init", None)
    if create_init is not None:
        apply_with_init = create_init
        warnings.warn(
            "`create_init` has been deprecated in 14.0, please use "
            "`apply_with_init`.", DeprecationWarning,
            stacklevel=2,
        )
    attrs = _ensure_attributes(attrs, defaults=kw.pop("defaults", NOTHING))

    if kw:
        raise TypeError(
            "attributes() got an unexpected keyword argument {0!r}".format(
                next(iter(kw)),
            )
        )

    def wrap(cl):
        store_attributes(cl, attrs)

        if apply_with_repr is True:
            cl = with_repr(attrs)(cl)
        if apply_with_cmp is True:
            cl = with_cmp(attrs)(cl)
        if apply_immutable is True:
            cl = immutable(attrs)(cl)
        if apply_with_init is True:
            cl = with_init(attrs)(cl)
        return cl
    return wrap


def _attrs_to_script(attrs):
    """
    Return a valid Python script of an initializer for *attrs*.
    """
    if all(a.default_value is NOTHING
           and a.default_factory is None
           and a.instance_of is None
           for a in attrs) and not PY26:
        # Simple version does not work with Python 2.6 because of
        # http://bugs.python.org/issue10221
        lines = _simple_init(attrs)
    else:
        lines = _verbose_init(attrs)

    return """\
def characteristic_init(self, *args, **kw):
    '''
    Attribute initializer automatically created by characteristic.

    The original `__init__` method is renamed to `__original_init__` and
    is called at the end with the initialized attributes removed from the
    keyword arguments.
    '''
    {setters}
    self.__original_init__(*args, **kw)
""".format(setters="\n    ".join(lines))


def _simple_init(attrs):
    """
    Create an init for *attrs* that doesn't care about defaults, default
    factories, or argument validators.  This is a common case thus it's worth
    optimizing for.
    """
    lines = ["try:"]
    for a in attrs:
        lines.append("    self.{a.name} = kw.pop('{a._kw_name}')".format(a=a))

    lines += [
        # We include "pass" here in case attrs is empty.  Otherwise the "try"
        # suite is empty.
        "    pass",
        "except KeyError as e:",
        "     raise ValueError(\"Missing keyword value for "
        "'%s'.\" % (e.args[0],))"
    ]
    return lines


def _verbose_init(attrs):
    """
    Create return a list of lines that initialize *attrs* while honoring
    default values.
    """
    lines = []
    for i, a in enumerate(attrs):
        # attrs is passed into the the exec later to enable default_value
        # and default_factory.  To find it, enumerate and 'i' are used.
        lines.append(
            "self.{a.name} = kw.pop('{a._kw_name}', {default})"
            .format(
                a=a,
                # Save a lookup for the common case of no default value.
                default="attrs[{i}].default_value".format(i=i)
                if a.default_value is not NOTHING else "NOTHING"
            )
        )
        if a.default_value is NOTHING:
            lines.append("if self.{a.name} is NOTHING:".format(a=a))
            if a.default_factory is None:
                lines.append(
                    "     raise ValueError(\"Missing keyword value for "
                    "'{a._kw_name}'.\")".format(a=a),
                )
            else:
                lines.append(
                    "    self.{a.name} = attrs[{i}].default_factory()"
                    .format(a=a, i=i)
                )
        if a.instance_of:
            lines.append(
                "if not isinstance(self.{a.name}, attrs[{i}].instance_of):\n"
                .format(a=a, i=i)
            )
            lines.append(
                "    raise TypeError(\"Attribute '{a.name}' must be an"
                " instance of '{type_name}'.\")"
                .format(a=a, type_name=a.instance_of.__name__)
            )

    return lines
