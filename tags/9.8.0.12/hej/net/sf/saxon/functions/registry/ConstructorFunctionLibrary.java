////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.functions.registry;

import com.saxonica.functions.hof.AtomicConstructorFunction;
import net.sf.saxon.expr.ListConstructorFunction;
import net.sf.saxon.functions.CallableFunction;
import net.sf.saxon.functions.FunctionLibrary;
import net.sf.saxon.type.SpecificFunctionType;
import net.sf.saxon.Configuration;
import net.sf.saxon.expr.*;
import net.sf.saxon.lib.NamespaceConstant;
import net.sf.saxon.om.Function;
import net.sf.saxon.om.NamespaceResolver;
import net.sf.saxon.om.Sequence;
import net.sf.saxon.om.StructuredQName;
import net.sf.saxon.trans.SymbolicName;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.type.*;
import net.sf.saxon.value.AtomicValue;
import net.sf.saxon.value.EmptySequence;
import net.sf.saxon.value.SequenceType;

/**
 * The ConstructorFunctionLibrary represents the collection of constructor functions for atomic types. These
 * are provided for the built-in types such as xs:integer and xs:date, and also for user-defined atomic types.
 */

public class ConstructorFunctionLibrary implements FunctionLibrary {

    private Configuration config;

    /**
     * Create a SystemFunctionLibrary
     *
     * @param config the Configuration
     */

    public ConstructorFunctionLibrary(Configuration config) {
        this.config = config;
    }

//#ifdefined HOF

    /**
     * Test whether a function with a given name and arity is available; if so, return a function
     * item that can be dynamically called.
     * <p/>
     * <p>This supports the function-lookup() function in XPath 3.0.</p>
     *
     * @param functionName  the qualified name of the function being called
     * @param staticContext the static context to be used by the function, in the event that
     *                      it is a system function with dependencies on the static context
     * @return if a function of this name and arity is available for calling, then a corresponding
     *         function item; or null if the function does not exist
     * @throws net.sf.saxon.trans.XPathException
     *          in the event of certain errors, for example attempting to get a function
     *          that is private
     */
    public Function getFunctionItem(SymbolicName.F functionName, StaticContext staticContext) throws XPathException {
        if (functionName.getArity() != 1) {
            return null;
        }
        final String uri = functionName.getComponentName().getURI();
        final String localName = functionName.getComponentName().getLocalPart();
        final SchemaType type = config.getSchemaType(new StructuredQName("", uri, localName));
        if (type == null || type.isComplexType()) {
            return null;
        }
        final NamespaceResolver resolver = ((SimpleType) type).isNamespaceSensitive() ? staticContext.getNamespaceResolver() : null;
        if (type instanceof AtomicType) {
            return new AtomicConstructorFunction((AtomicType) type, resolver);
        } else if (type instanceof ListType) {
            return new ListConstructorFunction((ListType)type, resolver, true);
        } else {
            Callable callable = new Callable() {
                public Sequence call(XPathContext context, Sequence[] arguments) throws XPathException {
                    AtomicValue value = (AtomicValue) arguments[0].head();
                    if (value == null) {
                        return EmptySequence.getInstance();
                    }
                    return CastToUnion.cast(value, (UnionType) type, resolver, context.getConfiguration().getConversionRules());
                }
            };
            SequenceType returnType = ((UnionType) type).getResultTypeOfCast();
            return new CallableFunction(1, callable,
                                        new SpecificFunctionType(new SequenceType[]{SequenceType.OPTIONAL_ATOMIC}, returnType));
        }
    }

    //#endif


    public boolean isAvailable(SymbolicName.F functionName) {
        if (functionName.getArity() != 1) {
            return false;
        }
        final String uri = functionName.getComponentName().getURI();
        final String localName = functionName.getComponentName().getLocalPart();
        final SchemaType type = config.getSchemaType(new StructuredQName("", uri, localName));
        if (type == null || type.isComplexType()) {
            return false;
        }
        if (type.isAtomicType() && ((AtomicType) type).isAbstract()) {
            return false;
        }
        return type != AnySimpleType.getInstance();
    }

    /**
     * Bind a static function call, given the URI and local parts of the function name,
     * and the list of expressions supplied as arguments. This method is called at compile
     * time.
     *
     * @param functionName The QName of the function
     * @param arguments    The expressions supplied statically in the function call. The intention is
     *                     that the static type of the arguments (obtainable via getItemType() and getCardinality() may
     *                     be used as part of the binding algorithm.
     * @param env          The static context
     * @return An object representing the constructor function to be called, if one is found;
     *         null if no constructor function was found matching the required name and arity.
     * @throws net.sf.saxon.trans.XPathException
     *          if a function is found with the required name and arity, but
     *          the implementation of the function cannot be loaded or used; or if an error occurs
     *          while searching for the function; or if this function library "owns" the namespace containing
     *          the function call, but no function was found.
     */

    public Expression bind(SymbolicName.F functionName, Expression[] arguments, StaticContext env)
            throws XPathException {
        final String uri = functionName.getComponentName().getURI();
        final String localName = functionName.getComponentName().getLocalPart();
        boolean builtInNamespace = uri.equals(NamespaceConstant.SCHEMA);
        if (builtInNamespace) {
            // it's a constructor function: treat it as shorthand for a cast expression
            if (functionName.getArity() != 1) {
                throw new XPathException("A constructor function must have exactly one argument");
            }
            SimpleType type = Type.getBuiltInSimpleType(uri, localName);
            if (type != null) {
                if (type.isAtomicType()) {
                    if (((AtomicType) type).isAbstract()) {
                        XPathException err = new XPathException("Abstract type used in constructor function: {" + uri + '}' + localName);
                        err.setErrorCode("XPST0017");
                        err.setIsStaticError(true);
                        throw err;
                    } else {
                        CastExpression cast = new CastExpression(arguments[0], (AtomicType) type, true);
                        if (arguments[0] instanceof StringLiteral) {
                            cast.setOperandIsStringLiteral(true);
                        }
                        return cast;
                    }
                } else if (type.isUnionType()) {
                    NamespaceResolver resolver = env.getNamespaceResolver();
                    UnionConstructorFunction ucf = new UnionConstructorFunction((UnionType) type, resolver, true);
                    return new StaticFunctionCall(ucf, arguments);
                } else {
                    NamespaceResolver resolver = env.getNamespaceResolver();
                    ListConstructorFunction lcf = new ListConstructorFunction((ListType)type, resolver, true);
                    return new StaticFunctionCall(lcf, arguments);
                }
            } else {
                XPathException err = new XPathException("Unknown constructor function: {" + uri + '}' + localName);
                err.setErrorCode("XPST0017");
                err.setIsStaticError(true);
                throw err;
            }

        }

        // Now see if it's a constructor function for a user-defined type

        if (arguments.length == 1) {
            SchemaType st = config.getSchemaType(new StructuredQName("", uri, localName));
            if (st instanceof SimpleType) {
                if (st instanceof AtomicType) {
                    return new CastExpression(arguments[0], (AtomicType) st, true);
                } else if (st instanceof ListType && env.getXPathVersion() >= 30) {
                    NamespaceResolver resolver = env.getNamespaceResolver();
                    ListConstructorFunction lcf = new ListConstructorFunction((ListType) st, resolver, true);
                    return new StaticFunctionCall(lcf, arguments);
                } else if (((SimpleType) st).isUnionType() && env.getXPathVersion() >= 30) {
                    NamespaceResolver resolver = env.getNamespaceResolver();
                    UnionConstructorFunction ucf = new UnionConstructorFunction((UnionType) st, resolver, true);
                    return new StaticFunctionCall(ucf, arguments);
                }
            }
        }

        return null;
    }

    /**
     * This method creates a copy of a FunctionLibrary: if the original FunctionLibrary allows
     * new functions to be added, then additions to this copy will not affect the original, or
     * vice versa.
     *
     * @return a copy of this function library. This must be an instance of the original class.
     */

    public FunctionLibrary copy() {
        return this;
    }

}