////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.expr;

import net.sf.saxon.Configuration;
import net.sf.saxon.expr.parser.*;
import net.sf.saxon.ma.arrays.ArrayItem;
import net.sf.saxon.ma.arrays.ArrayItemType;
import net.sf.saxon.ma.map.MapItem;
import net.sf.saxon.ma.map.MapType;
import net.sf.saxon.ma.map.TupleItemType;
import net.sf.saxon.ma.map.TupleType;
import net.sf.saxon.om.*;
import net.sf.saxon.trace.ExpressionPresenter;
import net.sf.saxon.trans.SaxonErrorCode;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.tree.iter.EmptyIterator;
import net.sf.saxon.type.*;
import net.sf.saxon.value.*;

import java.util.Map;


/**
 * A lookup expression is an expression of the form A?B. Here A must be a sequence of maps or arrays.
 * In the general case B is an expression that computes a key/index into the map or array; the case where
 * B is constant needs to be handled efficiently. The class also implements the unary lookup expression
 * ?B, which is interpreted as .?B. It does not handle the case A?* - that is handled as a LookupAllExpression.
 */

public class LookupExpression extends BinaryExpression {

    boolean isClassified = false;
    boolean isArrayLookup = false;
    boolean isMapLookup = false;
    boolean isObjectLookup = false;
    boolean isSingleContainer = false;
    boolean isSingleEntry = false;

    /**
     * Constructor
     *
     * @param start The left hand operand (which must always select a sequence of maps or arrays).
     * @param step  The step to be followed from each map/array in the start expression to yield a new
     *              sequence
     */

    public LookupExpression(Expression start, Expression step) {
        super(start, Token.QMARK, step);
    }

    @Override
    protected OperandRole getOperandRole(int arg) {
        return arg == 0 ? OperandRole.INSPECT : OperandRole.ABSORB;
    }

    @Override
    public String getExpressionName() {
        return "lookupExp";
    }


    /**
     * Determine the data type of the items returned by this expression
     *
     * @return the type of the expression, as far as this is known. Prior to type-checking,
     * the method returns {@link AnyItemType}
     */

    /*@NotNull*/
    public final ItemType getItemType() {
        if (isClassified) {
            if (isArrayLookup) {
                ItemType arrayType = getLhsExpression().getItemType();
                if (arrayType instanceof ArrayItemType) {
                    return ((ArrayItemType) arrayType).getMemberType().getPrimaryType();
                }
            } else {
                if (isMapLookup) {
                    ItemType mapType = getLhsExpression().getItemType();
                    if (mapType instanceof TupleItemType && getRhsExpression() instanceof StringLiteral) {
                        String fieldName = ((StringLiteral) getRhsExpression()).getStringValue();
                        SequenceType fieldType = ((TupleItemType) mapType).getFieldType(fieldName);
                        return fieldType == null ? ErrorType.getInstance() : fieldType.getPrimaryType();
                    } else if (mapType instanceof MapType) {
                        return ((MapType) mapType).getValueType().getPrimaryType();
                    }
                } else if (isObjectLookup) {
                    // Can only happen in PE or higher
                    ItemType objectType = getLhsExpression().getItemType();
                    if (objectType instanceof JavaExternalObjectType && getRhsExpression() instanceof StringLiteral) {
                        String fieldName = ((StringLiteral) getRhsExpression()).getStringValue();
                        Map<String, Function> methodMap =
                                getConfiguration().makeMethodMap(((JavaExternalObjectType)objectType).getJavaClass(), fieldName);
                        Function f = methodMap.get(fieldName);
                        if (f != null) {
                            return f.getFunctionItemType();
                        }

                    }
                }
            }
        }
        return AnyItemType.getInstance();
    }


    /**
     * Get the static type of the expression as a UType, following precisely the type
     * inference rules defined in the XSLT 3.0 specification.
     *
     * @param contextItemType not used
     * @return the static item type of the expression according to the XSLT 3.0 defined rules
     */
    @Override
    public UType getStaticUType(UType contextItemType) {
        return getItemType().getUType();
    }

    /**
     * Type-check the expression
     */

    /*@NotNull*/
    public Expression typeCheck(ExpressionVisitor visitor, ContextItemStaticInfo contextInfo) throws XPathException {

        Configuration config = visitor.getConfiguration();
        TypeHierarchy th = config.getTypeHierarchy();

        // Check the first operand
        getLhs().typeCheck(visitor, contextInfo);

        ItemType containerType = getLhsExpression().getItemType();
        isArrayLookup = containerType instanceof ArrayItemType;
        boolean isTupleLookup = containerType instanceof TupleType;
        isMapLookup = containerType instanceof MapType || isTupleLookup;
        if (containerType instanceof AnyExternalObjectType) {
            config.checkLicensedFeature(Configuration.LicenseFeature.PROFESSIONAL_EDITION, "use of lookup expressions on external objects", -1);
            isObjectLookup = true;
        }
        isSingleContainer = getLhsExpression().getCardinality() == StaticProperty.EXACTLY_ONE;

        if (!isArrayLookup && !isMapLookup && !isObjectLookup) {
            if (th.relationship(containerType, MapType.ANY_MAP_TYPE) == TypeHierarchy.DISJOINT &&
                    th.relationship(containerType, ArrayItemType.getInstance()) == TypeHierarchy.DISJOINT &&
                    th.relationship(containerType, AnyExternalObjectType.THE_INSTANCE) == TypeHierarchy.DISJOINT) {
                XPathException err = new XPathException("The left-hand operand of '?' must be a map or an array; the supplied expression is of type " + containerType, "XPTY0004");
                err.setLocation(getLocation());
                err.setIsTypeError(true);
                err.setFailingExpression(this);
                throw err;
            }
        }

        // Now check the second operand

        getRhs().typeCheck(visitor, contextInfo);
        RoleDiagnostic role = new RoleDiagnostic(RoleDiagnostic.BINARY_EXPR, "?", 1);
        TypeChecker tc = config.getTypeChecker(false);
        SequenceType req = BuiltInAtomicType.ANY_ATOMIC.zeroOrMore();
        if (isArrayLookup) {
            req = BuiltInAtomicType.INTEGER.zeroOrMore();
        } else if (isObjectLookup) {
            req = BuiltInAtomicType.STRING.oneOrMore();
        }
        setRhsExpression(tc.staticTypeCheck(getRhsExpression(), req, role, visitor));
        isSingleEntry = getRhsExpression().getCardinality() == StaticProperty.EXACTLY_ONE;

        if (isTupleLookup && getRhsExpression() instanceof StringLiteral) {
            String fieldName = ((StringLiteral)getRhsExpression()).getStringValue();
            if (((TupleType)containerType).getFieldType(fieldName) == null) {
                XPathException err = new XPathException("Field " + fieldName + " is not defined in the tuple type", SaxonErrorCode.SXTT0001);
                err.setIsTypeError(true);
                err.setLocation(getLocation());
                throw err;
            }
        }
        isClassified = true;
        return this;
    }

    public Expression optimize(ExpressionVisitor visitor, ContextItemStaticInfo contextInfo) throws XPathException {
        getLhs().optimize(visitor, contextInfo);
        getRhs().optimize(visitor, contextInfo);
        return this;
    }


    /**
     * Return the estimated cost of evaluating an expression. This is a very crude measure based
     * on the syntactic form of the expression (we have no knowledge of data values). We take
     * the cost of evaluating a simple scalar comparison or arithmetic expression as 1 (one),
     * and we assume that a sequence has length 5. The resulting estimates may be used, for
     * example, to reorder the predicates in a filter expression so cheaper predicates are
     * evaluated first.
     */
    @Override
    public int getCost() {
        return getLhsExpression().getCost() * getRhsExpression().getCost();
    }

    /**
     * An implementation of Expression must provide at least one of the methods evaluateItem(), iterate(), or process().
     * This method indicates which of these methods is provided directly. The other methods will always be available
     * indirectly, using an implementation that relies on one of the other methods.
     *
     * @return the implementation method, for example {@link #ITERATE_METHOD} or {@link #EVALUATE_METHOD} or
     * {@link #PROCESS_METHOD}
     */
    @Override
    public int getImplementationMethod() {
        return ITERATE_METHOD;
    }


    /**
     * Copy an expression. This makes a deep copy.
     *
     * @param rebindings a mutable list of (old binding, new binding) pairs
     *                   *                   that is used to update the bindings held in any
     *                   *                   local variable references that are copied.
     * @return the copy of the original expression
     */

    /*@NotNull*/
    public LookupExpression copy(RebindingMap rebindings) {
        LookupExpression exp = new LookupExpression(getLhsExpression().copy(rebindings), getRhsExpression().copy(rebindings));
        ExpressionTool.copyLocationInfo(this, exp);
        exp.isObjectLookup = isObjectLookup;
        exp.isArrayLookup = isArrayLookup;
        exp.isMapLookup = isMapLookup;
        exp.isSingleEntry = isSingleEntry;
        exp.isSingleContainer = isSingleContainer;
        return exp;
    }


    /**
     * Determine the static cardinality of the expression
     */

    public int computeCardinality() {
        if (isSingleContainer && isSingleEntry) {
            if (isArrayLookup) {
                ItemType arrayType = getLhsExpression().getItemType();
                if (arrayType instanceof ArrayItemType) {
                    return ((ArrayItemType) arrayType).getMemberType().getCardinality();
                }
            } else if (isMapLookup) {
                ItemType mapType = getLhsExpression().getItemType();
                if (mapType instanceof TupleItemType && getRhsExpression() instanceof StringLiteral) {
                    String fieldName = ((StringLiteral) getRhsExpression()).getStringValue();
                    SequenceType fieldType = ((TupleItemType) mapType).getFieldType(fieldName);
                    boolean isOptional = ((TupleItemType) mapType).isOptional(fieldName);
                    int card = fieldType.getCardinality();
                    if (isOptional) {
                        card = Cardinality.union(card, StaticProperty.ALLOWS_ZERO);
                    }
                    return fieldType == null ? StaticProperty.ALLOWS_ZERO : card;
                } else if (mapType instanceof MapType) {
                    return ((MapType) mapType).getValueType().getCardinality();
                }
            } else if (isObjectLookup) {
                return StaticProperty.ALLOWS_ZERO_OR_ONE;
            }
        }
        return StaticProperty.ALLOWS_ZERO_OR_MORE;
    }

    /**
     * Is this expression the same as another expression?
     */

    public boolean equals(Object other) {
        if (!(other instanceof LookupExpression)) {
            return false;
        }
        LookupExpression p = (LookupExpression) other;
        return getLhsExpression().isEqual(p.getLhsExpression()) && getRhsExpression().isEqual(p.getRhsExpression());
    }

    /**
     * get HashCode for comparing two expressions
     */

    public int computeHashCode() {
        return "LookupExpression".hashCode() ^ getLhsExpression().hashCode() ^ getRhsExpression().hashCode();
    }

    /**
     * Iterate the lookup-expression in a given context
     *
     * @param context the evaluation context
     */

    /*@NotNull*/
    public SequenceIterator<? extends Item> iterate(final XPathContext context) throws XPathException {
        Configuration config = context.getConfiguration();
        if (isArrayLookup) {
            if (isSingleContainer && isSingleEntry) {
                ArrayItem array = (ArrayItem) getLhsExpression().evaluateItem(context);
                IntegerValue subscript = (IntegerValue) getRhsExpression().evaluateItem(context);
                return array.get((int) subscript.longValue() - 1).iterate();
            } else if (isSingleEntry) {
                SequenceIterator<? extends Item> baseIterator = getLhsExpression().iterate(context);
                int subscript = (int) ((IntegerValue) getRhsExpression().evaluateItem(context)).longValue() - 1;
                return new MappingIterator<>(baseIterator,
                                           baseItem -> ((ArrayItem) baseItem).get(subscript).iterate());
            } else {
                SequenceIterator<? extends Item> baseIterator = getLhsExpression().iterate(context);
                GroundedValue<? extends Item> rhs = getRhsExpression().iterate(context).materialize();
                return new MappingIterator<>(baseIterator, baseItem -> new MappingIterator<>(
                        (SequenceIterator<? extends Item>) rhs.iterate(),
                        index -> ((ArrayItem) baseItem).get((int) ((IntegerValue) index).longValue() - 1).iterate()));
            }
        } else if (isMapLookup) {
            if (isSingleContainer && isSingleEntry) {
                MapItem map = (MapItem) getLhsExpression().evaluateItem(context);
                AtomicValue key = (AtomicValue) getRhsExpression().evaluateItem(context);
                GroundedValue value = map.get(key);
                return value == null ? EmptyIterator.emptyIterator() : value.iterate();
            } else if (isSingleEntry) {
                SequenceIterator<? extends Item> baseIterator = getLhsExpression().iterate(context);
                AtomicValue key = (AtomicValue) getRhsExpression().evaluateItem(context);
                return new MappingIterator<>(baseIterator, baseItem -> {
                    GroundedValue<? extends Item> value = ((MapItem) baseItem).get(key);
                    return value == null ? EmptyIterator.emptyIterator() : value.iterate();
                });
            } else {
                SequenceIterator<? extends Item> baseIterator = getLhsExpression().iterate(context);
                GroundedValue<? extends Item> rhs = getRhsExpression().iterate(context).materialize();
                return new MappingIterator<>(
                        baseIterator,
                        baseItem -> new MappingIterator<>(rhs.iterate(), index -> {
                            GroundedValue<? extends Item> value = ((MapItem) baseItem).get((AtomicValue) index);
                            return value == null ? EmptyIterator.emptyIterator() : value.iterate();
                        }));
            }
        } else if (isObjectLookup) {
            SequenceIterator<? extends Item<?>> baseIterator = getLhsExpression().iterate(context);
            GroundedValue<? extends Item<?>> rhs = getRhsExpression().iterate(context).materialize();
            String key = rhs.getStringValue();
            return new ItemMappingIterator<>(baseIterator,
                                           item -> (Item<?>)config.externalObjectAsMap((ObjectValue)item, key).get((StringValue)rhs));
        } else {
            SequenceIterator<? extends Item<?>> baseIterator = getLhsExpression().iterate(context);
            GroundedValue<? extends Item<?>> rhs = getRhsExpression().iterate(context).materialize();
            MappingFunction<? extends Item<?>, ? extends Item<?>> mappingFunction = baseItem -> {
                if (baseItem instanceof ArrayItem) {
                    MappingFunction<Item<?>, Item<?>> arrayAccess = index -> {
                        if (index instanceof IntegerValue) {
                            GroundedValue<? extends Item> member = ((ArrayItem) baseItem).get((int) ((IntegerValue) index).longValue() - 1);
                            return member.iterate();
                        } else {
                            XPathException exception = new XPathException(
                                    "An item on the LHS of the '?' operator is an array, but a value on the RHS of the operator (" +
                                            baseItem.toShortString() + ") is not an integer", "XPTY0004");
                            exception.setIsTypeError(true);
                            exception.setLocation(getLocation());
                            exception.setFailingExpression(LookupExpression.this);
                            throw exception;
                        }
                    };
                    SequenceIterator<? extends Item<?>> rhsIter = rhs.iterate();
                    return new MappingIterator<>(rhsIter, arrayAccess);
                } else if (baseItem instanceof MapItem) {
                    SequenceIterator<? extends Item<?>> rhsIter = rhs.iterate();
                    return new MappingIterator<>(rhsIter, key -> {
                        GroundedValue<? extends Item<?>> value = ((MapItem) baseItem).get((AtomicValue) key);
                        return value == null ? EmptyIterator.emptyIterator() : value.iterate();
                    });
                } else if (baseItem instanceof ObjectValue) {
                    if (!(rhs instanceof StringValue)) {
                        XPathException exception = new XPathException(
                                "An item on the LHS of the '?' operator is an external object, but a value on the RHS of the operator (" +
                                        baseItem.toShortString() + ") is not a singleton string", "XPTY0004");
                        exception.setIsTypeError(true);
                        exception.setLocation(getLocation());
                        exception.setFailingExpression(LookupExpression.this);
                        throw exception;
                    }
                    String key = rhs.getStringValue();
                    return config.externalObjectAsMap((ObjectValue) baseItem, key).get((StringValue) rhs).iterate();
                } else {
                    mustBeArrayOrMap(this, baseItem);
                    return null;
                }
            };
            return new MappingIterator(baseIterator, mappingFunction);

        }

    }

    protected static void mustBeArrayOrMap(Expression exp, Item baseItem) throws XPathException {
        XPathException exception = new XPathException("The items on the LHS of the '?' operator must be maps or arrays; but value (" +
                                                              baseItem.toShortString() + ") was supplied", "XPTY0004");
        exception.setIsTypeError(true);
        exception.setLocation(exp.getLocation());
        exception.setFailingExpression(exp);
        throw exception;
    }

    /**
     * Diagnostic print of expression structure. The abstract expression tree
     * is written to the supplied output destination.
     */

    public void export(ExpressionPresenter destination) throws XPathException {
        destination.startElement("lookup", this);
        getLhsExpression().export(destination);
        getRhsExpression().export(destination);
        destination.endElement();
    }

    /**
     * The toString() method for an expression attempts to give a representation of the expression
     * in an XPath-like form, but there is no guarantee that the syntax will actually be true XPath.
     * In the case of XSLT instructions, the toString() method gives an abstracted view of the syntax
     *
     * @return a representation of the expression as a string
     */

    public String toString() {
        String rhs;
        if (getRhsExpression() instanceof Literal) {
            Literal lit = (Literal)getRhsExpression();
            if (lit instanceof StringLiteral && NameChecker.isValidNCName(((StringLiteral) lit).getStringValue())) {
                rhs = ((StringLiteral) lit).getStringValue();
            } else if (lit.getValue() instanceof Int64Value) {
                rhs = lit.getValue().toString();
            } else {
                rhs = ExpressionTool.parenthesize(lit);
            }
        } else {
            rhs = ExpressionTool.parenthesize(getRhsExpression());
        }
        return ExpressionTool.parenthesize(getLhsExpression()) + "?" + rhs;
    }


}

