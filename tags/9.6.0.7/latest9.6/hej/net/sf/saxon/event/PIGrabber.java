////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 Saxonica Limited.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
// This Source Code Form is "Incompatible With Secondary Licenses", as defined by the Mozilla Public License, v. 2.0.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

package net.sf.saxon.event;

import net.sf.saxon.Configuration;
import net.sf.saxon.lib.StandardURIResolver;
import net.sf.saxon.om.NodeName;
import net.sf.saxon.trans.XPathException;
import net.sf.saxon.tree.util.ProcInstParser;
import net.sf.saxon.type.SchemaType;

import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.sax.SAXSource;
import java.util.ArrayList;
import java.util.List;

/**
 * The PIGrabber class is a Receiver that looks for xml-stylesheet processing
 * instructions and tests whether they match specified criteria; for those that do, it creates
 * an InputSource object referring to the relevant stylesheet
 *
 * @author Michael H. Kay
 */

public class PIGrabber extends ProxyReceiver {

    private Configuration config = null;
    private String reqMedia = null;
    private String reqTitle = null;
    private String baseURI = null;
    private URIResolver uriResolver = null;
    private List<String> stylesheets = new ArrayList<String>();
    private boolean terminated = false;

    public PIGrabber(Receiver next) {
        super(next);
    }

    public void setFactory(Configuration config) {
        this.config = config;
    }

    public void setCriteria(String media, String title) {
        this.reqMedia = media;
        this.reqTitle = title;
    }

    /**
     * Set the base URI
     *
     * @param uri the base URI
     */

    public void setBaseURI(String uri) {
        baseURI = uri;
    }

    /**
     * Set the URI resolver to be used for the href attribute
     *
     * @param resolver the URI resolver
     */

    public void setURIResolver(URIResolver resolver) {
        uriResolver = resolver;
    }

    /**
     * Abort the parse when the first start element tag is found
     */

    public void startElement(NodeName namecode, SchemaType typecode, int locationId, int properties)
            throws XPathException {
        terminated = true;
        // abort the parse when the first start element tag is found
        throw new XPathException("#start#");
    }

    /**
     * Determine whether the parse terminated because the first start element tag was found
     *
     * @return true if the parse was terminated when the document element was encountered (as distinct
     *         from being terminated because of some exception condition, for example a parse error)
     */

    public boolean isTerminated() {
        return terminated;
    }

    /**
     * Handle xml-stylesheet PI
     */

    public void processingInstruction(String target, CharSequence data, int locationId, int properties)
            throws XPathException {
        if (target.equals("xml-stylesheet")) {

            String value = data.toString();
            String piMedia = ProcInstParser.getPseudoAttribute(value, "media");
            String piTitle = ProcInstParser.getPseudoAttribute(value, "title");
            String piType = ProcInstParser.getPseudoAttribute(value, "type");
            String piAlternate = ProcInstParser.getPseudoAttribute(value, "alternate");

            if (piType == null) return;

            // System.err.println("Found xml-stylesheet media=" + piMedia + " title=" + piTitle);

            if ((piType.equals("text/xml") || piType.equals("application/xml") ||
                    piType.equals("text/xsl") || piType.equals("applicaton/xsl") || piType.equals("application/xml+xslt")) &&

                    (reqMedia == null || piMedia == null || reqMedia.equals(piMedia)) &&

                    ((piTitle == null && (piAlternate == null || piAlternate.equals("no"))) ||
                            (reqTitle == null) ||
                            (piTitle != null && piTitle.equals(reqTitle)))) {
                String href = ProcInstParser.getPseudoAttribute(value, "href");
                if (href == null) {
                    throw new XPathException("xml-stylesheet PI has no href attribute");
                }

                // System.err.println("Adding " + href);
                if (piTitle == null && (piAlternate == null || piAlternate.equals("no"))) {
                    stylesheets.add(0, href);
                } else {
                    stylesheets.add(href);
                }
            } else {
                //System.err.println("No match on required media=" + reqMedia + " title=" + reqTitle );
            }
        }
    }

    /**
     * Return list of stylesheets that matched, as an array of Source objects
     *
     * @return null if there were no matching stylesheets.
     * @throws net.sf.saxon.trans.XPathException
     *          if a URI cannot be resolved
     */

    /*@Nullable*/
    public Source[] getAssociatedStylesheets() throws TransformerException {
        if (stylesheets.size() == 0) {
            return null;
        }
        if (uriResolver == null) {
            uriResolver = new StandardURIResolver(config);
        }
        Source[] result = new Source[stylesheets.size()];
        for (int i = 0; i < stylesheets.size(); i++) {
            String href = stylesheets.get(i);
            Source s = uriResolver.resolve(href, baseURI);
            if (s instanceof SAXSource) {
                ((SAXSource) s).setXMLReader(config.getStyleParser());
            }
            if (s == null) {
                s = config.getSystemURIResolver().resolve(href, baseURI);
            }
            result[i] = s;
        }
        return result;
    }

}//