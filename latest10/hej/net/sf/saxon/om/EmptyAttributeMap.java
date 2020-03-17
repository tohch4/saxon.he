////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020 Saxonica Limited
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.om;


import org.jetbrains.annotations.NotNull;

import java.util.Collections;
import java.util.Iterator;

/**
 * An implementation of AttributeMap suitable for small collections of attributes (say, up to four or five).
 * Searching for a particular attribute involves a sequential search, and adding a new attribute constructs
 * a full copy.
 */

public class EmptyAttributeMap implements AttributeMap {

    private static EmptyAttributeMap THE_INSTANCE = new EmptyAttributeMap();

    private EmptyAttributeMap(){}

    public static EmptyAttributeMap getInstance() {
        return THE_INSTANCE;
    }

    /**
     * Return the number of attributes in the map.
     *
     * @return The number of attributes in the map.
     */

    public int size() {
        return 0;
    }

    @Override
    public AttributeInfo get(NodeName name) {
        return null;
    }

    @Override
    public AttributeInfo get(String uri, String local) {
        return null;
    }

    public AttributeInfo getByFingerprint(int fingerprint, NamePool namePool) {
        return null;
    }

    @Override
    public AttributeMap put(AttributeInfo att) {
        return SingletonAttributeMap.of(att);
    }

    @Override
    public AttributeMap remove(NodeName name) {
        return this;
    }

    @NotNull
    @Override
    public Iterator<AttributeInfo> iterator() {
        return Collections.emptyIterator();
    }

    public AttributeMap apply(java.util.function.Function<AttributeInfo, AttributeInfo> mapper) {
        return this;
    }
}

