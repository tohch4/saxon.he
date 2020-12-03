// XsltExecutable.cpp : Defines the exported functions for the DLL application.
//

#include "XsltExecutable.h"
#include "XdmValue.h"
#include "XdmItem.h"
#include "XdmNode.h"
#include "XdmFunctionItem.h"
#include "XdmMap.h"
#include "XdmArray.h"
#include "XdmAtomicValue.h"

#ifdef DEBUG
#include <typeinfo> //used for testing only
#endif


XsltExecutable::XsltExecutable(jobject exObject, std::string curr) {



    /*
     * Look for class.
     */
    cppClass = lookForClass(SaxonProcessor::sxn_environ->env,
                            "net/sf/saxon/option/cpp/Xslt30Processor");

    messageListenerClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                               "net/sf/saxon/option/cpp/SaxonCMessageListener");

#ifdef DEBUG
    jmethodID debugMID = SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass, "setDebugMode", "(Z)V");
    SaxonProcessor::sxn_environ->env->CallStaticVoidMethod(cppClass, debugMID, (jboolean)true);

#endif
    tunnel = false;
    selection = nullptr;
    selectionV = nullptr;
    saxonMessageListenerObj = nullptr;
    executableObject = exObject;
    cwdXE = curr;
}

XsltExecutable::~XsltExecutable() {
    clearProperties();
    clearParameters();
    if (selectionV != nullptr) {
        selectionV->decrementRefCount();
        if (selectionV->getRefCount() == 0) {
            delete selectionV;
        }
    }
    SaxonProcessor::sxn_environ->env->DeleteGlobalRef(executableObject);

}


XsltExecutable::XsltExecutable(const XsltExecutable &other) {

    /*
     * Look for class.
     */
    cppClass = lookForClass(SaxonProcessor::sxn_environ->env,
                            "net/sf/saxon/option/cpp/Xslt30Processor");

    executableObject = other.executableObject;
    selectionV = other.selectionV;
    if (selectionV != nullptr) {
        setInitialMatchSelection(other.selectionV);
    } else {
        selection = other.selection;
    }
    tunnel = other.tunnel;

    std::map<std::string, XdmValue *>::const_iterator paramIter = other.parameters.begin();
    while (paramIter != other.parameters.end()) {

        XdmValue *valuei = paramIter->second;
        if (valuei == nullptr) {
            std::cerr << "Error in XsltExecutable copy constructor" << std::endl;
        } else {
            parameters[paramIter->first] = new XdmValue(*(valuei));
        }
        paramIter++;
    }

    std::map<std::string, std::string>::const_iterator propIter = other.properties.begin();
    while (propIter != other.properties.end()) {
        properties[propIter->first] = propIter->second;
        propIter++;
    }

}


bool XsltExecutable::exceptionOccurred() {
    return SaxonProcessor::sxn_environ->env->ExceptionCheck();
}

const char *XsltExecutable::getErrorCode() {
    SaxonApiException *exception = SaxonProcessor::checkForExceptionCPP(SaxonProcessor::sxn_environ->env, cppClass,
                                                                        nullptr);
    if (exception == nullptr) { return nullptr; }
    return exception->getErrorCode();
}


void XsltExecutable::setGlobalContextItem(XdmItem *value) {
    if (value != nullptr) {
        value->incrementRefCount();
        parameters["node"] = value;
    }
}

void XsltExecutable::setGlobalContextFromFile(const char *ifile) {
    if (ifile != nullptr) {
        setProperty("s", ifile);
    }
}

void XsltExecutable::setInitialMatchSelection(XdmValue *_selection) {
    if (_selection != nullptr) {
        _selection->incrementRefCount();
        selectionV = _selection;
        selection = _selection->getUnderlyingValue();
    }
}


void XsltExecutable::setInitialMatchSelectionAsFile(const char *filename) {
    if (filename != nullptr) {
        selection = SaxonProcessor::sxn_environ->env->NewStringUTF(filename);
    }
}

void XsltExecutable::setOutputFile(const char *ofile) {
    setProperty("o", ofile);
}

void XsltExecutable::setBaseOutputURI(const char *baseURI) {
    if (baseURI != nullptr) {
        setProperty("baseoutput", baseURI);
    }
}


void XsltExecutable::setParameter(const char *name, XdmValue *value) {
    if (value != nullptr && name != nullptr) {
        value->incrementRefCount();
        int s = parameters.size();
        std::string skey = "param:" + std::string(name);
        parameters[skey] = value;
        if (s == parameters.size()) {
            std::map<std::string, XdmValue *>::iterator it;
            it = parameters.find(skey);
            if (it != parameters.end()) {
                XdmValue *valuei = it->second;
                valuei->decrementRefCount();
                if (valuei != nullptr && valuei->getRefCount() < 1) {
                    delete value;
                }
                parameters.erase(skey);
                parameters[skey] = value;
            }
        }
    }
}

void XsltExecutable::setInitialTemplateParameters(std::map<std::string, XdmValue *> _itparameters, bool _tunnel) {
    for (std::map<std::string, XdmValue *>::iterator itr = _itparameters.begin(); itr != _itparameters.end(); itr++) {
        parameters["itparam:" + std::string(itr->first)] = itr->second;
    }
    tunnel = _tunnel;
    if (tunnel) {
        setProperty("tunnel", "true");
    }
}

XdmValue *XsltExecutable::getParameter(const char *name) {
    std::map<std::string, XdmValue *>::iterator it;
    it = parameters.find("param:" + std::string(name));
    if (it != parameters.end())
        return it->second;
    else {
        it = parameters.find("sparam:" + std::string(name));
        if (it != parameters.end())
            return it->second;
    }
    return nullptr;
}

bool XsltExecutable::removeParameter(const char *name) {
    return (bool) (parameters.erase("param:" + std::string(name)));
}


void XsltExecutable::setResultAsRawValue(bool option) {
    if (option) {
        setProperty("outvalue", "yes");
    }
}

Xslt30Processor * Xslt30Processor::clone() {
    Xslt30Processor * proc = new Xslt30Processor(*this);
    return proc;

}

void XsltExecutable::setProperty(const char *name, const char *value) {
    if (name != nullptr) {
        int s = properties.size();
        std:
        std::string skey = std::string(name);
        properties.insert(std::pair<std::string, std::string>(skey, std::string((value == nullptr ? "" : value))));

        if (s == properties.size()) {
            std::map<std::string, std::string>::iterator it;
            it = properties.find(skey);
            if (it != properties.end()) {
                properties.erase(skey);
                properties[skey] = std::string((value == nullptr ? "" : value));
            }
        }
    }
}

const char *XsltExecutable::getProperty(const char *name) {
    std::map<std::string, std::string>::iterator it;
    it = properties.find(std::string(name));
    if (it != properties.end())
        return it->second.c_str();
    return nullptr;
}

void XsltExecutable::clearParameters(bool delValues) {
    if (delValues) {
        for (std::map<std::string, XdmValue *>::iterator itr = parameters.begin(); itr != parameters.end(); itr++) {

            XdmValue *value = itr->second;
            value->decrementRefCount();
#ifdef DEBUG
            std::cout<<"clearParameter() - XdmValue refCount="<<value->getRefCount()<<std::endl;
#endif
            if (value != nullptr && value->getRefCount() < 1) {
                delete value;
            }
        }

        SaxonProcessor::sxn_environ->env->DeleteLocalRef(selection);
        selection = nullptr;
    } else {
        for (std::map<std::string, XdmValue *>::iterator itr = parameters.begin(); itr != parameters.end(); itr++) {

            XdmValue *value = itr->second;
            value->decrementRefCount();

        }

        selection = nullptr;
    }
    parameters.clear();


}

void XsltExecutable::clearProperties() {
    properties.clear();

}


std::map<std::string, XdmValue *> &XsltExecutable::getParameters() {
    std::map<std::string, XdmValue *> &ptr = parameters;
    return ptr;
}

std::map<std::string, std::string> &XsltExecutable::getProperties() {
    std::map<std::string, std::string> &ptr = properties;
    return ptr;
}

void XsltExecutable::exceptionClear() {
    SaxonProcessor::sxn_environ->env->ExceptionClear();
}

void XsltExecutable::setcwd(const char *dir) {
    if (dir != nullptr) {
        cwdXE = std::string(dir);
    }
}


void XsltExecutable::exportStylesheet(const char *filename) {


    static jmethodID exportmID = nullptr;

    if (!exportmID) {
        exportmID = SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass, "save",
                                                                        "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;)V");
    }

    if (filename == nullptr) {
        std::cerr << "Error: Error: export file name is nullptr" << std::endl;
        return;
    }
    SaxonProcessor::sxn_environ->env->CallStaticVoidMethod(cppClass, exportmID,
                                                           SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                   cwdXE.c_str()),
                                                           executableObject,
                                                           SaxonProcessor::sxn_environ->env->NewStringUTF(filename));

}

void XsltExecutable::applyTemplatesReturningFile(const char *output_filename) {

    if (selection == nullptr) {
        std::cerr
                << "Error: The initial match selection has not been set. Please set it using setInitialMatchSelection or setInitialMatchSelectionFile."
                << std::endl;
        return;
    }

    static jmethodID atmID = nullptr;

    if (atmID == nullptr) {
        (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                        "applyTemplatesReturningFile",
                                                                        "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)V");

    }

    if (!atmID) {
        std::cerr << "Error: " << getDllname() << "applyTemplatesAsFile" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);
        SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, atmID,
                                                                 SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                         cwdXE.c_str()), executableObject, selection,
                                                                 (output_filename != nullptr
                                                                  ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                 output_filename) : nullptr),
                                                                 comboArrays.stringArray, comboArrays.objectArray);
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }

    }
    return;

}

const char *XsltExecutable::applyTemplatesReturningString() {

    if (selection == nullptr) {
        std::cerr
                << "Error: The initial match selection has not been set. Please set it using setInitialMatchSelection or setInitialMatchSelectionFile."
                << std::endl;
        return nullptr;
    }


    jmethodID atsmID =
            (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                            "applyTemplatesReturningString",
                                                                            "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/Object;[Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;");
    if (!atsmID) {
        std::cerr << "Error: " << getDllname() << "applyTemplatesAsString" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);

        jstring result = nullptr;
        jobject obj = (SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, atsmID,
                                                                                SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                        cwdXE.c_str()), executableObject,selection,
                comboArrays.stringArray, comboArrays.objectArray));

        if (obj) {
            result = (jstring) obj;
        }
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (result) {
            const char *str = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result,
                                                                                  nullptr);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(obj);
            return str;
        }
    }
    return nullptr;


}

XdmValue *XsltExecutable::applyTemplatesReturningValue() {

    if (selection == nullptr) {
        std::cerr
                << "Error: The initial match selection has not been set. Please set it using setInitialMatchSelection or setInitialMatchSelectionFile."
                << std::endl;
        return nullptr;
    }


    static jmethodID atsmID = nullptr;
    if (atsmID == nullptr) {
        atsmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                                 "applyTemplatesReturningValue",
                                                                                 "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/Object;[Ljava/lang/String;[Ljava/lang/Object;)Lnet/sf/saxon/s9api/XdmValue;");
    }
    if (!atsmID) {
        std::cerr << "Error: " << getDllname() << "applyTemplatesAsValue" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);


        // jstring result = nullptr;
        jobject result = (jobject) (SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, atsmID,
                                                                                             SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                     cwdXE.c_str()),
                                                                                             executableObject,
                                                                                             selection,
                                                                                             comboArrays.stringArray,
                                                                                             comboArrays.objectArray));
        /*if(obj) {
            result = (jobject)obj;
        }*/
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (result) {
            jclass atomicValueClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                   "net/sf/saxon/s9api/XdmAtomicValue");
            jclass nodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
            jclass functionItemClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                    "net/sf/saxon/s9api/XdmFunctionItem");
            XdmValue *value = nullptr;
            XdmItem *xdmItem = nullptr;


            if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, atomicValueClass) == JNI_TRUE) {
                xdmItem = new XdmAtomicValue(result);

                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;

            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, nodeClass) == JNI_TRUE) {
                xdmItem = new XdmNode(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, functionItemClass) == JNI_TRUE) {
                xdmItem = new XdmFunctionItem(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else {
                value = new XdmValue(result, true);
                for (int z = 0; z < value->size(); z++) {
                }
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return value;
            }
        }
    }
    return nullptr;

}


void XsltExecutable::callFunctionReturningFile(const char *functionName, XdmValue **arguments, int argument_length,
                                               const char *outfile) {


    static jmethodID afmID = nullptr;

    if (afmID == nullptr) {
        afmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                                "callFunctionReturningFile",
                                                                                "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/Object;[Ljava/lang/String;[Ljava/lang/Object;)V");
    }

    if (!afmID) {
        std::cerr << "Error: " << getDllname() << "callFunctionReturningFile" << " not found\n"
                  << std::endl;
        return;
    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);

        jobjectArray argumentJArray = SaxonProcessor::createJArray(arguments, argument_length);

        SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, afmID,
                                                                 SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                         cwdXE.c_str()),
                                                                 executableObject,
                                                                 (functionName != nullptr ?
                                                                  SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                          functionName) :
                                                                  nullptr), argumentJArray,
                                                                 (outfile != nullptr ?
                                                                  SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                          outfile) :
                                                                  nullptr),
                                                                 comboArrays.stringArray, comboArrays.objectArray);
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (argumentJArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(argumentJArray);
        }

    }
    return;


}

const char *
XsltExecutable::callFunctionReturningString(const char *functionName, XdmValue **arguments, int argument_length) {


    static jmethodID afsmID =
            (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                            "callFunctionReturningString",
                                                                            "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;[Ljava/lang/Object;[Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;");
    if (!afsmID) {
        std::cerr << "Error: " << getDllname() << "callFunctionReturningString" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);
        jobjectArray argumentJArray = SaxonProcessor::createJArray(arguments, argument_length);

        jstring result = nullptr;
        jobject obj = (SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, afsmID,
                                                                                SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                        cwdXE.c_str()), executableObject,
                                                                                        (functionName != nullptr
                                                                                         ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                        functionName)
                                                                                         : nullptr),
                                                                                argumentJArray, comboArrays.stringArray,
                                                                                comboArrays.objectArray));
        if (obj) {
            result = (jstring) obj;
        }
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (result) {
            const char *str = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result,
                                                                                  nullptr);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(obj);
            return str;
        }
    }
    return nullptr;
}


XdmValue *
XsltExecutable::callFunctionReturningValue(const char *functionName, XdmValue **arguments, int argument_length) {


    static jmethodID cfvmID = nullptr;
    if (cfvmID == nullptr) {
        cfvmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                                 "callFunctionReturningValue",
                                                                                 "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;[Ljava/lang/Object;[Ljava/lang/String;[Ljava/lang/Object;)Lnet/sf/saxon/s9api/XdmValue;");
    }
    if (!cfvmID) {
        std::cerr << "Error: " << getDllname() << "callFunctionReturningValue" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);
        jobjectArray argumentJArray = SaxonProcessor::createJArray(arguments, argument_length);

        jobject result = (jobject) (SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, cfvmID,
                                                                                             SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                     cwdXE.c_str()),
                                                                                             executableObject,
                                                                                             (functionName != nullptr
                                                                                              ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                             functionName)
                                                                                              : nullptr),
                                                                                             argumentJArray,
                                                                                             comboArrays.stringArray,
                                                                                             comboArrays.objectArray));

        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (argumentJArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(argumentJArray);
        }
        if (result) {
            jclass atomicValueClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                   "net/sf/saxon/s9api/XdmAtomicValue");
            jclass nodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
            jclass functionItemClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                    "net/sf/saxon/s9api/XdmFunctionItem");
            XdmValue *value = nullptr;
            XdmItem *xdmItem = nullptr;


            if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, atomicValueClass) == JNI_TRUE) {
                xdmItem = new XdmAtomicValue(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, nodeClass) == JNI_TRUE) {
                xdmItem = new XdmNode(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;

            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, functionItemClass) == JNI_TRUE) {
                xdmItem = new XdmFunctionItem(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else {
                value = new XdmValue(result, true);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return value;
            }
            value = new XdmValue();
            value->addUnderlyingValue(result);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
            return value;
        }
    }
    return nullptr;

}


void XsltExecutable::callTemplateReturningFile(const char *templateName, const char *outfile) {


    static jmethodID ctmID =
            (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                            "callTemplateReturningFile",
                                                                            "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)V");
    if (!ctmID) {
        std::cerr << "Error: " << getDllname() << "callTemplateReturningFile" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);
        SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, ctmID,
                                                                 SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                         cwdXE.c_str()),
                                                                 executableObject,
                                                                 (templateName != nullptr
                                                                  ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                 templateName) : nullptr),
                                                                 (outfile != nullptr
                                                                  ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                 outfile) : nullptr),
                                                                 comboArrays.stringArray, comboArrays.objectArray);
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }


    }

}


const char *XsltExecutable::callTemplateReturningString(const char *templateName) {


    jmethodID ctsmID =
            (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                            "callTemplateReturningString",
                                                                            "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;");
    if (!ctsmID) {
        std::cerr << "Error: " << getDllname() << "callTemplateReturningString" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);


        jstring result = nullptr;
        jobject obj = (jobject) (SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, ctsmID,
                                                                                          SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                  cwdXE.c_str()),
                                                                                          executableObject,
                                                                                          (templateName != nullptr
                                                                                           ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                          templateName)
                                                                                           : nullptr),
                                                                                          comboArrays.stringArray,
                                                                                          comboArrays.objectArray));
        if (obj) {
            result = (jstring) obj;
        }
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (result) {
            const char *str = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result,
                                                                                  nullptr);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(obj);
            return str;
        }
    }
    return nullptr;


}

XdmValue *XsltExecutable::callTemplateReturningValue(const char *templateName) {


    static jmethodID ctsmID = nullptr;
    if (ctsmID == nullptr) {
        ctsmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                                 "callTemplateReturningValue",
                                                                                 "(Ljava/lang/String;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)Lnet/sf/saxon/s9api/XdmValue;");
    }
    if (!ctsmID) {
        std::cerr << "Error: " << getDllname() << "callTemplateReturningValue" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);


        jstring result = nullptr;
        jobject obj = (jobject) (SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, ctsmID,
                                                                                          SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                  cwdXE.c_str()),
                                                                                          executableObject,
                                                                                          (templateName != nullptr
                                                                                           ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                                          templateName)
                                                                                           : nullptr),
                                                                                          comboArrays.stringArray,
                                                                                          comboArrays.objectArray));
        if (obj) {
            result = (jstring) obj;
        }
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (result) {
            jclass atomicValueClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                   "net/sf/saxon/s9api/XdmAtomicValue");
            jclass nodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
            jclass functionItemClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                    "net/sf/saxon/s9api/XdmFunctionItem");
            XdmValue *value = nullptr;

            XdmItem *xdmItem = nullptr;
            if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, atomicValueClass) == JNI_TRUE) {
                xdmItem = new XdmAtomicValue(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;

            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, nodeClass) == JNI_TRUE) {
                xdmItem = new XdmNode(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, functionItemClass) == JNI_TRUE) {
                xdmItem = new XdmFunctionItem(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else {
                value = new XdmValue(result, true);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return value;
            }
            value = new XdmValue();
            value->addUnderlyingValue(result);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
            return value;
        }
    }
    return nullptr;
}


XdmValue *XsltExecutable::transformFileToValue(const char *sourcefile) {

    if (sourcefile == nullptr) {

        return nullptr;
    }


    static jmethodID tfvMID = nullptr;

    if (tfvMID == nullptr) {
        tfvMID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                                 "transformToValue",
                                                                                 "(Ljava/lang/String;Lnet/sf/saxon/option/cpp/Xslt30Processor;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)Lnet/sf/saxon/s9api/XdmValue;");
    }
    if (!tfvMID) {
        std::cerr << "Error: " << getDllname() << ".transformtoValue" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);

        jobject result = (jobject) (
                SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, tfvMID,
                                                                         SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                 cwdXE.c_str()), nullptr, executableObject,
                                                                         (sourcefile != nullptr
                                                                          ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                         sourcefile) : nullptr),
                                                                         comboArrays.stringArray,
                                                                         comboArrays.objectArray));
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (result) {
            jclass atomicValueClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                   "net/sf/saxon/s9api/XdmAtomicValue");
            jclass nodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
            jclass functionItemClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                    "net/sf/saxon/s9api/XdmFunctionItem");
            jclass mapItemClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                                    "net/sf/saxon/s9api/XdmMap");
            jclass arrayItemClass = lookForClass(SaxonProcessor::sxn_environ->env,
                                               "net/sf/saxon/s9api/XdmArray");
            XdmValue *value = nullptr;
            XdmItem *xdmItem = nullptr;


            if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, atomicValueClass) == JNI_TRUE) {
                xdmItem = new XdmAtomicValue(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;

            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, nodeClass) == JNI_TRUE) {
                xdmItem = new XdmNode(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;

            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, functionItemClass) == JNI_TRUE) {
                xdmItem = new XdmFunctionItem(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, mapItemClass) == JNI_TRUE) {
                xdmItem = new XdmMap(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, arrayItemClass) == JNI_TRUE) {
                xdmItem = new XdmArray(result);
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                return xdmItem;
            } else {
                value = new XdmValue(result, true);
                return value;
            }
            value = new XdmValue();
            value->addUnderlyingValue(result);

            SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
            return value;
        }
    }
    return nullptr;

}


void XsltExecutable::transformFileToFile(const char *source, const char *outputfile) {


    static jmethodID tffMID = nullptr;

    if (tffMID == nullptr) {
        tffMID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                                 "transformToFile",
                                                                                 "(Ljava/lang/String;Lnet/sf/saxon/option/cpp/Xslt30Processor;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)V");
    }
    if (!tffMID) {
        std::cerr << "Error: " << getDllname() << "transformToFile" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);

        SaxonProcessor::sxn_environ->env->CallStaticVoidMethod(cppClass, tffMID,
                                                               SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                       cwdXE.c_str()), nullptr, executableObject,
                                                                       (source != nullptr
                                                                        ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                       source) : nullptr), nullptr,
                                                               (outputfile != nullptr
                                                                ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                               outputfile) : nullptr),
                                                               comboArrays.stringArray, comboArrays.objectArray);
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
    }

}


void XsltExecutable::setupXslMessage(bool create, const char * filename) {
    if (create) {

        static jmethodID messageID =   (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(messageListenerClass,
                                                                                    "<init>",
                                                                                    "(Ljava/lang/String;Ljava/lang/String;)V");
        if (!messageID) {
            std::cerr << "Error: "<<"SaxonCMessageListener" << " in " <<getDllname() << " not found\n"
                      << std::endl;

        }

        if (filename == nullptr) {
            saxonMessageListenerObj = (jobject)SaxonProcessor::sxn_environ->env->NewObject(messageListenerClass, messageID,
                                                                                           SaxonProcessor::sxn_environ->env->NewStringUTF("-:on"));
            //setProperty("m", "on");
        } else {
            saxonMessageListenerObj = (jobject)SaxonProcessor::sxn_environ->env->NewObject(messageListenerClass, messageID,
                                                                                           SaxonProcessor::sxn_environ->env->NewStringUTF(filename));
        }
    } else {
        setProperty("m", "off");
    }


}

XdmValue * XsltExecutable::getXslMessages(){

    if(saxonMessageListenerObj) {
        static jmethodID getmessageID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(messageListenerClass,
                                                                                                  "getXslMessages",
                                                                                                  "()[Lnet/sf/saxon/s9api/XdmNode;");
        if (!getmessageID) {
            std::cerr << "Error: " << getDllname() << ".getXslMessages" << " not found\n"
                      << std::endl;

        } else {
            jobjectArray results = (jobjectArray) (
                    SaxonProcessor::sxn_environ->env->CallObjectMethod(saxonMessageListenerObj, getmessageID));
            int sizex = SaxonProcessor::sxn_environ->env->GetArrayLength(results);

            if (sizex > 0) {
                XdmValue *value = new XdmValue();

                for (int p = 0; p < sizex; ++p) {
                    jobject resulti = SaxonProcessor::sxn_environ->env->GetObjectArrayElement(results, p);
                    value->addUnderlyingValue(resulti);
                }
                SaxonProcessor::sxn_environ->env->DeleteLocalRef(results);
                return value;
            }
        }
    }
    return NULL;


}


const char *XsltExecutable::transformFileToString(const char *source) {


    static jmethodID tftMID = tftMID;
    if (tftMID == nullptr) {
        tftMID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass,
                                                                                 "transformToString",
                                                                                 "(Ljava/lang/String;Lnet/sf/saxon/option/cpp/Xslt30Processor;Lnet/sf/saxon/s9api/XsltExecutable;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;");
    }
    if (!tftMID) {
        std::cerr << "Error: " << getDllname() << "transformFileToString" << " not found\n"
                  << std::endl;

    } else {
        JParameters comboArrays;
        comboArrays = SaxonProcessor::createParameterJArray(parameters, properties);

        jstring result = nullptr;
        jobject obj = SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(cppClass, tftMID,
                                                                               SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                       cwdXE.c_str()), nullptr,
                                                                               executableObject,
                                                                               (source != nullptr
                                                                                ? SaxonProcessor::sxn_environ->env->NewStringUTF(
                                                                                               source) : nullptr),
                                                                               comboArrays.stringArray,
                                                                               comboArrays.objectArray);
        if (comboArrays.stringArray != nullptr) {
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.stringArray);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(comboArrays.objectArray);
        }
        if (obj) {
            result = (jstring) obj;
        }
        if (result) {
            const char *str = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result,
                                                                                  nullptr);
            SaxonProcessor::sxn_environ->env->DeleteLocalRef(obj);
            return str;
        }
    }
    return nullptr;
}


const char *XsltExecutable::transformToString(XdmNode *source) {

    if (source != nullptr) {
        source->incrementRefCount();
        parameters["node"] = source;
    }
    return transformFileToString(nullptr);
}


XdmValue *XsltExecutable::transformToValue(XdmNode *source) {

    if (source != nullptr) {
        source->incrementRefCount();
        parameters["node"] = source;
    }
    return transformFileToValue(nullptr);
}

void XsltExecutable::transformToFile(XdmNode *source) {

    if (source != nullptr) {
        source->incrementRefCount();
        parameters["node"] = source;
    }
    transformFileToFile(nullptr, nullptr);
}

const char *XsltExecutable::getErrorMessage() {
    SaxonApiException *exception = SaxonProcessor::checkForExceptionCPP(SaxonProcessor::sxn_environ->env, cppClass,
                                                                        nullptr);
    if (exception == nullptr) { return nullptr; }
    return exception->getMessage();
}


SaxonApiException *XsltExecutable::getException() {
    return SaxonProcessor::checkForExceptionCPP(SaxonProcessor::sxn_environ->env, cppClass, nullptr);
}
