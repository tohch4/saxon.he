////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.option.cpp;

import com.saxonica.functions.extfn.cpp.CPPFunctionSet;
import com.saxonica.functions.extfn.cpp.PHPFunctionSet;
import net.sf.saxon.Configuration;
import net.sf.saxon.lib.AugmentedSource;
import net.sf.saxon.lib.Validation;
import net.sf.saxon.s9api.*;

import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import java.io.*;
import java.util.*;


/**
 * * This class is to use with Saxon/C on C++
 */
public class Xslt30Processor extends SaxonCAPI {


    /**
     * Constructor to initialise XsltProcessor with processor and license flag
     *
     * @param proc - s9api processor
     */
    public Xslt30Processor(Processor proc) {
        super(proc);
        Configuration config = processor.getUnderlyingConfiguration();
        schemaAware = config.isLicensedFeature(Configuration.LicenseFeature.ENTERPRISE_XSLT);
        if (debug) {
            System.err.println("Xslt30Processor constructor1(proc, l), Processor: " + System.identityHashCode(proc));
        }
//#if EE==true || PE==true
        if (config.isLicensedFeature(Configuration.LicenseFeature.PROFESSIONAL_EDITION)) {
            if (debug) {
                System.err.println("Xslt30Processor1-1(l), Processor: function libraries added");
            }
            config.getBuiltInExtensionLibraryList().addFunctionLibrary(PHPFunctionSet.getInstance());
            config.getBuiltInExtensionLibraryList().addFunctionLibrary(CPPFunctionSet.getInstance());
        }
//#endif
    }


    /**
     * default Constructor to initialise XsltProcessor. Assume no license file available
     */
    public Xslt30Processor() {
        super();
        if (debug) {
            System.err.println("Xslt30Processor constructor2(), Processor: " + System.identityHashCode(processor));
        }
    }


    /**
     * Constructor to initialise XsltProcessor with license flag
     *
     * @param license - flag indicating presence of license file
     */
    public Xslt30Processor(boolean license) {
        processor = new Processor(license);
        Configuration config = processor.getUnderlyingConfiguration();
        schemaAware = config.isLicensedFeature(Configuration.LicenseFeature.ENTERPRISE_XSLT);
        if (debug) {
            System.err.println("XsltProcessor3(l), Processor: " + System.identityHashCode(processor));
        }
//#if EE==true || PE==true
        if (config.isLicensedFeature(Configuration.LicenseFeature.PROFESSIONAL_EDITION)) {
            if (debug) {
                System.err.println("XsltProcessor3-1(l), Processor: function libraries added");
            }
            config.getBuiltInExtensionLibraryList().addFunctionLibrary(PHPFunctionSet.getInstance());
            config.getBuiltInExtensionLibraryList().addFunctionLibrary(CPPFunctionSet.getInstance());
        }
//#endif

    }


    /**
     * Create new object of this class
     *
     * @param proc - s9api processor
     * @return XsltProcessor
     */
    public static Xslt30Processor newInstance(Processor proc) {
        return new Xslt30Processor(proc);
    }


    /**
     * Compile package from source file and save resulting sef to file store.
     *
     * @param cwd         - current working directory
     * @param xsl         - File name of the stylesheet
     * @param outFilename - the file to which the compiled package should be saved
     */
    public void compileFromFileAndSave(String cwd, String xsl, String outFilename) {
        XsltCompiler compiler = processor.newXsltCompiler();
        try {
            Source source = resolveFileToSource(cwd, xsl);
            XsltPackage pack = compiler.compilePackage(source);
            File file = absoluteFile(cwd, outFilename);
            pack.save(file);
        } catch (SaxonApiException e) {
            e.printStackTrace();
        }
    }


    /**
     * Produce a representation of the compiled stylesheet and save to file, in XML form, suitable for
     * distribution and reloading.
     * @param executable
     * @param filename
     */
    public static void save(String cwd, XsltExecutable executable, String filename) throws SaxonApiException {
        File file = SaxonCAPI.resolveFile(cwd, filename);
        FileOutputStream stream = null;
        try {
            stream = new FileOutputStream(file);
        } catch (FileNotFoundException e) {
            throw new SaxonApiException(e.getMessage());
        }
        executable.export(stream);
    }

    /**
     * Compile package from string and save resulting sef to file store.
     *
     * @param cwd      - current working directory
     * @param str      - File name of the stylsheet
     * @param filename - the file to which the compiled package should be saved
     */
    public void compileFromStringAndSave(String cwd, String str, String filename) {

        XsltCompiler compiler = processor.newXsltCompiler();

        try {
            XsltPackage pack = compiler.compilePackage(new StreamSource(new StringReader(str)));
            File file = absoluteFile(cwd, filename);
            pack.save(file);
        } catch (SaxonApiException e) {
            e.printStackTrace();
        }
    }

    /**
     * Compile package from Xdm node and save resulting sef to file store.
     *
     * @param cwd      - current working directory
     * @param obj      -
     * @param filename - the file to which the compiled package should be saved
     */
    public void compileFromXdmNodeAndSave(String cwd, Object obj, String filename) throws SaxonApiException {

        XsltCompiler compiler = processor.newXsltCompiler();
        XdmNode node;
        if (obj instanceof XdmNode) {
            node = (XdmNode) obj;
        } else {
            SaxonApiException ex = new SaxonApiException("Failed to create Stylesheet from XdoNode");
            throw ex;
        }

        XsltPackage pack = compiler.compilePackage(node.asSource());
        File file = new File(filename);
        pack.save(file);

    }


    public XsltExecutable compileFromAssociatedFile(String cwd, String filename, boolean jit, String[] sparams, Object[] values) throws SaxonApiException {

        try {
            XsltCompiler compiler = processor.newXsltCompiler();
            setStaticParametersFromArray(compiler, sparams, values);
            if (jit) {
                compiler.setJustInTimeCompilation(jit);
            }
            XsltExecutable executable = null;
            /*if (packagesToLoad.size() > 0) {
                compilePackages(compiler);
            } */
            Source source = resolveFileToSource(cwd, filename);

            //compiler.setErrorListener(errorListener);
            compiler.setSchemaAware(schemaAware);
            executable = compiler.compile(compiler.getAssociatedStylesheet(source, null, null, null));
            return executable;
        } catch (SaxonApiException ex) {
            throw ex;
        }
    }


    /**
     * Compile the stylesheet from file  for use later
     *
     * @param cwd      - current working directory
     * @param filename - File name of the stylsheet
     * @return XsltExecutable
     */
    public XsltExecutable compileFromFile(String cwd, String filename, boolean jit, String[] sparams, Object[] values) throws SaxonApiException {

        XsltCompiler compiler = processor.newXsltCompiler();
        setStaticParametersFromArray(compiler, sparams, values);
        if (jit) {
            compiler.setJustInTimeCompilation(jit);
        }
        XsltExecutable executable = null;
        /* if (packagesToLoad.size() > 0) {
            compilePackages(compiler);
        }  */
        Source source = resolveFileToSource(cwd, filename);

        //compiler.setErrorListener(errorListener);
        compiler.setSchemaAware(schemaAware);
        executable = compiler.compile(source);
        return executable;

    }

    /**
     * Compile the stylesheet from string  for use later
     *
     * @param cwd - current working directory
     * @param str - stylesheet as a string
     * @param jit - Falg for Just in Time compilation
     * @return XsltExecutable
     */
    public XsltExecutable compileFromString(String cwd, String str, boolean jit, String[] sparams, Object[] values) throws SaxonApiException {
        XsltCompiler compiler = processor.newXsltCompiler();
        setStaticParametersFromArray(compiler, sparams, values);
        if (jit) {
            compiler.setJustInTimeCompilation(jit);
        }
        Source source;

        StringReader reader = new StringReader(str);
        compiler.setSchemaAware(schemaAware);
        source = new StreamSource(reader);
        if (cwd != null && cwd.length() > 0) {
            if (!cwd.endsWith("/")) {
                cwd = cwd.concat("/");
            }
            source.setSystemId(cwd);

        }

        XsltExecutable executable = compiler.compile(source);
        return executable;


    }

    /**
     * Compile the stylesheet from string  for use later
     *
     * @param cwd     - current working directory
     * @param obj     - stylesheet as a XdmNode object
     * @param sparams - Supplied static parameters
     * @param values
     * @return XsltExecutable
     */
    public XsltExecutable compileFromXdmNode(String cwd, Object obj, boolean jit, String[] sparams, Object[] values) throws SaxonApiException {

        XsltCompiler compiler = processor.newXsltCompiler();
        setStaticParametersFromArray(compiler, sparams, values);
        if (jit) {
            compiler.setJustInTimeCompilation(jit);
        }

        XdmNode node;
        if (obj instanceof XdmNode) {
            node = (XdmNode) obj;
        } else {
            SaxonApiException ex = new SaxonApiException("Failed to create Stylesheet from XdmNode");
            throw ex;
        }

        //compiler.setErrorListener(errorListener);
        /* if (packagesToLoad.size() > 0) {
            compilePackages(compiler);
        }  */
        XsltExecutable executable = compiler.compile(node.asSource());
        return executable;


    }

    private void setStaticParametersFromArray(XsltCompiler compiler, String[] sparams, Object[] values) {
        if (sparams != null && values != null) {
            for (int i = 0; i < sparams.length; i++) {
                if (sparams[i].startsWith("sparam:")) {
                    //static parameters
                    String paramName = sparams[i].substring(7);
                    Object value = values[i];
                    XdmValue valueForCpp = null;
                    QName qname = QName.fromClarkName(paramName);
                    valueForCpp = convertObjectToXdmValue(values[i]);
                    compiler.setParameter(qname, valueForCpp);
                }
            }

        }
    }


    public Xslt30Transformer getXslt30Transformer(String cwd, XsltExecutable executable, String stylesheet, Map<QName, XdmValue> staticParameters) throws SaxonApiException {
        Xslt30Transformer transformer = null;
        if (stylesheet == null && executable != null) {
            if(debug){
                System.err.println("Reusing executable");
            }
            transformer = executable.load30();
            return transformer;
        } else {
            if (stylesheet == null) {
                throw new SaxonApiException("Stylesheet file is null");
            }
            XsltCompiler compiler = processor.newXsltCompiler();

            Source source = resolveFileToSource(cwd, stylesheet);
            //compiler.setErrorListener(errorListener);

            compiler.setSchemaAware(schemaAware);

            /*if ( packagesToLoad.size() > 0) {
                compilePackages(compiler);
            } */
            if (!staticParameters.isEmpty()) {
                for (Map.Entry<QName, XdmValue> entry : staticParameters.entrySet()) {
                    compiler.setParameter(entry.getKey(), entry.getValue());
                }
            }
            transformer = compiler.compile(source).load30();
            return transformer;
        }

    }




    public static XdmValue applyTemplatesReturningValue(String cwd, XsltExecutable executable, Object sourceObj, String[] params, Object[] values) throws SaxonApiException {
        Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();
        applyXsltTransformerProperties(cwd, processor, transformer, null, props, paramsMap, globalParameters, initialTemplateParameters);
        Destination xdmResult;
        boolean returnXdmValue = false;
        if (paramsMap.containsKey("outvalue")) {
            //Set if the return type of callTemplate, applyTemplate and transform methods is to return XdmValue,
            //otherwise return XdmNode object with root Document node
            Object valuei = paramsMap.get("outvalue");
            if (valuei instanceof Boolean) {
                returnXdmValue = (boolean) valuei;
            } else if (valuei instanceof String && valuei.equals("yes") || valuei.equals("true")) {
                returnXdmValue = true;
            }
        }

        if (returnXdmValue) {
            xdmResult = new RawDestination();
        } else {
            xdmResult = new XdmDestination();
        }
        if (sourceObj instanceof String) {
            Source source = resolveFileToSource(cwd, (String) sourceObj);
            if (paramsMap.containsKey("dtd")) {
                AugmentedSource asource = AugmentedSource.makeAugmentedSource(source);
                String option = (String) paramsMap.get("dtd");
                if (!option.isEmpty()) {
                    asource.setDTDValidationMode(Validation.getCode(option));
                }
                source = asource;
            }

            transformer.applyTemplates(source, xdmResult);
        } else if (sourceObj instanceof XdmValue) {
            XdmValue selection = (XdmValue) sourceObj;

            transformer.applyTemplates(selection, xdmResult);
        } else if (sourceObj instanceof XdmItem[]) {
            XdmItem[] itemArr = (XdmItem[]) sourceObj;
            XdmValue valueSeq;
            if (itemArr.length == 0) {
                throw new SaxonApiException("Source object is an empty array of XdmItems");

            } else if (itemArr.length == 1) {
                valueSeq = itemArr[0];
            } else {
                List<XdmItem> itemList = Arrays.asList((XdmItem[]) sourceObj);
                valueSeq = new XdmValue(itemList);
            }

            transformer.applyTemplates(valueSeq, xdmResult);
        } else {
            throw new SaxonApiException("Source object is neither a String value of an XdmValue");
        }


        return getXdmValue(xdmResult, paramsMap);
    }

    private static XdmValue getXdmValue(Destination xdmResult, Map<String, Object> paramsMap) {
        boolean returnXdmValue = false;
        if (paramsMap.containsKey("outvalue")) {
            //Set if the return type of callTemplate, applyTemplate and transform methods is to return XdmValue,
            //otherwise return XdmNode object with root Document node
            Object valuei = paramsMap.get("outvalue");
            if (valuei instanceof Boolean) {
                returnXdmValue = (boolean) valuei;
            } else if (valuei instanceof String && valuei.equals("yes") || valuei.equals("true")) {
                returnXdmValue = true;
            }
        }
        if (returnXdmValue) {
            return ((RawDestination) xdmResult).getXdmValue();
        } else {
            return ((XdmDestination) xdmResult).getXdmNode();
        }
    }

    /**
     *  Call applyTemplates returnign result to the given file name
     * @param cwd
     * @param executable
     * @param sourceObj
     * @param outFilename
     * @param params
     * @param values
     * @throws SaxonApiException
     */
    public static void applyTemplatesReturningFile(String cwd, XsltExecutable executable, Object sourceObj, String outFilename, String[] params, Object[] values) throws SaxonApiException {
        Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");

        }
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();
        Serializer serializer = null;
        if (outFilename != null) {
            if (debug) {
                System.err.println("DEBUG: XSLT30TransformerForCpp param: " + outFilename);
            }
            serializer = resolveOutputFile(processor, cwd, outFilename);
        } else if (paramsMap.containsKey("o")) {
            String outfile = (String) paramsMap.get("o");
            serializer = resolveOutputFile(processor, cwd, outfile);
            if (debug) {
                System.err.println("DEBUG: XSLT30TransformerForCpp param: " + outfile);
            }
        } else if (serializer == null) {
            throw new SaxonApiException("Output file not set for this transformation");
        }

        applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);

        if (sourceObj instanceof String) {

            Source source = resolveFileToSource(cwd, (String) sourceObj);
            if (paramsMap.containsKey("dtd")) {
                AugmentedSource asource = AugmentedSource.makeAugmentedSource(source);
                String option = (String) paramsMap.get("dtd");
                if (!option.isEmpty()) {
                    asource.setDTDValidationMode(Validation.getCode(option));
                }
                source = asource;
            }
            transformer.applyTemplates(source, serializer);
        } else if (sourceObj instanceof XdmValue) {
            XdmValue selection = (XdmValue) sourceObj;
            transformer.applyTemplates(selection, serializer);
        } else if (sourceObj instanceof XdmItem[]) {
            XdmItem[] itemArr = (XdmItem[]) sourceObj;
            XdmValue valueSeq;
            if (itemArr.length == 0) {
                throw new SaxonApiException("Source object is an empty array of XdmItem objects");

            } else if (itemArr.length == 1) {
                valueSeq = itemArr[0];
            } else {
                List<XdmItem> itemList = Arrays.asList((XdmItem[]) sourceObj);
                valueSeq = new XdmValue(itemList);
            }

            transformer.applyTemplates(valueSeq, serializer);
        } else {
            throw new SaxonApiException("Source object is not of type (String, XdmValue or XdmValue[])");
        }


    }

    public static String applyTemplatesReturningString(String cwd, XsltExecutable executable, Object sourceObj, String[] params, Object[] values) throws SaxonApiException {
        Map<QName, XdmValue> staticParameters = (executable!= null?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");

        }
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();
        StringWriter sw = new StringWriter();
        Serializer serializer = processor.newSerializer(sw);

        applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);


        if (sourceObj instanceof String) {
            Source source = resolveFileToSource(cwd, (String) sourceObj);
            if (paramsMap.containsKey("dtd")) {
                AugmentedSource asource = AugmentedSource.makeAugmentedSource(source);
                String option = (String) paramsMap.get("dtd");
                if (!option.isEmpty()) {
                    asource.setDTDValidationMode(Validation.getCode(option));
                }
                source = asource;
            }
            transformer.applyTemplates(source, serializer);
        } else if (sourceObj instanceof XdmValue) {
            XdmValue selection = (XdmValue) sourceObj;
            transformer.applyTemplates(selection, serializer);
        } else if (sourceObj instanceof XdmItem[]) {
            XdmItem[] itemArr = (XdmItem[]) sourceObj;
            XdmValue valueSeq;
            if (itemArr.length == 0) {
                throw new SaxonApiException("Source object is an empty array of XdmItem objects");

            } else if (itemArr.length == 1) {
                valueSeq = itemArr[0];
            } else {
                List<XdmItem> itemList = Arrays.asList((XdmItem[]) sourceObj);
                valueSeq = new XdmValue(itemList);
            }

            transformer.applyTemplates(valueSeq, serializer);
        } else {
            throw new SaxonApiException("Source object is not of type (String, XdmValue or XdmValue[])");
        }
        return sw.toString();
    }

    private static XdmValue[] getArguments(Object[] arguments) throws SaxonApiException {
        if (arguments != null && arguments.length > 0) {
            XdmValue[] values = new XdmValue[arguments.length];
            if (debug && arguments != null) {
                System.err.println("Xslt30Processor getArguments.length=" + arguments.length);
            }
            for (int i = 0; i < arguments.length; i++) {
                values[i] = convertObjectToXdmValue(arguments[i]);
                if (values[i] == null) {
                    throw new SaxonApiException("Argument is not of type xdmValue");
                }

            }
            return values;
        }
        return new XdmValue[]{};

    }


    public static void callFunctionReturningFile(String cwd, XsltExecutable executable, String cFuncName, String outFilename, Objects[] arguments, String[] params, Object[] values) throws SaxonApiException {
        Map<QName, XdmValue> staticParameters = (executable!= null?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        QName qname = QName.fromClarkName(cFuncName);
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");
        }
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();
        Serializer serializer = null;
        if (outFilename != null) {
            serializer = resolveOutputFile(processor, cwd, outFilename);
        } else if (paramsMap.containsKey("o")) {
            String outfile = (String) paramsMap.get("o");
            serializer = resolveOutputFile(processor, cwd, outfile);
        } else {
            throw new SaxonApiException("File name for CallFunction has not been set");
        }

        applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);

        transformer.callFunction(qname, getArguments(arguments), serializer);

    }

    public static XdmValue callFunctionReturningValue(String cwd, XsltExecutable executable, String cFuncName, Object[] arguments, String[] params, Object[] values) throws SaxonApiException {

        Map<QName, XdmValue> staticParameters = (executable!= null?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        QName qname = QName.fromClarkName(cFuncName);
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, null, initialTemplateParameters, true);

        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");
        }
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();

        applyXsltTransformerProperties(cwd, processor, transformer, null, props, paramsMap, globalParameters, initialTemplateParameters);

        XdmValue result = transformer.callFunction(qname, getArguments(arguments));
        return result;
    }

    public static String callFunctionReturningString(String cwd, XsltExecutable executable, String cFuncName, Object[] arguments, String[] params, Object[] values) throws SaxonApiException {
        Map<QName, XdmValue> staticParameters = (executable!= null?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        QName qname = QName.fromClarkName(cFuncName);
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);

        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");
        }
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();

        StringWriter sw = new StringWriter();
        Serializer serializer = processor.newSerializer(sw);
        applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);

        transformer.callFunction(qname, getArguments(arguments), serializer);
        serializer = null;
        return sw.toString();
    }


    public static void callTemplateReturningFile(String cwd, XsltExecutable executable, String templateName, String outFilename, String[] params, Object[] values) throws SaxonApiException {
        Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        
        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");
        }
        QName qname = null;
        if (templateName != null) {
            qname = QName.fromClarkName(templateName);
        }
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();
        Serializer serializer = null;
        if (outFilename != null) {
            serializer = resolveOutputFile(processor, cwd, outFilename);
        } else if (paramsMap.containsKey("o")) {
            String outfile = (String) paramsMap.get("o");
            serializer = resolveOutputFile(processor, cwd, outfile);
        } else {
            throw new SaxonApiException("File name for CallFunction has not been set");
        }

        applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);

        transformer.callTemplate(qname, serializer);
    }

    public static XdmValue callTemplateReturningValue(String cwd, XsltExecutable executable, String clarkName, String[] params, Object[] values) throws SaxonApiException {
        Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        QName qname = null;

        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");
        }

        if (clarkName != null) {
            qname = QName.fromClarkName(clarkName);
        }
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();

        //setsource(cwd, transformer, sourceObj, paramsMap);
        applyXsltTransformerProperties(cwd, processor, transformer, null, props, paramsMap, globalParameters, initialTemplateParameters);
        Destination xdmResult;
        boolean returnXdmValue = false;
        if (paramsMap.containsKey("outvalue")) {
            //Set if the return type of callTemplate, applyTemplate and transform methods is to return XdmValue,
            //otherwise return XdmNode object with root Document node
            Object valuei = paramsMap.get("outvalue");
            if (valuei instanceof Boolean) {
                returnXdmValue = (boolean) valuei;
            } else if (valuei instanceof String && valuei.equals("yes") || valuei.equals("true")) {
                returnXdmValue = true;
            }
        }
        if (returnXdmValue) {
            xdmResult = new RawDestination();
        } else {
            xdmResult = new XdmDestination();
        }

        transformer.callTemplate(qname, xdmResult);

        return getXdmValue(xdmResult, paramsMap);
    }

    private void setsource(String cwd, Xslt30Transformer transformer, Object sourceObj, Map<String, Object> paramsMap) throws SaxonApiException {
        if (sourceObj != null) {
            if (sourceObj instanceof String) {
                XdmNode node;
                DocumentBuilder builder = processor.newDocumentBuilder();
                Source source = resolveFileToSource(cwd, (String) sourceObj);
                if (paramsMap.containsKey("dtd")) {
                    AugmentedSource asource = AugmentedSource.makeAugmentedSource(source);
                    String option = (String) paramsMap.get("dtd");
                    if (!option.isEmpty()) {
                        asource.setDTDValidationMode(Validation.getCode(option));
                    }
                    source = asource;
                    node = builder.build(source);
                    transformer.setGlobalContextItem(node);
                }

            } else if (sourceObj instanceof XdmItem) {
                XdmItem node = (XdmItem) sourceObj;
                transformer.setGlobalContextItem(node);
            }
        }

    }

    public static String callTemplateReturningString(String cwd, XsltExecutable executable, String clarkName, String[] params, Object[] values) throws SaxonApiException {
        QName qname = null;
        if (clarkName != null) {
            qname = QName.fromClarkName(clarkName);
        }
        Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();

        if(executable == null) {
            throw new SaxonApiException("Error XsltExecutable is null");
        }

        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        Xslt30Transformer transformer = executable.load30();
        Processor processor = executable.getProcessor();

        StringWriter sw = new StringWriter();
        Serializer serializer = processor.newSerializer(sw);

        //setsource(cwd, transformer, sourceObj, paramsMap);

        applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);


        transformer.callTemplate(qname, serializer);
        return sw.toString();
    }

    /**
     * Do transformation. Save result to file
     * This method is designed to be used with the compile[File/String] methods above,
     * therefore executable can be loaded in a previous step.
     * However this method can be used as a one-shot method to go
     * through the stages of compilation, loading any source documents and execution.
     * Here we supply parameters and properties required to do the transformation.
     * The parameter names and values are supplied as a two arrays in the form of a key and value.
     *
     * @param cwd            - current working directory
     * @param sourceFilename - source supplied as a file name
     * @param stylesheet     - File name of the stylesheet
     * @param outFilename    - Save result of transformation to the given file name
     * @param params         - parameters and property names given as an array of stings
     * @param values         - the values of the paramaters and properties. given as a array of Java objects
     */
    public static void transformToFile(String cwd, Xslt30Processor xslt30Processor, XsltExecutable executable, String sourceFilename, String stylesheet, String outFilename, String[] params, Object[] values) throws SaxonApiException {
        try {
            Serializer serializer = null;
            Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
            Map<QName, XdmValue> globalParameters = new HashMap<>();
            Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
            Properties props = new Properties();
            Source source;
            Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
            Xslt30Transformer transformer;
            Processor processor;

            if (executable != null){
                transformer = executable.load30();
                processor = executable.getProcessor();
            }else if(xslt30Processor != null) {
                processor = xslt30Processor.getProcessor();
                transformer = xslt30Processor.getXslt30Transformer(cwd, executable, stylesheet, staticParameters);
            } else {
                  throw new SaxonApiException("Error XsltExecutable is null");
            }
            
            if (outFilename != null) {
                serializer = resolveOutputFile(processor, cwd, outFilename);
            } else if (paramsMap.containsKey("o")) {
                String outfile = (String) paramsMap.get("o");
                serializer = resolveOutputFile(processor, cwd, outfile);
            } else if (serializer == null) {
                throw new SaxonApiException("Output file not set for this transformation");
            }

            applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);
            XdmNode doc = null;

            Object valuei = paramsMap.get("node");
            if (valuei == null) {
                valuei = paramsMap.get("param:node");
            }
            if (valuei != null && valuei instanceof XdmNode) {
                doc = (XdmNode) valuei;
            }

            if (sourceFilename == null && doc != null) {
                transformer.setGlobalContextItem(doc);
            } else if (sourceFilename != null) {
                source = resolveFileToSource(cwd, sourceFilename);

                transformer.transform(source, serializer);
            }


            serializer = null;

        } catch (SaxonApiException e) {
            SaxonCException ex = new SaxonCException(e);
            throw e;
        } catch (NullPointerException ex) {
            SaxonApiException ex2 = new SaxonApiException(ex);
            throw ex2;
        } catch (Exception ex) {
            SaxonApiException ex2 = new SaxonApiException(ex);
            throw ex2;
        }
    }


    /**
     * Applies the properties and parameters required in the transformation.
     * In addition we can supply the source, stylesheet and output file names.
     * We can also supply values to xsl:param and xsl:variables required in the stylesheet.
     * The parameter names and values are supplied as a two arrays in the form of a key and value.
     *  @param cwd                       - current working directory
     * @param processor                 - required to use the same processor as for the compiled stylesheet
     * @param transformer               - pass the current object to set local variables supplied in the parameters
     * @param serializer
     * @param props
     * @param map                       - parameters and property names given as string keys. Their Values given as a array of Java objects
     * @param globals
     * @param initialTemplateParameters
     */
    public static void applyXsltTransformerProperties(String cwd, Processor processor, Xslt30Transformer transformer, Serializer serializer, Properties props, Map<String, Object> map, Map<QName, XdmValue> globals, Map<QName, XdmValue> initialTemplateParameters) throws SaxonApiException {
        boolean tunnel = false;
        if (!map.isEmpty()) {
            String initialMode;
            XdmItem item;
            String outfile = null;
            Source source;
            Object valuei = null;
            DocumentBuilder builder = processor.newDocumentBuilder();

            if (cwd != null && cwd.length() > 0) {
                if (!cwd.endsWith("/")) {
                    cwd = cwd.concat("/");
                }
            }

            if (map.containsKey("extc")) {
                //extension function library path
                String libName = (String) map.get("extc");
                SaxonCAPI.setLibrary("", libName);


            }

            if (map.containsKey("baseoutput")) {
                valuei = map.get("baseoutput");
                if (valuei instanceof String) {
                    transformer.setBaseOutputURI((String) valuei);
                }

            }

            if (map.containsKey("im")) {
                valuei = map.get("im");
                if (valuei instanceof String) {
                    initialMode = (String) valuei;
                    transformer.setInitialMode(new QName(initialMode));
                } else if (debug) {
                    System.err.println("DEBUG: value error for property 'im'");
                }
            }
            if (map.containsKey("s")) {
                valuei = map.get("s");
                if (valuei instanceof String) {
                    source = SaxonCAPI.resolveFileToSource(cwd, (String) valuei);
                    XdmNode node = builder.build(source);
                    transformer.setGlobalContextItem(node);
                } else if (debug) {
                    System.err.println("DEBUG: value error for property 's'");
                }
            }
            if (map.containsKey("item")) {


                valuei = map.get("item");
                if (valuei instanceof XdmItem) {
                    if (debug && valuei != null) {
                        System.err.println("DEBUG: Type of value=" + (valuei).getClass().getName());

                    }
                }
                item = (XdmItem) valuei;
                transformer.setGlobalContextItem(item);

            } else if (map.containsKey("node")) {


                valuei = map.get("node");
                if (valuei instanceof XdmItem) {
                    if (debug && valuei != null) {
                        System.err.println("DEBUG: Type of value=" + (valuei).getClass().getName());

                    }
                }
                item = (XdmItem) valuei;
                transformer.setGlobalContextItem(item);
            } else if (map.containsKey("param:node")) {


                valuei = map.get("param:node");
                if (valuei instanceof XdmItem) {
                    if (debug && valuei != null) {
                        System.err.println("DEBUG: Type of value=" + (valuei).getClass().getName());

                    }
                }
                item = (XdmItem) valuei;
                transformer.setGlobalContextItem(item);
            }

            if (map.containsKey("m")) {
                String value = (String) map.get("m");
                transformer.setMessageListener(new SaxonCMessageListener(cwd, value));

            }


            if (map.containsKey("tunnel")) {
                valuei = map.get("tunnel");
                if (valuei instanceof Boolean) {
                    tunnel = (Boolean) valuei;
                } else if (valuei instanceof String) {
                    tunnel = ((String) valuei).equals("yes") || valuei.equals("true");
                }
            }


        }
        if (!props.isEmpty() && serializer != null) {
            serializer.setOutputProperties(props);
        }

        if (!initialTemplateParameters.isEmpty()) {
            transformer.setInitialTemplateParameters(initialTemplateParameters, tunnel);
        }

        transformer.setStylesheetParameters(globals);

    }


    /**
     * Do transformation and return result as an Xdm node in memory
     * This method is designed to be used with the createStylesheet[File/String] methods above,
     * therefore executable can be loaded in a previous step.
     * However this method can be used as a one-shot method to go
     * through the stages of compilation, loading any source documents and execution.
     * Here we supply parameters and properties required to do the transformation.
     * The parameter names and values are supplied as a two arrays in the form of a key and value.
     *
     * @param cwd        - current working directory
     * @param sourceFile - source supplied as a file name
     * @param stylesheet - File name of the stylesheet
     * @param params     - parameters and property names given as an array of stings
     * @param values     - the values of the paramaters and properties. given as a array of Java objects
     * @return result as an XdmNode
     */
    public static XdmValue transformToValue(String cwd, Xslt30Processor xslt30Processor, XsltExecutable executable, String sourceFile, String stylesheet, String[] params, Object[] values) throws SaxonApiException {
        Source source;

        Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        try {
            Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
            Xslt30Transformer transformer;
            Processor processor;

            if (executable != null){
                transformer = executable.load30();
                processor = executable.getProcessor();
            }else if(xslt30Processor != null) {
                processor = xslt30Processor.getProcessor();
                transformer = xslt30Processor.getXslt30Transformer(cwd, executable, stylesheet, staticParameters);
            } else {
                  throw new SaxonApiException("Error XsltExecutable is null");
            }

            applyXsltTransformerProperties(cwd, processor, transformer, null, props, paramsMap, globalParameters, initialTemplateParameters);
            Destination destination;
            boolean returnXdmValue = false;
            if (paramsMap.containsKey("outvalue")) {
                //Set if the return type of callTemplate, applyTemplate and transform methods is to return XdmValue,
                //otherwise return XdmNode object with root Document node
                Object valuei = paramsMap.get("outvalue");
                if (valuei instanceof Boolean) {
                    returnXdmValue = (boolean) valuei;
                } else if (valuei instanceof String && valuei.equals("yes") || valuei.equals("true")) {
                    returnXdmValue = true;
                }
            }
            if (returnXdmValue) {
                destination = new RawDestination();
            } else {
                destination = new XdmDestination();
            }
            XdmNode doc = null;

            if(sourceFile == null && paramsMap.containsKey("node")) {
                Object valuei = paramsMap.get("node");
                if (valuei == null) {
                    valuei = paramsMap.get("param:node");
                }
                if (valuei != null && valuei instanceof XdmNode) {
                    doc = (XdmNode) valuei;
                }
            } else if (sourceFile == null && paramsMap.containsKey("s")) {
                 Object valuei = paramsMap.get("s");
                 if (valuei instanceof String) {
                     DocumentBuilder builder = processor.newDocumentBuilder();
                     source = SaxonCAPI.resolveFileToSource(cwd, (String) valuei);
                     doc = builder.build(source);
                 } else if (debug) {
                     System.err.println("DEBUG: value error for property 's'");
                 }
             }

            if (sourceFile == null && doc != null) {
                transformer.transform(doc.asSource(), destination);
            } else if (sourceFile != null) {
                source = resolveFileToSource(cwd, sourceFile);

                transformer.transform(source, destination);

            } else {
                throw new SaxonApiException("Source not set in transform method");
            }

            return getXdmValue(destination, paramsMap);
        } catch (Exception ex) {
            throw new SaxonApiException(ex);
        }

    }


    /**
     * Do transformation. The result is serialized as string representation in memory
     * This method is designed to be used with the createStylesheet[File/String] methods above,
     * therefore executable can be loaded in a previous step.
     * However this method can be used as a one-shot method to go
     * through the stages of compilation, loading any source documents and execution.
     * Here we supply parameters and properties required to do the transformation.
     * The parameter names and values are supplied as a two arrays in the form of a key and value.
     *
     * @param cwd        - current working directory
     * @param sourceFile - source supplied as a file name
     * @param stylesheet - File name of the stylesheet
     * @param params     - parameters and property names given as an array of stings
     * @param values     - the values of the paramaters and properties. given as a array of Java objects
     * @return result as a string representation in a byte array
     */
    public static byte[] transformToString(String cwd, Xslt30Processor xslt30Processor, XsltExecutable executable, String sourceFile, String stylesheet, String[] params, Object[] values) throws SaxonApiException {

        Map<QName, XdmValue> staticParameters = (executable != null ?  null : new HashMap<>());
        Map<QName, XdmValue> globalParameters = new HashMap<>();
        Map<QName, XdmValue> initialTemplateParameters = new HashMap<>();
        Properties props = new Properties();
        Map<String, Object> paramsMap = setupConfigurationAndBuildMap(params, values, props, null, staticParameters, globalParameters, initialTemplateParameters, true);
        try {
            Source source = null;
            Xslt30Transformer transformer;
            Processor processor;

            if (executable != null){
                transformer = executable.load30();
                processor = executable.getProcessor();
            }else if(xslt30Processor != null) {
                processor = xslt30Processor.getProcessor();
                transformer = xslt30Processor.getXslt30Transformer(cwd, null, stylesheet, staticParameters);
            } else {
                  throw new SaxonApiException("Error XsltExecutable is null");
            }

            if (debug) {
                System.err.println("transformToString, Processor: " + System.identityHashCode(processor));
            }

            ByteArrayOutputStream bStream = new ByteArrayOutputStream();
            Serializer serializer = processor.newSerializer(bStream);


            applyXsltTransformerProperties(cwd, processor, transformer, serializer, props, paramsMap, globalParameters, initialTemplateParameters);
            XdmNode doc = null;

            if(paramsMap.containsKey("node")) {
                Object valuei = paramsMap.get("node");
                if (valuei == null) {
                    valuei = paramsMap.get("param:node");
                }
                if (valuei != null && valuei instanceof XdmNode) {
                    doc = (XdmNode) valuei;
                }
            }

            if (doc == null && paramsMap.containsKey("s")) {
                 Object valuei = paramsMap.get("s");
                 if (valuei instanceof String) {
                     DocumentBuilder builder = processor.newDocumentBuilder();
                     source = SaxonCAPI.resolveFileToSource(cwd, (String) valuei);
                     doc = builder.build(source);
                 } else if (debug) {
                     System.err.println("DEBUG: value error for property 's'");
                 }
             }

            if (sourceFile == null && doc != null) {
                source = doc.asSource();
            } else if (sourceFile != null) {
                source = resolveFileToSource(cwd, sourceFile);
            }
            //transformer.setErrorListener(errorListener);
            transformer.transform(source, serializer);
            serializer = null;
            return bStream.toByteArray();
        } catch (Exception e) {
            SaxonApiException ex = new SaxonApiException(e);
            throw ex;
        }
    }


    /***
     * Compile a library package and link it for use.
     * <p>The source argument identifies an XML file containing an &lt;xsl:package&gt; element. Any packages
     * on which this package depends must have been made available to the <code>XsltCompiler</code>
     * by importing them.</p>
     */
    /*public void compilePackages(XsltCompiler compiler) throws SaxonApiException {
        try {
            PackageLibrary library = new PackageLibrary(compiler.getUnderlyingCompilerInfo(), packagesToLoad);
            compiler.getUnderlyingCompilerInfo().setPackageLibrary(library);
        } catch (XPathException e) {
            throw new SaxonApiException(e);
        }
    }    */






    public static void main(String[] args) throws Exception {
        String cwd2 = "/Users/ond1/work/development/svn/archive/opensource/latest9.9/hec/samples/php/trax";
        String cwd = "/Users/ond1/work/development/tests/jeroen";
        // String cwd = "C:///www///html///trax";
        //String cwd = "http://localhost/trax";
        /*if (args.length > 0) {
            cwd = args[0];
        }      */

        String sourcefile1 = "/Users/ond1/work/development/tests/jeroen/xml/kamervragen.xml";//"sinsello.xml";//"saxon_php3/xmark64.xml";
        String stylesheet12 = "xslt/overzicht-resultaten.xslt";//"cadenaoriginal_3_2.xslt";//""saxon_php3/q8.xsl";//"test.xsl";
        String outfile = "outfile.html";
        Processor processor = new Processor(true);
        Xslt30Processor cpp = new Xslt30Processor(processor);
        //cpp.compileFromFile(cwd2, "xsl/foo.xsl");
        cpp.compileFromFileAndSave(cwd2, "xsl/foo.xsl", "xsl/foo.xslp");
        String resultStr = new String(cpp.transformToString(cwd2, cpp, null,"xml/foo.xml", "xsl/foo.xslp", null, null));
        System.out.println(resultStr);

        try {
            String resultStr4 = new String(cpp.transformToString(cwd2, cpp,null,"xml/foo.xml", "xsl/fooExFunc.xsl", null, null));
            System.out.println("Result String 4 = " + resultStr4);
        } catch (SaxonApiException ex) {
            System.out.println("Error in transformation Message: " + ex.getMessage());
        }


        XsltExecutable executable1 = cpp.compileFromFile(cwd2, "xsl/fooExFunc.xsl", false, null, null);
        XdmValue resultV = cpp.callFunctionReturningValue(cwd2, executable1, "{http://www.saxonica.com/myfunction}is-licensed-EE", null, null, null);
        String resultCF = cpp.callFunctionReturningString(cwd2, executable1,  "{http://www.saxonica.com/myfunction}is-licensed-EE", null, new String[]{"!omit-xml-declaration"}, new Object[]{"yes"});

        System.out.println("Running callFunction is-licensed-EE= " + resultV.toString());
        System.out.println("Running callFunction is-licensed-EE= " + resultCF);

        String resultStr3 = new String(cpp.transformToString(cwd2, cpp,null,"xml/foo.xml", "xsl/foo4.xsl", null, null));
        System.out.println("Using initial-template: " + resultStr3);

        XdmNode node2 = cpp.parseXmlFile("/Users/ond1/work/development/campos", "ORP0301177AA__EE__30954_sinsello.xml");
        String[] paramsx = {"node"};
        Object[] valuesx = {node2};
        String result2 = new String(cpp.transformToString("/Users/ond1/work/development/campos", cpp,null, "ORP0301177AA__EE__30954_sinsello.xml", "campos.xsl", paramsx, valuesx));
        Object[] arrValues = {2, "test"};

        String[] params1 = {"resources", "param:test1", "node", "m", "xmlversion"};
        Object[] values1 = {"/Users/ond1/work/development/tests/jeroen/data", arrValues, node2, "m", "1.1"};
        XsltExecutable executable2 = cpp.compileFromFile(cwd, stylesheet12, false, null, null);
        String outputdoc = new String(cpp.transformToString(cwd, cpp, executable2, null, null, params1, values1));
        // System.out.println(outputdoc);
        // System.exit(0);
        // Processor processor = cpp.getProcessor();
        // XsltTransformer transformer = cpp.xsltParseStylesheetFile(args[0]).load();
        //XdmNode sourceNode = cpp.xmlParseFile(cwd, "xml/foo.xml");
        XdmNode sourceNode2 = SaxonCAPI.parseXmlString(null, processor, null, "<result><assert-xml file=\"type-0501.out\"/></result>");
        XdmValue node1 = (XdmValue) cpp.createXdmAtomicItem("string", "textXXXXX");

        XdmValue resultNode2 = cpp.parseXmlString("<?xml version=\"1.0\" encoding=\"UTF-8\"?><html><head><title>Untitled</title></head><body leftmargin=\"100\"></body></html>");

        XdmValue value1 = SaxonCAPI.createXdmAtomicItem("string", "good bye");
        String[] params2 = {"o"};
        Object[] values2 = {"output_test.xml"};
        String[] params3 = {"node", "!indent", "output_test.xml", "xmlversion"};
        String[] param4 = {"s", "param:a-param"};
        Object[] values4 = {"xml/foo.xml", value1};
        Object[] values3 = {sourceNode2, "yes", "o", "1.0"};
        cpp.compileFromFile(cwd, stylesheet12, false,null, null);


        String result = "";
        int repeat = 1;
        try {
            XsltExecutable executable3 = cpp.compileFromString("samples", "<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"\n" +
                    "    version=\"2.0\" xmlns:pf=\"http://example.com\">\n" +
                    "<xsl:param name=\"pf:param-name\"  />" +
                    "<xsl:param name=\"test1\"  />" +

                    "    \n" +
                    "    \n" +
                    "    <xsl:template match=\"/\" >\n" +
                    "   <xsl:message>test messages</xsl:message>" +
                    "        <xsl:copy-of select=\".\"/>\n" +
                    "       XXXXXX <xsl:value-of select=\"$pf:param-name\"/>\n" +
                    "    </xsl:template>\n" +
                    "    \n" +
                    "   \n" +
                    "</xsl:stylesheet>", false, null, null);


            String valueStr = new String(cpp.transformToString(cwd, cpp, executable3, "categories.xml", null, null, null));
            if (valueStr != null) {
                System.out.println("Output = " + valueStr);
            } else {
                System.out.println("valueSt is null");
            }


            String resultStr2 = new String(cpp.transformToString(cwd2, cpp, executable3, null, null, param4, values4));

           /* cpp.transformToFile(cwd, "categories.xml", stylesheet12, "outputTest.txt", null, null);
            long startTime = System.currentTimeMillis();
            for (int i = 0; i < repeat; i++) {
                //result = cpp.xsltApplyStylesheet(cwd, null, "xsl/foo.xsl", params3, values3);
                result = cpp.transformToString(cwd, null, null, params1, values1);

            }
            long endTime = System.currentTimeMillis();
            //  System.out.println("output:" + result + " Time:" + ((endTime - startTime) / 5));

            System.out.println("output:" + result2); */
        } catch (Exception ex) {
            System.out.println("Error found=" + ex.getMessage());
        }
        //SaxonCException[] exceptionForCpps = cpp.getExceptions();

        //System.out.println("xslMessage output:" + cpp.getXslMessages().length);

    }


}
