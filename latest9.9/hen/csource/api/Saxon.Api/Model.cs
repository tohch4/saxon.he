﻿using System;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using JConfiguration = net.sf.saxon.Configuration;
using JNamePool = net.sf.saxon.om.NamePool;
using JAtomicValue = net.sf.saxon.value.AtomicValue;
using JFunction = net.sf.saxon.om.Function;
using JItem = net.sf.saxon.om.Item;
using JZeroOrOne = net.sf.saxon.om.ZeroOrOne;
using JOne = net.sf.saxon.om.One;
using JEmptySequence = net.sf.saxon.value.EmptySequence;
using JSequenceExtent = net.sf.saxon.value.SequenceExtent;
using JConversionResult = net.sf.saxon.type.ConversionResult;
using JValidationFailure = net.sf.saxon.type.ValidationFailure;
using JSequenceIterator = net.sf.saxon.om.SequenceIterator;
using JSequenceXdmIterator = net.sf.saxon.s9api.XdmSequenceIterator;
using JStandardNames = net.sf.saxon.om.StandardNames;
using JStructuredQName = net.sf.saxon.om.StructuredQName;
using JXPathContext = net.sf.saxon.expr.XPathContext;
using JDotNetReceiver = net.sf.saxon.dotnet.DotNetReceiver;
using JDotNetObjectValue = net.sf.saxon.dotnet.DotNetObjectValue;
using JBigDecimal = java.math.BigDecimal;
using JArrayList = java.util.ArrayList;
using JCharSequence = java.lang.CharSequence;
using JSequence = net.sf.saxon.om.Sequence;
using JNodeInfo = net.sf.saxon.om.NodeInfo;
using JAxisInfo = net.sf.saxon.om.AxisInfo;
using JInscopeNamespaceResolver = net.sf.saxon.om.InscopeNamespaceResolver;
using JNameChecker = net.sf.saxon.om.NameChecker;
using JSingletonIterator = net.sf.saxon.tree.iter.SingletonIterator;
using JQNameValue = net.sf.saxon.value.QNameValue;
using JStringValue = net.sf.saxon.value.StringValue;
using JInt64Value = net.sf.saxon.value.Int64Value;
using JBigDecimalValue = net.sf.saxon.value.BigDecimalValue;
using JFloatValue = net.sf.saxon.value.FloatValue;
using JDoubleValue = net.sf.saxon.value.DoubleValue;
using JBooleanValue = net.sf.saxon.value.BooleanValue;
using JAnyURIValue = net.sf.saxon.value.AnyURIValue;
using JNumericValue = net.sf.saxon.value.NumericValue;
using JStringToDouble11 = net.sf.saxon.value.StringToDouble11;
using JIntegerValue = net.sf.saxon.value.IntegerValue;
using JNameTest = net.sf.saxon.pattern.NameTest;
using JAtomicType = net.sf.saxon.type.AtomicType;
using JSchemaType = net.sf.saxon.type.SchemaType;
using JType = net.sf.saxon.type.Type;
using JStringToDouble = net.sf.saxon.type.StringToDouble;
using JSequenceTool = net.sf.saxon.om.SequenceTool;
using JExplicitLocation = net.sf.saxon.expr.parser.ExplicitLocation;
using JHashTrieMap = net.sf.saxon.ma.map.HashTrieMap;
using JMapItem = net.sf.saxon.ma.map.MapItem;
using JArrayItem = net.sf.saxon.ma.arrays.ArrayItem;
using JKeyValuePair = net.sf.saxon.ma.map.KeyValuePair;
using JSimpleArrayItem = net.sf.saxon.ma.arrays.SimpleArrayItem;
using JDecimalValue = net.sf.saxon.value.DecimalValue;
using JObjectValue = net.sf.saxon.value.ObjectValue;
using JGroundedValue = net.sf.saxon.om.GroundedValue;
using JQName = net.sf.saxon.s9api.QName;
using JItemConsumer = net.sf.saxon.om.ItemConsumer;
using JXdmValue = net.sf.saxon.s9api.XdmValue;

namespace Saxon.Api
{

    /// <summary>
    /// A value in the XDM data model. A value is a sequence of zero or more
    /// items, each item being an atomic value, a node, or a function item.
    /// </summary>
    /// <remarks>
    /// <para>An <c>XdmValue</c> is immutable.</para>
    /// <para>A sequence consisting of a single item <i>may</i> be represented
    /// as an instance of <see cref="XdmItem"/>, which is a subtype of <c>XdmValue</c>. However,
    /// there is no guarantee that all single-item sequences will be instances of
	/// <c>XdmItem</c>: if you want to ensure this, use the <see cref="Simplify"/> property.</para>
    /// <para>There are various ways of creating an <c>XdmValue</c>. To create an atomic
    /// value, use one of the constructors on <see cref="XdmAtomicValue"/> (which is a subtype of <c>XdmValue</c>).
    /// To construct an <see cref="XdmNode"/> (another subtype) by parsing an XML document, or by wrapping a DOM document,
	/// use a <see cref="DocumentBuilder"/>. To create a sequence of values, use the <see cref="Append(XdmValue)"/>
    /// method on this class to form a list from individual items or sublists.</para>
    /// <para>An <c>XdmValue</c> is also returned as the result of evaluating a query
    /// using the XQuery and XPath interfaces.</para>
    /// <para>The subtype <see cref="XdmEmptySequence"/> represents an empty sequence: an
    /// <c>XdmValue</c> of length zero. Again, there is no guarantee that every empty sequence
    /// will be represented as an instance of <c>XdmEmptySequence</c>, unless you use
    /// the <c>Simplify</c> property.</para>
    /// </remarks>

    [Serializable]
    public class XdmValue : IXdmEnumerable<XdmItem>, IEnumerable
    {

        internal JGroundedValue value;

        // Internal constructor

        internal XdmValue() { }

        /// <summary>
        /// Create a value from a collection of items
        /// </summary>
        /// <param name="items">An enumerable collection providing the items to make up the sequence. Every
        /// member of this collection must be an instance of <c>XdmItem</c>
        /// </param>

        public XdmValue(IEnumerable<XdmItem> items)
        {
            JArrayList list = new JArrayList();
            foreach (XdmItem c in items)
            {
                list.add((JItem)c.Unwrap());
            }
            value = JSequenceExtent.makeSequenceExtent(list);
        }

        /// <summary>
        /// Create a new <c>XdmValue</c> by concatenating the sequences of items in 
        /// this <c>XdmValue</c> and another <c>XdmValue</c>
        /// </summary>
        /// <remarks>
        /// Neither of the input <c>XdmValue</c> objects is modified by this operation
        /// </remarks>
        /// <param name="otherValue">
        /// The other <c>XdmValue</c>, whose items are to be appended to the items from this <c>XdmValue</c>
        /// </param>
        
        public XdmValue Append(XdmValue otherValue) {
            JArrayList list = new JArrayList();
            foreach (XdmItem item in this) {
                list.add(item.Unwrap());
            }
            foreach (XdmItem item in otherValue) {
                list.add(item.Unwrap());
            }
            JGroundedValue gv = JSequenceExtent.makeSequenceExtent(list);
            return FromGroundedValue(gv);
        }


        /// <summary>
        /// Create an <c>XdmValue</c> from an underlying Saxon <c>Sequence</c> object.
        /// This method is provided for the benefit of applications that need to mix
        /// use of the Saxon .NET API with direct use of the underlying objects
        /// and methods offered by the Java implementation.
        /// </summary>
        /// <param name="value">An object representing an XDM value in the
        /// underlying Saxon implementation. If the parameter is null,
        /// the method returns null.</param>
        /// <returns>An <c>XdmValue</c> that wraps the underlying Saxon XDM value
        /// representation.</returns>

        public static XdmValue Wrap(JSequence value)
        {
            if (value == null) {
                return XdmEmptySequence.INSTANCE;
            }
            JGroundedValue gv;
            try
            {
                gv = value.materialize();
            }
            catch (Exception e) {
                throw new DynamicError(e.Message);
            }
            XdmValue result;
            if (gv.getLength() == 0)
            {

                return XdmEmptySequence.INSTANCE;
            } else if (gv.getLength() == 1) {
                JItem first = gv.head();
                if (first is JAtomicValue)
                {
                    result = new XdmAtomicValue();
                    result.value = (JAtomicValue)first;
                    return result;
                }
                else if (first is JNodeInfo)
                {
                    result = new XdmNode();
                    result.value = (JNodeInfo)first;
                    return result;
                }
                else if (first is JZeroOrOne)
                {
                    return Wrap(((JZeroOrOne)value).head());
                }
                else if (first is JOne)
                {
                    return Wrap(((JOne)value).head());
                }
                else if (first is JMapItem)
                {
                    result = new XdmMap();
                    result.value = (JMapItem)first;
                    return result;
                }
                else if (first is JArrayItem)
                {
                    result = new XdmArray();
                    result.value = (JArrayItem)first;
                    return result;
                }
                else if (first is JFunction)
                {
                    result = new XdmFunctionItem();
                    result.value = (JFunction)first;
                    return result;
                }
                else if (first is JObjectValue)
                {
                    result = new XdmExternalObjectValue(((JObjectValue)first).getObject());
                    return result;
                }
                else {
                    result = new XdmValue();
                    result.value = first;
                    return result;
                }

            } 
            else
            {
                return FromGroundedValue(gv);
            }
            
        }

        static internal XdmValue FromGroundedValue(JGroundedValue value) {
            XdmValue result = new XdmValue();
            result.value = value;
            return result;
        }

        static internal JXdmValue FromGroundedValueToJXdmValue(JGroundedValue value) {
            return net.sf.saxon.s9api.XdmValue.wrap(value);
        }

        /// <summary>
        /// Make an XDM value from a .NET object. 
		/// </summary>
		/// <remarks>
		/// The supplied object may be any of the following:
        /// <list>
        /// <item>An instance of <c>XdmValue</c> (for example an <c>XdmAtomicValue</c>, 
        /// <c>XdmMap</c>, <c>XdmArray</c> or <c>XdmNode</c>), which is returned unchanged</item>
        /// <item>An instance of Saxon's Java class <c>net.sf.saxon.om.Sequence</c>, which is wrapped
        /// as an <c>XdmValue</c></item>
        /// <item>An instance of <c>IDictionary</c> (which is wrapped as an <c>XdmMap</c> using the method <see cref="XdmMap.MakeMap"/>)</item>
        /// <item>An array of objects, which are converted by applying these rules recursively,
		/// and then wrapped as an <c>XdmArray</c>.</item>
		/// </list>
		/// </remarks>
        /// <param name="o">The supplied object</param>
        /// <returns>the result of conversion if successful</returns>
        public static XdmValue MakeValue(object o) {
           
            if (o == null)
            {
                return null;
            }
            if (o is JSequence)
            {
                return XdmValue.Wrap((JSequence)o);
            }
            else if (o is XdmValue)
            {
                return (XdmValue)o;
            }
            else if (o is IDictionary)
            {
                return XdmMap.MakeMap((IDictionary)o);
            }
            else if (o.GetType().IsArray) {
                return XdmArray.MakeArray((object[])o);
            } else if (o is IEnumerable) {
                return XdmValue.MakeSequence((IEnumerable)o);
            }

            else
            {
                return XdmAtomicValue.MakeAtomicValue(o);

            }

        }

        private static XdmValue MakeSequence(IEnumerable o)
        {
            JArrayList list = new JArrayList();

            if (o is string)
            {
                return XdmAtomicValue.MakeAtomicValue((object)o);
            }
            foreach (object oi in o)
            {
                XdmValue v = XdmValue.MakeValue(oi);
                if (v is XdmItem)
                {
                    list.add((JItem)v.Unwrap());
                }
                else
                {
                    list.add(new XdmArray(v).Unwrap());
                }

            }
            JSequence value = new JSequenceExtent(list);
            return XdmValue.Wrap(value);
        }


        /// <summary>
        /// Extract the underlying Saxon <c>Sequence</c> object from an <c>XdmValue</c>.
        /// This method is provided for the benefit of applications that need to mix
        /// use of the Saxon .NET API with direct use of the underlying objects
        /// and methods offered by the Java implementation.
        /// </summary>
        /// <returns>An object representing the XDM value in the
        /// underlying Saxon implementation.</returns>


        public JSequence Unwrap()
        {
            return value;
        }

        /// <summary>
        /// Get the sequence of items in the form of an <c>IList</c>
        /// </summary>
        /// <returns>
        /// The list of items making up this XDM value. Each item in the list
        /// will be an object of type <c>XdmItem</c>
        /// </returns>        

        public IList<XdmItem> GetList()
        {
            if (value == null)
            {
                return new List<XdmItem>();
            }
            else if (value is JItem)
            {
                IList<XdmItem> list = new List<XdmItem>(1);
                list.Add((XdmItem)XdmValue.Wrap(value));
                return list;
            }
            else
            {
                IList<XdmItem> list = new List<XdmItem>();
                JSequenceIterator iter = value.iterate();
                while (true)
                {
                    JItem jitem = iter.next();
                    if (jitem == null)
                    {
                        break;
                    }
                    list.Add((XdmItem)XdmValue.Wrap(jitem));
                }
                return list;
            }
        }

        IEnumerator IEnumerable.GetEnumerator() {
            if (value == null)
            {
                return EmptyEnumerator<XdmItem>.INSTANCE;
            }
            else if (value is JItem)
            {
                return new SequenceEnumerator<XdmItem>(new JSequenceXdmIterator(JSingletonIterator.makeIterator((JItem)value)));
            }
            else
            {
                return new SequenceEnumerator<XdmItem>(new JSequenceXdmIterator(value.iterate()));
            }
        }

        /// <summary>
        /// Get the sequence of items in the form of an <c>IXdmEnumerator</c>
        /// </summary>
        /// <returns>
        /// An enumeration over the list of items making up this XDM value. Each item in the list
        /// will be an object of type <c>XdmItem</c>
        /// </returns>    

       IEnumerator<XdmItem> IEnumerable<XdmItem>.GetEnumerator()
        {
            if (value == null)
            {
                return (IEnumerator<XdmItem>)EmptyEnumerator<XdmItem>.INSTANCE;
            }
            else if (value is JItem)
            {
                return (IEnumerator<XdmItem>)new SequenceEnumerator<XdmItem>(new JSequenceXdmIterator(JSingletonIterator.makeIterator((JItem)value)));
            }
            else
            {
                return (IEnumerator<XdmItem>)new SequenceEnumerator<XdmItem>(new JSequenceXdmIterator(value.iterate()));
            }
        }
        

        /// <summary>
        /// Get the i'th item in the value, counting from zero.
        /// </summary>
        /// <param name="i">the item that irequired, counting the first item in the sequence as item zero</param>
        /// <returns>the i'th item in the sequence making up the value, counting from zero.</returns>
        public XdmItem ItemAt(int i)
        {
            if (i < 0 || i >= Count) {
                throw new IndexOutOfRangeException("" + i);
            }
            try {
                JItem item = JSequenceTool.itemAt(value, i);
                return (XdmItem)XdmItem.Wrap(item);
            }
            catch (net.sf.saxon.trans.XPathException e) {
                throw new StaticError(e);
            }

        }


        /// <summary>
        /// Create a string representation of the value. The is the result of serializing
        /// the value using the adaptive serialization method.
        /// </summary>
        /// <returns>a string representation of the value</returns>
        public override String ToString() {
            return XdmValue.FromGroundedValueToJXdmValue(value).toString();

        }




        /// <summary>
        /// Return a new XdmValue containing the nodes present in this XdmValue,
        /// with duplicates eliminated, and sorted into document order
        /// </summary>
        /// <returns>the same nodes, sorted into document order, with duplicates eliminated</returns>
        public XdmValue DocumentOrder() {
            try
            {
                JSequenceIterator iter = value.iterate();
                JSequenceIterator sorted = new net.sf.saxon.expr.sort.DocumentOrderIterator(iter, net.sf.saxon.expr.sort.GlobalOrderComparer.getInstance());
                return XdmValue.Wrap(sorted.materialize());

            }
            catch (net.sf.saxon.trans.XPathException e) {
                throw new StaticError(e);
            }

        }

        /// <summary>
        /// Get the number of items in the sequence
        /// </summary>
        /// <returns>
        /// The number of items in the sequence. Note that for a single item (including
        /// a map or an array) this always returns 1 (one).
        /// </returns> 

        public int Count
        {
            get
            {
                if (value == null)
                {
                    return 0;
                }
                else if (value is JItem)
                {
                    return 1;
                }
                else
                {
                    return value.getLength();
                }
            }
        }

        /// <summary>
		/// Simplify an XDM value: that is, reduce it to the simplest possible form.
		/// </summary>
		/// <remarks>
        /// <list>
        /// <item>If the sequence is empty, the result will be an instance of <c>XdmEmptySequence</c></item>
        /// <item>If the sequence is a single node, the result will be an instance of <c>XdmNode</c></item>
        /// <item>If it is a single atomic value, it will be an instance of <c>XdmAtomicValue</c></item>
        /// <item>If it is a map, it will be an instance of <c>XdmMap</c></item>
        /// <item>If it is an array, it will be an instance of <c>XdmArray</c></item>
        /// <item>If it is any other kind of function, it will be an instance of <c>XdmFunctionItem</c></item>
        /// <item>If it is a wrapper around a .NET object, it will be an instance of <c>XdmExternalObjectValue</c></item>
        /// </list>
		/// </remarks>
		/// <returns>The XDM value reduced to the simplest possible form</returns>

        public XdmValue Simplify
        {
            get
            {
                switch (value.getLength())
                {
                    case 0:
                        if (this is XdmEmptySequence)
                        {
                            return this;
                        }
                        return XdmEmptySequence.INSTANCE;

                    case 1:
                        if (this is XdmItem)
                        {
                            return this;
                        }
                        return XdmValue.Wrap(value);

                    default:
                        return this;
                }
            }
        }

    }



    /// <summary inherits="XdmItem">
    /// The class <c>XdmExternalObjectValue</c> represents an XDM item that wraps an external .NET object.
    /// As such, it is outside the scope of the W3C XDM specification (but permitted as an extension).
    /// </summary>
    [Serializable]
    public class XdmExternalObjectValue : XdmItem
    {

        /// <summary>
        /// Constructor to create an <c>XdmExternalObjectValue</c> that wraps a supplied .NET object
        /// </summary>
        /// <param name="o">the supplied .NET object</param>
        public XdmExternalObjectValue(object o)
        {
            value = new JDotNetObjectValue(o);
        }

        /// <summary>
        /// Determine whether the item is an atomic value
        /// </summary>
        /// <returns>
        /// false (the item is not an atomic value)
        /// </returns>
        public override bool IsAtomic()
        {
            return false;
        }



        /// <summary>
        /// Compare two external objects for equality. Two instances of XdmExternalObjectValue are equal
        /// if the .NET objects that they wrap are equal.
        /// </summary>
        /// <param name="other">the object to be compared</param>
        /// <returns>tre if the other object is an XdmExternalObjectValue and the two wrapped objects are 
        /// equal under the equals method.</returns>
        public bool Equals(XdmExternalObjectValue other) {
            return other is XdmExternalObjectValue && value == other.value;
        }

        /// <summary>
        /// Return a hash code for the object. This respects the semantics of equals(Object)
        /// </summary>
        /// <returns>a suitable hash code</returns>
        public override int GetHashCode() {
            return ((java.lang.Object)value).hashCode();
        }

        /// <summary>
        /// Get the wrapped .NET object
        /// </summary>
        /// <returns>the wrapped object</returns>
        public object GetExternalObject()
        {
            return ((JObjectValue)value).getObject();
        }

        /// <summary>
        /// Get the result of converting the external value to a string.
        /// </summary>
        /// <returns>the result of applying <c>ToString()</c> to the wrapped external object</returns>
        public override string ToString()
        {
            return GetExternalObject().ToString();
        }
    }
    
    /// <summary inherits="XdmValue">
    /// The class <c>XdmItem</c> represents an item in a sequence, as defined
    /// by the XDM data model. An item may be an atomic value, a node, a function (including maps
    /// and arrays), or an external object.
    /// </summary>
    /// <remarks>
    /// <para>An item is a member of a sequence, but it can also be considered as
    /// a sequence (of length one) in its own right. <c>XdmItem</c> is a subtype
    /// of <c>XdmValue</c> because every item in the XDM data model is also a
    /// value.</para>
    /// <para>It cannot be assumed that every sequence of length one will always be 
    /// represented by an <c>XdmItem</c>. It is quite possible for an <c>XdmValue</c>
	/// that is not an <c>XdmItem</c> to hold a singleton sequence. Use <see cref="XdmValue.Simplify"/> 
    /// to ensure that a singleton is represented as an <c>XdmItem</c>.</para>
    /// </remarks> 

    [Serializable]
    public abstract class XdmItem : XdmValue
    {

        /// <summary>
        /// Determine whether the item is an atomic value
        /// </summary>
        /// <returns>
        /// true if the item is an atomic value, false if it is a node, function, or external object
        /// </returns>

        public abstract bool IsAtomic();
        
        /// <summary>
		/// Get the string value of the item. 
		/// </summary>
		/// <remarks>
        /// <list>
        /// <item>For an atomic value, the result is the same as casting the value to a string.</item>
        /// <item>For a node, the method returns the string
		/// value of the node. This is not the same as the result of <see cref="XdmNode.ToString()"/>, which
        /// returns the result of serializing the node.</item>
        /// <item>For a function, including a map or array, the result is an error.</item>
        /// <item>For an external object, the result is the same as the result of calling <c>ToString()</c>
        /// on the external object.</item>
		/// </list>
		/// <para>In all cases the result is the same as applying the XPath <c>string()</c> function.</para>
		/// </remarks>
        /// <returns>The result of converting the item to a string</returns>
        
        public String GetStringValue() {
            return ((JItem)value).getStringValue();
        }

        internal static net.sf.saxon.s9api.XdmItem FromXdmItemItemToJXdmItem(XdmItem value)
        {
            return (net.sf.saxon.s9api.XdmItem)net.sf.saxon.s9api.XdmValue.wrap(value== null ? null : value.value);
        }

        /// <summary>
        /// Determine whether this item matches a given item type
        /// </summary>
        /// <param name="typei">typei the item type to be tested against this item</param>
        /// <returns>true if the item matches this item type, false if it does not match.</returns>
        public bool Matches(XdmItemType typei)
        {
            return typei.Matches(this);
        }
    }

    /// <summary inherits="XdmItem">
    /// The class <c>XdmAtomicValue</c> represents an item in an XDM sequence
    /// that is an atomic value. The value may belong to any of the 19 primitive types
    /// defined in XML Schema, or to a type derived from these primitive types, or to 
    /// the XDM-specific type <c>xs:untypedAtomic</c>
    /// </summary>
    /// <remarks>
    /// Note that there is no guarantee that every <c>XdmValue</c> comprising a single
    /// atomic value will be an instance of this class. To force this, use the <c>Simplify</c>
    /// property of the <c>XdmValue</c>.
    /// </remarks>

    [Serializable]
    public class XdmAtomicValue : XdmItem
    {
		// Internal constructor

        internal XdmAtomicValue() { }

        /// <summary>
        /// Determine whether the item is an atomic value
        /// </summary>
        /// <returns>
        /// true (the item is an atomic value)
        /// </returns>

        public override bool IsAtomic()
        {
            return true;
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:string</c>
        /// </summary>
        /// <param name="str">The string value</param>

        public XdmAtomicValue(String str)
        {
            this.value = new JStringValue(str);
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:integer</c> from a supplied <c>long</c>
        /// </summary>
        /// <param name="i">The integer value</param>

        public XdmAtomicValue(long i)
        {
            this.value = JInt64Value.makeDerived(i, (JAtomicType)XdmAtomicType.LONG.Unwrap().getUnderlyingItemType());
        }


        /// <summary>
        /// Construct an atomic value of type <c>xs:integer</c> from a supplied <c>long</c>
        /// </summary>
        /// <param name="i">The integer value</param>

        public XdmAtomicValue(int i)
        {
            this.value = JInt64Value.makeDerived(i, (JAtomicType)XdmAtomicType.INT.Unwrap().getUnderlyingItemType());
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:integer</c> from a supplied <c>byte</c>
        /// </summary>
        /// <param name="i">The integer value, in the range -128 to +127</param>
        public XdmAtomicValue(byte i)
        {
            this.value = JInt64Value.makeDerived(i, (JAtomicType)XdmAtomicType.BYTE.Unwrap().getUnderlyingItemType());
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:decimal</c>
        /// </summary>
        /// <param name="d">The decimal value</param>

        public XdmAtomicValue(decimal d)
        {
            this.value = new JBigDecimalValue(new JBigDecimal(d.ToString()));
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:float</c>
        /// </summary>
        /// <param name="f">The float value</param>        

        public XdmAtomicValue(float f)
        {
            this.value = new JFloatValue(f);
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:double</c>
        /// </summary>
        /// <param name="d">The double value</param>

        public XdmAtomicValue(double d)
        {
            this.value = new JDoubleValue(d);
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:boolean</c>
        /// </summary>
        /// <param name="b">The boolean value</param>

        public XdmAtomicValue(bool b)
        {
            this.value = JBooleanValue.get(b);
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:anyURI</c>
        /// </summary>
        /// <param name="u">The uri value</param>

        public XdmAtomicValue(Uri u)
        {
            this.value = new JAnyURIValue(u.ToString());
        }

        /// <summary>
        /// Construct an atomic value of type <c>xs:QName</c>
        /// </summary>
        /// <param name="q">The QName value</param>                

        public XdmAtomicValue(QName q)
        {
            this.value = new JQNameValue(
                q.Prefix, q.Uri, q.LocalName);
		}

		/// <summary>
		/// Construct an atomic value of a given type
		/// </summary>
		/// <param name="lexicalForm">The string representation of the value (any value that is acceptable
		/// in the lexical space, as defined by XML Schema Part 2). Whitespace normalization as defined by
		/// the target type will be applied to the value.</param>
		/// <param name="type">The type given as an <c>XdmAtomicType</c></param>

        public XdmAtomicValue(String lexicalForm, XdmAtomicType type) {
            net.sf.saxon.type.ItemType it = type.Unwrap().getUnderlyingItemType();
            if (!it.isPlainType()) {
                throw new StaticError(new net.sf.saxon.s9api.SaxonApiException("Requested type is not atomic"));
            }
            if (((JAtomicType)it).isAbstract()) {
                throw new StaticError(new net.sf.saxon.s9api.SaxonApiException("Requested type is not namespace-sensitive"));

            }
            try
            {
                net.sf.saxon.type.StringConverter Converter = ((JAtomicType)it).getStringConverter(type.Unwrap().getConversionRules());
                this.value = Converter.convertString(lexicalForm).asAtomic();
            }
            catch (Exception ex) { }
            
        }

        /// <summary>
        /// Construct an atomic value of a given built-in or user-defined type
        /// </summary>
        /// <example>
        ///   <code>XdmAtomicValue("abcd", QName.XDT_UNTYPED_ATOMIC, processor)</code>
        ///   <para>creates an untyped atomic value containing the string "abcd"</para>
        /// </example>
        /// <param name="lexicalForm">The string representation of the value (any value that is acceptable
        /// in the lexical space, as defined by XML Schema Part 2). Whitespace normalization as defined by
        /// the target type will be applied to the value.</param>
        /// <param name="type">The QName giving the name of the target type. This must be an atomic
        /// type, and it must not be a type that is namespace-sensitive (QName, NOTATION, or types derived
        /// from these). If the type is a user-defined type then its definition must be present
        /// in the schema cache maintained by the <c>SchemaManager</c>.</param> 
        /// <param name="processor">The <c>Processor</c> object. This is needed for looking up user-defined
        /// types, and also because some conversions are context-sensitive, for example they depend on the
        /// implicit timezone or the choice of XML 1.0 versus XML 1.1 for validating names.</param>
        /// <exception cref="ArgumentException">Thrown if the type is unknown or unsuitable, or if the supplied string is not
        /// a valid lexical representation of a value of the given type.</exception>

        public XdmAtomicValue(String lexicalForm, QName type, Processor processor)
        {
			JConfiguration jconfig = processor.Implementation;
            int fp = jconfig.getNamePool().getFingerprint(type.Uri, type.LocalName);
            if (fp == -1)
            {
                throw new ArgumentException("Unknown name " + type);
            }
			JSchemaType st = jconfig.getSchemaType(new JStructuredQName("", type.Uri.ToString(), type.LocalName));
            if (st == null)
            {
                throw new ArgumentException("Unknown type " + type);
            }
            if (!(st is JAtomicType))
            {
                throw new ArgumentException("Specified type " + type + " is not atomic");
            }
            if (((JAtomicType)st).isNamespaceSensitive())
            {
                throw new ArgumentException("Specified type " + type + " is namespace-sensitive");
            }
			JConversionResult result = ((JAtomicType)st).getStringConverter(jconfig.getConversionRules()).convertString((JCharSequence)lexicalForm);
 
            if (result is JValidationFailure)
            {
                throw new ArgumentException(((JValidationFailure)result).getMessage());
            }
            this.value = (JAtomicValue)result;
        }


        
        /// <summary>
		/// Create an atomic value of a type appropriate to the supplied value. 
		/// </summary>
		/// <remarks>
		/// The supplied value must be one of the following:
        /// <list>
        /// <item>An instance of the Saxon Java class <c>net.sf.saxon.value.AtomicValue</c></item>
        /// <item>A <c>Boolean</c> - returns an instance of <c>xs:boolean</c></item>
        /// <item>A (signed) <c>int</c>, <c>long</c>, <c>short</c>, or <c>byte</c> - returns an instance of <c>xs:integer</c></item>
        /// <item>A <c>Char</c> - TODO ???????</item>
        /// <item>A <c>String</c> - returns an instance of <c>xs:string</c></item>
        /// <item>A <c>Double</c> - returns an instance of <c>xs:double</c></item>
        /// <item>A <c>Float</c> - returns an instance of <c>xs:float</c></item>
        /// <item>A <c>decimal</c> - returns an instance of <c>xs:decimal</c></item>
        /// <item>A <c>URI</c> - returns an instance of <c>xs:anyURI</c></item>
        /// <item>A <c>QName</c> - returns an instance of <c>xs:QName</c></item>
        /// </list>
		/// </remarks>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The converted value</returns>

        public static XdmAtomicValue MakeAtomicValue(object value)
        {
            if (value is JAtomicValue)
            {
                return (XdmAtomicValue)XdmValue.Wrap((JAtomicValue)value);
            }
            else if (value is Boolean)
            {
                return new XdmAtomicValue((Boolean)value);
            }
            else if (value is int)
            {
                return new XdmAtomicValue((int)value);
            }
            else if (value is long)
            {
                return new XdmAtomicValue((long)value);
            }
            else if (value is short)
            {
                return new XdmAtomicValue((short)value);
            }
            else if (value is Char)
            {
                return new XdmAtomicValue((long)value);
            }
            else if (value is Byte)
            {
                return new XdmAtomicValue((Byte)value);
            }
            else if (value is String)
            {
                return new XdmAtomicValue((String)value);
            }
            else if (value is Double)
            {
                return new XdmAtomicValue((Double)value);
            }
            else if (value is float)
            {
                return new XdmAtomicValue((float)value);
            }
            else if (value is decimal)
            {
                return new XdmAtomicValue((decimal)value);
            }
            else if (value is Uri)
            {
                return new XdmAtomicValue((Uri)value);
            }
            else if (value is QName)
            {
                return new XdmAtomicValue((QName)value);
            }
            if (value is XdmAtomicValue)
            {
                return (XdmAtomicValue)value;
            }
            else
            {
                throw new ArgumentException(value.ToString());
            }
        }


        /// <summary>
        /// Get the value converted to a boolean using the XPath casting rules
        /// </summary>
        /// <returns>the result of converting to a boolean (Note: this is not the same as the
        /// effective boolean value).</returns> 

        public bool GetBooleanValue()
        {
            JAtomicValue av = (JAtomicValue)this.value;
            if (av is JBooleanValue) {
                return ((JBooleanValue)av).getBooleanValue();
            } else if (av is JNumericValue) {
                return !av.isNaN() && ((JNumericValue)av).signum() != 0;
            } else if (av is JStringValue) {
                String s = av.getStringValue().Trim();
                return "1".Equals(s) || "true".Equals(s);
            } else {
                throw new ArgumentException("Cannot cast item to a boolean");
            }
        }


        /// <summary>
        /// Get the value converted to a long using the XPath casting rules
        /// </summary>
        /// <returns>the result of converting to a long</returns>

        public long GetLongValue()
        {
            JAtomicValue av = (JAtomicValue)this.value;
            if (av is JBooleanValue) {
                return ((JBooleanValue)av).getBooleanValue() ? 0L : 1L;
            } else if (av is JNumericValue) {
            try {
                return ((JNumericValue)av).longValue();
            } catch (Exception) {
                throw new ArgumentException("Cannot cast item to an integer");
            }
            } else if (av is JStringValue) {
                JStringToDouble converter = JStringToDouble.getInstance();
                return (long)converter.stringToNumber(av.getStringValueCS());
            } else {
                throw new ArgumentException("Cannot cast item to an integer");
            }
        }


        /// <summary>
		/// Get the value converted to a double using the XPath casting rules.
		/// </summary>
        /// <remarks>If the value is a string, the XSD 1.1 rules are used, which means that the string
		/// "+INF" is recognised.</remarks>
        /// <returns>the result of converting to a double</returns>

        public double GetDoubleValue()
        {
            JAtomicValue av = (JAtomicValue)this.value;
            if (av is JBooleanValue) {
                return ((JBooleanValue)av).getBooleanValue() ? 0.0 : 1.0;
            } else if (av is JNumericValue) {
                return ((JNumericValue)av).getDoubleValue();
            } else if (av is JStringValue) {
            try {
                JStringToDouble converter = JStringToDouble11.getInstance();
                return converter.stringToNumber(av.getStringValueCS());
            } catch (Exception e) {
                throw new ArgumentException(e.Message);
            }
            } else {
                throw new ArgumentException("Cannot cast item to a double");
            }
        }


        /// <summary>
        /// Get the value converted to a decimal using the XPath casting rules
        /// </summary>
        /// <returns>the result of converting to a decimal</returns>

        public Decimal GetDecimalValue() 
        {
            JAtomicValue av = (JAtomicValue)this.value;
            if (av is JBooleanValue) {
                return ((JBooleanValue)av).getBooleanValue() ? 0  : 1;
            } else if (av is JNumericValue) {
                try {
                    return Convert.ToDecimal(((JNumericValue)av).getDecimalValue().toString());
                } catch (Exception) {
                    throw new ArgumentException("Cannot cast item to a decimal");
                }   
            } else if (av is JStringValue) {
                return Convert.ToDecimal(av.getStringValueCS().toString());
            } else {
                throw new ArgumentException("Cannot cast item to a decimal");
            }
        }



        /// <summary>
        /// Convert the atomic value to a string
        /// </summary>
        /// <returns>The value converted to a string, according to the rules
        /// of the XPath cast expression</returns>        

        public override String ToString()
        {
            return ((JAtomicValue)value).getStringValue();
        }
        
        /// <summary>
        /// Compare two atomic values for equality
        /// </summary>
        /// <returns>The result of the equality comparison, using the rules of the
		/// <c>op:is-same-key()</c> comparison used for comparing key values in maps</returns>
        
        public override Boolean Equals(object other)
        {
            if (other is XdmAtomicValue)
            {
                return ((JAtomicValue)value).asMapKey().Equals(((JAtomicValue)((XdmAtomicValue)other).value).asMapKey());

            }
            else
            {
                return false;
            }
        }
        
        /// <summary>
        /// Get a hash code to support equality comparison
        /// </summary>
        /// <returns>A suitable hash code</returns>
        
        public override int GetHashCode()
        {
            return ((JAtomicValue)value).asMapKey().GetHashCode();
        }         

        /// <summary>
        /// Get the name of the value's XDM type
        /// </summary>
		/// <returns>The type of the value, as a QName.</returns>


        public QName GetTypeName()
        {
            JStructuredQName sqname = ((JAtomicValue)value).getItemType().getStructuredQName();
            return new QName(sqname.getPrefix(),
                             sqname.getURI(),
                             sqname.getLocalPart());
        }
        
        /// <summary>
        /// Get the name of the value's XDM type
        /// </summary>
        /// <param name="processor">The <code>Processor</code> object. 
        /// This parameter is no longer used, but is accepted for backwards compatibility.</param>
		/// <returns>The type of the value, as a QName.</returns>


        public QName GetTypeName(Processor processor)
        {
            return GetTypeName();
        }

        /// <summary>
        /// Get the name of the primitive type of the value
        /// </summary>
        /// <returns>The primitive type of the value, as a QName. This will be the name of
        /// one of the primitive types defined in XML Schema Part 2, or the XPath-defined
        /// type <c>xs:untypedAtomic</c>. For the purposes of this method, <c>xs:integer</c> is considered
        /// to be a primitive type.
        /// </returns>


        public QName GetPrimitiveTypeName()
        {
            int fp = ((JAtomicValue)value).getItemType().getPrimitiveType();
            return new QName(JStandardNames.getPrefix(fp),
                             JStandardNames.getURI(fp),
                             JStandardNames.getLocalName(fp));
        }

        /// <summary>Get the value as a CLI object of the nearest equivalent type.</summary>
        /// <remarks>
        /// <para>The return type is as follows:</para>
        /// <list>
		/// <item><c>xs:string</c> - String</item>
		/// <item><c>xs:integer</c> - Long</item>
		/// <item><c>xs:decimal</c> - Decimal</item>
		/// <item><c>xs:double</c> - Double</item>
		/// <item><c>xs:float</c> - Float</item>
		/// <item><c>xs:boolean</c> - Bool</item>
		/// <item><c>xs:QName</c> - QName</item>
		/// <item><c>xs:anyURI</c> - Uri</item>
		/// <item><c>xs:untypedAtomic</c> - String</item>
        /// <item>wrapped external object - the original external object</item>
        /// <item>Other types - currently String, but this may change in the future</item>
        /// </list>
        /// </remarks>
        /// <returns>The value converted to the most appropriate CLI type</returns>

        public Object Value
        {
            get
            {
                if (value is JIntegerValue)
                {
                    return ((JIntegerValue)value).longValue();
                }
                else if (value is JDoubleValue)
                {
                    return ((JDoubleValue)value).getDoubleValue();
                }
                else if (value is JFloatValue)
                {
                    return ((JFloatValue)value).getFloatValue();
                }
                else if (value is JDecimalValue)
                {
                    return Decimal.Parse(((JDecimalValue)value).getStringValue());
                }
                else if (value is JBooleanValue)
                {
                    return ((JBooleanValue)value).getBooleanValue();
                }
                else if (value is JAnyURIValue)
                {
                    return new Uri(((JAnyURIValue)value).getStringValue());
                }
                else if (value is JQNameValue)
                {
                    return new QName((JQNameValue)value);
                }
                else if (value is JDotNetObjectValue) // TODO: can't happen?
                {
                    return ((JDotNetObjectValue)value).getObject();
                }
                else
                {
                    return ((JAtomicValue)value).getStringValue();
                }
            }
        }


    }

    /// <summary inherits="XdmItem">
    /// The class <c>XdmFunctionItem</c> represents an item in an XDM sequence
    /// that holds a function.
    /// </summary>
    /// <remarks>
    /// <para>Note that there is no guarantee that every <c>XdmValue</c> comprising a single
    /// function item will be an instance of this class. To force this, use the <c>Simplify</c>
    /// property of the <c>XdmValue</c>.</para>
    /// <para>At present the only way of creating an instance of this class is as the result of
    /// an XPath or XQuery expression that returns a function item.</para>
    /// </remarks>

    [Serializable]
    public class XdmFunctionItem : XdmItem
    {
        /// <summary>
        /// The name of the function, as a QName
        /// </summary>
		/// <returns>The name of the function. The result will be null if the function is anonymous.</returns>
        
        public QName FunctionName
        {
            get
            {
                JStructuredQName sqname = ((JFunction)value).getFunctionName();
                return (sqname == null ? null : QName.FromStructuredQName(sqname));
            }
        }

        /// <summary>
        /// The arity of the function, that is, the number of arguments it expects
        /// </summary>
		/// <returns>The number of arguments that the function takes</returns>

        public int Arity
        {
            get
            {
                return ((JFunction)value).getArity();
            }
        }

        /// <summary>
        /// Determine whether the item is an atomic value
        /// </summary>
        /// <returns>
        /// false (a function item is not an atomic value)
        /// </returns>

        public override bool IsAtomic()
        {
            return false;
        }


            /// <summary>
            /// Invoke the function
            /// </summary>
            /// <param name="arguments">The arguments to the function</param>
            /// <param name="processor">The Saxon processor, used to provide context information</param>
            /// <returns>The result of calling the function</returns>
            /// 
            public XdmValue Invoke(XdmValue[] arguments, Processor processor)
        {
            JXdmValue[] args = new JXdmValue[arguments.Length];
            for (int i = 0; i < arguments.Length; i++)
            {
                args[i] = FromGroundedValueToJXdmValue(arguments[i].value);
            }
            JFunction function = (JFunction)value;
            net.sf.saxon.s9api.XdmFunctionItem functionItem = new net.sf.saxon.s9api.XdmFunctionItem(function);
            JXdmValue result = functionItem.call(processor.JProcessor, args);
           
            return XdmValue.Wrap(result.getUnderlyingValue());
        }
    }

    /// <summary inherits="XdmFunctionItem">
    /// The class <c>XdmArray</c> represents an array item in an XDM 3.1 sequence:
    /// this is a new kind of item in the XDM data model. An array is a list of zero or 
    /// more members, each of which is an arbitrary XDM value. An array is also a function:
    /// it maps a positive integer to the array member found at that position in the array.
    /// </summary>
    [Serializable]
    public class XdmArray : XdmFunctionItem
    {


		///<summary> Constructor to create an empty <c>XdmArray</c></summary>
        public XdmArray() {
            this.value = JSimpleArrayItem.EMPTY_ARRAY;
        }

        ///<summary>Create an <c>XdmArray</c> whose members are single items, corresponding
        ///one-to-one with the items making up a supplied sequence.</summary>
        ///<param name="value">A sequence of items; each item becomes a member of the array.</param>
        public XdmArray(XdmValue value)
        {
            int length = value.Count;
            JArrayList list = new JArrayList(length);
            foreach (XdmItem item in value.GetList())
            {
                list.add(item.Unwrap());
            }
            this.value = new JSimpleArrayItem(list);
        }

        ///<summary> Create an <c>XdmArray</c> supplying the members as an array of <c>XdmValue</c> objects.</summary>
        /// <param name="members">An array of <c>XdmValue</c> objects. Note that subsequent changes 
        /// to the array will have no effect on the <c>XdmArray</c>.</param>


        public XdmArray(XdmValue[] members) {
            JArrayList list = new JArrayList(members.Length);
            for (int i =0; i< members.Length;i++) {
                list.add(members[i].Unwrap());
            }
            this.value = new JSimpleArrayItem(list);
        }

		// Internal constructor

        internal XdmArray(JArrayList list)
        {
            this.value = new JSimpleArrayItem(list);
        }

		// Internal constructor

        internal XdmArray(JArrayItem array) {
            this.value = array;
        }


        /// <summary>Create an <c>XdmArray</c> supplying the members as a list of <c>XdmValue</c> objects</summary>
        /// <param name="members">A sequence of <c>XdmValue</c> objects. Note that if this is supplied as 
        /// a list or similar collection, subsequent changes to the list/collection will have no effect on 
		/// the <c>XdmValue</c>.</param>
		/// <remarks>Note that the argument can be a single <c>XdmValue</c> representing a sequence, in which case the
        ///  constructed array will have one member for each item in the supplied sequence.</remarks>
    
        public XdmArray(List<XdmValue> members) {
            JArrayList list = new JArrayList(members.Count);
            for (int i = 0; i < members.Count; i++)
            {
                list.add(members[i].Unwrap());
            }
            this.value = new JSimpleArrayItem(list);
        }

        /// <summary>
        /// Get the number of members in the array
        /// </summary>
        /// <returns>the number of members in the array.</returns> 
		/// <remarks>(Note that the <see cref="XdmValue.Count"/> property returns 1 (one),
        /// because an XDM array is an item.)</remarks>
        public int ArrayLength() {
            return ((JArrayItem)value).arrayLength();
        }

        /// <summary>
        /// Get the n'th member in the array, counting from zero.
        /// </summary>
		/// <param name="n">the position of the member that is required, counting the first member in 
		/// the array as member zero</param>
        /// <returns>the n'th member in the sequence making up the array, counting from zero</returns>
        public XdmValue Get(int n) {
            try {
                JSequence member = ((JArrayItem)value).get(n);
                return XdmValue.Wrap(member);
            }
            catch (Exception) {
                throw new IndexOutOfRangeException();
            }
        }


        /// <summary>
        /// Create a new array in which one member is replaced with a new value.
        /// </summary>
        /// <param name="n">the position of the member that is to be replaced, counting the first member
        /// in the array as member zero</param>
        /// <param name="valuei">the new value</param>
        /// <returns>the new array</returns>
        public XdmArray Put(int n, XdmValue valuei) {
            try {
                return (XdmArray)XdmValue.Wrap(((JArrayItem)this.value).put(n, valuei.value));
            } catch (Exception) {
                throw new IndexOutOfRangeException();
            }
        }


        /// <summary>
        /// Append a new member to an array
        /// </summary>
        /// <param name="value">the new member</param>
        /// <returns>a new array, one item longer than the original</returns>
        public XdmArray AppendMember(XdmValue value) {
            try {
                JGroundedValue member = value.value;
                JArrayItem newArray = net.sf.saxon.ma.arrays.ArrayFunctionSet.ArrayAppend.append((JArrayItem)this.value, member);
                return (XdmArray)Wrap(newArray);
            }
            catch (net.sf.saxon.trans.XPathException e) {
                throw new StaticError(e);
            }
        }

        /// <summary>
        /// Concatenate another array
        /// </summary>
        /// <param name="value">the other array</param>
        /// <returns>a new array, containing the members of this array followed by the members 
        /// of the other array</returns>
        public XdmArray Concat(XdmArray value)
        {
            try
            {
                JArrayItem other = (JArrayItem)value.value;
                JArrayItem newArray = ((JArrayItem)this.value).concat(other);
                return (XdmArray)Wrap(newArray);
            }
            catch (net.sf.saxon.trans.XPathException e)
            {
                throw new StaticError(e);
            }
        }


        /// <summary>
        /// Get the members of the array in the form of a list.
        /// </summary>
        /// <returns>A list of the members of this array.</returns>
        public List<XdmValue> AsList() {

            JArrayItem val = ((JArrayItem)value);
            java.util.Iterator  iter = val.members().iterator();
            List<XdmValue> result = new List<XdmValue>(val.getLength());
            while (iter.hasNext()) {
                result.Add(XdmValue.Wrap((JSequence)iter.next()));

            }
            return result;
        }

        /// <summary>
        /// Make an XDM array from an object array. Each member of the supplied array
        /// is converted to a single member in the result array using the method
		/// <see cref="XdmValue.MakeValue(Object)"/>        
        /// </summary>
        /// <param name="o">the array of objects</param>
        /// <returns>the result of the conversion if successful</returns>
        
        public static XdmArray MakeArray(object[] o)
        {
            JArrayList list = new JArrayList(o.Length);
            for (int i = 0; i <o.Length; i++)
            {
                list.add(XdmValue.MakeValue(o[i]).Unwrap());
            }
            return new XdmArray(list);
        }

        /// <summary>
		/// Make an <c>XdmArray</c> whose members are <c>xs:boolean</c> values       
        /// </summary>
        /// <param name="o">the input array of booleans</param>
		/// <returns>an <c>XdmArray</c> whose members are <c>xs:boolean</c> values corresponding one-to-one with the input</returns>
        
        public static XdmArray MakeArray(bool[] o)
        {
            JArrayList list = new JArrayList(o.Length);
            for (int i = 0; i < o.Length; i++)
            {
                list.add(new XdmAtomicValue(o[i]).Unwrap());
            }
            return new XdmArray(list);
        }


        /// <summary>
		/// Make an <c>XdmArray</c> whose members are <c>xs:integer</c> values      
        /// </summary>
        /// <param name="o">the input array of long values</param>
		/// <returns>an <c>XdmArray</c> whose members are <c>xs:integer</c> values corresponding one-to-one with the input</returns>
        
        public static XdmArray MakeArray(long[] o)
        {
            JArrayList list = new JArrayList(o.Length);
            for (int i = 0; i < o.Length; i++)
            {
                list.add(new XdmAtomicValue(o[i]).Unwrap());
            }
            return new XdmArray(list);
        }


        /// <summary>
		/// Make an <c>XdmArray</c> whose members are <c>xs:integer</c> values      
        /// </summary>
        /// <param name="o">the input array of int values</param>
		/// <returns>an <c>XdmArray</c> whose members are <c>xs:integer</c> values corresponding one-to-one with the input</returns>
        
        public static XdmArray MakeArray(int[] o)
        {
            JArrayList list = new JArrayList(o.Length);
            for (int i = 0; i < o.Length; i++)
            {
                list.add(new XdmAtomicValue(o[i]).Unwrap());
            }
            return new XdmArray(list);
        }

        /// <summary>
		/// Make an <c>XdmArray</c> whose members are <c>xs:integer</c> values      
        /// </summary>
        /// <param name="o">the input array of byte values</param>
		/// <returns>an <c>XdmArray</c> whose members are <c>xs:integer</c> values corresponding one-to-one with the input</returns>
        
        public static XdmArray MakeArray(byte[] o)
        {
            JArrayList list = new JArrayList(o.Length);
            for (int i = 0; i < o.Length; i++)
            {
                list.add(new XdmAtomicValue(o[i]).Unwrap());
            }
            return new XdmArray(list);
        }
    }

    /// <summary inherits="XdmFunctionItem">
    /// The class <c>XdmMap</c> represents a map item in an XPath 3.1 sequence:
	/// this is a new kind of item in the XDM data model. A map is a list of zero or more entries, each of which
    /// is a pair comprising a key (which is an atomic value) and a value (which is an arbitrary value).
    /// </summary>


    [Serializable]
    public class XdmMap :  XdmFunctionItem
    {
  

        /// <summary>
		/// Create an empty <c>XdmMap</c>
        /// </summary>
        public XdmMap() { 
			this.value = new JHashTrieMap(); 
		}

		// Internal constructor

        internal XdmMap(JHashTrieMap map) { 
			this.value = map; 
		}


        /// <summary>
        /// Get the number of entries in the map
        /// </summary>
		/// <remarks>(Note that the <see cref="XdmValue.Count"/> method returns 1 (one),
        /// because an XDM map is an item.)</remarks>
        
        public int Size {
            get {
                return ((JMapItem)value).size();
            }
        }


        /// <summary>
		/// Ask whether the <c>XdmMap</c> is empty
        /// </summary>
        /// <returns>Returns <code>true</code> if this map contains no key-value pairs, that is
        /// if the <c>Size</c> property is zero.</returns>
        
        public bool IsEmpty() {
            return ((JMapItem)value).isEmpty();
        }

        /// <summary>
        /// Create a new map containing an additional (key, value) pair.
        /// If there is an existing entry with the same key, it is removed.
        /// </summary>
        /// <param name="key">The key of the new entry.</param>
        /// <param name="value">The value part of the new entry.</param>
        /// <returns>A new map containing the additional entry (or replaced entry). The original map is unchanged.</returns>
        
        public XdmMap Put(XdmAtomicValue key, XdmValue value)
        {
            XdmMap map2 = new XdmMap();
            map2.value = ((JMapItem)this.value).addEntry((JAtomicValue)key.Unwrap(), value.value);
            return map2;
        }


        /// <summary>
        /// Create a new map in which the entry for a given key has been removed.
        /// If there is no entry with the same key, the new map has the same content as the old (it may or may not
        /// be the same .NET object)
        /// </summary>
        /// <param name="key">The key of the entry that is to be removed</param>
        /// <returns>A map without the specified entry. The original map is unchanged.</returns>
        
        public XdmMap Remove(XdmAtomicValue key)
        {
            XdmMap map2 = new XdmMap();
            map2.value = ((JMapItem)this.value).remove((JAtomicValue)key.Unwrap());
            return map2;
        }


        /// <summary>
        /// Return a corresponding .NET Dictionary collection of keys and values.
        /// </summary>
        /// <returns>A mutable Dictionary from atomic values to (sequence) values, containing the
        /// same entries as this map</returns>
        
        public Dictionary<XdmAtomicValue, XdmValue> AsDictionary() {
            Dictionary<XdmAtomicValue, XdmValue> map = new Dictionary<XdmAtomicValue, XdmValue>();
            JMapItem jmap = (JMapItem)value;
            java.util.Iterator iter = jmap.keyValuePairs().iterator();
            JKeyValuePair pair = null;
            while (iter.hasNext()) {
                pair = (JKeyValuePair)iter.next();
                map.Add((XdmAtomicValue)XdmValue.Wrap(pair.key), XdmValue.Wrap(pair.value));
            }
            return map;
        }


        /// <summary>
        /// Get the keys present in the map in the form of a set.
        /// </summary>
        /// <returns>a set of the keys present in this map, with no defined ordering.</returns>
        public HashSet<XdmAtomicValue> KeySet()
        {
            HashSet<XdmAtomicValue> result = new HashSet<XdmAtomicValue>();
            JMapItem jmap = (JMapItem)value;
            net.sf.saxon.tree.iter.AtomicIterator iter = jmap.keys();
            JAtomicValue key = null;
            while ((key = iter.next()) != null)
            {
                result.Add((XdmAtomicValue)XdmValue.Wrap(key));
            }
            return result;
        }


        /// <summary>
        /// Returns <code>true</code> if this map contains a mapping for the specified
        /// key. More formally, returns <code>true</code> if and only if
        /// this map contains a mapping for a key <code>k</code> such that
        /// <code>(key==null ? k==null : key.Equals(k))</code>.  (There can be
        /// at most one such mapping.)
        /// </summary>
        /// <param name="key">the key whose presence in this map is to be tested</param>
        /// <returns><c>true</c> if this map contains a mapping for the specified key</returns>
        
        public bool ContainsKey(XdmAtomicValue key) {
            JAtomicValue k = (JAtomicValue)key.value;
            return ((JMapItem)value).get(k)!= null;
        }


        /// <summary>
        /// Returns the value to which the specified key is mapped,
		/// or <c>null</c> if this map contains no mapping for the key.
        /// </summary>
        /// <param name="key">the key whose associated value is to be returned. If this is
		/// not an <c>XdmAtomicValue</c>, the method attempts to construct an
		/// <c>XdmAtomicValue</c> using the method <see cref="XdmAtomicValue.MakeAtomicValue(Object)"/>;
        /// it is therefore possible to pass a simple key such as a string or integer.</param>
        /// <returns>the value to which the specified key is mapped, or
		/// <c>null</c> if this map contains no mapping for the key</returns>
        
        public XdmValue Get(XdmAtomicValue key)
        {
            if (key == null) {
                throw new ArgumentNullException();
            }
           
            JAtomicValue k = (JAtomicValue)(key).value;
            JSequence v = ((JMapItem)value).get(k);
            return v==null ? null : XdmValue.Wrap(v);
        }

        /// <summary>
        /// Returns the value to which the specified key is mapped,
        /// or <c>null</c> if this map contains no mapping for the key.
        /// </summary>
        /// <param name="key">the key whose associated value is to be returned.</param>
        /// <returns>the value to which the specified key is mapped, or
        /// <c>null</c> if this map contains no mapping for the key</returns>

        public XdmValue Get(String key)
        {
            if (key == null)
            {
                throw new ArgumentNullException();
            }
    
            JAtomicValue k = (JAtomicValue)(new XdmAtomicValue(key)).value;
            JSequence v = ((JMapItem)value).get(k);
            return v == null ? null : XdmValue.Wrap(v);
        }


        /// <summary>
        /// Returns the value to which the specified key is mapped,
        /// or <c>null</c> if this map contains no mapping for the key.
        /// </summary>
        /// <param name="key">the key whose associated value is to be returned.</param>
        /// <returns>the value to which the specified key is mapped, or
        /// <c>null</c> if this map contains no mapping for the key</returns>

        public XdmValue Get(long key)
        {

            JAtomicValue k = (JAtomicValue)(new XdmAtomicValue(key)).value;
            JSequence v = ((JMapItem)value).get(k);
            return v == null ? null : XdmValue.Wrap(v);
        }

        /// <summary>
        /// Returns a <c>Collection</c> containing the values found in this map.
        /// </summary>
        /// <returns>A collection of the values found in this map, that is, the value
        /// parts of the key-value pairs. The result may contain duplicates, and the
        /// order of results is unpredictable.</returns>
        public ICollection Values() {
            List<XdmValue> result = new List<XdmValue>();

            JMapItem jmap = (JMapItem)value;
            java.util.Iterator iter = jmap.iterator();
            JKeyValuePair pair = null;
            while ((pair = (JKeyValuePair)iter.next()) != null)
            {
                result.Add((XdmAtomicValue)XdmValue.Wrap(pair.value));
            }

            return result;
        }


        /// <summary>
		/// Returns a <c>HashSet</c> view of the mappings contained in this map.
        /// </summary>
        /// <returns>a set view of the mappings contained in this map</returns>
        public HashSet<DictionaryEntry> EntrySet() {
            HashSet<DictionaryEntry> result = new HashSet<DictionaryEntry>();
            JMapItem jmap = (JMapItem)value;
            java.util.Iterator iter = jmap.iterator();
            JKeyValuePair pair = null;
            while ((pair = (JKeyValuePair)iter.next()) != null)
            {
                result.Add(new DictionaryEntry(pair.key, pair.value));
            }
            return result;
        }


        /// <summary>
        /// Static factory method to construct an XDM map by converting each entry
        /// in a supplied generic collection of key/value pairs; <code>IDictionary</code>. The keys in the 
        /// Dictionary must be convertible to XDM atomic values using the 
		/// <see cref="XdmAtomicValue.MakeAtomicValue(Object)"/> method. The associated values 
        /// must be convertible to XDM sequences
		/// using the <see cref="XdmValue.MakeValue(Object)"/> method.
        /// </summary>
        /// <param name="input">the supplied map</param>
		/// <returns>the resulting <c>XdmMap</c></returns>
        public static XdmMap MakeMap(IDictionary input) {
            JHashTrieMap result = new JHashTrieMap();
            XdmAtomicValue key;
            XdmValue value;
            
            foreach (object keyi in input.Keys)
            {
                key = XdmAtomicValue.MakeAtomicValue(keyi);
                value = XdmValue.MakeValue(input[keyi]);
                result.initialPut((JAtomicValue)key.Unwrap(), value.value);
            }
           
            return new XdmMap(result);
        }

        


    }
    
    /// <summary inherits="XdmItem">
    /// The class <c>XdmNode</c> represents a Node in the XDM Data Model. A Node
    /// is an <c>XdmItem</c>, and is therefore an <c>XdmValue</c> in its own right, and may also participate
    /// as one item within a sequence value.
    /// </summary>
    /// <remarks>
    /// <para>An <c>XdmNode</c> is implemented as a wrapper around an object
    /// of type <c>net.sf.saxon.NodeInfo</c>. Because this is a key interface
    /// within Saxon, it is exposed via this API, even though it is a Java
    /// interface that is not part of the API proper.</para>
    /// <para>The <c>XdmNode</c> interface exposes basic properties of the node, such
    /// as its name, its string value, and its typed value. Navigation to other nodes
    /// is supported through a single method, <c>EnumerateAxis</c>, which allows
    /// other nodes to be retrieved by following any of the XPath axes.</para>
    /// </remarks>

    [Serializable]
    public class XdmNode : XdmItem
    {

        /// <summary>
        /// Determine whether the item is an atomic value
        /// </summary>
        /// <returns>
        /// false (the item is not an atomic value)
        /// </returns>

        public override bool IsAtomic()
        {
            return false;
        }

        /// <summary>
        /// The name of the node, as a <c>QName</c>. Returns null in the case of unnamed nodes.
        /// </summary>

        public QName NodeName
        {
            get
            {
                JNodeInfo node = (JNodeInfo)value;
                String local = node.getLocalPart();
                if (local == "")
                {
                    return null;
                }
                String prefix = node.getPrefix();
                String uri = node.getURI();
                return new QName(prefix, uri, local);
            }
        }

        /// <summary>
        /// The kind of node, as an instance of <c>System.Xml.XmlNodeType</c>.
        /// </summary>
		/// <remarks>For a namespace node in the XDM model, the value <c>XmlNodeType.None</c> 
        /// is returned.
        /// </remarks>

        public XmlNodeType NodeKind
        {
            get
            {
                JNodeInfo node = (JNodeInfo)value;
                int kind = node.getNodeKind();
                switch (kind)
                {
                    case JType.DOCUMENT:
                        return XmlNodeType.Document;
                    case JType.ELEMENT:
                        return XmlNodeType.Element;
                    case JType.ATTRIBUTE:
                        return XmlNodeType.Attribute;
                    case JType.TEXT:
                        return XmlNodeType.Text;
                    case JType.COMMENT:
                        return XmlNodeType.Comment;
                    case JType.PROCESSING_INSTRUCTION:
                        return XmlNodeType.ProcessingInstruction;
                    case JType.NAMESPACE:
                        return XmlNodeType.None;
                    default:
                        throw new ArgumentException("Unknown node kind");
                }
            }
        }

        /// <summary>
		/// Get the line number of the node in a source document. 
		/// </summary>
		/// <remarks>
		/// For a document constructed using the document
        /// builder, this is available only if the line numbering option was set when the document was built (and
        /// then only for element nodes). If the line number is not available, the value -1 is returned.
        /// Line numbers will typically be as reported by a SAX parser; this means that the line number for an element
        /// node is the line number containing the closing ">" of the start tag.
		/// </remarks>
         
        public int LineNumber {
            get { return ((JNodeInfo)value).getLineNumber(); }
        }


        /// <summary>
		/// Get the column number of the node in a source document. 
		/// </summary>
		/// <remarks>
		/// For a document constructed using the document
        /// builder, this is available only if the line numbering option was set when the document was built (and
        /// then only for element nodes). If the column number is not available, the value -1 is returned.
        /// Line numbers will typically be as reported by a SAX parser; this means that the column number for an element
        /// node is the column number containing the closing ">" of the start tag.
		/// </remarks>
        
		public int ColumnNumber
        {
            get { return ((JNodeInfo)value).getColumnNumber(); }
        }

        /// <summary>
        /// The typed value of the node, as an instance of <c>XdmValue</c>.
        /// </summary>
        /// <remarks>
		/// A <c>DynamicError</c> is thrown if the node has no typed value, as will be the case for
        /// an element with element-only content.
		/// </remarks>

        public XdmValue TypedValue
        {
            get { return XdmValue.Wrap(((JNodeInfo)value).atomize()); }
        }

        /// <summary>
		/// Get a <see cref="Processor"/> suitable for use with this <see cref="XdmNode"/>.
        /// </summary>
        /// <remarks>
		/// <para>In most cases this will be the original <see cref="Processor"/>
		/// object used to create the <see cref="DocumentBuilder"/> that built the document that 
		/// contains this node. If that <see cref="Processor"/> is not available, it will be a 
		/// compatible <c>Processor</c>, one that shares the same underlying <see cref="net.sf.saxon.Configuration"/>, 
        /// and hence is initialized with the same configuration settings, schema components, license features,
        /// and so on.</para>
		/// <para><i>Note: the only case where the original <c>Processor</c> is not available is when
		/// the same <c>Configuration</c> is used with multiple APIs, for example mixing s9api
        /// and JAXP or XQJ in the same application.</i></para>
        /// </remarks>
		/// <returns>Returns a <c>Processor</c> suitable for performing further operations on this node, for example
		/// for creating a <see cref="Serializer"/> or an <see cref="XPathCompiler"/>.</returns>
        public Processor Processor {
            get {
                JConfiguration config = Implementation.getConfiguration();
                object originator = config.getProcessor();
                if (originator is Processor)
                {
                    return (Processor)originator;
                }
                else {
                    return new Processor(new net.sf.saxon.s9api.Processor(config));
                }
            }
        }


        /// <summary>
		/// Unwraps the underlying <c>XmlNode</c> object from the <c>XdmValue</c>.
		/// If the method does not wrap a <c>XmlNode</c> then a null is returned
        /// </summary>
		/// <returns>The underlying <c>XmlNode</c></returns>
        public XmlNode getUnderlyingXmlNode()
        {

            if (value is net.sf.saxon.dotnet.DotNetNodeWrapper)
            {

                return (XmlNode)((net.sf.saxon.dotnet.DotNetNodeWrapper)value).getRealNode();
            }
            return null;
        }

        /// <summary>
        /// Get the string value of the node.
        /// </summary>

        public String StringValue
        {
            get { return ((JNodeInfo)value).getStringValue(); }
        }

        /// <summary>
        /// Get the parent of this node.
        /// </summary>
        /// <remarks>
        /// Returns either a document node, an element node, or null in the case where
        /// this node has no parent. 
        /// </remarks>

        public XdmNode Parent
        {
            get {
                JNodeInfo parent = ((JNodeInfo)value).getParent();
                return (parent == null ? null : (XdmNode)XdmValue.Wrap(parent)); 
            }
        }

        /// <summary>
        /// Get the root of the tree containing this node.
        /// </summary>
        /// <remarks>
        /// Returns the root of the tree containing this node (which might be this node itself).
        /// </remarks>

        public XdmNode Root
        {
            get
            {
                XdmNode parent = Parent;
                if (parent == null)
                {
                    return this;
                }
                else
                {
                    return parent.Root;
                }
            }
        }

        /// <summary>
        /// Get a the string value of a named attribute of this element. 
        /// </summary>
        /// <remarks>
        /// Returns null if this node is not an element, or if this element has no
        /// attribute with the specified name.
        /// </remarks>
        /// <param name="name">The name of the attribute whose value is required</param>

        public String GetAttributeValue(QName name)
        {
            return ((JNodeInfo)value).getAttributeValue(name.Uri, name.LocalName);
        }

        /// <summary>
        /// Get an enumerator that supplies all the nodes on one of the XPath
        /// axes, starting with this node.
        /// </summary>
        /// <param name="axis">
        /// The axis to be navigated, for example <c>XdmAxis.Child</c> for the child axis.
        /// </param>
        /// <remarks>
        /// The nodes are returned in axis order: that is, document order for a forwards
        /// axis, reverse document order for a reverse axis.
        /// </remarks>

        public IEnumerator<XdmNode> EnumerateAxis(XdmAxis axis)
        {
            return (new SequenceEnumerator<XdmNode>(JSequenceXdmIterator.ofNodes(((JNodeInfo)value).iterateAxis(GetAxisNumber(axis)))));
        }

        /// <summary>
        /// Get an enumerator that selects all the nodes on one of the XPath
        /// axes, provided they have a given name. The nodes selected are those of the principal
        /// node kind (elements for most axes, attributes for the attribute axis, namespace nodes
        /// for the namespace axis) whose name matches the name given in the second argument.
        /// </summary>
        /// <param name="axis">
        /// The axis to be navigated, for example <c>XdmAxis.Child</c> for the child axis.
        /// </param>
        /// <param name="nodeName">
        /// The name of the required nodes, for example <c>new QName("", "item")</c> to select
        /// nodes with local name "item", in no namespace.
        /// </param>
        /// <remarks>
        /// The nodes are returned in axis order: that is, document order for a forwards
        /// axis, reverse document order for a reverse axis.
        /// </remarks>

        public IEnumerator<XdmNode> EnumerateAxis(XdmAxis axis, QName nodeName)
        {
            int kind;
            switch (axis)
            {
                case XdmAxis.Attribute:
                    kind = net.sf.saxon.type.Type.ATTRIBUTE;
                    break;
                case XdmAxis.Namespace:
                    kind = net.sf.saxon.type.Type.NAMESPACE;
                    break;
                default:
                    kind = net.sf.saxon.type.Type.ELEMENT;
                    break;
            }
            JNamePool pool = ((JNodeInfo)value).getConfiguration().getNamePool();
            JNameTest test = new JNameTest(kind, nodeName.Uri, nodeName.LocalName, pool);
            return new SequenceEnumerator<XdmNode>(JSequenceXdmIterator.ofNodes(((JNodeInfo)value).iterateAxis(GetAxisNumber(axis), test)));
        }

        private static byte GetAxisNumber(XdmAxis axis)
        {
            switch (axis)
            {
                case XdmAxis.Ancestor: return JAxisInfo.ANCESTOR;
                case XdmAxis.AncestorOrSelf: return JAxisInfo.ANCESTOR_OR_SELF;
                case XdmAxis.Attribute: return JAxisInfo.ATTRIBUTE;
                case XdmAxis.Child: return JAxisInfo.CHILD;
                case XdmAxis.Descendant: return JAxisInfo.DESCENDANT;
                case XdmAxis.DescendantOrSelf: return JAxisInfo.DESCENDANT_OR_SELF;
                case XdmAxis.Following: return JAxisInfo.FOLLOWING;
                case XdmAxis.FollowingSibling: return JAxisInfo.FOLLOWING_SIBLING;
                case XdmAxis.Namespace: return JAxisInfo.NAMESPACE;
                case XdmAxis.Parent: return JAxisInfo.PARENT;
                case XdmAxis.Preceding: return JAxisInfo.PRECEDING;
                case XdmAxis.PrecedingSibling: return JAxisInfo.PRECEDING_SIBLING;
                case XdmAxis.Self: return JAxisInfo.SELF;
            }
            return 0;
        }

        /// <summary>
        /// Get the base URI of the node.
        /// </summary>

        public Uri BaseUri
        {
            get { 
				string baseUriStr = ((JNodeInfo)value).getBaseURI();
				if (baseUriStr == null || baseUriStr.Equals("")) {
					return null;
				}
				return new Uri(baseUriStr); 
			}
        }

        /// <summary>
        /// Get the document URI of the node.
        /// </summary>

        public Uri DocumentUri
        {
            get
            {
                String s = ((JNodeInfo)value).getSystemId();
                if (s == null || s.Length == 0)
                {
                    return null;
                }
                return new Uri(s);
            }
        }

        /// <summary>
        /// Send the node (that is, the subtree rooted at this node) to an <c>XmlWriter</c>
        /// </summary>
        /// <remarks>
        /// Note that an <c>XmlWriter</c> can only handle a well-formed XML document. This method
        /// will therefore signal an exception if the node is a document node with no children, or with
        /// more than one element child.
        /// </remarks>
        /// <param name="writer">
        /// The <c>XmlWriter</c> to which the node is to be written
        /// </param>

        public void WriteTo(XmlWriter writer)
        {
            JNodeInfo node = ((JNodeInfo)value);
            JDotNetReceiver receiver = new JDotNetReceiver(writer);
            receiver.setPipelineConfiguration(node.getConfiguration().makePipelineConfiguration());
            receiver.open();
			node.copy(receiver, net.sf.saxon.om.CopyOptions.ALL_NAMESPACES, JExplicitLocation.UNKNOWN_LOCATION);
            receiver.close();
        }

        /// <summary>
        /// Return a serialization of this node as lexical XML
        /// </summary>
        /// <remarks>
        /// <para>In the case of an element node, the result will be a well-formed
        /// XML document serialized as defined in the W3C XSLT/XQuery serialization specification,
		/// using options <c>method="xml"</c>, <c>indent="yes"</c>, <c>omit-xml-declaration="yes"</c>.</para>
        /// <para>In the case of a document node, the result will be a well-formed
        /// XML document provided that the document node contains exactly one element child,
        /// and no text node children. In other cases it will be a well-formed external
        /// general parsed entity.</para>
        /// <para>In the case of an attribute node, the output is a string in the form
        /// <c>name="value"</c>. The name will use the original namespace prefix.</para>
        /// <para>Other nodes, such as text nodes, comments, and processing instructions, are
        /// represented as they would appear in lexical XML.</para>
        /// </remarks>

        public String OuterXml
        {
            get
            {
                JNodeInfo node = ((JNodeInfo)value);

                if (node.getNodeKind() == JType.ATTRIBUTE)
                {
                    String val = node.getStringValue().Replace("\"", "&quot;");
                    val = val.Replace("<", "&lt;");
                    val = val.Replace("&", "&amp;");
                    return node.getDisplayName() + "=\"" + val + '"';
                } else if (node.getNodeKind() == JType.NAMESPACE) {
                    String val = node.getStringValue().Replace("\"", "&quot;");
                    val = val.Replace("<", "&lt;");
                    val = val.Replace("&", "&amp;");
                    String name = node.getDisplayName();
                    name = (name.Equals("") ? "xmlns" : "xmlns:" + name);
                    return name + "=\"" + val + '"';
                }
                return net.sf.saxon.query.QueryResult.serialize(node);
                
            }
        }

        /// <summary>
		/// Two instances of <c>XdmNode</c> are equal if they represent the same node. That is, the <c>Equals()</c>
        /// method returns the same result as the XPath "is" operator.
        /// </summary>
        /// <param name="obj">The object node to be compared</param>
         
        public override bool Equals(object obj)
        {
            return obj is XdmNode && ((JNodeInfo)value).equals((JNodeInfo)((XdmNode)obj).value);
        }

        /// <summary>
		/// The hash code of a node reflects the equality relationship: if two <c>XdmNode</c> instances
        /// represent the same node, then they have the same hash code
        /// </summary>

        public override int GetHashCode()
        {
            return ((JNodeInfo)value).hashCode();
        }

        /// <summary>
        /// Return a string representation of the node.
        /// </summary>
        /// <remarks>
		/// This method returns the value of the <see cref="OuterXml"/> property.
		/// To get the string value of a node as defined in XPath, use the <see cref="StringValue"/> property.
        /// </remarks>

        public override String ToString()
        {
            return OuterXml;
        }

        internal void SetProcessor(Processor processor)
        {
            Implementation.getConfiguration().setProcessor(processor.JProcessor);
        }

        /// <summary>
        /// Escape hatch to the underlying class in the Java implementation
        /// </summary>

        public JNodeInfo Implementation
        {
            get { return ((JNodeInfo)value); }
        }


    }


    /// <summary inherits="XdmValue">
    /// The class <c>XdmEmptySequence</c> represents an empty sequence in the XDM Data Model.
    /// </summary>
    /// <remarks>
    /// <para>An empty sequence <i>may</i> also be represented by an <c>XdmValue</c> whose length
    /// happens to be zero. Applications should therefore not test to see whether an object
    /// is an instance of this class in order to decide whether it is empty.</para>
    /// <para>In interfaces that expect an <c>XdmItem</c>, an empty sequence is represented
    /// by a CLI <c>null</c> value.</para> 
    /// </remarks>

    [Serializable]
    public sealed class XdmEmptySequence : XdmValue
    {

        ///<summary>The singular instance of this class</summary>

        public static XdmEmptySequence INSTANCE = new XdmEmptySequence();

        private XdmEmptySequence()
        {
            this.value = JEmptySequence.getInstance();
        }
    }


    /// <summary>
	/// The <c>QName</c> class represents an instance of <c>xs:QName</c>, as defined in the XPath 2.0
    /// data model. Internally, it has three components, a namespace URI, a local name, and
    /// a prefix. The prefix is intended to be used only when converting the value back to 
    /// a string.
    /// </summary>
    /// <remarks>
	/// Note that a <c>QName</c> is not itself an <c>XdmItem</c> in this model; however it can
    /// be converted to an <c>XdmAtomicValue</c>.
    /// </remarks>    

    [Serializable]
    public sealed class QName
    {

        private JQName sqName;
        //private String prefix;
        //private String uri;
        //private String local;
        //int hashcode = -1;      // evaluated lazily


        private static String XS = NamespaceConstant.SCHEMA;

        /// <summary>QName constant for the name xs:string</summary>
        public static readonly QName XS_STRING = new QName(XS, "xs:string");

        /// <summary>QName constant for the name xs:integer</summary>
        public static readonly QName XS_INTEGER = new QName(XS, "xs:integer");

        /// <summary>QName constant for the name xs:double</summary>
        public static readonly QName XS_DOUBLE = new QName(XS, "xs:double");

        /// <summary>QName constant for the name xs:float</summary>
        public static readonly QName XS_FLOAT = new QName(XS, "xs:float");

        /// <summary>QName constant for the name xs:decimal</summary>
        public static readonly QName XS_DECIMAL = new QName(XS, "xs:decimal");

        /// <summary>QName constant for the name xs:boolean</summary>
        public static readonly QName XS_BOOLEAN = new QName(XS, "xs:boolean");

        /// <summary>QName constant for the name xs:anyURI</summary>
        public static readonly QName XS_ANYURI = new QName(XS, "xs:anyURI");

        /// <summary>QName constant for the name xs:QName</summary>
        public static readonly QName XS_QNAME = new QName(XS, "xs:QName");

        /// <summary>QName constant for the name xs:untypedAtomic</summary>
        public static readonly QName XS_UNTYPED_ATOMIC = new QName(XS, "xs:untypedAtomic");

        /// <summary>QName constant for the name xs:untypedAtomic (for backwards compatibility)</summary>
        public static readonly QName XDT_UNTYPED_ATOMIC = new QName(XS, "xs:untypedAtomic");

        /// <summary>
		/// Construct a <c>QName</c> representing a name in no namespace
        /// </summary>
        /// <remarks>
        /// This constructor does not check that the components of the QName are
        /// lexically valid.
        /// </remarks>
        /// <param name="local">The local part of the name
        /// </param>

        public QName(String local)
        {
            // TODO: check for validity
            int colon = local.IndexOf(':');
            if (colon < 0)
            {
                sqName = new JQName("", "", local);
            }
            else {
                
                    throw new ArgumentException("Local name contains a colon");
                }             
        }

        /// <summary>
		/// Construct a <c>QName</c> using a namespace URI and a lexical representation.
        /// The lexical representation may be a local name on its own, or it may 
        /// be in the form <c>prefix:local-name</c>
        /// </summary>
        /// <remarks>
        /// This constructor does not check that the components of the QName are
        /// lexically valid.
        /// </remarks>
        /// <param name="uri">The namespace URI. Use either the string "" or null
        /// for names that are not in any namespace.
        /// </param>
        /// <param name="lexical">Either the local part of the name, or the prefix
        /// and local part in the format <c>prefix:local</c>
        /// </param>

        public QName(String uri, String lexical)
        {
            // TODO: check for validity
            uri = (uri == null ? "" : uri);
            int colon = lexical.IndexOf(':');
            if (colon < 0)
            {
                sqName = new JQName("", uri, lexical);
            }
            else
            {

                string prefix = lexical.Substring(0, colon);
                string local = lexical.Substring(colon + 1);
                sqName = new JQName(prefix, uri, local);
            }
        }

        /// <summary>
		/// Construct a <c>QName</c> using a namespace prefix, a namespace URI, and a local name
        /// (in that order).
        /// </summary>
        /// <remarks>
        /// This constructor does not check that the components of the QName are
        /// lexically valid.
        /// </remarks>
        /// <param name="prefix">The prefix of the name. Use either the string ""
        /// or null for names that have no prefix (that is, they are in the default
        /// namespace)</param>
        /// <param name="uri">The namespace URI. Use either the string "" or null
        /// for names that are not in any namespace.
        /// </param>
        /// <param name="local">The local part of the name</param>

        public QName(String prefix, String uri, String local)
        {
            sqName = new JQName(prefix, uri, local);
        }

        /// <summary>
		/// Construct a <c>QName</c> from a lexical QName, supplying an element node whose
        /// in-scope namespaces are to be used to resolve any prefix contained in the QName.
        /// </summary>
        /// <remarks>
        /// <para>This constructor checks that the components of the QName are
        /// lexically valid.</para>
        /// <para>If the lexical QName has no prefix, the name is considered to be in the
        /// default namespace, as defined by <c>xmlns="..."</c>.</para>
        /// </remarks>
        /// <param name="lexicalQName">The lexical QName, in the form <code>prefix:local</code>
        /// or simply <c>local</c>.</param>
        /// <param name="element">The element node whose in-scope namespaces are to be used
        /// to resolve the prefix part of the lexical QName.</param>
        /// <exception cref="ArgumentException">If the prefix of the lexical QName is not in scope</exception>
        /// <exception cref="ArgumentException">If the lexical QName is invalid 
        /// (for example, if it contains invalid characters)</exception>
        /// 

        public QName(String lexicalQName, XdmNode element)
        {
            try
            {
                JNodeInfo node = (JNodeInfo)element.value;
				sqName = new JQName(JStructuredQName.fromLexicalQName(lexicalQName, true, true, new JInscopeNamespaceResolver(node)));
				
            }
            catch (net.sf.saxon.trans.XPathException err)
            {
                throw new ArgumentException(err.getMessage());
            }
        }

        /// <summary>
        /// Construct a <c>QName</c> from an <c>XmlQualifiedName</c> (as defined in the
        /// <c>System.Xml</c> package).
        /// </summary>
        /// <remarks>
        /// Note that an <c>XmlQualifiedName</c> does not contain any prefix, so the result
        /// will always have a prefix of ""
        /// </remarks>
		/// <param name="qualifiedName">The <c>XmlQualifiedName</c></param>

        public QName(XmlQualifiedName qualifiedName)
        {
            string uri = qualifiedName.Namespace;
            string local = qualifiedName.Name;
            string prefix = String.Empty;
            sqName = new JQName(prefix, uri, prefix);
        }

        //  internal constructor from a QNameValue

        internal QName(JQNameValue q)
        {
            sqName = new JQName(q.getPrefix(), q.getNamespaceURI(), q.getLocalName());
        }

        //  internal constructor with JQName object as argument

        internal QName(JQName q)
        {
            sqName = q;
        }

        /// <summary>
		/// Factory method to construct a <c>QName</c> from a string containing the expanded
        /// QName in Clark notation, that is, <c>{uri}local</c>
        /// </summary>
        /// <remarks>
        /// The prefix part of the <c>QName</c> will be set to an empty string.
        /// </remarks>
        /// <param name="expandedName">The URI in Clark notation: <c>{uri}local</c> if the
        /// name is in a namespace, or simply <c>local</c> if not.</param> 

        public static QName FromClarkName(String expandedName)
        {
            String namespaceURI;
            String localName;
            if (expandedName[0] == '{')
            {
                int closeBrace = expandedName.IndexOf('}');
                if (closeBrace < 0)
                {
                    throw new ArgumentException("No closing '}' in Clark name");
                }
                namespaceURI = expandedName.Substring(1, closeBrace - 1);
                if (closeBrace == expandedName.Length)
                {
                    throw new ArgumentException("Missing local part in Clark name");
                }
                localName = expandedName.Substring(closeBrace + 1);
            }
            else
            {
                namespaceURI = "";
                localName = expandedName;
            }

            return new QName("", namespaceURI, localName);
        }


        /// <summary>
		/// Factory method to construct a <c>QName</c> from a string containing the expanded
        /// QName in EQName notation, that is, <c>Q{uri}local</c>
        /// </summary>
        /// <remarks>
        /// The prefix part of the <c>QName</c> will be set to an empty string.
        /// </remarks>
        /// <param name="expandedName">The QName in EQName notation: <c>Q{uri}local</c>. 
		/// For a name in no namespace, either of the
        /// forms <c>Q{}local</c> or simply <c>local</c> are accepted.</param>
        /// <returns> the QName corresponding to the supplied name in EQName notation. This will always
        /// have an empty prefix.</returns>
       
        public static QName FromEQName(String expandedName)
        {
            String namespaceURI;
            String localName;
            if (expandedName[0] == 'Q' && expandedName[1] == '{')
            {
                int closeBrace = expandedName.IndexOf('}');
                if (closeBrace < 0)
                {
                    throw new ArgumentException("No closing '}' in EQName");
                }
                namespaceURI = expandedName.Substring(2, closeBrace);
                if (closeBrace == expandedName.Length)
                {
                    throw new ArgumentException("Missing local part in EQName");
                }
                localName = expandedName.Substring(closeBrace + 1);
            }
            else
            {
                namespaceURI = "";
                localName = expandedName;
            }

            return new QName("", namespaceURI, localName);
        }

        // internal method: Factory method to construct a QName from Saxon's internal <c>StructuredQName</c>
        // representation.

        internal static QName FromStructuredQName(JStructuredQName sqn) {
            return new QName(sqn.getPrefix(), sqn.getURI(), sqn.getLocalPart());
        }

        /// <summary>
		/// Register a <c>QName</c> with the <c>Processor</c>. This makes comparison faster
        /// when the QName is compared with others that are also registered with the <c>Processor</c>.
        /// Depreacted method.
        /// </summary>
        /// <remarks>
        /// A given <c>QName</c> object can only be registered with one <c>Processor</c>.
        /// </remarks>
		/// <param name="processor">The <c>Processor</c> in which the name is to be registered.</param>
        [System.Obsolete("This method is no longer in use")]
        public void Register(Processor processor)
        {}

        /// <summary>
		/// Validate the <c>QName</c> against the XML 1.0 or XML 1.1 rules for valid names.
        /// </summary>
        /// <param name="processor">This argument is no longer used (at one time it was used
        /// to establish whether XML 1.0 or XML 1.1 rules should be used for validation, but the
        /// two versions of the XML specification have since been aligned).</param>
        /// <returns>true if the name is valid, false if not</returns>

        public bool IsValid(Processor processor)
        {
            return IsValid();
        }
        
        /// <summary>
		/// Validate the <c>QName</c> against the XML rules for valid names.
        /// </summary>
        /// <returns>true if the name is valid, false if not</returns>

        public bool IsValid()
        {
            if(this.Prefix != String.Empty)
            {
                if (!JNameChecker.isValidNCName(Prefix))
                {
                    return false;
                }
            }
            if (!JNameChecker.isValidNCName(this.LocalName))
            {
                return false;
            }
            return true;
        }

		/// <summary>Get the prefix of the <c>QName</c>. This plays no role in operations such as comparison
        /// of QNames for equality, but is retained (as specified in XPath) so that a string representation
        /// can be reconstructed.
        /// </summary>
        /// <remarks>
        /// Returns the zero-length string in the case of a QName that has no prefix.
        /// </remarks>

        public String Prefix
        {
            get { return sqName.getPrefix(); }
        }

		/// <summary>Get the namespace URI of the <c>QName</c>. Returns "" (the zero-length string) if the
        /// QName is not in a namespace.
        /// </summary>

        public String Uri
        {
            get { return sqName.getNamespaceURI(); }
        }

		/// <summary>Get the local part of the <c>QName</c></summary>

        public String LocalName
        {
            get { return sqName.getLocalName(); }
        }

        /// <summary>Get the expanded name, as a string using the notation devised by James Clark.
        /// If the name is in a namespace, the resulting string takes the form <c>{uri}local</c>.
        /// Otherwise, the value is the local part of the name.
        /// </summary>

        public String ClarkName
        {
            get
            {
                string uri = Uri;
                if (uri.Equals(""))
                {
                    return LocalName;
                }
                else
                {
                    return "{" + uri + "}" + LocalName;
                }
            }
        }

        /// <summary>Get the expanded name in EQName format, that is <c>Q{uri}local</c>. A no namespace name is returned as <c>Q{}local</c>.
        /// </summary>
        public String EQName
        {
            get
            {
                return "Q{" + Uri + "}" + LocalName;
            }
        }

        /// <summary>
        /// Convert the value to a string. The resulting string is the lexical form of the QName,
        /// using the original prefix if there was one.
        /// </summary>

        public override String ToString()
        {

            if (Prefix.Equals(""))
            {
                return LocalName;
            }
            else
            {
				return Prefix + ":" + LocalName;
            }
        }

        /// <summary>
		/// Get a hash code for the <c>QName</c>, to support equality matching. This supports the
        /// semantics of equality, which considers only the namespace URI and local name, and
        /// not the prefix.
        /// </summary>
        /// <remarks>
        /// The algorithm for allocating a hash code does not depend on registering the QName 
        /// with the <c>Processor</c>.
        /// </remarks>

        public override int GetHashCode()
        {
            return sqName.hashCode();
        }

        /// <summary>
        /// Test whether two QNames are equal. This supports the
        /// semantics of equality, which considers only the namespace URI and local name, and
        /// not the prefix.
        /// </summary>
		/// <param name="other">The value to be compared with this <c>QName</c>. If this value is not a <c>QName</c>, the
        /// result is always false. Otherwise, it is true if the namespace URI and local name both match.</param>

        public override bool Equals(Object other)
        {
            if (!(other is QName))
            {
                return false;
            }
            return sqName.equals(((QName)other).sqName);
        }

        /// <summary>
        /// Convert the value to an <c>XmlQualifiedName</c> (as defined in the
        /// <c>System.Xml</c> package)
        /// </summary>
        /// <remarks>
        /// Note that this loses the prefix.
        /// </remarks>

        public XmlQualifiedName ToXmlQualifiedName()
        {
            return new XmlQualifiedName(LocalName, Uri);
        }

		// internal method: Convert to a net.sf.saxon.value.QNameValue

        internal JQNameValue ToQNameValue()
        {
            return new JQNameValue(sqName.getPrefix(), sqName.getNamespaceURI(), sqName.getLocalName(), null);
		}

		// internal method

        internal JStructuredQName ToStructuredQName()
        {
            return new JStructuredQName(Prefix, Uri, LocalName);
        }

        // internal method

        internal JQName UnderlyingQName()
        {
            return sqName;
        }




    }

    /// <summary>
    /// This class represents an enumeration of the values in an XPath
    /// sequence. It implements the <c>IEnumerator</c> interface, and the objects
    /// returned are always instances of <c>XdmItem</c>.
    /// </summary>
    /// <remarks>
    /// Because the underlying value can be evaluated lazily, it is possible
    /// for exceptions to occur as the sequence is being read.
    /// </remarks>

    public interface IXdmEnumerator<out XdmItem> : IEnumerator<XdmItem>
    {


    }

    /// <summary>
    /// This class represents an enumeration of the values in an XPath
    /// sequence. It implements the <c>IEnumerable</c> interface, and the objects
    /// returned are always instances of <c>XdmItem</c>.
    /// </summary>
    /// <remarks>
    /// Because the underlying value can be evaluated lazily, it is possible
    /// for exceptions to occur as the sequence is being read.
    /// </remarks>
    public interface IXdmEnumerable<out XdmItem> : IEnumerable<XdmItem>
    {


    }

    /// <summary>
    /// This class is an implementation of <c>IXdmEnumerator</c> that wraps
	/// a (Java) <c>SequenceIterator</c>.
    /// </summary>
    /// <remarks>
    /// Because the underlying value can be evaluated lazily, it is possible
    /// for exceptions to occur as the sequence is being read.
    /// </remarks>

    [Serializable]
    internal class SequenceEnumerator<T> : IXdmEnumerator<T>
          where T : XdmItem
    {

        private JSequenceXdmIterator iter;
        private JItem current;

        internal SequenceEnumerator(JSequenceXdmIterator iter)
        {
            this.iter = iter;
            current = null;
        }


        /// <summary>Return the current item in the sequence</summary>
        /// <returns>An object which will always be an instance of <c>XdmItem</c></returns>
        /// 
        public XdmItem Current
        {
            get {
                return current == null ? null : (XdmItem)XdmValue.Wrap(current);
            }
        }

        object IEnumerator.Current
        {
            get {
                return Current;
            }
        }

        T IEnumerator<T>.Current
        {

            get { return (T)Current; }
        }

        /// <summary>Move to the next item in the sequence</summary>
        /// <returns>true if there are more items in the sequence</returns>

        public bool MoveNext()
        {
            try
            {
                if (!iter.hasNext())
                {
                    return false;
                }
            }
            catch (net.sf.saxon.s9api.SaxonApiUncheckedException e)
            {
                return false;
            }
            net.sf.saxon.s9api.XdmItem nextXdmItem = iter.next();
                if (nextXdmItem != null)
                {
                    JItem nextItem = nextXdmItem.getUnderlyingValue();
                    current = nextItem;
                    return (nextItem != null);
                }
                return false;
            
        }

        /// <summary>Deprecated. Reset the enumeration so that the next call of
        /// <c>MoveNext</c> will position the enumeration at the
        /// first item in the sequence</summary>
        [System.Obsolete("MethodAccessException no longer used")]
        public void Reset()
        {
            
        }

        /// <summary>
        /// The Dispose method does not have any effect on this Enumerator
        /// </summary>
        public void Dispose()
        {
           
        }
    }

    /// <summary>
	/// Implementation of the (Java) interface <c>SequenceIterator</c> that wraps
	/// a (.NET) <c>IXdmEnumerator</c>
    /// </summary>

    internal class DotNetSequenceIterator : JSequenceIterator
    {
        // TODO: catch errors and throw an XPathException if necessary.

        IEnumerator iter;
        int pos = 0;

        public DotNetSequenceIterator(IEnumerator iter)
        {
            this.iter = iter;
        }

        public JItem next()
        {
            if (pos < 0)
            {
                return null;
            }
            bool more = iter.MoveNext();
            if (more)
            {
                pos++;
                XdmItem i = (XdmItem)iter.Current;
                return (JItem)i.Unwrap();
            }
            else
            {
                pos = -1;
                return null;
            }

        }

        public JItem current()
        {
            if (pos < 0)
            {
                return null;
            }
            XdmItem i = (XdmItem)iter.Current;
            return (JItem)i.Unwrap();
        }

        public int position()
        {
            return pos;
        }

        public void close()
        {
        }


        public int getProperties()
        {
            return 0;
        }

        public void forEachOrFail(JItemConsumer consumer)
        {
            JItem item = null;

            while ((item = next()) != null)
            {
                consumer.accept(item);
            }
        }

        public void forEachOrFail(JSequenceIterator value1, JItemConsumer consumer)
        {
            JItem item = null;

            while ((item = value1.next()) != null)
            {
                consumer.accept(item);
            }
        }

        public JGroundedValue materialize() {
            return JSequenceExtent.fromIterator(this);
        }

        public JGroundedValue materialize(JSequenceIterator value)
        {
            return JSequenceExtent.fromIterator(value);
        }

       

        public void Dispose()
        {
            throw new NotImplementedException();
        }

        JGroundedValue JSequenceIterator.materialize()
        {
            return JSequenceExtent.fromIterator(this);
        }


    }

    /// <summary>
    /// Enumeration identifying the thirteen XPath axes
    /// </summary>

    public enum XdmAxis
    {
        /// <summary>The XPath ancestor axis</summary> 
        Ancestor,
        /// <summary>The XPath ancestor-or-self axis</summary> 
        AncestorOrSelf,
        /// <summary>The XPath attribute axis</summary> 
        Attribute,
        /// <summary>The XPath child axis</summary> 
        Child,
        /// <summary>The XPath descendant axis</summary> 
        Descendant,
        /// <summary>The XPath descandant-or-self axis</summary> 
        DescendantOrSelf,
        /// <summary>The XPath following axis</summary> 
        Following,
        /// <summary>The XPath following-sibling axis</summary> 
        FollowingSibling,
        /// <summary>The XPath namespace axis</summary> 
        Namespace,
        /// <summary>The XPath parent axis</summary> 
        Parent,
        /// <summary>The XPath preceding axis</summary> 
        Preceding,
        /// <summary>The XPath preceding-sibling axis</summary> 
        PrecedingSibling,
        /// <summary>The XPath self axis</summary> 
        Self
    }

    /// <summary>
    /// An implementation of <code>IXdmEnumerator</code> that iterates over an empty sequence.
    /// </summary>

    public class EmptyEnumerator<T> : IXdmEnumerator<T>
       where T : XdmItem
    {

        /// <summary>
		/// Create an instance of the enumerator with the <c>XdmItem</c> as the generic type
        /// </summary>
        public static EmptyEnumerator<XdmItem> INSTANCE = new EmptyEnumerator<XdmItem>();

        /// <summary>
		/// Create an instance of the enumerator with the <c>XdmNode</c> as the generic type
        /// </summary>
        public static EmptyEnumerator<XdmNode> NODE_INSTANCE = new EmptyEnumerator<XdmNode>();

        private EmptyEnumerator() { }

        /// <summary>
        /// Reset the enumerator
        /// </summary>
        public void Reset() { }
        



        object IEnumerator.Current
        {
            get
            {
                return null;
            }
        }

        /// <summary>
        /// The current item in the enumerator
        /// </summary>
        T IEnumerator<T>.Current
        {
            get
            {
                return null;
            }
        }

        /// <summary>
        /// Move to the next item in the enumerator..
        /// </summary>
        /// <returns>true if successful move, false otherwise.</returns>
        public bool MoveNext()
        {
            return false;
        }

		/// <summary>
		/// The Dispose method is not implemented on this Enumerator
		/// </summary>
        public void Dispose()
        {
            throw new NotImplementedException();
        }
    }


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////