////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is “Incompatible With Secondary Licenses”, as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.style;

import net.sf.saxon.lib.Validation;
import net.sf.saxon.om.AxisInfo;
import net.sf.saxon.om.NodeInfo;
import net.sf.saxon.om.StandardNames;
import net.sf.saxon.om.StructuredQName;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.tree.iter.AxisIterator;

/**
 * Class representing xsl:stylesheet, xsl:transform, or xsl:package
 */
public abstract class XSLModuleRoot extends StyleElement {

    public static final int ANNOTATION_UNSPECIFIED = 0;
    public static final int ANNOTATION_STRIP = 1;
    public static final int ANNOTATION_PRESERVE = 2;


    protected StructuredQName defaultMode;
    protected int defaultValidation = Validation.STRIP;

    /**
      * Get the default mode
      *
      * @return the default mode name for this stylesheet module. A return value of null indicates either that
      *         no default mode was specified, or that default-mode="#unnamed" was specified.
      */

     public StructuredQName getDefaultMode() {
         return defaultMode;
     }

    /**
     * Get the value of the default validation attribute
     *
     * @return the value of the default-validation attribute, as a constant such
     *         as {@link net.sf.saxon.lib.Validation#STRIP}
     */

    public int getDefaultValidation() {
        return defaultValidation;
    }

    /**
     * Process the attributes of every node in the stylesheet module
     */

    public void processAllAttributes() throws XPathException {
        prepareAttributes();
        AxisIterator iter = iterateAxis(AxisInfo.CHILD);
        NodeInfo node;
        while ((node = iter.next()) != null) {
            if (node instanceof StyleElement) {
                try {
                    ((StyleElement) node).processAllAttributes();
                } catch (XPathException err) {
                    ((StyleElement) node).compileError(err);
                }
            }
        }
    }


    @Override
    public void index(ComponentDeclaration decl, StylesheetPackage top) throws XPathException {
        compileError(getDisplayName() + " can appear only as the outermost element", "XTSE0010");
    }

    /**
     * Get the value of the input-type-annotations attribute, for this module alone.
     * The value is an or-ed combination of the two bits
     * {@link #ANNOTATION_STRIP} and {@link #ANNOTATION_PRESERVE}
     *
     * @return the value if the input-type-annotations attribute in this stylesheet module
     * @throws net.sf.saxon.trans.XPathException
     *          if an error is detected
     */

    public int getInputTypeAnnotationsAttribute() throws XPathException {
        String inputTypeAnnotationsAtt = getAttributeValue("", StandardNames.INPUT_TYPE_ANNOTATIONS);
        if (inputTypeAnnotationsAtt != null) {
            if (inputTypeAnnotationsAtt.equals("strip")) {
                return ANNOTATION_STRIP;
            } else if (inputTypeAnnotationsAtt.equals("preserve")) {
                return ANNOTATION_PRESERVE;
            } else if (inputTypeAnnotationsAtt.equals("unspecified")) {
                return ANNOTATION_UNSPECIFIED;
            } else {
                compileError("Invalid value for input-type-annotations attribute. " +
                        "Permitted values are (strip, preserve, unspecified)", "XTSE0020");
            }
        }
        return -1;
    }





}

