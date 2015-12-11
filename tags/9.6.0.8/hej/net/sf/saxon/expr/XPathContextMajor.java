////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.expr;

import net.sf.saxon.Configuration;
import net.sf.saxon.Controller;
import net.sf.saxon.expr.instruct.Executable;
import net.sf.saxon.expr.instruct.ParameterSet;
import net.sf.saxon.expr.instruct.SlotManager;
import net.sf.saxon.expr.instruct.UserFunction;
import net.sf.saxon.expr.sort.GroupIterator;
import net.sf.saxon.lib.DelegatingErrorListener;
import net.sf.saxon.lib.StandardURIResolver;
import net.sf.saxon.lib.UnfailingErrorListener;
import net.sf.saxon.om.FocusTrackingIterator;
import net.sf.saxon.om.Item;
import net.sf.saxon.om.Sequence;
import net.sf.saxon.om.StructuredQName;
import net.sf.saxon.regex.RegexIterator;
import net.sf.saxon.trace.InstructionInfo;
import net.sf.saxon.trace.Location;
import net.sf.saxon.trans.Mode;
import net.sf.saxon.trans.Rule;
import net.sf.saxon.trans.RuleManager;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.tree.iter.SingletonIterator;
import net.sf.saxon.tree.iter.UnfailingIterator;

import javax.xml.transform.ErrorListener;
import javax.xml.transform.URIResolver;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * This class represents a "major context" in which an XPath expression is evaluated:
 * a "major context" object allows all aspects of the dynamic context to change, whereas
 * a "minor context" only allows changes to the focus and the destination for push output.
 */

public class XPathContextMajor extends XPathContextMinor {

    private ParameterSet localParameters;
    private ParameterSet tunnelParameters;
    /*@Nullable*/ private UserFunction tailCallFunction;
    private Mode currentMode;
    /*@Nullable*/ private Rule currentTemplate;
    private GroupIterator currentGroupIterator;
    private GroupIterator currentMergeGroupIterator;
    private RegexIterator currentRegexIterator;
    Object origin;
    private ThreadManager threadManager = null;
    private URIResolver uriResolver;
    private UnfailingErrorListener errorListener;
    private Component currentComponent;

    public class ThreadManager {
        List<Thread> threads = new ArrayList<Thread>();
        Exception errFound;

        public void addThread(Thread t) {
            threads.add(t);
        }

        public List<Thread> getChildThreads() {
            return threads;
        }


        public void setException(Exception err) {
            errFound = err;
        }

        public XPathException getException() {
            if (errFound instanceof XPathException) {
                return (XPathException) errFound;
            }
            throw new AssertionError(errFound);
        }

    }

    /**
     * Add a new thread to the list of child threads
     * for this context. Create threadManager if null.
     *
     * @param t the Thread
     */

    public void addChildThread(Thread t) {
        if (threadManager == null) {
            threadManager = new ThreadManager();
        }
        synchronized (threadManager.threads) {
            threadManager.addThread(t);
        }
    }


    public ThreadManager getThreadManager() {
        return threadManager;
    }

    public void createThreadManager() {
        threadManager = new ThreadManager();
    }

    /**
     * Check for running child threads and wait for them
     * to complete and join to main thread.
     * Throw any exception caught in any of the running threads
     */
    public void notifyChildThreads() throws XPathException {
//        if (getCaller() != null) {
//            getCaller().notifyChildThreads();
//        }

        if (threadManager != null) {

            List<Thread> childThreads = threadManager.getChildThreads();
            while (!childThreads.isEmpty()) {

                try {

                    if (threadManager.errFound != null) {
                        throw threadManager.getException();
                    }

                    childThreads.get(0).join();
                    synchronized (threadManager.threads) {
                        childThreads.remove(0);
                    }

                } catch (InterruptedException e) {
                    throw new AssertionError(e);
                } catch (IndexOutOfBoundsException e) {

                }


            }
            if (threadManager.errFound != null) {
                XPathException err = threadManager.getException();
                threadManager.errFound = null;
                throw err;
            }
            //  threadManager = null;
        }
    }


    /**
     * Constructor should only be called by the Controller,
     * which acts as a XPathContext factory.
     *
     * @param controller the Controller
     */

    public XPathContextMajor(Controller controller) {
        this.controller = controller;
        stackFrame = StackFrame.EMPTY;
        origin = controller;
    }


    /**
     * Private Constructor
     */

    private XPathContextMajor() {
    }

    /**
     * Constructor for use in free-standing Java applications.
     *
     * @param item the item to use as the initial context item. If this is null,
     *             the comtext item is initially undefined (which will cause a dynamic error
     *             if it is referenced).
     * @param exec the Executable
     */

    public XPathContextMajor(Item item, Executable exec) {
        controller = new Controller(exec.getConfiguration(), exec);
        if (item != null) {
            UnfailingIterator iter = SingletonIterator.makeIterator(item);
            currentIterator = new FocusTrackingIterator(iter);
            try {
                currentIterator.next();
            } catch (XPathException e) {
                // cannot happen
            }
            last = new LastValue(1);
        }
        origin = controller;
    }

    /**
     * Constructor for use in free-standing Java applications.
     *
     * @param item   the item to use as the initial context item. If this is null,
     *               the comtext item is initially undefined (which will cause a dynamic error
     *               if it is referenced).
     * @param config the Saxon Configuration
     * @deprecated since 9.0 - use {@link #XPathContextMajor(Item, Executable)}
     */

    public XPathContextMajor(Item item, Configuration config) {
        // No longer used internally but retained for backwards compatibility (8.8)
        Executable exec = new Executable(config);
        exec.setHostLanguage(Configuration.JAVA_APPLICATION, true);
        controller = new Controller(config, exec);
        if (item != null) {
            UnfailingIterator iter = SingletonIterator.makeIterator(item);
            iter.next();
            currentIterator = new FocusTrackingIterator(iter);
            last = new LastValue(1);
        }
        origin = controller;
    }

    /**
     * Construct a new context as a copy of another. The new context is effectively added
     * to the top of a stack, and contains a pointer to the previous context.
     */

    public XPathContextMajor newContext() {
        XPathContextMajor c = new XPathContextMajor();
        c.controller = controller;
        c.currentIterator = currentIterator;
        c.stackFrame = stackFrame;
        c.localParameters = localParameters;
        c.tunnelParameters = tunnelParameters;
        c.last = last;
        c.currentReceiver = currentReceiver;
        c.currentDestination = currentDestination;
        c.currentMode = currentMode;
        c.currentTemplate = currentTemplate;
        c.currentRegexIterator = currentRegexIterator;
        c.currentGroupIterator = currentGroupIterator;
        c.currentMergeGroupIterator = currentMergeGroupIterator;
        c.caller = this;
        c.tailCallFunction = null;
        c.temporaryOutputState = temporaryOutputState;
        c.threadManager = threadManager;
        c.currentComponent = currentComponent;
        c.errorListener = errorListener;
        c.uriResolver = uriResolver;
        return c;
    }

    /**
     * Create a new "major" context (one that is capable of holding a stack frame with local variables
     *
     * @param prev the previous context (the one causing the new context to be created)
     * @return the new major context
     */

    public static XPathContextMajor newContext(XPathContextMinor prev) {
        XPathContextMajor c = new XPathContextMajor();
        XPathContext p = prev;
        while (!(p instanceof XPathContextMajor)) {
            p = p.getCaller();
        }
        c.controller = p.getController();
        c.currentIterator = prev.getCurrentIterator();
        c.stackFrame = prev.getStackFrame();
        c.localParameters = p.getLocalParameters();
        c.tunnelParameters = p.getTunnelParameters();
        c.last = prev.last;
        c.currentReceiver = prev.currentReceiver;
        c.currentDestination = prev.currentDestination;
        c.currentMode = p.getCurrentMode();
        c.currentTemplate = p.getCurrentTemplateRule();
        c.currentRegexIterator = p.getCurrentRegexIterator();
        c.currentGroupIterator = p.getCurrentGroupIterator();
        c.caller = prev;
        c.tailCallFunction = null;
        c.threadManager = ((XPathContextMajor) p).threadManager;
        c.currentComponent = ((XPathContextMajor) p).currentComponent;
        c.errorListener = ((XPathContextMajor) p).errorListener;
        c.uriResolver = ((XPathContextMajor) p).uriResolver;
        return c;
    }

    /**
     * Make a copy of the supplied context for use in a new thread (typically for
     * an asynchronous xsl:result-document
     *
     * @param prev the context to be copied
     * @return the copy of the context
     */

    public static XPathContextMajor newThreadContext(XPathContextMinor prev) {
        XPathContextMajor c = newContext(prev);
        c.stackFrame = prev.stackFrame.copy();
        return c;
    }

    /**
     * Get the local parameters for the current template call.
     *
     * @return the supplied parameters
     */

    public ParameterSet getLocalParameters() {
        if (localParameters == null) {
            localParameters = new ParameterSet();
        }
        return localParameters;
    }

    /**
     * Set the local parameters for the current template call.
     *
     * @param localParameters the supplied parameters
     */

    public void setLocalParameters(ParameterSet localParameters) {
        this.localParameters = localParameters;
    }

    /**
     * Get the tunnel parameters for the current template call.
     *
     * @return the supplied tunnel parameters
     */

    public ParameterSet getTunnelParameters() {
        return tunnelParameters;
    }

    /**
     * Set the tunnel parameters for the current template call.
     *
     * @param tunnelParameters the supplied tunnel parameters
     */

    public void setTunnelParameters(ParameterSet tunnelParameters) {
        this.tunnelParameters = tunnelParameters;
    }

    /**
     * Set the creating expression (for use in diagnostics). The origin is generally set to "this" by the
     * object that creates the new context. It's up to the debugger to determine whether this information
     * is useful. The object will either be an {@link Expression}, allowing information
     * about the calling instruction to be obtained, or null.
     */

    public void setOrigin(InstructionInfo expr) {
        origin = expr;
    }

    /**
     * Set the type of creating expression (for use in diagnostics). When a new context is created, either
     * this method or {@link XPathContextMajor#setOrigin} should be called.
     *
     * @param loc The originating location: the argument must be one of the integer constants in class
     *            {@link net.sf.saxon.trace.Location}
     */

    public void setOriginatingConstructType(int loc) {
        origin = Integer.valueOf(loc);
    }

    /**
     * Get the type of location from which this context was created.
     */

    public int getOriginatingConstructType() {
        if (origin == null) {
            return -1;
        }
        if (origin instanceof Expression) {
            if (origin instanceof SlashExpression) {
                return Location.PATH_EXPRESSION;
            }
            return ((Expression) origin).getConstructType();
        } else if (origin instanceof Integer) {
            return ((Integer) origin).intValue();
        } else if (origin instanceof InstructionInfo) {
            return ((InstructionInfo) origin).getConstructType();
        } else {
            return -1;
        }
    }

    /**
     * Get information about the creating expression or other construct.
     */

    public InstructionInfo getOrigin() {
        if (origin instanceof InstructionInfo) {
            return (InstructionInfo) origin;
        } else {
            return null;
        }
    }


    /**
     * Set the local stack frame. This method is used when creating a Closure to support
     * delayed evaluation of expressions. The "stack frame" is actually on the Java heap, which
     * means it can survive function returns and the like.
     *
     * @param map       the SlotManager, which holds static details of the allocation of variables to slots
     * @param variables the array of "slots" to hold the actual variable values. This array will be
     *                  copied if it is too small to hold all the variables defined in the SlotManager
     */

    public void setStackFrame(SlotManager map, Sequence[] variables) {
        stackFrame = new StackFrame(map, variables);
        if (map != null && variables.length != map.getNumberOfVariables()) {
            if (variables.length > map.getNumberOfVariables()) {
                throw new IllegalStateException(
                        "Attempting to set more local variables (" + variables.length +
                                ") than the stackframe can accommodate (" + map.getNumberOfVariables() + ")");
            }
            stackFrame.slots = new Sequence[map.getNumberOfVariables()];
            System.arraycopy(variables, 0, stackFrame.slots, 0, variables.length);
        }
    }

    /**
     * Reset the stack frame variable map, while reusing the StackFrame object itself. This
     * is done on a tail call to a different function
     *
     * @param map            the SlotManager representing the stack frame contents
     * @param numberOfParams the number of parameters required on the new stack frame
     */

    public void resetStackFrameMap(SlotManager map, int numberOfParams) {
        stackFrame.map = map;
        if (stackFrame.slots.length != map.getNumberOfVariables()) {
            Sequence[] v2 = new Sequence[map.getNumberOfVariables()];
            System.arraycopy(stackFrame.slots, 0, v2, 0, numberOfParams);
            stackFrame.slots = v2;
        } else {
            // not strictly necessary
            Arrays.fill(stackFrame.slots, numberOfParams, stackFrame.slots.length, null);
        }
    }

    /**
     * Get a all the variables in the stack frame
     *
     * @return an array holding all the variables, each referenceable by its slot number
     */

    public Sequence[] getAllVariableValues() {
        return stackFrame.getStackFrameValues();
    }

    /**
     * Overwrite all the variables in the stack frame
     *
     * @param values an array holding all the variables, each referenceable by its slot number;
     *               the caller must ensure this is the correct length (and valid in other ways)
     */

    public void resetAllVariableValues(Sequence[] values) {
        stackFrame.setStackFrameValues(values);
    }

    /**
     * Overwrite all the parameters in the stack frame. (Used from compiled bytecode)
     *
     * @param values an array holding all the parameters, each referenceable by its slot number;
     *               the caller must ensure this is the correct length (and valid in other ways)
     */

    public void resetParameterValues(Sequence[] values) {
        System.arraycopy(values, 0, stackFrame.slots, 0, values.length);
    }

    /**
     * Reset the local stack frame. This method is used when processing a tail-recursive function.
     * Instead of the function being called recursively, the parameters are set to new values and the
     * function body is evaluated repeatedly
     *
     * @param fn        the user function being called using tail recursion
     * @param variables the parameter to be supplied to the user function
     */

    public void requestTailCall(UserFunction fn, Sequence[] variables) {
        if (variables.length > stackFrame.slots.length) {
            Sequence[] v2 = new Sequence[fn.getStackFrameMap().getNumberOfVariables()];
            System.arraycopy(variables, 0, v2, 0, variables.length);
            stackFrame.slots = v2;
        } else {
            System.arraycopy(variables, 0, stackFrame.slots, 0, variables.length);
        }
        tailCallFunction = fn;
    }


    /**
     * Determine whether the body of a function is to be repeated, due to tail-recursive function calls
     *
     * @return null if no tail call has been requested, or the name of the function to be called otherwise
     */

    public UserFunction getTailCallFunction() {
        UserFunction fn = tailCallFunction;
        tailCallFunction = null;
        return fn;
    }

    /**
     * Create a new stack frame for local variables, using the supplied SlotManager to
     * define the allocation of slots to individual variables
     *
     * @param map the SlotManager for the new stack frame
     */
    public void openStackFrame(SlotManager map) {
        int numberOfSlots = map.getNumberOfVariables();
        if (numberOfSlots == 0) {
            stackFrame = StackFrame.EMPTY;
        } else {
            stackFrame = new StackFrame(map, new Sequence[numberOfSlots]);
        }
    }

    /**
     * Create a new stack frame large enough to hold a given number of local variables,
     * for which no stack frame map is available. This is used in particular when evaluating
     * match patterns of template rules.
     *
     * @param numberOfVariables The number of local variables to be accommodated.
     */

    public void openStackFrame(int numberOfVariables) {
        stackFrame = new StackFrame(new SlotManager(numberOfVariables),
                new Sequence[numberOfVariables]);
    }

    /**
     * Set the current mode.
     *
     * @param mode the new current mode
     */

    public void setCurrentMode(Mode mode) {
        this.currentMode = mode;
    }

    /**
     * Get the current mode.
     *
     * @return the current mode. May return null if the current mode is the default mode.
     */

    public Mode getCurrentMode() {
        Mode m = currentMode;
        if (m == null) {
            RuleManager rm = getController().getRuleManager();
            if (rm != null) {
                return rm.getUnnamedMode();
            } else {
                return null;
            }
        } else {
            return m;
        }
    }

    /**
     * Set the current template. This is used to support xsl:apply-imports. The caller
     * is responsible for remembering the previous current template and resetting it
     * after use.
     *
     * @param rule the current template rule, or null to indicate that there is no current template rule
     */

    public void setCurrentTemplateRule(/*@Nullable*/ Rule rule) {
        this.currentTemplate = rule;
    }

    /**
     * Get the current template. This is used to support xsl:apply-imports
     *
     * @return the current template
     */

    public Rule getCurrentTemplateRule() {
        return currentTemplate;
    }

    /**
     * Set the current grouping iterator. This supports the current-group() and
     * current-grouping-key() functions in XSLT 2.0
     *
     * @param iterator the new current GroupIterator
     */

    public void setCurrentGroupIterator(GroupIterator iterator) {
        this.currentGroupIterator = iterator;
    }

    /**
     * Get the current group iterator. This supports the current-group() and
     * current-grouping-key() functions in XSLT 2.0
     *
     * @return the current grouped collection
     */

    public GroupIterator getCurrentGroupIterator() {
        return currentGroupIterator;
    }

    /**
     * Set the current merge group iterator. This supports the current-merge-group() and
     * current-merge-key() functions in XSLT 3.0
     *
     * @param iterator the new current GroupIterator
     */

    public void setCurrentMergeGroupIterator(GroupIterator iterator) {
        this.currentMergeGroupIterator = iterator;
    }

    /**
     * Get the current merge group iterator. This supports the current-merge-group() and
     * current-merge-key() functions in XSLT 3.0
     *
     * @return the current grouped collection
     */

    public GroupIterator getCurrentMergeGroupIterator() {
        return currentMergeGroupIterator;
    }

    /**
     * Set the current regex iterator. This supports the functionality of the regex-group()
     * function in XSLT 2.0.
     *
     * @param currentRegexIterator the current regex iterator
     */

    public void setCurrentRegexIterator(RegexIterator currentRegexIterator) {
        this.currentRegexIterator = currentRegexIterator;
    }

    /**
     * Get the current regex iterator. This supports the functionality of the regex-group()
     * function in XSLT 2.0.
     *
     * @return the current regular expressions iterator
     */

    public RegexIterator getCurrentRegexIterator() {
        return currentRegexIterator;
    }

    /**
     * Use local parameter. This is called when a local xsl:param element is processed.
     * If a parameter of the relevant name was supplied, it is bound to the xsl:param element.
     * Otherwise the method returns false, so the xsl:param default will be evaluated
     *
     * @param parameterId
     * @param slotNumber
     * @param isTunnel    True if a tunnel parameter is required, else false  @return ParameterSet.NOT_SUPPLIED, ParameterSet.SUPPLIED, or ParameterSet.SUPPLIED_AND_CHECKED
     */

    public int useLocalParameter(
            StructuredQName parameterId, int slotNumber, boolean isTunnel) throws XPathException {

        ParameterSet params = (isTunnel ? getTunnelParameters() : localParameters);
        if (params == null) {
            return ParameterSet.NOT_SUPPLIED;
        }
        int index = params.getIndex(parameterId);
        if (index < 0) {
            return ParameterSet.NOT_SUPPLIED;
        }
        Sequence val = params.getValue(index);
        stackFrame.slots[slotNumber] = val;
        boolean checked = params.isTypeChecked(index);
        return (checked ? ParameterSet.SUPPLIED_AND_CHECKED : ParameterSet.SUPPLIED);
    }

    /**
     * Set an object that will be used to resolve URIs used in
     * document(), etc. This method allows a URIResolver to be set that is local
     * to this particular XPathContext, which is useful when local behaviour is
     * needed, e.g. during schema validation. The URIResolver set in the Controller
     * and in the Configuration are not affected by this call.
     *
     * @param resolver An object that implements the URIResolver interface, or
     *                 null.
     * @since 9.6
     */

    public void setURIResolver(URIResolver resolver) {
        uriResolver = resolver;
        if (resolver instanceof StandardURIResolver) {
            ((StandardURIResolver) resolver).setConfiguration(getConfiguration());
        }
    }

    /**
     * Get the URI resolver. This gets the local URIResolver set in the XPathContext if there
     * is one; if not, it gets the URIResolver from the Controller (which itself defaults to the
     * one set in the Configuration).
     *
     * @return the user-supplied URI resolver if there is one, or null otherwise.
     * @since 9.6
     */

    public URIResolver getURIResolver() {
        return (uriResolver == null ? controller.getURIResolver() : uriResolver);
    }

    /**
     * Set the error listener. The ErrorListener is set locally to this XPathContext
     * object.
     *
     * @param listener the ErrorListener to be used
     * @since 9.6
     */

    public void setErrorListener(ErrorListener listener) {
        if (listener instanceof UnfailingErrorListener) {
            errorListener = (UnfailingErrorListener) listener;
        } else {
            errorListener = new DelegatingErrorListener(listener);
        }
    }

    /**
     * Get the error listener. If no ErrorListener
     * has been set locally, the ErrorListener in the Controller is returned; this in turn defaults
     * to the ErrorListener set in the Configuration.
     *
     * @return the ErrorListener in use. This will always be an UnfailingErrorListener,
     * which is a Saxon subclass of ErrorListener that throws no exceptions.
     * @since 9.6
     */

    public UnfailingErrorListener getErrorListener() {
        return (errorListener == null ? controller.getErrorListener() : errorListener);
    }



    /**
     * Get the current component
     */

    public Component getCurrentComponent() {
        return currentComponent;
    }

    /**
     * Set the current component, that is, the component being evaluated. This is used during the evaluation
     * to determine the bindings to other components (such as global variables, functions, and templates) referenced
     * during the evaluation
     * @param component the current componenet
     */

    public void setCurrentComponent(Component component) {
        currentComponent = component;
    }

    /**
     * Get the target component corresponding to a specific binding slot. The method accesses the
     * relevant entry in the binding vector for the current component.
     * @param bindingSlot identifies the relevant entry in the binding vector
     * @return the component to be invoked
     */

    public Component getTargetComponent(int bindingSlot) {
        try {
            ComponentBinding binding = currentComponent.getComponentBindings().get(bindingSlot);
            return binding.getTarget();
        } catch (NullPointerException e) {
            e.printStackTrace();
            throw e;
        } catch (IndexOutOfBoundsException e) {
            e.printStackTrace();
            throw e;
        }
    }
}

