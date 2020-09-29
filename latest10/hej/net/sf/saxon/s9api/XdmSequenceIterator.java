////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020 Saxonica Limited
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.s9api;

import net.sf.saxon.om.SequenceIterator;
import net.sf.saxon.s9api.streams.XdmStream;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.tree.iter.AxisIterator;
import net.sf.saxon.tree.iter.LookaheadIterator;
import net.sf.saxon.tree.iter.LookaheadIteratorImpl;
import net.sf.saxon.tree.iter.SingletonIterator;
import net.sf.saxon.tree.iter.UnfailingIterator;

import java.util.Iterator;
import java.util.Spliterator;
import java.util.Spliterators;
import java.util.stream.Stream;
import java.util.stream.StreamSupport;

/**
 * An iterator over an XPath sequence.
 * <p>This class implements the standard Java Iterator interface.</p>
 * <p>Because the <code>Iterator</code> interface does not define any checked
 * exceptions, the <code>hasNext()</code> method of this iterator throws an unchecked
 * exception if a dynamic error occurs while evaluating the expression. Applications
 * wishing to control error handling should take care to catch this exception.</p>
 */
public class XdmSequenceIterator<T extends XdmItem> implements Iterator<T> {
    private final LookaheadIterator base;

    protected XdmSequenceIterator(SequenceIterator base) {
        try {
            this.base = LookaheadIteratorImpl.makeLookaheadIterator(base);
        } catch (XPathException xe) {
            throw new SaxonApiUncheckedException(xe);
        }
    }

    public XdmSequenceIterator(UnfailingIterator base) {
        try {
            this.base = LookaheadIteratorImpl.makeLookaheadIterator(base);
        } catch (XPathException xe) {
            throw new SaxonApiUncheckedException(xe);
        }
    }

    public static XdmSequenceIterator<XdmNode> ofNodes(AxisIterator base) {
        return new XdmSequenceIterator<>(base);
    }

    public static XdmSequenceIterator<XdmAtomicValue> ofAtomicValues(UnfailingIterator base) {
        return new XdmSequenceIterator<>(base);
    }

    protected static XdmSequenceIterator<XdmNode> ofNode(XdmNode node) {
        return new XdmSequenceIterator<>(SingletonIterator.makeIterator(node.getUnderlyingNode()));
    }

    /**
     * Returns <tt>true</tt> if the iteration has more elements. (In other
     * words, returns <tt>true</tt> if <tt>next</tt> would return an element
     * rather than throwing an exception.)
     *
     * @return <tt>true</tt> if the iterator has more elements.
     * @throws SaxonApiUncheckedException if a dynamic error occurs during XPath evaluation that
     *                                    is detected at this point.
     */
    @Override
    public boolean hasNext() throws SaxonApiUncheckedException {
        return base.hasNext();
    }

    /**
     * Returns the next element in the iteration.  Calling this method
     * repeatedly until the {@link #hasNext()} method returns false will
     * return each element in the underlying collection exactly once.
     *
     * @return the next element in the iteration.
     * @throws java.util.NoSuchElementException
     *          iteration has no more elements.
     */
    @Override
    public T next() {
        try {
            //noinspection unchecked
            return (T) XdmItem.wrapItem(base.next());
        } catch (XPathException xe) {
            // I don't believe this is actually possible in this case
            throw new SaxonApiUncheckedException(xe);
        }
    }

    /**
     * Not supported on this implementation.
     *
     * @throws UnsupportedOperationException always
     */

    @Override
    public void remove() {
        throw new UnsupportedOperationException();
    }

    /**
     * The close() method should be called to release resources if the caller wants to stop reading
     * data before reaching the end. This is particularly relevant if the query uses saxon:stream()
     * to read its input, since there will then be another thread supplying data, which will be left
     * in suspended animation if no-one is consuming the data.
     * @since 9.5.1.5 (see bug 2016)
     */

    public void close() {
        base.close();
    }


    /**
     * Convert this iterator to a Stream
     *
     * @return a Stream delivering the same items as this iterator
     * @since 9.9
     */
    public XdmStream<T> stream() {
        Stream<T> base = StreamSupport.stream(Spliterators.spliteratorUnknownSize(
                this, Spliterator.ORDERED), false);
        base = base.onClose(XdmSequenceIterator.this::close);
        return new XdmStream<>(base);
    }

}

