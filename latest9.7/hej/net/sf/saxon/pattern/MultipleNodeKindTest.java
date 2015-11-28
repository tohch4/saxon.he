////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is “Incompatible With Secondary Licenses”, as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.pattern;

import net.sf.saxon.om.NodeInfo;
import net.sf.saxon.om.NodeName;
import net.sf.saxon.tree.tiny.TinyTree;
import net.sf.saxon.tree.util.FastStringBuffer;
import net.sf.saxon.type.PrimitiveUType;
import net.sf.saxon.type.SchemaType;
import net.sf.saxon.type.Type;
import net.sf.saxon.type.UType;

import java.util.LinkedList;

/**
 * An MultipleNodeKindTest is a nodetest that matches nodes belonging to any subset of possible
 * node kinds, for example element and document nodes, or attribute and namespace nodes
 */

public final class MultipleNodeKindTest extends NodeTest {

    public final static MultipleNodeKindTest PARENT_NODE =
            new MultipleNodeKindTest(UType.DOCUMENT.union(UType.ELEMENT));

    public final static MultipleNodeKindTest DOC_ELEM_ATTR =
        new MultipleNodeKindTest(UType.DOCUMENT.union(UType.ELEMENT).union(UType.ATTRIBUTE));

    UType uType;
    int nodeKindMask;

    public MultipleNodeKindTest(UType u) {
        this.uType = u;
        if (UType.DOCUMENT.overlaps(u)) {
            nodeKindMask |= 1 << Type.DOCUMENT;
        }
        if (UType.ELEMENT.overlaps(u)) {
            nodeKindMask |= 1 << Type.ELEMENT;
        }
        if (UType.ATTRIBUTE.overlaps(u)) {
            nodeKindMask |= 1 << Type.ATTRIBUTE;
        }
        if (UType.TEXT.overlaps(u)) {
            nodeKindMask |= 1 << Type.TEXT;
        }
        if (UType.COMMENT.overlaps(u)) {
            nodeKindMask |= 1 << Type.COMMENT;
        }
        if (UType.PI.overlaps(u)) {
            nodeKindMask |= 1 << Type.PROCESSING_INSTRUCTION;
        }
        if (UType.NAMESPACE.overlaps(u)) {
            nodeKindMask |= 1 << Type.NAMESPACE;
        }
    }

    /**
     * Get the corresponding {@link net.sf.saxon.type.UType}. A UType is a union of primitive item
     * types.
     *
     * @return the smallest UType that subsumes this item type
     */
    public UType getUType() {
        return uType;
    }

    /**
     * Test whether this node test is satisfied by a given node. This method is only
     * fully supported for a subset of NodeTests, because it doesn't provide all the information
     * needed to evaluate all node tests. In particular (a) it can't be used to evaluate a node
     * test of the form element(N,T) or schema-element(E) where it is necessary to know whether the
     * node is nilled, and (b) it can't be used to evaluate a node test of the form
     * document-node(element(X)). This in practice means that it is used (a) to evaluate the
     * simple node tests found in the XPath 1.0 subset used in XML Schema, and (b) to evaluate
     * node tests where the node kind is known to be an attribute.
     *
     * @param nodeKind   The kind of node to be matched
     * @param name       identifies the expanded name of the node to be matched.
     *                   The value should be null for a node with no name.
     * @param annotation The actual content type of the node
     */
    @Override
    public boolean matches(int nodeKind, NodeName name, SchemaType annotation) {
        return (nodeKindMask & (1<<nodeKind)) != 0;
    }

    /**
     * Test whether this node test is satisfied by a given node on a TinyTree. The node
     * must be a document, element, text, comment, or processing instruction node.
     * This method is provided
     * so that when navigating a TinyTree a node can be rejected without
     * actually instantiating a NodeInfo object.
     *
     * @param tree   the TinyTree containing the node
     * @param nodeNr the number of the node within the TinyTree
     * @return true if the node matches the NodeTest, otherwise false
     */

    public boolean matches(TinyTree tree, int nodeNr) {
        int nodeKind = tree.nodeKind[nodeNr];
        if (nodeKind == Type.WHITESPACE_TEXT) {
            nodeKind = Type.TEXT;
        }
        return (nodeKindMask & (1<<nodeKind)) != 0;
    }

    /**
     * Test whether this node test is satisfied by a given node. This alternative
     * method is used in the case of nodes where calculating the fingerprint is expensive,
     * for example DOM or JDOM nodes.
     *
     * @param node the node to be matched
     */

    public boolean matchesNode(NodeInfo node) {
        int nodeKind = node.getNodeKind();
        return (nodeKindMask & (1<<nodeKind)) != 0;
    }


    /**
     * Determine the default priority to use if this pattern appears as a match pattern
     * for a template with no explicit priority attribute.
     */

    public double getDefaultPriority() {
        return -0.5;
    }

    /**
     * Get a mask indicating which kinds of nodes this NodeTest can match. This is a combination
     * of bits: 1<<Type.ELEMENT for element nodes, 1<<Type.TEXT for text nodes, and so on.
     */

    public int getNodeKindMask() {
        return nodeKindMask;
    }

    /*@NotNull*/
    public String toString() {
        FastStringBuffer fsb = new FastStringBuffer(FastStringBuffer.C64);
        LinkedList<PrimitiveUType> types = new LinkedList<PrimitiveUType>(uType.decompose());
        format(types, fsb);
        return fsb.toString();
    }

    private void format(LinkedList<PrimitiveUType> list, FastStringBuffer fsb) {
        if (list.size() == 1) {
            fsb.append(list.get(0).toItemType().toString());
        } else {
            fsb.append('(');
            fsb.append(list.removeFirst().toItemType().toString());
            fsb.append('|');
            format(list, fsb);
            fsb.append(')');
        }
    }

    /**
     * Returns a hash code value for the object.
     */

    public int hashCode() {
        return uType.hashCode();
    }

}

