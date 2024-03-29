////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020 Saxonica Limited
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.testdriver;

import net.sf.saxon.Configuration;
import net.sf.saxon.event.Receiver;
import net.sf.saxon.expr.parser.Loc;
import net.sf.saxon.functions.ResolveURI;
import net.sf.saxon.lib.*;
import net.sf.saxon.om.Sequence;
import net.sf.saxon.om.TreeModel;
import net.sf.saxon.s9api.*;
import net.sf.saxon.trace.ExpressionPresenter;
import net.sf.saxon.trans.LicenseException;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.trans.XmlProcessingIncident;

import javax.xml.stream.XMLStreamException;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.stream.StreamSource;
import java.io.File;
import java.io.IOException;
import java.io.StringWriter;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.*;
import java.util.function.Predicate;

import static net.sf.saxon.s9api.streams.Predicates.*;
import static net.sf.saxon.s9api.streams.Steps.*;

/**
 * Test Driver for the QT3 test suite
 */
public class QT3TestDriverHE extends TestDriver {


    public static String RNS = "http://www.w3.org/2012/08/qt-fots-results";
    public static String CNS = "http://www.w3.org/2010/09/qt-fots-catalog";

    public Map<String, Dependency> getDependencyMap() {
        return dependencyMap;
    }


    //private FotsResultsDocument resultsDoc;

    @Override
    public String catalogNamespace() {
        return CNS;
    }

    @Override
    public void go(String[] args) throws Exception {
        super.go(args);
    }

    @Override
    public void processSpec(String specStr) {

        switch (specStr) {
            case "XP20":
                spec = Spec.XP20;
                break;
            case "XP30":
                spec = Spec.XP30;
                break;
            case "XP31":
                spec = Spec.XP31;
                break;
            case "XQ10":
                spec = Spec.XQ10;
                break;
            case "XQ30":
                spec = Spec.XQ30;
                break;
            case "XQ31":
                spec = Spec.XQ31;
                break;
            case "XT30":  // map function tests
                spec = Spec.XT30;
                break;
            default:
                throw new IllegalArgumentException("The specific language must be one of the following: XP20, XP30, XP31, XQ10, XQ30, XQ31, XT30");
        }
        resultsDoc = new QT3TestReport(this, spec);
    }

    public static void main(String[] args) throws Exception {
        if (args.length == 0 || args[0].equals("-?")) {
            usage();
            return;
        }
        new QT3TestDriverHE().go(args);
    }

    public static void usage() {
        System.err.println("java net.sf.saxon.testdriver.QT3TestSuiteDriverHE testsuiteDir catalog [-o:resultsdir] [-s:testSetName]" +
                " [-t:testNamePattern] [-unfolded] [-bytecode:on|off|debug] [-tree] [-lang:XP20|XP30|XQ10|XQ30]");

    }


    @Override
    protected void createGlobalEnvironments(XdmNode catalog, XPathCompiler xpc) throws SaxonApiException {
        Environment environment = null;
        for (XdmNode env : catalog.select(descendant("environment")).asList()) {
            try {
                environment = Environment.processEnvironment(
                        this, xpc, env, globalEnvironments, localEnvironments.get("default"));
            } catch (NullPointerException ex) {
                ex.printStackTrace();
                System.err.println("Failed to load environment");
            }
        }
        try {
            buildDependencyMap(driverProc, catalog, environment);
        } catch (Exception ex) {
            System.err.println("Environment map error" + ex.getMessage());
            ex.printStackTrace();
        }
    }

    /**
     * Decide whether a dependency is satisfied
     *
     * @param dependency the dependency element in the catalog
     * @param env        an environment in the catalog, which can be modified to satisfy the dependency if necessary.
     *                   May be null.
     * @return true if the environment satisfies the dependency, else false
     */

    @Override
    public boolean ensureDependencySatisfied(XdmNode dependency, Environment env) {
        String type = dependency.attribute("type");
        String value = dependency.attribute("value");
        boolean inverse = "false".equals(dependency.attribute("satisfied"));
        if ("saxon:config".equals(type)) {
            // Used in QT3Extra tests
            final String name = dependency.attribute("name");
            final Object oldValue = env.processor.getConfigurationProperty(name);
            env.processor.setConfigurationProperty(name, value);
            env.resetActions.add(new Environment.ResetAction() {
                @Override
                public void reset(Environment env) {
                    env.processor.setConfigurationProperty(name, oldValue);
                }
            });
            return true;
        } else if ("xml-version".equals(type)) {
            if (value.equals("1.0:4-") && !inverse) {
                // we don't support XML 1.0 4th edition or earlier
                return false;
            }
            if (value.contains("1.1") && !inverse) {
                if (treeModel.getName().equals("JDOM") || treeModel.getName().equals("JDOM2") || treeModel.getName().equals("XOM") || treeModel.getName().equals("Axiom")) {
                    return false;
                }
                if (env != null) {
                    env.processor.setXmlVersion("1.1");
                } else {
                    return false;
                }
            } else if (value.contains("1.0") && !inverse) {
                if (env != null) {
                    env.processor.setXmlVersion("1.0");
                } else {
                    return false;
                }
            }
            return true;
        } else if ("xsd-version".equals(type)) {
            final String old = env.processor.getConfigurationProperty(Feature.XSD_VERSION);
            if ("1.1".equals(value)) {
                if (env != null) {
                    env.processor.setConfigurationProperty(Feature.XSD_VERSION, inverse ? "1.0" : "1.1");
                } else {
                    return false;
                }
            } else if ("1.0".equals(value)) {
                if (env != null) {
                    env.processor.setConfigurationProperty(Feature.XSD_VERSION, inverse ? "1.1" : "1.0");
                } else {
                    return false;
                }
            }
            env.resetActions.add(new Environment.ResetAction() {
                @Override
                public void reset(Environment env) {
                    env.processor.setConfigurationProperty(Feature.XSD_VERSION, old);
                }
            });
            return true;
        } else if ("limits".equals(type)) {
            return ("year_lt_0".equals(value) || "big_integer".equals(value)) && !inverse;
        } else if ("spec".equals(type)) {
            return isApplicableToSpecVersion(value, spec);
        } else if ("collection-stability".equals(type)) {
            // SAXON has a problem here - we don't support stable collections
            return "false".equals(value) != inverse;
        } else if ("default-language".equals(type)) {
            final String old = (String) env.processor.getConfigurationProperty(Feature.DEFAULT_LANGUAGE);
            if (!value.equals(old)){
                env.processor.setConfigurationProperty(Feature.DEFAULT_LANGUAGE, value);
                env.resetActions.add(new Environment.ResetAction() {
                    @Override
                    public void reset(Environment env) {
                        env.processor.setConfigurationProperty(Feature.DEFAULT_LANGUAGE, old);
                    }
                });
            }
            return true;
        } else if ("directory-as-collection-uri".equals(type)) {
            return "true".equals(value) != inverse;
        } else if ("language".equals(type)) {
            return isSupportedLanguage(value);
            //return ("en".equals(value) || "de".equals(value) || "fr".equals(value) || "it".equals(value) || "xib".equals(value)) != inverse;
        } else if ("calendar".equals(type)) {
            return ("AD".equals(value) || "ISO".equals(value)) != inverse;
        } else if ("format-integer-sequence".equals(type)) {
            return !inverse;
        } else if ("unicode-normalization-form".equals(type)) {
            return value.equalsIgnoreCase("FULLY-NORMALIZED") == inverse;
        } else if ("feature".equals(type)) {
            String edition = env.processor.getSaxonEdition();
            if ("namespace-axis".equals(value)) {
                return !inverse;
            } else if ("higherOrderFunctions".equals(value)) {
                return (edition.equals("PE") || edition.equals("EE")) ^ inverse;
            } else if ("schemaImport".equals(value) || "schemaValidation".equals(value) || "schemaAware".equals(value)) {
                return makeSchemaAware(env, inverse);
            } else if ("xpath-1.0-compatibility".equals(value)) {
                if (env != null && !env.processor.getSaxonEdition().equals("HE")) {
                    env.xpathCompiler.setBackwardsCompatible(!inverse);
                    return true;
                } else {
                    return false;
                }
            } else if ("staticTyping".equals(value)) {
                return inverse;
            } else if ("remote_http".equals(value)) {
                return !inverse;
            } else if ("moduleImport".equals(value)) {
                return !inverse;
            } else if ("schema-location-hint".equals(value)) {
                return (edition.equals("PE") || edition.equals("EE")) ^ inverse;
            } else if ("infoset-dtd".equals(value)) {
                if (treeModel == TreeModel.TINY_TREE || treeModel == TreeModel.LINKED_TREE ||
                        treeModel == TreeModel.TINY_TREE_CONDENSED) {
                    return !inverse;
                } else {
                    return inverse;
                }
            } else if ("serialization".equals(value)) {
                return true;
            } else if ("non_unicode_codepoint_collation".equals(value)) {
                return !inverse;
            } else if ("non_empty_sequence_collection".equals(value)) {
                return !inverse;
            } else if ("fn-transform-XSLT".equals(value)) {
                return !inverse;
            } else if ("fn-transform-XSLT30".equals(value)) {
                return !inverse;
            } else if ("fn-load-xquery-module".equals(value)) {
                return edition.equals("EE") ^ inverse;
            } else if ("fn-format-integer-CLDR".equals(value)) {
                return (edition.equals("PE") || edition.equals("EE")) ^ inverse;
            } else if ("simple-uca-fallback".equals(value)) {
                return !inverse;
            } else if ("advanced-uca-fallback".equals(value)) {
                return (edition.equals("PE") || edition.equals("EE")) ^ inverse;
            } else {
                println("**** feature = " + value + "  ????");
                return false;
            }
        } else {
            println("**** dependency not recognized: " + type);
            return false;
        }
    }

    protected boolean makeSchemaAware(Environment env, boolean inverse) {
        // Saxon-HE cannot run schema-aware tests
        return inverse;
    }

    protected boolean isSupportedLanguage(String language) {
        return "en".equals(language);
    }

    /**
     * Run a test case
     *
     * @param testCase the test case element in the catalog
     * @param xpc      the XPath compiler to be used for compiling XPath expressions against the catalog
     * @throws SaxonApiException
     */

    @Override
    @SuppressWarnings({"ThrowableResultOfMethodCallIgnored"})
    protected void runTestCase(XdmNode testCase, XPathCompiler xpc) throws SaxonApiException {
        boolean run = true;
        String hostLang;
        String langVersion;
        Spec specOpt = null;
        XPathCompiler xpath = driverProc.newXPathCompiler();
        String testCaseName = testCase.attribute("name");
        String testSetName = testCase.getParent().attribute("name");
        XdmNode resultElement = testCase.select(child("result")).asNode();
        boolean needSerializedResult = resultElement.select(descendant("assert-serialization-error")).exists()
                || resultElement.select(descendant("serialization-matches")).exists();
        boolean needResultValue = true;
        if (needSerializedResult) {
            needResultValue = resultElement.select(descendant(isElement()).where(not(
                    hasLocalName("serialization-matches")
                            .or(hasLocalName("assert-serialization-error"))
                            .or(hasLocalName("any-of"))
                            .or(hasLocalName("all-of"))))).exists();
        }

        XdmNode alternativeResult = null;
        XdmNode optimization = null;


        hostLang = spec.shortSpecName;
        langVersion = spec.version;


        Environment env = getEnvironment(testCase, xpc);
        if (env == null) {
            writeTestcaseElement(testCaseName, "notRun", "test catalog error");
            notrun++;
            return;
        }
        if (env.failedToBuild) {
            writeTestcaseElement(testCaseName, "fail", "unable to build environment");
            noteFailure(testSetName, testCaseName);
            return;
        }
        if (!env.usable) {
            writeTestcaseElement(testCaseName, "n/a", "environment dependencies not satisfied");
            notrun++;
            return;
        }
        env.xpathCompiler.setBackwardsCompatible(false);
        env.processor.setXmlVersion("1.0");
        env.processor.setConfigurationProperty(Feature.XQUERY_MULTIPLE_MODULE_IMPORTS, true);


        List<XdmNode> dependencies = testCase.getParent().select(child("dependency")).asList();
        dependencies.addAll(testCase.select(child("dependency")).asList());
        for (XdmNode dependency : dependencies) {
            String type = dependency.attribute("type");
            if (type == null) {
                throw new IllegalStateException("dependency/@type is missing");
            }
            String value = dependency.attribute("value");
            if (value == null) {
                throw new IllegalStateException("dependency/@value is missing");
            }

            if (type.equals("spec")) {
                boolean applicable = isApplicableToSpecVersion(value, spec);
                if (!applicable) {
                    writeTestcaseElement(testCaseName, "n/a", "not" + spec.specAndVersion);
                    notrun++;
                    return;
                }
            }
            if (langVersion.equals("3.0") || langVersion.equals("3.1")) {
                EnvironmentVariableResolver resolver = new EnvironmentVariableResolver() {
                    @Override
                    public Set<String> getAvailableEnvironmentVariables() {
                        Set<String> strings = new HashSet<String>();
                        strings.add("QTTEST");
                        strings.add("QTTEST2");
                        strings.add("QTTESTEMPTY");
                        return strings;
                    }

                    @Override
                    public String getEnvironmentVariable(String name) {
                        if (name.equals("QTTEST")) {
                            return "42";
                        } else if (name.equals("QTTEST2")) {
                            return "other";
                        } else if (name.equals("QTTESTEMPTY")) {
                            return "";
                        } else {
                            return null;
                        }
                    }
                };
                env.processor.setConfigurationProperty(FeatureKeys.ENVIRONMENT_VARIABLE_RESOLVER, resolver);
            }
            /*if (type.equals("feature") && value.equals("xpath-1.0-compatibility")) {
                hostLang = "XP";
                langVersion = "3.0";
                xpDependency = true;
                specOpt = Spec.XP30;
            }
            if (type.equals("feature") && value.equals("namespace-axis")) {
                hostLang = "XP";
                langVersion = "3.0";
                xpDependency = true;
                specOpt = Spec.XP30;
            } */
            if (!ensureDependencySatisfied(dependency, env)) {
                if ("false".equals(dependency.attribute("satisfied"))) {
                    type = "!" + type;
                }
                println("*** Dependency not satisfied: " + type + ":" + value);
                writeTestcaseElement(testCaseName, "n/a", "dependency not satisfied: " + type + ":" + value);
                //run = false;
                notrun++;
                return;
            }
        }

        XdmNode exceptionElement = exceptionsMap.get(testCaseName);
        if (exceptionElement == null) {
            exceptionElement = exceptionsMap.get("$" + testSetName);
        }
        if (exceptionElement != null) {
            Optional<XdmNode> config = exceptionElement.select(child("configuration")).asOptionalNode();

            String runAtt = exceptionElement.attribute("run");
            Optional<String> reasonMsg = exceptionElement.select(child("reason")).asOptionalString();
            String reportAtt = exceptionElement.attribute("report");
            if (reportAtt == null) {
                reportAtt = "";
            }

            if (config.isPresent()) {
                XdmItem paramValue = xpath.evaluateSingle("param[@name='not-unfolded' and @value='yes']/@name", config.get());
                if (unfolded && paramValue != null) {
                    writeTestcaseElement(testCaseName, "notRun", reasonMsg.orElse("no reason given"));
                    notrun++;
                    return;
                }
            }

            if ("false".equals(runAtt)) {
                writeTestcaseElement(testCaseName, reportAtt, reasonMsg.orElse("no reason given"));
                notrun++;
                return;
            }

            alternativeResult = exceptionElement.select(child("result")).asOptionalNode().orElse(null);
            optimization = exceptionElement.select(child("optimization")).asOptionalNode().orElse(null);
        }

        if (run && (specOpt == null || specOpt == spec)) {

            TestOutcome outcome = new TestOutcome(this);
            String exp = null;
            try {
                exp = xpc.evaluate("if (test/@file) then unparsed-text(resolve-uri(test/@file, base-uri(.))) else string(test)", testCase).toString();
            } catch (SaxonApiException err) {
                println("*** Failed to read query: " + err.getMessage());
                if (err.getErrorCode().getLocalName().equals("FOUT1190")) {
                    outcome.setException(new QName("XPST0003"));
                } else {
                    outcome.setException(err.getErrorCode());
                }
            }

            if (exp != null && env.processor.getSaxonEdition().equals("HE") && testSetName.equals("app-spec-examples")) {
                // Some tests in app-spec-examples do not have their dependencies tagged

                if (exp.contains("http://www.w3.org/2013/collation/UCA?lang=en;alternate=blanked;strength=primary")) {
                    writeTestcaseElement(testCaseName, "notRun", "requires ICU");
                    notrun++;
                    return;
                }

                if (exp.contains("validate lax")) {
                    writeTestcaseElement(testCaseName, "notRun", "requires schema-awareness");
                    notrun++;
                    return;
                }

                if (exp.contains("'fallback':function($s)")) {
                    writeTestcaseElement(testCaseName, "notRun", "requires HOF");
                    notrun++;
                    return;
                }
            }

            //noinspection ThrowableResultOfMethodCallIgnored
            if (outcome.getException() == null) {
                switch (hostLang) {
                    case "XP":
                    case "XT": {
                        XPathCompiler testXpc = env.xpathCompiler;
                        testXpc.setLanguageVersion(langVersion);
                        testXpc.declareNamespace("fn", NamespaceConstant.FN);
                        testXpc.declareNamespace("xs", NamespaceConstant.SCHEMA);
                        //testXpc.declareNamespace("math", NamespaceConstant.MATH);
                        testXpc.declareNamespace("map", NamespaceConstant.MAP_FUNCTIONS);
                        testXpc.declareNamespace("array", NamespaceConstant.ARRAY_FUNCTIONS);

                        copySchemaNamespaces(env, testXpc);  // ensure environment has schema namespaces


                        ModuleResolver mr = new ModuleResolver(xpc); //

                        mr.setTestCase(testCase);
                        testXpc.getProcessor().getUnderlyingConfiguration().setModuleURIResolver(mr);

                        try {
                            XPathSelector selector = testXpc.compile(exp).load();
                            for (QName varName : env.params.keySet()) {
                                selector.setVariable(varName, env.params.get(varName));
                            }
                            if (env.contextItem != null) {
                                selector.setContextItem(env.contextItem);
                            }
                            selector.setURIResolver(new TestURIResolver(env));
                            //selector.setURIResolver(new TestURIResolver(env));
                            if (env.unparsedTextResolver != null) {
                                selector.getUnderlyingXPathContext().setUnparsedTextURIResolver(env.unparsedTextResolver);
                            }
                            XdmValue result = selector.evaluate();
                            outcome.setPrincipalResult(result);
                        } catch (SaxonApiException err) {
                            println(err.getMessage());
                            outcome.setException(err.getErrorCode());
                        } catch (Exception err) {
                            println(err.getMessage());
                            err.printStackTrace();
                            writeTestcaseElement(testCaseName, "fail", "*** crashed: " + err.getMessage());
                            //notrun++;
                            //return;
                        }
                        break;
                    }
                    case "XQ": {
                        XQueryCompiler testXqc = env.xqueryCompiler;
                        //testXqc.setLanguageVersion(langVersion);
                        testXqc.declareNamespace("fn", NamespaceConstant.FN);
                        testXqc.declareNamespace("xs", NamespaceConstant.SCHEMA);
                        //testXqc.declareNamespace("math", NamespaceConstant.MATH);
                        testXqc.declareNamespace("map", NamespaceConstant.MAP_FUNCTIONS);
                        testXqc.declareNamespace("array", NamespaceConstant.ARRAY_FUNCTIONS);
                        ErrorCollector errorCollector = new ErrorCollector();
                        testXqc.setErrorReporter(errorCollector);
                        String decVars = env.paramDecimalDeclarations.toString();
                        if (!decVars.isEmpty()) {
                            int x = exp.indexOf("(:%DECL%:)");
                            if (x < 0) {
                                exp = decVars + exp;
                            } else {
                                exp = exp.substring(0, x) + decVars + exp.substring(x + 13);
                            }
                        }
                        String vars = env.paramDeclarations.toString();
                        if (!vars.isEmpty()) {
                            int x = exp.indexOf("(:%VARDECL%:)");
                            if (x < 0) {
                                exp = vars + exp;
                            } else {
                                exp = exp.substring(0, x) + vars + exp.substring(x + 13);
                            }
                        }
                        ModuleResolver mr = new ModuleResolver(xpc);
                        mr.setTestCase(testCase);
                        testXqc.getProcessor().getUnderlyingConfiguration().setModuleURIResolver(mr);
                        testXqc.setModuleURIResolver(mr);

                        try {
                            testXqc.getUnderlyingStaticContext().setModuleLocation(
                                    new Loc(testXqc.getBaseURI().toString(), testCase.getLineNumber(), -1));
                            XQueryExecutable q = null;
                            try {
                                q = testXqc.compile(exp);
                            } catch (SaxonApiException e) {
                                println("Static error in query");
                                throw e;
                            }
                            if (optimization != null) {
                                // Test whether required optimizations have been performed
                                XdmDestination expDest = new XdmDestination();
                                Configuration config = driverProc.getUnderlyingConfiguration();
                                try {
                                    Receiver receiver = expDest.getReceiver(config.makePipelineConfiguration(), config.obtainDefaultSerializationProperties());
                                    ExpressionPresenter presenter = new ExpressionPresenter(config, receiver);
                                    q.getUnderlyingCompiledQuery().explain(presenter);
                                    presenter.close();
                                } catch (XPathException e) {
                                    e.printStackTrace();
                                }
                                XdmNode explanation = expDest.getXdmNode();
                                XPathCompiler assertXpc = xpc.getProcessor().newXPathCompiler();
                                XdmItem optResult = assertXpc.evaluateSingle(optimization.attribute("assert"), explanation);
                                if (((XdmAtomicValue) optResult).getBooleanValue()) {
                                    println("Optimization result OK");
                                } else {
                                    println("Failed optimization test {" + optimization.attribute("assert") + "}");
                                    driverProc.writeXdmValue(explanation, driverProc.newSerializer(System.err));
                                    writeTestcaseElement(testCaseName, "fail", "Failed optimization assertions");
                                    noteFailure(testSetName, testCaseName);
                                    return;
                                }

                            }
                            XQueryEvaluator selector = q.load();
                            for (QName varName : env.params.keySet()) {
                                selector.setExternalVariable(varName, env.params.get(varName));
                            }
                            if (env.contextItem != null) {
                                selector.setContextItem(env.contextItem);
                            }
                            selector.setURIResolver(new TestURIResolver(env));
                            if (env.unparsedTextResolver != null) {
                                selector.getUnderlyingQueryContext().setUnparsedTextURIResolver(env.unparsedTextResolver);
                            }
                            if (needSerializedResult) {
                                StringWriter sw = new StringWriter();
                                Serializer serializer = env.processor.newSerializer(sw);
                                selector.setDestination(serializer);
                                selector.run();
                                outcome.setPrincipalSerializedResult(sw.toString());
                            }
                            if (needResultValue) {
                                XdmValue result = selector.evaluate();
                                outcome.setPrincipalResult(result);
                            }
                        } catch (SaxonApiException err) {
                            println("in TestSet " + testSetName);
                            println(err.getMessage());
                            outcome.setException(err.getErrorCode());
                            outcome.setErrorsReported(errorCollector.getErrorCodes());
                        } catch (LicenseException err) {
                            // treat this as "facility not available", XQST0075
                            println("in TestSet " + testSetName);
                            println(err.getMessage());
                            outcome.setException(new QName("XPST0075"));
                            errorCollector.error(new XmlProcessingIncident(err.getMessage(), "XPST0075"));
                            outcome.setErrorsReported(errorCollector.getErrorCodes());
                        }
                        break;
                    }
                    default:
                        writeTestcaseElement(testCaseName, "notRun", "No processor found");
                        notrun++;
                        return;
                }
            }

            for (Environment.ResetAction action : env.resetActions) {
                action.reset(env);
            }
            env.resetActions.clear();
            XdmNode assertion;
            if (alternativeResult != null) {
                assertion = (XdmNode) xpc.evaluateSingle("*[1]", alternativeResult);
            } else {
                assertion = (XdmNode) xpc.evaluateSingle("result/*[1]", testCase);
            }
            if (assertion == null) {
                println("*** No assertions found for test case " + testCaseName);
                writeTestcaseElement(testCaseName, "disputed", "No assertions in test case");
                noteFailure(testSetName, testCaseName);
                return;
            }
            XPathCompiler assertXpc = env.processor.newXPathCompiler();
            assertXpc.setLanguageVersion("3.1");
            assertXpc.declareNamespace("fn", NamespaceConstant.FN);
            assertXpc.declareNamespace("xs", NamespaceConstant.SCHEMA);
            assertXpc.declareNamespace("math", NamespaceConstant.MATH);
            assertXpc.declareNamespace("map", NamespaceConstant.MAP_FUNCTIONS);
            assertXpc.declareNamespace("array", NamespaceConstant.ARRAY_FUNCTIONS);
            assertXpc.declareNamespace("j", NamespaceConstant.FN);
            assertXpc.declareVariable(new QName("result"));
            assertXpc.setBaseURI(assertion.getBaseURI());

            copySchemaNamespaces(env, assertXpc);  // ensure environment has schema namespaces

            boolean success = outcome.testAssertion(assertion, outcome, outcome.getPrincipalResultDoc(), assertXpc, xpath, debug);
            if (success) {
                successes++;
                writeTestcaseElement(testCaseName, "pass", null);
            } else {
                XdmItem expectedError = xpc.evaluateSingle("result//error/@code", testCase);
                if (outcome.isException()){
                    if (expectedError != null){
                        wrongErrorResults++;
                        successes++;
                        if (outcome.getWrongErrorMessage() != null) {
                            outcome.setComment(outcome.getWrongErrorMessage());
                            writeTestcaseElement(testCaseName, "wrongError", outcome.getComment());
                        } else {
                            writeTestcaseElement(testCaseName, "wrongError",
                                    "Expected error:" + expectedError.getStringValue() + ", got " + outcome.getException());
                        }
                        println("*** TEST WRONG ERRORCODE: result " + outcome.getException() +
                                " Expected error:" + expectedError.getStringValue());
                    } else {
                        noteFailure(testSetName, testCaseName);
                        writeTestcaseElement(testCaseName, "fail", "Expected success, got " + outcome.getException());
                        println("*** TEST-FAILURE: result " + outcome.getException() +
                                " Expected success.");
                    }
                } else {
                    noteFailure(testSetName, testCaseName);
                    if (expectedError == null){
//                      if (debug) {
//                            outcome.getException().printStackTrace(System.out);
//                      }
                        writeTestcaseElement(testCaseName, "fail", "Wrong results, got " +
                                truncate(outcome.serialize(assertXpc.getProcessor(), outcome.getPrincipalResultDoc())));
                    } else {
                        writeTestcaseElement(testCaseName, "fail",
                                "Expected error:" + expectedError.getStringValue() + ", got " +
                                        truncate(outcome.serialize(assertXpc.getProcessor(), outcome.getPrincipalResultDoc())));
                    }
                    if (debug) {
                        try {
                            println("*** TEST-FAILURE. Result:");
                            StringWriter sw = new StringWriter();
                            driverSerializer.setOutputWriter(sw);
                            driverSerializer.serializeXdmValue(outcome.getPrincipalResult());
                            println(sw.toString());
                            println("<=======");
                        } catch (Exception err) {
                            // ignore exception
                        }
                        //println(outcome.getResult());
                    } else {
                        println("*** TEST-FAILURE (use -debug to show actual result)");
                        //failures++;
                    }
                }
            }
        }
    }

    private static boolean isApplicableToSpecVersion(String value, Spec spec) {
        boolean applicable = false;
        if (!value.contains(spec.shortSpecName)) {
            applicable = false;
        } else if (value.contains(spec.specAndVersion)) {
            applicable = true;
        } else if ((spec.specAndVersion.equals("XQ30") || spec.specAndVersion.equals("XQ31")) &&
                (value.contains("XQ10+") || value.contains("XQ30+"))) {
            applicable = true;
        } else if ((spec.specAndVersion.equals("XP30") || spec.specAndVersion.equals("XP31")) &&
                (value.contains("XP20+") || value.contains("XP30+"))) {
            applicable = true;
        }
        return applicable;
    }

    private String truncate(String in) {
        if (in.length() > 80) {
            return in.substring(0, 80) + "...";
        } else {
            return in;
        }
    }


    @Override
    protected void writeResultFilePreamble(Processor processor, XdmNode catalog) throws IOException, SaxonApiException, XMLStreamException {
        resultsDoc.writeResultFilePreamble(processor, catalog);
    }

    @Override
    protected void writeResultFilePostamble() throws XMLStreamException {
        resultsDoc.writeResultFilePostamble();
        if (requestedTestSet != null) {
            System.err.println("Failing tests: ");
            for (Map.Entry<String, Integer> entry : failSummary.entrySet()) {
                System.err.println("  " + entry.getKey());
            }
        } else {
            System.err.println("Failures by Test Set: ");
            for (Map.Entry<String, Integer> entry : failSummary.entrySet()) {
                System.err.println("  " + entry.getKey() + " : " + entry.getValue());
            }
        }
    }


    @Override
    protected void startTestSetElement(XdmNode testSetNode) {
        resultsDoc.startTestSetElement(testSetNode);
    }


    @Override
    protected void writeTestSetEndElement() {
        resultsDoc.endElement();
    }

    private void writeTestcaseElement(String name, String result, String comment) {
        resultsDoc.writeTestcaseElement(name, result, comment);
    }

    public static class ModuleResolver extends StandardModuleURIResolver {

        XPathCompiler catXPC;
        XdmNode testCase;

        public ModuleResolver(XPathCompiler xpc) {
            super(xpc.getProcessor().getUnderlyingConfiguration());
            this.catXPC = xpc;
        }

        public void setTestCase(XdmNode testCase) {
            this.testCase = testCase;
        }

        @Override
        public StreamSource[] resolve(String moduleURI, String baseURI, String[] locations) throws XPathException {
            if (locations.length == 0) {
                try {
                    XdmValue files = testCase.select(child("module").where(attributeEq("uri", moduleURI))
                                                             .then(attribute("file")).then(atomize())).asXdmValue();
                    if (files.size() == 0) {
                        throw new XPathException("Failed to find module entry for " + moduleURI);
                    }
                    StreamSource[] ss = new StreamSource[files.size()];
                    for (int i = 0; i < files.size(); i++) {
                        URI uri = testCase.getBaseURI().resolve(files.itemAt(i).toString());
                        ss[i] = getQuerySource(uri);
                    }
                    return ss;
                } catch (SaxonApiUncheckedException e) {
                    throw new XPathException(e);
                }
            } else {
                try {
                    StreamSource[] ss = new StreamSource[locations.length];
                    for (int i=0; i<locations.length; i++) {
                        String loc = locations[i];
                        URI hint = new URI(baseURI).resolve(loc);
                        Predicate<XdmNode> matching = n ->
                            n.attribute("uri").equals(moduleURI)
                                    && n.attribute("location") != null
                                    && testCase.getBaseURI().resolve(n.attribute("location")).equals(hint);

                        XdmValue files = testCase.select(
                                child("module").where(matching).then(attribute("file")).then(atomize())).asXdmValue();
                        if (files.size() == 0) {
                            throw new XPathException("Failed to find module entry for " + moduleURI + " at " + loc);
                        }
                        URI uri = testCase.getBaseURI().resolve(files.itemAt(0).toString());
                        ss[i] = getQuerySource(uri);
                    }
                    return ss;
                } catch (URISyntaxException e) {
                    throw new XPathException(e);
                }
            }
        }
    }

    public static class TestURIResolver implements URIResolver {
        Environment env;

        public TestURIResolver(Environment env) {
            this.env = env;
        }

        @Override
        public Source resolve(String href, String base) throws TransformerException {
            try {
                String abs = ResolveURI.makeAbsolute(href, base).toString();
                XdmNode node = env.sourceDocs.get(abs);
                if (node == null) {
                    node = env.sourceDocs.get(href);
                    if (node == null) {
                        return null;
                    }
                }
                return node.asSource();
            } catch (URISyntaxException e) {
                throw new XPathException("Invalid URI: " + e.getMessage(), "FODC0005");
            }
        }
    }


    /**
     * Static method called as an external function call to evaluate a literal when running in "unfolded" mode.
     * The function simply returns the value of its argument - but the optimizer doesn't know that, so it
     * can't pre-evaluate the call at compile time.
     *
     * @param value the value to be returned
     * @return the supplied value, unchanged
     */

    public static Sequence lazyLiteral(Sequence value) {
        return value;
    }

    /**
     * Collect together information about all the dependencies of tests that use a given environment
     *
     * @param processor the Saxon processor
     * @param env       the environment for which dependency information is to be gathered
     * @throws SaxonApiException if a Saxon error occurs
     */

    private void buildDependencyMap(Processor processor, XdmNode catalog, Environment env) throws SaxonApiException {
        XQueryCompiler xqCompiler = processor.newXQueryCompiler();
        xqCompiler.setBaseURI(new File(System.getProperty("user.dir")).toURI());
        XQueryEvaluator eval = xqCompiler.compile(
                "        declare namespace fots = \"http://www.w3.org/2010/09/qt-fots-catalog\";\n" +
                        "        let $testsets := //fots:test-set/@file/doc(resolve-uri(., exactly-one(base-uri(.))))\n" +
                        "        for $dependencyTS in $testsets//fots:dependency\n" +
                        "        let $type := $dependencyTS/@type\n" +
                        "        let $name := $dependencyTS/@name\n" +
                        "        let $value := $dependencyTS/@value\n" +
                        "        group by $type, $value\n" +
                        "        order by $type, $value\n" +
                        "        return <dependency type='{$type}' name='{$name}' value='{$value}' />").load();
        eval.setContextItem(catalog);
        XdmValue result = eval.evaluate();
        for (XdmItem item : result) {
            XdmNode node = (XdmNode) item;
            String type = node.attribute("type");
            String value = node.attribute("value");
            addDependency(type, value, ensureDependencySatisfied(node, env));
        }


    }


    protected class Dependency {
        public String dType;
        public boolean satisfied;
    }

    private Map<String, Dependency> dependencyMap = new HashMap<String, Dependency>();

    public void addDependency(String depStr, String value, boolean satisfied) {
        if (!dependencyMap.containsKey(value)) {
            Dependency dep = new Dependency();
            dep.dType = depStr;
            dep.satisfied = satisfied;
            dependencyMap.put(value, dep);
        }
    }


}

