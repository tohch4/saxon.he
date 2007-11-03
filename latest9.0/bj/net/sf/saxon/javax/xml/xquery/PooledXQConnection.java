package net.sf.saxon.javax.xml.xquery;

/**
 * XQJ interfaces reconstructed from version 0.9 documentation
 */
public interface PooledXQConnection {

    void addConnectionEventListener(XQConnectionEventListener listener);

    void close() throws XQException;

    XQConnection getConnection() throws XQException;

    void removeConnectionEventListener(XQConnectionEventListener listener);
}


//
// This interface definition is transcribed from the Public Draft Specification (version 0.9)
// of the XQuery API for Java (XQJ) 1.0 Specification, available at
// http://jcp.org/aboutJava/communityprocess/pr/jsr225/index.html
//
// Copyright 2003, 2006, 2007 Oracle. All rights reserved.
// For licensing conditions, see the above specification
//
