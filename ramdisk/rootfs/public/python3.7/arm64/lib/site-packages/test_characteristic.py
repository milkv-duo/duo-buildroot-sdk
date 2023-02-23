from __future__ import absolute_import, division, print_function

import linecache
import sys
import warnings

import pytest

from characteristic import (
    Attribute,
    NOTHING,
    PY26,
    _attrs_to_script,
    _ensure_attributes,
    attributes,
    immutable,
    with_cmp,
    with_init,
    with_repr,
)

PY2 = sys.version_info[0] == 2

warnings.simplefilter("always")


class TestAttribute(object):
    def test_init_simple(self):
        """
        Instantiating with just the name initializes properly.
        """
        a = Attribute("foo")
        assert "foo" == a.name
        assert NOTHING is a.default_value

    def test_init_default_factory(self):
        """
        Instantiating with default_factory creates a proper descriptor for
        _default.
        """
        a = Attribute("foo", default_factory=list)
        assert NOTHING is a.default_value
        assert list() == a.default_factory()

    def test_init_default_value(self):
        """
        Instantiating with default_value initializes default properly.
        """
        a = Attribute("foo", default_value="bar")
        assert "bar" == a.default_value

    def test_ambiguous_defaults(self):
        """
        Instantiating with both default_value and default_factory raises
        ValueError.
        """
        with pytest.raises(ValueError):
            Attribute(
                "foo",
                default_value="bar",
                default_factory=lambda: 42
            )

    def test_missing_attr(self):
        """
        Accessing inexistent attributes still raises an AttributeError.
        """
        a = Attribute("foo")
        with pytest.raises(AttributeError):
            a.bar

    def test_alias(self):
        """
        If an attribute with a leading _ is defined, the initializer keyword
        is stripped of it.
        """
        a = Attribute("_private")
        assert "private" == a._kw_name

    def test_non_alias(self):
        """
        The keyword name of a non-private
        """
        a = Attribute("public")
        assert "public" == a._kw_name

    def test_dunder(self):
        """
        Dunder gets all _ stripped.
        """
        a = Attribute("__very_private")
        assert "very_private" == a._kw_name

    def test_init_aliaser_none(self):
        """
        No aliasing if init_aliaser is None.
        """
        a = Attribute("_private", init_aliaser=None)
        assert a.name == a._kw_name

    def test_init_aliaser(self):
        """
        Any callable works for aliasing.
        """
        a = Attribute("a", init_aliaser=lambda _: "foo")
        assert "foo" == a._kw_name

    def test_repr(self):
        """
        repr returns the correct string.
        """
        a = Attribute(
            name="name",
            exclude_from_cmp=True,
            exclude_from_init=True,
            exclude_from_repr=True,
            exclude_from_immutable=True,
            default_value=42,
            instance_of=str,
            init_aliaser=None
        )
        assert (
            "<Attribute(name='name', exclude_from_cmp=True, "
            "exclude_from_init=True, exclude_from_repr=True, "
            "exclude_from_immutable=True, "
            "default_value=42, default_factory=None, instance_of=<{0} 'str'>,"
            " init_aliaser=None)>"
        ).format("type" if PY2 else "class") == repr(a)

    def test_eq_different_types(self):
        """
        Comparing Attribute with something else returns NotImplemented.
        """
        assert NotImplemented == Attribute(name="name").__eq__(None)

    def test_eq_equal(self):
        """
        Equal Attributes are detected equal.
        """
        kw = {
            "name": "name",
            "exclude_from_cmp": True,
            "exclude_from_init": False,
            "exclude_from_repr": True,
            "exclude_from_immutable": False,
            "default_value": 42,
            "instance_of": int,
        }
        assert Attribute(**kw) == Attribute(**kw)

    def test_eq_unequal(self):
        """
        Equal Attributes are detected equal.
        """
        kw = {
            "name": "name",
            "exclude_from_cmp": True,
            "exclude_from_init": False,
            "exclude_from_repr": True,
            "exclude_from_immutable": False,
            "default_value": 42,
            "instance_of": int,
        }
        for arg in kw.keys():
            kw_mutated = dict(**kw)
            kw_mutated[arg] = "mutated"
            assert Attribute(**kw) != Attribute(**kw_mutated)


@with_cmp(["a", "b"])
class CmpC(object):
    def __init__(self, a, b):
        self.a = a
        self.b = b


class TestWithCmp(object):
    def test_equal(self):
        """
        Equal objects are detected as equal.
        """
        assert CmpC(1, 2) == CmpC(1, 2)
        assert not (CmpC(1, 2) != CmpC(1, 2))

    def test_unequal_same_class(self):
        """
        Unequal objects of correct type are detected as unequal.
        """
        assert CmpC(1, 2) != CmpC(2, 1)
        assert not (CmpC(1, 2) == CmpC(2, 1))

    def test_unequal_different_class(self):
        """
        Unequal objects of differnt type are detected even if their attributes
        match.
        """
        class NotCmpC(object):
            a = 1
            b = 2
        assert CmpC(1, 2) != NotCmpC()
        assert not (CmpC(1, 2) == NotCmpC())

    @pytest.mark.parametrize(
        "a,b", [
            ((1, 2), (2, 1)),
            ((1, 2), (1, 3)),
            (("a", "b"), ("b", "a")),
        ]
    )
    def test_lt(self, a, b):
        """
        __lt__ compares objects as tuples of attribute values.
        """
        assert CmpC(*a) < CmpC(*b)

    def test_lt_unordable(self):
        """
        __lt__ returns NotImplemented if classes differ.
        """
        assert NotImplemented == (CmpC(1, 2).__lt__(42))

    @pytest.mark.parametrize(
        "a,b", [
            ((1, 2),  (2, 1)),
            ((1, 2),  (1, 3)),
            ((1, 1),  (1, 1)),
            (("a", "b"), ("b", "a")),
            (("a", "b"), ("a", "b")),
        ]
    )
    def test_le(self, a, b):
        """
        __le__ compares objects as tuples of attribute values.
        """
        assert CmpC(*a) <= CmpC(*b)

    def test_le_unordable(self):
        """
        __le__ returns NotImplemented if classes differ.
        """
        assert NotImplemented == (CmpC(1, 2).__le__(42))

    @pytest.mark.parametrize(
        "a,b", [
            ((2, 1), (1, 2)),
            ((1, 3), (1, 2)),
            (("b", "a"), ("a", "b")),
        ]
    )
    def test_gt(self, a, b):
        """
        __gt__ compares objects as tuples of attribute values.
        """
        assert CmpC(*a) > CmpC(*b)

    def test_gt_unordable(self):
        """
        __gt__ returns NotImplemented if classes differ.
        """
        assert NotImplemented == (CmpC(1, 2).__gt__(42))

    @pytest.mark.parametrize(
        "a,b", [
            ((2, 1), (1, 2)),
            ((1, 3), (1, 2)),
            ((1, 1), (1, 1)),
            (("b", "a"), ("a", "b")),
            (("a", "b"), ("a", "b")),
        ]
    )
    def test_ge(self, a, b):
        """
        __ge__ compares objects as tuples of attribute values.
        """
        assert CmpC(*a) >= CmpC(*b)

    def test_ge_unordable(self):
        """
        __ge__ returns NotImplemented if classes differ.
        """
        assert NotImplemented == (CmpC(1, 2).__ge__(42))

    def test_hash(self):
        """
        __hash__ returns different hashes for different values.
        """
        assert hash(CmpC(1, 2)) != hash(CmpC(1, 1))

    def test_Attribute_exclude_from_cmp(self):
        """
        Ignores attribute if exclude_from_cmp=True.
        """
        @with_cmp([Attribute("a", exclude_from_cmp=True), "b"])
        class C(object):
            def __init__(self, a, b):
                self.a = a
                self.b = b

        assert C(42, 1) == C(23, 1)


@with_repr(["a", "b"])
class ReprC(object):
    def __init__(self, a, b):
        self.a = a
        self.b = b


class TestReprAttrs(object):
    def test_repr(self):
        """
        Test repr returns a sensible value.
        """
        assert "<ReprC(a=1, b=2)>" == repr(ReprC(1, 2))

    def test_Attribute_exclude_from_repr(self):
        """
        Ignores attribute if exclude_from_repr=True.
        """
        @with_repr([Attribute("a", exclude_from_repr=True), "b"])
        class C(object):
            def __init__(self, a, b):
                self.a = a
                self.b = b

        assert "<C(b=2)>" == repr(C(1, 2))


@with_init([Attribute("a"), Attribute("b")])
class InitC(object):
    def __init__(self):
        if self.a == self.b:
            raise ValueError


class TestWithInit(object):
    def test_sets_attributes(self):
        """
        The attributes are initialized using the passed keywords.
        """
        obj = InitC(a=1, b=2)
        assert 1 == obj.a
        assert 2 == obj.b

    def test_custom_init(self):
        """
        The class initializer is called too.
        """
        with pytest.raises(ValueError):
            InitC(a=1, b=1)

    def test_passes_args(self):
        """
        All positional parameters are passed to the original initializer.
        """
        @with_init(["a"])
        class InitWithArg(object):
            def __init__(self, arg):
                self.arg = arg

        obj = InitWithArg(42, a=1)
        assert 42 == obj.arg
        assert 1 == obj.a

    def test_passes_remaining_kw(self):
        """
        Keyword arguments that aren't used for attributes are passed to the
        original initializer.
        """
        @with_init(["a"])
        class InitWithKWArg(object):
            def __init__(self, kw_arg=None):
                self.kw_arg = kw_arg

        obj = InitWithKWArg(a=1, kw_arg=42)
        assert 42 == obj.kw_arg
        assert 1 == obj.a

    def test_does_not_pass_attrs(self):
        """
        The attributes are removed from the keyword arguments before they are
        passed to the original initializer.
        """
        @with_init(["a"])
        class InitWithKWArgs(object):
            def __init__(self, **kw):
                assert "a" not in kw
                assert "b" in kw
        InitWithKWArgs(a=1, b=42)

    def test_defaults(self):
        """
        If defaults are passed, they are used as fallback.
        """
        @with_init(["a", "b"], defaults={"b": 2})
        class InitWithDefaults(object):
            pass
        obj = InitWithDefaults(a=1)
        assert 2 == obj.b

    def test_missing_arg(self):
        """
        Raises `ValueError` if a value isn't passed.
        """
        with pytest.raises(ValueError) as e:
            InitC(a=1)
        assert "Missing keyword value for 'b'." == e.value.args[0]

    def test_defaults_conflict(self):
        """
        Raises `ValueError` if both defaults and an Attribute are passed.
        """
        with pytest.raises(ValueError) as e:
            @with_init([Attribute("a")], defaults={"a": 42})
            class C(object):
                pass
        assert (
            "Mixing of the 'defaults' keyword argument and passing instances "
            "of Attribute for 'attrs' is prohibited.  Please don't use "
            "'defaults' anymore, it has been deprecated in 14.0."
            == e.value.args[0]
        )

    def test_attribute(self):
        """
        String attributes are converted to Attributes and thus work.
        """
        @with_init(["a"])
        class C(object):
            pass
        o = C(a=1)
        assert 1 == o.a

    def test_default_factory(self):
        """
        The default factory is used for each instance of missing keyword
        argument.
        """
        @with_init([Attribute("a", default_factory=list)])
        class C(object):
            pass
        o1 = C()
        o2 = C()
        assert o1.a is not o2.a

    def test_underscores(self):
        """
        with_init takes keyword aliasing into account.
        """
        @with_init([Attribute("_a")])
        class C(object):
            pass
        c = C(a=1)
        assert 1 == c._a

    def test_plain_no_alias(self):
        """
        str-based attributes don't get aliased for backward-compatibility.
        """
        @with_init(["_a"])
        class C(object):
            pass
        c = C(_a=1)
        assert 1 == c._a

    def test_instance_of_fail(self):
        """
        Raise `TypeError` if an Attribute with an `instance_of` is is attempted
        to be set to a mismatched type.
        """
        @with_init([Attribute("a", instance_of=int)])
        class C(object):
            pass
        with pytest.raises(TypeError) as e:
            C(a="not an int!")
        assert (
            "Attribute 'a' must be an instance of 'int'."
            == e.value.args[0]
        )

    def test_instance_of_success(self):
        """
        Setting an attribute to a value that doesn't conflict with an
        `instance_of` declaration works.
        """
        @with_init([Attribute("a", instance_of=int)])
        class C(object):
            pass
        c = C(a=42)
        assert 42 == c.a

    def test_Attribute_exclude_from_init(self):
        """
        Ignores attribute if exclude_from_init=True.
        """
        @with_init([Attribute("a", exclude_from_init=True), "b"])
        class C(object):
            pass

        C(b=1)

    def test_deprecation_defaults(self):
        """
        Emits a DeprecationWarning if `defaults` is used.
        """
        with warnings.catch_warnings(record=True) as w:
            @with_init(["a"], defaults={"a": 42})
            class C(object):
                pass
        assert (
            '`defaults` has been deprecated in 14.0, please use the '
            '`Attribute` class instead.'
        ) == w[0].message.args[0]
        assert issubclass(w[0].category, DeprecationWarning)

    def test_linecache(self):
        """
        The created init method is added to the linecache so PDB shows it
        properly.
        """
        attrs = [Attribute("a")]

        @with_init(attrs)
        class C(object):
            pass

        assert tuple == type(linecache.cache[C.__init__.__code__.co_filename])

    def test_linecache_attrs_unique(self):
        """
        If the attributes are the same, only one linecache entry is created.
        Since the key within the cache is the filename, this effectively means
        that the filenames must be equal if the attributes are equal.
        """
        attrs = [Attribute("a")]

        @with_init(attrs[:])
        class C1(object):
            pass

        @with_init(attrs[:])
        class C2(object):
            pass

        assert (
            C1.__init__.__code__.co_filename
            == C2.__init__.__code__.co_filename
        )

    def test_linecache_different_attrs(self):
        """
        Different Attributes have different generated filenames.
        """
        @with_init([Attribute("a")])
        class C1(object):
            pass

        @with_init([Attribute("b")])
        class C2(object):
            pass

        assert (
            C1.__init__.__code__.co_filename
            != C2.__init__.__code__.co_filename
        )

    def test_no_attributes(self):
        """
        Specifying no attributes doesn't raise an exception.
        """
        @with_init([])
        class C(object):
            pass
        C()


class TestAttributes(object):
    def test_leaves_init_alone(self):
        """
        If *apply_with_init* or *create_init* is `False`, leave __init__ alone.
        """
        @attributes(["a"], apply_with_init=False)
        class C(object):
            pass

        @attributes(["a"], create_init=False)
        class CDeprecated(object):
            pass

        obj1 = C()
        obj2 = CDeprecated()

        with pytest.raises(AttributeError):
            obj1.a
        with pytest.raises(AttributeError):
            obj2.a

    def test_wraps_init(self):
        """
        If *create_init* is `True`, build initializer.
        """
        @attributes(["a", "b"], apply_with_init=True)
        class C(object):
            pass

        obj = C(a=1, b=2)
        assert 1 == obj.a
        assert 2 == obj.b

    def test_immutable(self):
        """
        If *apply_immutable* is `True`, make class immutable.
        """
        @attributes(["a"], apply_immutable=True)
        class ImmuClass(object):
            pass

        obj = ImmuClass(a=42)
        with pytest.raises(AttributeError):
            obj.a = "23"

    def test_apply_with_cmp(self):
        """
        Don't add cmp methods if *apply_with_cmp* is `False`.
        """
        @attributes(["a"], apply_with_cmp=False)
        class C(object):
            pass

        obj = C(a=1)
        if PY2:
            assert None is getattr(obj, "__eq__", None)
        else:
            assert object.__eq__ == C.__eq__

    def test_apply_with_repr(self):
        """
        Don't add __repr__ if *apply_with_repr* is `False`.
        """
        @attributes(["a"], apply_with_repr=False)
        class C(object):
            pass

        assert repr(C(a=1)).startswith("<test_characteristic.")

    def test_store_attributes(self):
        """
        store_attributes is called on the class to store the attributes that
        were passed in.
        """
        attrs = [Attribute("a"), Attribute("b")]

        @attributes(
            attrs, store_attributes=lambda cls, a: setattr(cls, "foo", a)
        )
        class C(object):
            pass

        assert C.foo == attrs

    def test_store_attributes_stores_Attributes(self):
        """
        The attributes passed to store_attributes are always instances of
        Attribute, even if they were simple strings when provided.
        """
        @attributes(["a", "b"])
        class C(object):
            pass

        assert C.characteristic_attributes == [Attribute("a"), Attribute("b")]

    def test_store_attributes_defaults_to_characteristic_attributes(self):
        """
        By default, store_attributes stores the attributes in
        `characteristic_attributes` on the class.
        """
        attrs = [Attribute("a")]

        @attributes(attrs)
        class C(object):
            pass

        assert C.characteristic_attributes == attrs

    def test_private(self):
        """
        Integration test for name mangling/aliasing.
        """
        @attributes([Attribute("_a")])
        class C(object):
            pass
        c = C(a=42)
        assert 42 == c._a

    def test_private_no_alias(self):
        """
        Integration test for name mangling/aliasing.
        """
        @attributes([Attribute("_a", init_aliaser=None)])
        class C(object):
            pass
        c = C(_a=42)
        assert 42 == c._a

    def test_deprecation_create_init(self):
        """
        Emits a DeprecationWarning if `create_init` is used.
        """
        with warnings.catch_warnings(record=True) as w:
            @attributes(["a"], create_init=False)
            class C(object):
                pass
        assert (
            '`create_init` has been deprecated in 14.0, please use '
            '`apply_with_init`.'
        ) == w[0].message.args[0]
        assert issubclass(w[0].category, DeprecationWarning)

    def test_deprecation_defaults(self):
        """
        Emits a DeprecationWarning if `defaults` is used.
        """
        with warnings.catch_warnings(record=True) as w:
            @attributes(["a"], defaults={"a": 42})
            class C(object):
                pass
        assert (
            '`defaults` has been deprecated in 14.0, please use the '
            '`Attribute` class instead.'
        ) == w[0].message.args[0]
        assert issubclass(w[0].category, DeprecationWarning)

    def test_does_not_allow_extra_keyword_arguments(self):
        """
        Keyword arguments other than the ones consumed are still TypeErrors.
        """
        with pytest.raises(TypeError) as e:
            @attributes(["a"], not_an_arg=12)
            class C(object):
                pass
        assert e.value.args == (
            "attributes() got an unexpected keyword argument 'not_an_arg'",
        )

    def test_no_attributes(self):
        """
        Specifying no attributes doesn't raise an exception.
        """
        @attributes([])
        class C(object):
            pass
        C()


class TestEnsureAttributes(object):
    def test_leaves_attribute_alone(self):
        """
        List items that are an Attribute stay an Attribute.
        """
        a = Attribute("a")
        assert a is _ensure_attributes([a], {})[0]

    def test_converts_rest(self):
        """
        Any other item will be transformed into an Attribute.
        """
        l = _ensure_attributes(["a"], {})
        assert isinstance(l[0], Attribute)
        assert "a" == l[0].name

    def test_defaults(self):
        """
        Legacy defaults are translated into default_value attributes.
        """
        l = _ensure_attributes(["a"], {"a": 42})
        assert 42 == l[0].default_value

    def test_defaults_Attribute(self):
        """
        Raises ValueError on defaults != {} and an Attribute within attrs.
        """
        with pytest.raises(ValueError):
            _ensure_attributes([Attribute("a")], defaults={"a": 42})


class TestImmutable(object):
    def test_bare(self):
        """
        In an immutable class, setting an definition-time attribute raises an
        AttributeError.
        """
        @immutable(["foo"])
        class ImmuClass(object):
            foo = "bar"

        i = ImmuClass()
        with pytest.raises(AttributeError):
            i.foo = "not bar"

    def test_Attribute(self):
        """
        Mutation is caught if user passes an Attribute instance.
        """
        @immutable([Attribute("foo")])
        class ImmuClass(object):
            def __init__(self):
                self.foo = "bar"

        i = ImmuClass()
        with pytest.raises(AttributeError):
            i.foo = "not bar"

    def test_init(self):
        """
        Changes within __init__ are allowed.
        """
        @immutable(["foo"])
        class ImmuClass(object):
            def __init__(self):
                self.foo = "bar"

        i = ImmuClass()
        assert "bar" == i.foo

    def test_with_init(self):
        """
        Changes in with_init's initializer are allowed.
        """
        @immutable(["foo"])
        @with_init(["foo"])
        class ImmuClass(object):
            pass

        i = ImmuClass(foo="qux")
        assert "qux" == i.foo

    def test_Attribute_exclude_from_immutable(self):
        """
        Ignores attribute if exclude_from_immutable=True.
        """
        @immutable([Attribute("a", exclude_from_immutable=True), "b"])
        class C(object):
            def __init__(self, a, b):
                self.a = a
                self.b = b

        c = C(1, 2)
        c.a = 3
        with pytest.raises(AttributeError):
            c.b = 4


class TestAttrsToScript(object):
    @pytest.mark.skipif(PY26, reason="Optimization works only on Python 2.7.")
    def test_optimizes_simple(self):
        """
        If no defaults and extra checks are passed, an optimized version is
        used on Python 2.7+.
        """
        attrs = [Attribute("a")]
        script = _attrs_to_script(attrs)
        assert "except KeyError as e:" in script


def test_nothing():
    """
    ``NOTHING`` has a sensible repr.
    """
    assert "NOTHING" == repr(NOTHING)


def test_doc():
    """
    The characteristic module has a docstring.
    """
    import characteristic
    assert characteristic.__doc__
