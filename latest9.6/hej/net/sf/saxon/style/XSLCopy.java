////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.style;

import net.sf.saxon.expr.*;
import net.sf.saxon.expr.instruct.*;
import net.sf.saxon.expr.parser.RoleLocator;
import net.sf.saxon.expr.parser.TypeChecker;
import net.sf.saxon.lib.Validation;
import net.sf.saxon.om.AttributeCollection;
import net.sf.saxon.om.AxisInfo;
import net.sf.saxon.om.StandardNames;
import net.sf.saxon.pattern.NodeKindTest;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.type.SchemaType;
import net.sf.saxon.value.SequenceType;
import net.sf.saxon.value.Whitespace;

/**
 * Handler for xsl:copy elements in stylesheet. <br>
 */

public class XSLCopy extends StyleElement {

    private String use;                     // value of use-attribute-sets attribute
    /*@Nullable*/ private AttributeSet[] attributeSets = null;
    private boolean copyNamespaces = true;
    private boolean inheritNamespaces = true;
    private int validationAction = Validation.PRESERVE;
    private SchemaType schemaType = null;
    private Expression select = null;
    private Expression onEmpty = null;
    private boolean selectSpecified = false;

    /**
     * Determine whether this node is an instruction.
     *
     * @return true - it is an instruction
     */

    public boolean isInstruction() {
        return true;
    }

    /**
     * Determine whether this type of element is allowed to contain a template-body
     *
     * @return true: yes, it may contain a template-body
     */

    public boolean mayContainSequenceConstructor() {
        return true;
    }

    public void prepareAttributes() throws XPathException {

        AttributeCollection atts = getAttributeList();
        String copyNamespacesAtt = null;
        String validationAtt = null;
        String typeAtt = null;
        String inheritAtt = null;
        String selectAtt = null;
        String onEmptyAtt = null;

        for (int a = 0; a < atts.getLength(); a++) {
            String f = atts.getQName(a);
            if (f.equals(StandardNames.USE_ATTRIBUTE_SETS)) {
                use = atts.getValue(a);
            } else if (f.equals(StandardNames.COPY_NAMESPACES)) {
                copyNamespacesAtt = Whitespace.trim(atts.getValue(a));
            } else if (f.equals(StandardNames.SELECT)) {
                selectAtt = atts.getValue(a);
            } else if (f.equals(StandardNames.TYPE)) {
                typeAtt = Whitespace.trim(atts.getValue(a));
            } else if (f.equals(StandardNames.VALIDATION)) {
                validationAtt = Whitespace.trim(atts.getValue(a));
            } else if (f.equals(StandardNames.INHERIT_NAMESPACES)) {
                inheritAtt = Whitespace.trim(atts.getValue(a));
            } else if (f.equals("on-empty")) {
                onEmptyAtt = Whitespace.trim(atts.getValue(a));
            } else {
                checkUnknownAttribute(atts.getNodeName(a));
            }
        }

        if (copyNamespacesAtt != null) {
            copyNamespaces = processBooleanAttribute("copy-namespaces", copyNamespacesAtt);
        }

        if (typeAtt != null && validationAtt != null) {
            compileError("The type and validation attributes must not both be specified", "XTSE1505");
        }

        if (validationAtt != null) {
            validationAction = validateValidationAttribute(validationAtt);
        } else {
            validationAction = getDefaultValidation();
        }

        if (typeAtt != null) {
            schemaType = getSchemaType(typeAtt);
            if (!isSchemaAware()) {
                compileError("The @type attribute is available only with a schema-aware XSLT processor", "XTSE1660");
            }
            validationAction = Validation.BY_TYPE;
        }
        if (inheritAtt != null) {
            inheritNamespaces = processBooleanAttribute("inherit-namespaces", inheritAtt);
        }

        if (selectAtt != null) {
            if (!isXslt30Processor()) {
                compileError("The @select attribute of xsl:copy requires XSLT 3.0");
            }
            select = makeExpression(selectAtt);
            selectSpecified = true;
        }

        if (onEmptyAtt != null) {
            if (!isXslt30Processor()) {
                compileError("The 'on-empty' attribute requires XSLT 3.0");
            }
            onEmpty = makeExpression(onEmptyAtt);
        }

    }

    public void validate(ComponentDeclaration decl) throws XPathException {
        if (use != null) {
            // find any referenced attribute sets
            attributeSets = getAttributeSets(use, null);
        }
        if (select != null && !isXslt30Processor()) {
            // we have a 2.0 processor
            if (forwardsCompatibleModeIsEnabled()) {
                compileWarning("xsl:copy/@select is ignored in forwards-compatibility mode", "");
            } else {
                compileError("The xsl:copy/@select attribute is not recognized by an XSLT 2.0 processor");
            }
        }
        if (onEmpty != null) {
            RoleLocator role = new RoleLocator(RoleLocator.INSTRUCTION, "xsl:copy/on-empty", 0);
            role.setErrorCode("XTTE3330");
            onEmpty = TypeChecker.staticTypeCheck(onEmpty, SequenceType.OPTIONAL_ELEMENT_NODE, false, role, makeExpressionVisitor());
        }
        if (select == null) {
            select = new ContextItemExpression();
            select.setLocationId(getLineNumber());
        }
    }

    public Expression compile(Compilation exec, ComponentDeclaration decl) throws XPathException {

        select = typeCheck("select", select);
        try {
            RoleLocator role = new RoleLocator(RoleLocator.INSTRUCTION, "xsl:copy/select", 0);
            role.setErrorCode("XTTE3180");
            select = TypeChecker.staticTypeCheck(select, SequenceType.OPTIONAL_ITEM,
                    false, role, makeExpressionVisitor());
        } catch (XPathException err) {
            compileError(err);
        }

        Expression content = compileSequenceConstructor(exec, decl, iterateAxis(AxisInfo.CHILD), true);

        if (attributeSets != null) {
            UseAttributeSets use = new UseAttributeSets(attributeSets);
            // The use-attribute-sets is ignored unless the context item is an element node. So we
            // wrap the UseAttributeSets instruction in a conditional to perform a run-time test
            Expression condition = new InstanceOfExpression(
                    new ContextItemExpression(),
                    SequenceType.makeSequenceType(NodeKindTest.ELEMENT, StaticProperty.EXACTLY_ONE));
            Expression choice = Choose.makeConditional(condition, use);
            if (content == null) {
                content = choice;
            } else {
                content = Block.makeBlock(choice, content);
                content.setLocationId(
                        allocateLocationId(getSystemId(), getLineNumber()));
            }
        }

        if (content == null) {
            content = Literal.makeEmptySequence(this);
        }

        Copy inst = new Copy(//select,
                //selectSpecified,
                copyNamespaces,
                inheritNamespaces,
                schemaType,
                validationAction,
                onEmpty);


        inst.setContentExpression(content);

        if (selectSpecified) {
            if (onEmpty != null && !Literal.isEmptySequence(onEmpty)) {
                try {
                    return getConfiguration().rewriteCopyOnEmpty(this, inst, select, onEmpty);
                } catch (Exception e) {
                    throw new XPathException("Problem in xsl:copy/@on-empty:"+ e.getMessage());
                }
            } else {
                return new ForEach(select, inst);
            }
        } else {
            return inst;
        }
    }

}

