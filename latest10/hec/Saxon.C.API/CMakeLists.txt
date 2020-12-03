cmake_minimum_required(VERSION 3.17)
project(Saxon_C_API)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)
include_directories(jni)
include_directories(jni/unix)
include_directories(jni/win32)

add_executable(Saxon_C_API
        jni/unix/jni_md.h
        jni/win32/jni_md.h
        jni/jni.h
        SaxonApiException.cpp
        SaxonApiException.h
        SaxonCGlue.c
        SaxonCGlue.h
        SaxonCProcessor.c
        SaxonCProcessor.h
        SaxonCXPath.c
        SaxonCXPath.h
        SaxonProcessor.cpp
        SaxonProcessor.h
        SchemaValidator.cpp
        SchemaValidator.h
        XdmArray.h
        XdmAtomicValue.cpp
        XdmAtomicValue.h
        XdmFunctionItem.cpp
        XdmFunctionItem.h
        XdmArray.cpp
        XdmArray.h
        XdmItem.cpp
        XdmItem.h
        XdmMap.cpp
        XdmMap.h
        XdmNode.cpp
        XdmNode.h
        XdmValue.cpp
        XdmValue.h
        XPathProcessor.cpp
        XPathProcessor.h
        XQueryProcessor.cpp
        XQueryProcessor.h
        Xslt30Processor.cpp
        Xslt30Processor.h
        XsltExecutable.cpp
        XsltExecutable.h
        XsltProcessor.cpp
        XsltProcessor.h)
