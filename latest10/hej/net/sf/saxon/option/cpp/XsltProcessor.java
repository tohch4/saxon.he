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
import net.sf.saxon.lib.FeatureKeys;
import net.sf.saxon.om.AtomicArray;
import net.sf.saxon.om.SequenceTool;
import net.sf.saxon.s9api.*;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.trans.packages.PackageLibrary;
import net.sf.saxon.value.AtomicValue;

import javax.xml.transform.Source;
import javax.xml.transform.SourceLocator;
import javax.xml.transform.stream.StreamSource;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;


//TODO: setParameter('name', 'value')
//TODO: setParameter('name', 'value', 'type')


/**
 * * This class is to use with Saxon/C on C++
 */
public class XsltProcessor extends SaxonCAPI {

    private XsltExecutable executable = null;
    private Set<File> packagesToLoad = new HashSet<File>();
    private boolean jitCompilation = false;

    /**
     * Constructor to initialise XsltProcessor with processor and license flag
     *
     * @param proc    - s9api processor
     */
    public XsltProcessor(Processor proc) {
        super(proc);
        Configuration config = processor.getUnderlyingConfiguration();
        schemaAware = config.isLicensedFeature(Configuration.LicenseFeature.ENTERPRISE_XSLT);
        if (debug) {
            System.err.println("XsltProcessor constructor1(proc, l), Processor: " + System.identityHashCode(proc));
        }
//#if EE==true || PE==true
        if(config.isLicensedFeature(Configuration.LicenseFeature.PROFESSIONAL_EDITION)) {
            if (debug) {
                System.err.println("XsltProcessor1-1(l), Processor: function libraries added");
            }
            config.getBuiltInExtensionLibraryList().addFunctionLibrary(PHPFunctionSet.getInstance());
            config.getBuiltInExtensionLibraryList().addFunctionLibrary(CPPFunctionSet.getInstance());
        }
//#endif
    }


    /**
     * default Constructor to initialise XsltProcessor. Assume no license file available
     */
    public XsltProcessor() {
        super();
        if (debug) {
            System.err.println("XsltProcessor constructor2(), Processor: " + System.identityHashCode(processor));
        }
    }


    /**
     * Constructor to initialise XsltProcessor with license flag
     *
     * @param license - flag indicating presence of license file
     */
    public XsltProcessor(boolean license) {
        processor = new Processor(license);
        Configuration config = processor.getUnderlyingConfiguration();
        schemaAware = config.isLicensedFeature(Configuration.LicenseFeature.ENTERPRISE_XSLT);
        if (debug) {
            System.err.println("XsltProcessor3(l), Processor: " + System.identityHashCode(processor));
        }
//#if EE==true || PE==true
        if(config.isLicensedFeature(Configuration.LicenseFeature.PROFESSIONAL_EDITION)) {
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
     * @param proc    - s9api processor
     * @return XsltProcessor
     */
    public static XsltProcessor newInstance(Processor proc) {
        return new XsltProcessor(proc);
    }

    // deprecated method
    public XdmNode[] getXslMessages() {
        return null;//xslMessages.toArray(new XdmNode[xslMessages.size()]);
    }

    public MessageListener newMessageListener(String cwd, String mode)  throws SaxonApiException {
        return new MyMessageListener(cwd, mode);
    }

    public class MyMessageListener implements MessageListener {

        int mode = 0;
        String messageFilename = null;
        PrintWriter out = null;

        public MyMessageListener(String cwd, String mode) throws SaxonApiException {
            if(mode.equals("on")) {
                this.mode = 1;

            } else if(mode.equals("off")) {
                this.mode = 0;
            } else {
                 this.mode = 2;
                 messageFilename = mode;
                 try {
                     FileWriter fw = new FileWriter(messageFilename, true);
                     BufferedWriter bw = new BufferedWriter(fw);
                     out = new PrintWriter(bw);

                 } catch (IOException e) {
                     throw new SaxonApiException(e.getMessage());
                 }
                //treat option as file name
            }
        }

        public void message(XdmNode content, boolean terminate, SourceLocator locator) {
            if(mode == 1) {
                System.err.println(content.getStringValue());
            }  else if( mode == 2) {
                 if(out != null) {
                     out.println(content);
                 } else {
                     System.err.println("Error could not write message to file:"+messageFilename+" xsl:mesage="+content.getStringValue());
                 }

            }
            //xslMessages.add(content);
        }
    }

    /**
     * Compile package from source file and save resulting sef to file store.
     *
     * @param cwd      - current working directory
     * @param xsl - File name of the stylesheet
     * @param outFilename - the file to which the compiled package should be saved
     */
    public void compileFromFileAndSave(String cwd, String xsl, String outFilename){
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

    public void setJustInTimeCompilation(boolean jit){
        jitCompilation = jit;
    }


    /**
     * Compile package from string and save resulting sef to file store.
     *
     * @param cwd      - current working directory
     * @param str - File name of the stylsheet
     * @param filename - the file to which the compiled package should be saved
     */
    public void compileFromStringAndSave(String cwd, String str, String filename){

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
     * @param obj -
     * @param filename - the file to which the compiled package should be saved
     */
    public void compileFromXdmNodeAndSave(String cwd, Object obj, String filename) throws SaxonApiException {

        XsltCompiler compiler = processor.newXsltCompiler();
        XdmNode node;
        if (obj instanceof XdmNode) {
            node = (XdmNode) obj;
        } else {
            SaxonApiException ex  = new SaxonApiException("Failed to create Stylesheet from XdoNode");
            throw ex;
        }

        XsltPackage pack = compiler.compilePackage(node.asSource());
        File file = new File(filename);
        pack.save(file);

    }




    /**
     * Compile the stylesheet from file  for use later
     *
     * @param cwd      - current working directory
     * @param filename - File name of the stylsheet
     * @return XsltExecutable
     */
    public XsltExecutable createStylesheetFromFile(String cwd, String filename) throws SaxonApiException {

            XsltCompiler compiler = processor.newXsltCompiler();
            if(jitCompilation) {
                compiler.setJustInTimeCompilation(jitCompilation);
            }
            executable = null;
            if(packagesToLoad.size()>0) {
                compilePackages(compiler);
            }
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
     * @return XsltExecutable
     */
    public XsltExecutable createStylesheetFromString(String cwd, String str) throws SaxonApiException {
        XsltCompiler compiler = processor.newXsltCompiler();
        if(jitCompilation) {
            compiler.setJustInTimeCompilation(jitCompilation);
        }
        Source source;
        executable = null;

        StringReader reader = new StringReader(str);
        compiler.setSchemaAware(schemaAware);
        source = new StreamSource(reader);
        if (cwd != null && cwd.length() > 0) {
            if (!cwd.endsWith("/")) {
                cwd = cwd.concat("/");
            }
            source.setSystemId(cwd + "file");

        }

        //compiler.setErrorListener(errorListener);
        if(packagesToLoad.size()>0) {
            compilePackages(compiler);
        }
        executable = compiler.compile(source);
        return executable;


    }

    /**
     * Compile the stylesheet from string  for use later
     *
     * @param cwd - current working directory
     * @param obj - stylesheet as a XdmNode object
     * @return XsltExecutable
     */
    public XsltExecutable createStylesheetFromXdmNode(String cwd, Object obj) throws SaxonApiException {
        executable = null;
        XsltCompiler compiler = processor.newXsltCompiler();
        if(jitCompilation) {
            compiler.setJustInTimeCompilation(jitCompilation);
        }

        XdmNode node;
        if (obj instanceof XdmNode) {
            node = (XdmNode) obj;
        } else {
            SaxonApiException ex  = new SaxonApiException("Failed to create Stylesheet from XdoNode");
            throw ex;
        }

        //compiler.setErrorListener(errorListener);
        if(packagesToLoad.size()>0) {
            compilePackages(compiler);
        }
        executable = compiler.compile(node.asSource());
        return executable;

    }


    /**
     * Do transformation. Save result to file
     * This method is designed to be used with the createStylesheet[File/String] methods above,
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
    public void transformToFile(String cwd, String sourceFilename, String stylesheet, String outFilename, String[] params, Object[] values) throws SaxonApiException {
        try {
            Serializer serializer = null;
            Source source;
            XsltTransformer transformer = null;
            if (stylesheet == null && executable != null) {
                transformer = executable.load();
            } else {
                XsltCompiler compiler = processor.newXsltCompiler();
                if(jitCompilation) {
                    compiler.setJustInTimeCompilation(jitCompilation);
                }
                source = resolveFileToSource(cwd, stylesheet);
                //compiler.setErrorListener(errorListener);

                compiler.setSchemaAware(schemaAware);
                if(packagesToLoad.size()>0) {
                    compilePackages(compiler);
                }
                transformer = compiler.compile(source).load();

            }
            if (outFilename != null) {
                serializer = resolveOutputFile(processor, cwd, outFilename);
            }
            applyXsltTransformerProperties(this, cwd, processor, transformer, serializer, params, values);
            if (sourceFilename != null) {
                source = resolveFileToSource(cwd, sourceFilename);

                transformer.setSource(source);
            }
            if (serializer == null) {
                SaxonCException ex = new SaxonCException("Output file not set for this transformation");
            }
            transformer.setDestination(serializer);
            transformer.transform();
            serializer = null;

        } catch (NullPointerException ex) {
            SaxonApiException ex2 = new SaxonApiException(ex);
            throw ex2;
        }
    }


    /**
     * Applies the properties and parameters required in the transformation.
     * In addition we can supply the source, stylesheet and output file names.
     * We can also supply values to xsl:param and xsl:variables required in the stylesheet.
     * The parameter names and values are supplied as a two arrays in the form of a key and value.
     *  @param cwd         - current working directory
     * @param processor   - required to use the same processor as for the compiled stylesheet
     * @param transformer - pass the current object to set local variables supplied in the parameters
     * @param serializer
     * @param params      - parameters and property names given as an array of stings
     * @param values      - the values of the parameters and properties. given as a array of Java objects
     */
    public static void applyXsltTransformerProperties(SaxonCAPI api, String cwd, Processor processor, XsltTransformer transformer, Serializer serializer, String[] params, Object[] values) throws SaxonApiException {
        if (params != null) {
            String initialTemplate;
            String initialMode;
            XdmNode node;
            String outfile = null;
            Source source;
            DocumentBuilder builder = processor.newDocumentBuilder();
            Map<Serializer.Property, String> propsList = new HashMap<Serializer.Property, String>();
            if (params.length != values.length) {
                throw new SaxonCException("Length of params array not equal to the length of values array");

            }
            if (params.length != 0) {
                if (cwd != null && cwd.length() > 0) {
                    if (!cwd.endsWith("/")) {
                        cwd = cwd.concat("/");
                    }
                }
                for (int i = 0; i < params.length; i++) {
                    if (values[i] == null && !params[i].equals("it")) {
                        throw new SaxonCException("Null parameter/property value found " + (params[i] != null ? "Check name: " + params[i] : ""));

                    }
                    if (debug) {
                        System.err.println("parameter name:" + params[i]);
                        System.err.println("parameter length:" + params[i].length());
                    }
                    if (params[i].startsWith("!")) {
                        String name = params[i].substring(1);
                        Serializer.Property prop = Serializer.Property.get(name);
                        if (prop == null) {
                            throw new SaxonApiException("Property name " + name + " not found");
                        }
                        propsList.put(prop, (String) values[i]);
                    } else if (params[i].startsWith("--") && values[i] != null) {
                        try {
                            processor.setConfigurationProperty("http://saxon.sf.net/feature/" + params[i].substring(2), (String) values[i]);
                        } catch (IllegalArgumentException err) {
                            throw new SaxonCException(err.getMessage());
                        }

                    } else if (params[i].equals("baseoutput")) {
                        if(values[i] instanceof String) {

                            transformer.setBaseOutputURI((String)values[i]);
                        }



                    } else if (params[i].equals("o") && outfile == null) {
                        if (values[i] instanceof String) {
                            outfile = (String) values[i];
                            serializer = api.resolveOutputFile(processor, cwd, outfile);
                            transformer.setDestination(serializer);
                        }
                    } else if (params[i].equals("it")) {
                        if (values[i] instanceof String) {
                            initialTemplate = (String) values[i];
                            transformer.setInitialTemplate(new QName(initialTemplate));
                        } else if (debug) {
                            System.err.println("DEBUG: value error for property 'it'");
                        }
                    } else if (params[i].equals("xsltversion") && values[i] != null) {
                        processor.setConfigurationProperty(FeatureKeys.XSLT_VERSION, values[i]);

                    } else if (params[i].equals("dtd")) {
                        String option = (String) values[i];
                        if (option.equals("on")) {
                            builder.setDTDValidation(true);
                        } else {
                            builder.setDTDValidation(false);
                        }

                    } else if(params[i].equals("extc")){
                        //extension function library path
                        String libName = (String) values[i];
                        SaxonCAPI.setLibrary("",libName);


                    } else if (params[i].equals("im")) {
                        if (values[i] instanceof String) {
                            initialMode = (String) values[i];
                            transformer.setInitialMode(new QName(initialMode));
                        } else if (debug) {
                            System.err.println("DEBUG: value error for property 'im'");
                        }
                    } else if (params[i].equals("s")) {
                        if (values[i] instanceof String) {
                            source = api.resolveFileToSource(cwd, (String) values[i]);
                            transformer.setSource(builder.build(source).asSource());
                        } else if (debug) {
                            System.err.println("DEBUG: value error for property 's'");
                        }
                    } else if (params[i].equals("item") || params[i].equals("node") || params[i].equals("param:node")) {
                        if (debug) {
                            System.err.println("DEBUG: is null value=" + (values[i] == null));
                            if (values[i] != null) {
                                System.err.println("DEBUG: Type of value=" + (values[i]).getClass().getName());

                            }
                            System.err.println("DEBUG: setting the source for node");
                            System.err.println("DEBUG: is value a XdmNode=" + (values[i] instanceof XdmNode));
                            System.err.println("DEBUG: is value a XdmValue=" + (values[i] instanceof XdmValue));

                        }
                        Object value = values[i];
                        if (value instanceof XdmNode) {
                            node = (XdmNode) value;
                            transformer.setSource((node).asSource());
                        } else if (debug) {
                            System.err.println("DEBUG: Type of node Property error.");
                        }
                    } else if (params[i].equals("m")) {
                        String value = (String) values[i];
                        transformer.setMessageListener(((XsltProcessor) api).newMessageListener(cwd, value));

                    }  else if (params[i].startsWith("param:")) {
                        String paramName = params[i].substring(6);
                        Object value = values[i];
                        XdmValue valueForCpp = null;
                        QName qname = QName.fromClarkName(paramName);
                        if (value instanceof XdmValue) {
                            valueForCpp = (XdmValue) value;
                            if (debug) {
                                System.err.println("DEBUG: XSLTTransformerForCpp: " + paramName);
                                System.err.println("DEBUG: XSLTTransformerForCpp: " + valueForCpp.getUnderlyingValue().toString());
                                net.sf.saxon.type.ItemType suppliedItemType = SequenceTool.getItemType(valueForCpp.getUnderlyingValue(), processor.getUnderlyingConfiguration().getTypeHierarchy());
                                System.err.println("DEBUG: XSLTTransformerForCpp: " + valueForCpp.getUnderlyingValue());
                                System.err.println("DEBUG: XSLTTransformerForCpp Type: " + suppliedItemType.toString());
                            }

                        } else if (value instanceof Object[]) {
                            Object[] arr = (Object[]) value;
                            if (debug) {
                                System.err.println("DEBUG: Array of parameters found. arr len=" + arr.length);

                            }
                            List<AtomicValue> valueList = new ArrayList<AtomicValue>();
                            for (int j = 0; j < arr.length; j++) {
                                Object itemi = arr[j];
                                if (itemi == null) {
                                    System.err.println("Error: Null item at " + i + "th position in array of XdmValues");
                                    break;
                                }
                                if (debug) {
                                    System.err.println("Java object:" + itemi);
                                }
                                if (itemi instanceof XdmValue) {
                                    valueList.add((AtomicValue) (((XdmValue) itemi).getUnderlyingValue()));
                                } else {
                                    XdmValue valuex = getXdmValue(itemi);
                                    if (valuex == null) {
                                        System.err.println("Error: Null item at " + i + "th position in array of XdmValues when converting");
                                        break;
                                    }
                                    valueList.add((AtomicValue) (getXdmValue(itemi)).getUnderlyingValue());
                                }
                            }
                            AtomicArray sequence = new AtomicArray(valueList);
                            valueForCpp = XdmValue.wrap(sequence);
                        } else {
                            //fast track for primitive values
                            valueForCpp = getXdmValue(value);
                            if (debug) {
                                System.err.println("DEBUG: primitive value found");
                                net.sf.saxon.type.ItemType suppliedItemType = SequenceTool.getItemType(valueForCpp.getUnderlyingValue(), processor.getUnderlyingConfiguration().getTypeHierarchy());
                                System.err.println("XSLTTransformerForCpp Type: " + suppliedItemType.toString());
                            }
                        }


                        if (qname != null && valueForCpp != null) {
                            transformer.setParameter(qname, valueForCpp);
                        }
                    }

                }

            }
            if (serializer != null) {
                for (Map.Entry pairi : propsList.entrySet()) {
                    serializer.setOutputProperty((Serializer.Property) pairi.getKey(), (String) pairi.getValue());
                }
            }
        }

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
    public XdmNode transformToNode(String cwd, String sourceFile, String stylesheet, String[] params, Object[] values) throws SaxonApiException {
        Source source;
        XsltTransformer transformer = null;

            if (stylesheet == null && executable != null) {
                transformer = executable.load();
            } else {
                XsltCompiler compiler = processor.newXsltCompiler();
                if(jitCompilation) {
                    compiler.setJustInTimeCompilation(jitCompilation);
                }
                source = resolveFileToSource(cwd, stylesheet);
                //compiler.setErrorListener(errorListener);
                if(packagesToLoad.size()>0) {
                    compilePackages(compiler);
                }
                transformer = compiler.compile(source).load();

            }
            XdmDestination destination = new XdmDestination();


            transformer.setDestination(destination);
            this.applyXsltTransformerProperties(this, cwd, processor, transformer, null, params, values);

            if (sourceFile != null) {
                source = resolveFileToSource(cwd, sourceFile);
                transformer.setSource(source);
            }
            transformer.transform();
            return destination.getXdmNode();


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
     * @return result as a string representation as byte array
     */
    public byte[] transformToString(String cwd, String sourceFile, String stylesheet, String[] params, Object[] values) throws SaxonApiException {
        if (debug) {
            System.err.println("xsltApplyStylesheet, Processor: " + System.identityHashCode(processor));
        }
            Source source;

            XsltTransformer transformer = null;
            if (stylesheet == null && executable != null) {
                transformer = executable.load();
            } else if (stylesheet == null) {
                SaxonApiException ex = new SaxonApiException("Stylesheet not found!");
                throw ex;
            } else {
                XsltCompiler compiler = processor.newXsltCompiler();
                if(jitCompilation) {
                    compiler.setJustInTimeCompilation(jitCompilation);
                }
                source = resolveFileToSource(cwd, stylesheet);
                //compiler.setErrorListener(errorListener);
                if(packagesToLoad.size()>0) {
                    compilePackages(compiler);
                }
                XsltExecutable tempExecutable =   compiler.compile(source);
                tempExecutable.getUnderlyingCompiledStylesheet();
                transformer = tempExecutable.load();

            }
            StringWriter sw = new StringWriter();   //TODO byteArrayOutputStream
        ByteArrayOutputStream bStream = new ByteArrayOutputStream();
        Serializer serializer = processor.newSerializer(bStream);
            transformer.setDestination(serializer);

            applyXsltTransformerProperties(this, cwd, processor, transformer, serializer, params, values);

            if (sourceFile != null) {
                source = resolveFileToSource(cwd, sourceFile);
                transformer.setSource(source);
            }
            //transformer.setErrorListener(errorListener);
            transformer.transform();
            serializer = null;
            return bStream.toByteArray();

    }



    /***
     * Compile a library package and link it for use.
     * <p>The source argument identifies an XML file containing an &lt;xsl:package&gt; element. Any packages
     * on which this package depends must have been made available to the <code>XsltCompiler</code>
     * by importing them.</p>
     */
    public void compilePackages(XsltCompiler compiler) throws SaxonApiException {
        try {
            PackageLibrary library = new PackageLibrary(compiler.getUnderlyingCompilerInfo(), packagesToLoad);
            compiler.getUnderlyingCompilerInfo().setPackageLibrary(library);
        } catch (XPathException e) {
            throw new SaxonApiException(e);
        }


    }


    /***
     * File names to XsltPackages stored on filestore are added to a set of packages, which
     * will be imported later when compiling
     * @param cwd current working directory
     * @param packs array of file names of XSLT packages stored in filestore
     */
    public void addPackages(String cwd, String [] packs){
        File filei;
          for(int i=0;i<packs.length;i++){
              try {
                  filei = resolveFile(cwd, packs[i]);
              } catch (SaxonApiException e) {
                  System.err.println("Java: Failure in adding packages "+ e.getMessage());
                  continue;
              }
              packagesToLoad.add(filei);
          }
    }


    /***
     * Clear set of packages.
     */
    public void clearPackages(){
        packagesToLoad.clear();

    }





    public static void main(String[] args) throws Exception {
        String cwd2 = "/Users/ond1/work/development/svn/archive/opensource/latest9.8/hec/samples/php/trax";
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
        XsltProcessor cpp = new XsltProcessor(processor);
        //cpp.createStylesheetFromFile(cwd2, "xsl/foo.xsl");
        cpp.compileFromFileAndSave(cwd2, "xsl/foo.xsl", "xsl/foo.xslp");
        String resultStr = new String(cpp.transformToString(cwd2, "xml/foo.xml", "xsl/foo.xslp", null, null));
        System.out.println(resultStr);

         String resultStr4 = new String(cpp.transformToString(cwd2, "xml/foo.xml", "xsl/fooExFunc.xsl", null, null));
               System.out.println(resultStr4);

        String [] params0 = {"it"};
        Object [] values0 = {null};
        String resultStr3 = new String(cpp.transformToString(cwd2, null, "xsl/foo4.xsl",params0, values0));
                System.out.println("Using initial-template: " + resultStr3);

        XdmNode node2 = cpp.parseXmlFile("/Users/ond1/work/development/campos", "ORP0301177AA__EE__30954_sinsello.xml");
        String[] paramsx = {"node"};
        Object[] valuesx = {node2};
        String result2 = new String(cpp.transformToString("/Users/ond1/work/development/campos", "ORP0301177AA__EE__30954_sinsello.xml", "campos.xsl", paramsx, valuesx));
        Object[] arrValues = {2, "test"};

        String[] params1 = {"resources", "param:test1", "node", "m", "xmlversion"};
        Object[] values1 = {"/Users/ond1/work/development/tests/jeroen/data", arrValues, node2, "m", "1.1"};
        String outputdoc = new String(cpp.transformToString(cwd, null, stylesheet12, params1, values1));
        // System.out.println(outputdoc);
        // System.exit(0);
        // Processor processor = cpp.getProcessor();
        // XsltTransformer transformer = cpp.xsltParseStylesheetFile(args[0]).load();
        //XdmNode sourceNode = cpp.xmlParseFile(cwd, "xml/foo.xml");
        XdmNode sourceNode2 = cpp.parseXmlString(  "<result><assert-xml file=\"type-0501.out\"/></result>");
        XdmValue node1 = (XdmValue) cpp.createXdmAtomicItem("string", "textXXXXX");

        XdmValue resultNode2 = cpp.parseXmlString("<?xml version=\"1.0\" encoding=\"UTF-8\"?><html><head><title>Untitled</title></head><body leftmargin=\"100\"></body></html>");

        XdmValue value1 = SaxonCAPI.createXdmAtomicItem("string", "good bye");
        String[] params2 = {"o"};
        Object[] values2 = {"output_test.xml"};
        String[] params3 = {"node", "!indent", "output_test.xml", "xmlversion"};
        String[]param4 = {"s", "param:a-param"};
        Object[] values4 = {"xml/foo.xml", value1};
        Object[] values3 = {sourceNode2, "yes", "o", "1.0"};
        cpp.createStylesheetFromFile(cwd, stylesheet12);


        String result = "";
        int repeat = 1;
        try {
            cpp.createStylesheetFromString("samples", "<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"\n" +
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
                    "</xsl:stylesheet>");

            cpp.createStylesheetFromString("samples", "<xsl:stylesheet \n" +
                    "    xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" \n" +
                    "    xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" \n" +
                    "    version=\"2.0\">\n" +
                    "\n" +
                    "    <xslt:output method=\"xml\" encoding=\"UTF-8\"/>\n" +
                    "\n" +
                    "    <xsl:template match=\"/\">        \n" +
                    "        <xsl:call-template name=\"test\">\n" +
                    "          <xsl:with-param name=\"xxx\" select=\"'test'\"/>\n" +
                    "        </xsl:call-template>\n" +
                    "    </xsl:template>\n" +
                    "\n" +
                    "    <xsl:template name='test'>\n" +
                    "        <xsl:param name=\"xxx\" tunnel=\"yes\" />\n" +
                    "        <p>xxx: <xsl:value-of select=\"$xxx\"/></p>\n" +
                    "    </xsl:template>\n" +
                    "\n" +
                    "</xsl:stylesheet>");

            String valueStr = new String(cpp.transformToString(cwd, "categories.xml", null, null, null));
            if (valueStr != null) {
                System.out.println("Output = " + valueStr);
            } else {
                System.out.println("valueSt is null");
            }

        }catch(SaxonCException ex) {}

        try {

            String resultStr2 = new String(cpp.transformToString(cwd2, null, null, param4, values4), StandardCharsets.UTF_8);

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
            ex.printStackTrace();
        }




    }


}
