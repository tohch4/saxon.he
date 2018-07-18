////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.serialize;

import net.sf.saxon.expr.parser.Location;
import net.sf.saxon.ma.arrays.ArrayItem;
import net.sf.saxon.ma.map.MapItem;
import net.sf.saxon.event.SequenceReceiver;
import net.sf.saxon.lib.SaxonOutputKeys;
import net.sf.saxon.om.*;
import net.sf.saxon.serialize.charcode.CharacterSet;
import net.sf.saxon.serialize.charcode.UTF16CharacterSet;
import net.sf.saxon.serialize.charcode.UTF8CharacterSet;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.type.Type;
import net.sf.saxon.value.AtomicValue;
import net.sf.saxon.value.ObjectValue;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Result;
import javax.xml.transform.stream.StreamResult;
import java.io.*;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.Properties;


/**
 * Emitter: This abstract class defines methods that must be implemented by
 * components that format SAXON output. There is one emitter for XML,
 * one for HTML, and so on. Additional methods are concerned with
 * setting options and providing a Writer.<p>
 * <p/>
 * The interface is deliberately designed to be as close as possible to the
 * standard SAX2 ContentHandler interface, however, it allows additional
 * information to be made available.
 * <p/>
 * An Emitter is a Receiver, specifically it is a Receiver that can direct output
 * to a Writer or OutputStream, using serialization properties defined in a Properties
 * object.
 */

public abstract class Emitter extends SequenceReceiver implements Result {
    protected StreamResult streamResult;
    protected Writer writer;
    protected OutputStream outputStream;
    protected Properties outputProperties;
    protected CharacterSet characterSet;
    protected boolean allCharactersEncodable = false;
    private boolean mustClose = false;

    public Emitter() {
        super(null);
    }

    /**
     * Set output properties
     *
     * @param details the output serialization properties
     * @throws net.sf.saxon.trans.XPathException
     *          if an error occurs finding the encoding property
     */

    public void setOutputProperties(Properties details) throws XPathException {
        if (characterSet == null) {
            characterSet = getConfiguration().getCharacterSetFactory().getCharacterSet(details);
            allCharactersEncodable = (characterSet instanceof UTF8CharacterSet ||
                    characterSet instanceof UTF16CharacterSet);
        }
        outputProperties = details;
    }

    /**
     * Get the output properties
     *
     * @return the output serialization properties. The returned value will be null if setOutputProperties()
     *         has not been called
     */

    public Properties getOutputProperties() {
        return outputProperties;
    }

    /**
     * Set the StreamResult acting as the output destination of the Emitter
     *
     * @param result the output destination
     * @throws net.sf.saxon.trans.XPathException
     *          if an error occurs
     */

    public void setStreamResult(StreamResult result) throws XPathException {
        streamResult = result;
        if (systemId == null) {
            systemId = result.getSystemId();
        }
    }

    /**
     * Make a Writer for this Emitter to use, given a StreamResult.
     *
     * @throws net.sf.saxon.trans.XPathException
     *          if an error occurs
     */

    protected void makeWriter() throws XPathException {
        if (writer != null) {
            return;
        }
        if (streamResult == null) {
            throw new IllegalStateException("Emitter must have either a Writer or a StreamResult to write to");
        }
        writer = streamResult.getWriter();
        if (writer == null) {
            OutputStream os = streamResult.getOutputStream();
            if (os != null) {
                setOutputStream(os);
            }
        }
        if (writer == null) {
            makeOutputStream();
        }
    }

    @SuppressWarnings({"ResultOfMethodCallIgnored"})
    protected OutputStream makeOutputStream() throws XPathException {
        String uriString = streamResult.getSystemId();
        if (uriString == null) {
            throw new XPathException("Result has no system ID, writer, or output stream defined");
        }

        try {
            URI uri = new URI(uriString);
            if (!uri.isAbsolute()) {
                try {
                    uri = new File(uriString).getAbsoluteFile().toURI();
                } catch (Exception e) {
                    // if we fail, we'll get another exception
                }
            }
            File file = new File(uri);
            try {
                if ("file".equals(uri.getScheme()) && !file.exists()) {
                    File directory = file.getParentFile();
                    if (directory != null && !directory.exists()) {
                        directory.mkdirs();
                    }
                    file.createNewFile();
                }
            } catch (IOException err) {
                throw new XPathException("Failed to create output file " + uri, err);
            }
            setOutputStream(new FileOutputStream(file));
            // Set the outputstream in the StreamResult object so that the
            // call on OutputURIResolver.close() can close it
            streamResult.setOutputStream(outputStream);
            mustClose = true;
        } catch (FileNotFoundException fnf) {
            throw new XPathException(fnf);
        } catch (URISyntaxException use) {
            throw new XPathException(use);
        } catch (IllegalArgumentException iae) {
            // for example, the system ID doesn't use the file: scheme
            throw new XPathException(iae);
        }
        return outputStream;
    }

    /**
     * Determine whether the Emitter wants a Writer for character output or
     * an OutputStream for binary output. The standard Emitters all use a Writer, so
     * this returns true; but a subclass can override this if it wants to use an OutputStream
     *
     * @return true if a Writer is needed, as distinct from an OutputStream
     */

    public boolean usesWriter() {
        return true;
    }

    /**
     * Set the output destination as a character stream
     *
     * @param writer the Writer to use as an output destination
     * @throws net.sf.saxon.trans.XPathException
     *          if an error occurs
     */

    public void setWriter(Writer writer) throws XPathException {
        this.writer = writer;

        // If the writer uses a known encoding, change the encoding in the XML declaration
        // to match. Any encoding actually specified in xsl:output is ignored, because encoding
        // is being done by the user-supplied Writer, and not by Saxon itself.

        if (writer instanceof OutputStreamWriter && outputProperties != null) {
            String enc = ((OutputStreamWriter) writer).getEncoding();
            outputProperties.setProperty(OutputKeys.ENCODING, enc);
            characterSet = getConfiguration().getCharacterSetFactory().getCharacterSet(outputProperties);
            allCharactersEncodable = (characterSet instanceof UTF8CharacterSet ||
                    characterSet instanceof UTF16CharacterSet);
        }
    }

    /**
     * Get the output writer
     *
     * @return the Writer being used as an output destination, if any
     */

    public Writer getWriter() {
        return writer;
    }

    /**
     * Set the output destination as a byte stream.
     * <p>Note that if a specific encoding (other than the default, UTF-8) is required, then
     * {@link #setOutputProperties(java.util.Properties)} must be called <i>before</i> calling
     * this method.</p>
     *
     * @param stream the OutputStream being used as an output destination
     * @throws net.sf.saxon.trans.XPathException
     *          if an error occurs
     */

    public void setOutputStream(OutputStream stream) throws XPathException {
        outputStream = stream;

        // If the user supplied an OutputStream, but the Emitter is written to
        // use a Writer (this is the most common case), then we create a Writer
        // to wrap the supplied OutputStream; the complications are to ensure that
        // the character encoding is correct.

        if (usesWriter()) {

            if (outputProperties == null) {
                outputProperties = new Properties();
            }

            String encoding = outputProperties.getProperty(OutputKeys.ENCODING);
            if (encoding == null) {
                encoding = "UTF8";
                allCharactersEncodable = true;
            } else if (encoding.equalsIgnoreCase("UTF-8")) {
                encoding = "UTF8";
                allCharactersEncodable = true;
            } else if (encoding.equalsIgnoreCase("UTF-16")) {
                encoding = "UTF16";
            }

            if (characterSet == null) {
                characterSet = getConfiguration().getCharacterSetFactory().getCharacterSet(outputProperties);
            }

            String byteOrderMark = outputProperties.getProperty(SaxonOutputKeys.BYTE_ORDER_MARK);
            if ("no".equals(byteOrderMark) && "UTF16".equals(encoding)) {
                // Java always writes a bom for UTF-16, so if the user doesn't want one, use utf16-be
                encoding = "UTF-16BE";
            } else if (!(characterSet instanceof UTF8CharacterSet)) {

                //if (characterSet instanceof PluggableCharacterSet) {
                encoding = characterSet.getCanonicalName();
            }

            while (true) {
                try {
                    String javaEncoding = encoding;
                    if (encoding.equalsIgnoreCase("iso-646") || encoding.equalsIgnoreCase("iso646")) {
                        javaEncoding = "US-ASCII";
                    }
                    if (encoding.equalsIgnoreCase("UTF8")) {
                        writer = new UTF8Writer(outputStream);
                    } else {
                        writer = new BufferedWriter(
                                new OutputStreamWriter(
                                        outputStream, javaEncoding));
                    }
                    break;
                } catch (Exception err) {
                    if (encoding.equalsIgnoreCase("UTF8")) {
                        throw new XPathException("Failed to create a UTF8 output writer");
                    }
                    XPathException de = new XPathException("Encoding " + encoding + " is not supported: using UTF8");
                    de.setErrorCode("SESU0007");
                    getPipelineConfiguration().getErrorListener().error(de);
                    encoding = "UTF8";
                    characterSet = UTF8CharacterSet.getInstance();
                    allCharactersEncodable = true;
                    outputProperties.setProperty(OutputKeys.ENCODING, "UTF-8");
                }
            }
        }

    }

    /**
     * Get the output stream
     *
     * @return the OutputStream being used as an output destination, if any
     */

    public OutputStream getOutputStream() {
        return outputStream;
    }

    /**
     * Set unparsed entity URI. Needed to satisfy the Receiver interface, but not used,
     * because unparsed entities can occur only in input documents, not in output documents.
     *
     * @param name     the entity name
     * @param uri      the entity system ID
     * @param publicId the entity public ID
     */

    public void setUnparsedEntity(String name, String uri, String publicId) throws XPathException {
    }

    /**
     * Notify the end of the event stream
     */

    public void close() throws XPathException {
        if (mustClose && outputStream != null) {
            try {
                outputStream.close();
            } catch (IOException e) {
                throw new XPathException("Failed to close output stream");
            }
        }
    }

    /**
     * Ask whether this Receiver (or the downstream pipeline) makes any use of the type annotations
     * supplied on element and attribute events
     *
     * @return true if the Receiver makes any use of this information. If false, the caller
     *         may supply untyped nodes instead of supplying the type annotation (or conversely, it may
     *         avoid stripping unwanted type annotations)
     */
    public boolean usesTypeAnnotations() {
        return false;
    }

    /**
     * Append an arbitrary item (node or atomic value) to the output
     */

    public void append(/*@Nullable*/ Item item, Location locationId, int copyNamespaces) throws XPathException {
        if (item != null) {
            if (item instanceof AtomicValue || item instanceof ObjectValue) {
                if (previousAtomic) {
                    characters(" ", locationId, 0);
                }
                characters(item.getStringValueCS(), locationId, 0);
                previousAtomic = true;
            } else if (item instanceof Function) {
                String what = "a function item";
                if (item instanceof MapItem) {
                    what = "a map";
                } else if (item instanceof ArrayItem) {
                    what = "an array";
                }
                throw new XPathException("Cannot serialize " +
                                                 what + " using the " + getOutputProperties().getProperty("method") + " method", "SENR0001");
            } else if (((NodeInfo) item).getNodeKind() == Type.DOCUMENT) {
                startDocument(0); // needed to ensure that illegal namespaces or attributes in the content are caught
                SequenceIterator iter = ((NodeInfo) item).iterateAxis(AxisInfo.CHILD);
                Item it;
                while ((it = iter.next()) != null) {
                    append(it, locationId, copyNamespaces);
                }
                previousAtomic = false;
                endDocument();
            } else {
                int copyOptions = CopyOptions.TYPE_ANNOTATIONS;
                if (copyNamespaces == NodeInfo.LOCAL_NAMESPACES) {
                    copyOptions |= CopyOptions.LOCAL_NAMESPACES;
                } else if (copyNamespaces == NodeInfo.ALL_NAMESPACES) {
                    copyOptions |= CopyOptions.ALL_NAMESPACES;
                }
                ((NodeInfo) item).copy(this, copyOptions, locationId);
                previousAtomic = false;
            }
        }
    }

}

