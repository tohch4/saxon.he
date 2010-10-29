package net.sf.saxon.trans;

import net.sf.saxon.expr.Optimizer;
import net.sf.saxon.expr.Token;
import net.sf.saxon.expr.XPathContext;
import net.sf.saxon.expr.instruct.Template;
import net.sf.saxon.om.NodeInfo;
import net.sf.saxon.om.StructuredQName;
import net.sf.saxon.pattern.*;
import net.sf.saxon.style.StylesheetModule;
import net.sf.saxon.trace.ExpressionPresenter;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Iterator;

/**
  * <B>RuleManager</B> maintains a set of template rules, one set for each mode
  * @version 10 December 1999: carved out of the old Controller class
  * @author Michael H. Kay
  */

public final class RuleManager implements Serializable {

    private Mode unnamedMode;           // node handlers with default mode
    private HashMap<StructuredQName, Mode> modes;
                                        // tables of node handlers for non-default modes
    private Mode omniMode = null;       // node handlers that specify mode="all"
    private int recoveryPolicy;

    /**
    * create a RuleManager and initialise variables.
    */

    public RuleManager() {
        resetHandlers();
    }

    /**
     * Set the policy for handling recoverable errrors. Note that for some errors the decision can be
     * made at run-time, but for the "ambiguous template match" error, the decision is (since 9.2)
     * fixed at compile time.
     * @param policy the recovery policy to be used. The options are {@link net.sf.saxon.Configuration#RECOVER_SILENTLY},
     * {@link net.sf.saxon.Configuration#RECOVER_WITH_WARNINGS}, or {@link net.sf.saxon.Configuration#DO_NOT_RECOVER}.
     * @since 9.2
     */

    public void setRecoveryPolicy(int policy) {
        recoveryPolicy = policy;
        unnamedMode.setRecoveryPolicy(policy);
    }

    /**
     * Get the policy for handling recoverable errors. Note that for some errors the decision can be
     * made at run-time, but for the "ambiguous template match" error, the decision is (since 9.2)
     * fixed at compile time.
     *
     * @return the current policy.
     * @since 9.2
     */

    public int getRecoveryPolicy() {
        return recoveryPolicy;
    }

    /**
    * Set up a new table of handlers.
    */

    public void resetHandlers() {
        unnamedMode = new Mode(Mode.UNNAMED_MODE, Mode.UNNAMED_MODE_NAME);
        unnamedMode.setRecoveryPolicy(recoveryPolicy);
        modes = new HashMap<StructuredQName, Mode>(5);
    }

    /**
     * Get the mode object for the unnamed mode
     */

    public Mode getUnnamedMode() {
        return unnamedMode;
    }

    /**
     * Get the Mode object for a named mode. If there is not one already registered.
     * a new Mode is created.
     * @param modeName The name of the mode. Supply null to get the default
     * mode or Mode.ALL_MODES to get the Mode object containing "mode=all" rules
     * @param createIfAbsent if true, then if the mode does not already exist it will be created.
     * If false, then if the mode does not already exist the method returns null.
     * @return the Mode with this name
     */

    public Mode getMode(StructuredQName modeName, boolean createIfAbsent) {
        if (modeName == null || modeName.equals(Mode.UNNAMED_MODE_NAME)) {
            return unnamedMode;
        }
        if (modeName.equals(Mode.ALL_MODES)) {
            if (omniMode==null) {
                omniMode = new Mode(Mode.NAMED_MODE, modeName);
                omniMode.setRecoveryPolicy(recoveryPolicy);
            }
            return omniMode;
        }
        //Integer modekey = new Integer(modeNameCode & 0xfffff);
        Mode m = modes.get(modeName);
        if (m == null && createIfAbsent) {
            m = new Mode(omniMode, modeName);
            m.setRecoveryPolicy(recoveryPolicy);
            modes.put(modeName, m);
            // when creating a specific mode, copy all the rules currently held
            // in the omniMode, as these apply to all modes
        }
        return m;
    }

    /**
      * Register a template rule for a particular pattern. The priority of the rule
      * is the default priority for the pattern, which depends on the syntax of
      * the pattern suppllied.
      * @param pattern A match pattern
      * @param eh The Template to be used
      * @param mode The processing mode
      * @param module The stylesheet module containing the template rule
      */

//    public void setTemplateRule(Pattern pattern, Template eh, Mode mode, StylesheetModule module) {
//        // for a union pattern, register the parts separately (each with its own priority)
//        if (pattern instanceof UnionPattern) {
//            UnionPattern up = (UnionPattern)pattern;
//            Pattern p1 = up.getLHS();
//            Pattern p2 = up.getRHS();
//            setTemplateRule(p1, eh, mode, module);
//            setTemplateRule(p2, eh, mode, module);
//            return;
//        }
//
//        double priority = pattern.getDefaultPriority();
//        setTemplateRule(pattern, eh, mode, module, priority);
//    }


    /**
      * Register a template for a particular pattern.
      * @param pattern Must be a valid Pattern.
      * @param eh The Template to be used
      * @param mode The processing mode to which this template applies
      * @param module The stylesheet module containing the template rule
      * @param priority The priority of the rule: if an element matches several patterns, the
      * one with highest priority is used
      * @see Pattern
      */

    public void setTemplateRule(Pattern pattern, Template eh,
                 Mode mode, StylesheetModule module, double priority) {

        // for a union pattern, register the parts separately
        // Note: technically this is only necessary if using default priorities and if the priorities
        // of the two halves are different. However, splitting increases the chance that the pattern
        // can be matched by hashing on the element name, so we do it always
        if (pattern instanceof UnionPattern) {
            UnionPattern up = (UnionPattern)pattern;
            Pattern p1 = up.getLHS();
            Pattern p2 = up.getRHS();
            setTemplateRule(p1, eh, mode, module, priority);
            setTemplateRule(p2, eh, mode, module, priority);
            return;
        }
        // some union patterns end up as a CombinedNodeTest. Need to split these.
        // (Same reasoning as above)
        if (pattern instanceof NodeTestPattern &&
                pattern.getNodeTest() instanceof CombinedNodeTest &&
                ((CombinedNodeTest)pattern.getNodeTest()).getOperator() == Token.UNION) {
            CombinedNodeTest cnt = (CombinedNodeTest)pattern.getNodeTest();
            NodeTest[] nt = cnt.getComponentNodeTests();
            setTemplateRule(new NodeTestPattern(nt[0]), eh, mode, module, priority);
            setTemplateRule(new NodeTestPattern(nt[1]), eh, mode, module, priority);
            return;
        }
        if (Double.isNaN(priority)) {
            priority = pattern.getDefaultPriority();
        }

        mode.addRule(pattern, eh, module, priority, true);

        // if adding a rule to the omniMode (mode='all') add it to all
        // the other modes as well

        if (mode==omniMode) {
            unnamedMode.addRule(pattern, eh, module, priority, false);
            Iterator<Mode> iter = modes.values().iterator();
            while (iter.hasNext()) {
                Mode m = iter.next();
                m.addRule(pattern, eh, module, priority, false);
            }
        }
    }

     /**
      * Find the template rule registered for a particular node in a specific mode.
      * @param node The NodeInfo for the relevant node
      * @param mode The processing mode
      * @param c The controller for this transformation
      * @return The template rule that will process this node
      * Returns null if there is no specific handler registered.
      */

    public Rule getTemplateRule (NodeInfo node, Mode mode, XPathContext c) throws XPathException {

        if (mode==null) {
            mode = unnamedMode;
        }

        return mode.getRule(node, c);
    }

    /**
     * Get a template rule whose import precedence is in a particular range. This is used to support
     * the xsl:apply-imports function
     * @param node The node to be matched
     * @param mode The mode for which a rule is required
     * @param min  The minimum import precedence that the rule must have
     * @param max  The maximum import precedence that the rule must have
     * @param c    The Controller for the transformation
     * @return     The template rule to be invoked
     * @throws XPathException
     */

    public Rule getTemplateRule (NodeInfo node, Mode mode, int min, int max, XPathContext c)
    throws XPathException {
        if (mode==null) {
            mode = unnamedMode;
        }
        return mode.getRule(node, min, max, c);
    }

    /**
     * Get the next-match handler after the current one
     * @param node  The node to be matched
     * @param mode  The processing mode
     * @param currentRule The current template rule
     * @param c     The dynamic context for the transformation
     * @return      The template rule to be executed
     * @throws XPathException
     */

    public Rule getNextMatchHandler(NodeInfo node, Mode mode, Rule currentRule, XPathContext c)
    throws XPathException {
        if (mode==null) {
            mode = unnamedMode;
        }
        return mode.getNextMatchRule(node, currentRule, c);
    }

    /**
     * Allocate rankings to the rules within each mode. This method must be called when all
     * the rules in each mode are known
     */

    public void computeRankings() throws XPathException {
        unnamedMode.computeRankings();
        Iterator<Mode> iter = modes.values().iterator();
        while (iter.hasNext()) {
            Mode mode = iter.next();
            mode.computeRankings();
        }
    }

    /**
     * Invert streamable templates in all streamable modes
     */

    public void invertStreamableTemplates(Optimizer opt) throws XPathException {
        unnamedMode.invertStreamableTemplates(opt);
        Iterator<Mode> iter = modes.values().iterator();
        while (iter.hasNext()) {
            Mode mode = iter.next();
            mode.invertStreamableTemplates(opt);
        }
    }

    /**
     * Explain (that is, output the expression tree) all template rules
     */

    public void explainTemplateRules(ExpressionPresenter presenter) {
        presenter.startElement("templateRules");
        unnamedMode.explainTemplateRules(presenter);
        Iterator<Mode> iter = modes.values().iterator();
        while (iter.hasNext()) {
            Mode mode = iter.next();
            int s = presenter.startElement("mode");
            if (!mode.isDefaultMode()) {
                presenter.emitAttribute("name", mode.getModeName().getDisplayName());
            }
            presenter.emitAttribute("streamable", Boolean.toString(mode.isStreamable()));
            mode.explainTemplateRules(presenter);
            int e = presenter.endElement();
            if (s != e) {
                throw new IllegalStateException("tree unbalanced");
            }
        }

        presenter.endElement();
    }
}

//
// The contents of this file are subject to the Mozilla Public License Version 1.0 (the "License");
// you may not use this file except in compliance with the License. You may obtain a copy of the
// License at http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied.
// See the License for the specific language governing rights and limitations under the License.
//
// The Original Code is: all this file.
//
// The Initial Developer of the Original Code is Michael H. Kay.
//
// Portions created by (your name) are Copyright (C) (your legal entity). All Rights Reserved.
//
// Contributor(s): none.
//
