package net.sf.saxon.option.cpp;

import net.sf.saxon.s9api.MessageListener2;
import net.sf.saxon.s9api.QName;
import net.sf.saxon.s9api.SaxonApiException;
import net.sf.saxon.s9api.XdmNode;

import javax.xml.transform.SourceLocator;
import java.io.*;
import java.util.ArrayList;
import java.util.List;

public class SaxonCMessageListener implements MessageListener2 {


    int mode = 0;
    String messageFilename = null;
    File file = null;
    List<XdmNode> xslMessages = new ArrayList<>();

    public SaxonCMessageListener(String cwd, String mode) throws SaxonApiException {
        if (mode.equals("-:on")) {
            this.mode = 1;

        } else if (mode.equals("-:off")) {
            this.mode = 0;
        }  else {
            this.mode = 2;
            messageFilename = cwd + mode.substring(1);
            file = SaxonCAPI.resolveFile(cwd, messageFilename);

        }
    }


    @Override
    public void message(XdmNode content, QName errorCode, boolean terminate, SourceLocator locator) {
        if(mode == 0 ) {
            return;
        } else if (mode == 1) {
            xslMessages.add(content);
            System.err.println(content.getStringValue());
        } else if (mode == 2) {
            xslMessages.add(content);
            try (FileWriter fw = new FileWriter(file, true);
                 BufferedWriter bw = new BufferedWriter(fw);
                 PrintWriter out = new PrintWriter(bw)) {
                out.println(content.getStringValue());

            } catch (IOException e) {
                System.err.println("Error could not write message to file:" + messageFilename + " xsl:mesage=" + content.getStringValue());
            }

        }
    }


    /**
     * Get the xsl:messages  as an array of XdmNode objects
     * @return array of XdmNode objects
     */
    public XdmNode[] getXslMessages() {
        if(xslMessages == null) {
            return null;
        } else {
            return xslMessages.toArray(new XdmNode[xslMessages.size()]);
        }
    }


}
