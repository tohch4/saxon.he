// XsltProcessor.cpp : Defines the exported functions for the DLL application.
//

//#include "xsltProcessor.cc"

#include "XsltProcessor.h"
#include "XdmValue.h"
#include "XdmItem.h"
#include "XdmNode.h"
#include "XdmAtomicValue.h"
#ifdef DEBUG
#include <typeinfo> //used for testing only
#endif

XsltProcessor::XsltProcessor() {

	SaxonProcessor *p = new SaxonProcessor(false);
	XsltProcessor(p, "");

}

XsltProcessor::XsltProcessor(SaxonProcessor * p, std::string curr) {

	proc = p;

	/*
	 * Look for class.
	 */
	cppClass = lookForClass(SaxonProcessor::sxn_environ->env,
			"net/sf/saxon/option/cpp/XsltProcessor");

	cppXT = createSaxonProcessor2(SaxonProcessor::sxn_environ->env, cppClass,
			"(Lnet/sf/saxon/s9api/Processor;)V", proc->proc);

#ifdef DEBUG
	jmethodID debugMID = SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass, "setDebugMode", "(Z)V");
	SaxonProcessor::sxn_environ->env->CallStaticVoidMethod(cppClass, debugMID, (jboolean)true);
    
#endif
	nodeCreated = false;
    jitCompilation = false;
	exception = nullptr;
	if(!(proc->cwd.empty()) && curr.empty()){
		cwdXT = proc->cwd;
	} else {
		cwdXT = curr;
	}

}

XsltProcessor::XsltProcessor(const XsltProcessor &other) {
	proc = other.proc;
	cppClass = other.cppClass;
	cppXT = other.cppXT;
	stylesheetObject = other.stylesheetObject;
    cwdXT = other.cwdXT;
	jitCompilation = other.jitCompilation;
	std::map<std::string, XdmValue*>::const_iterator paramIter = other.parameters.begin();
    while(paramIter != other.parameters.end())
    {

       XdmValue * valuei = paramIter->second;
       if(valuei == nullptr) {
    	 	//std::cerr<<"Error in XsltProcessor copy constructor"<<std::endl;
       } else {
            parameters[paramIter->first] = new XdmValue(*(valuei));
       }
       paramIter++;
    }

	std::map<std::string, std::string>::const_iterator propIter = other.properties.begin();
	while(propIter != other.properties.end())
    {
        properties[propIter->first] = propIter->second;
        propIter++;
    }

}


XsltProcessor * XsltProcessor::clone() {
     XsltProcessor * proc = new XsltProcessor(*this);
     return proc;
}


bool XsltProcessor::exceptionOccurred() {
	return proc->exceptionOccurred();
}

const char * XsltProcessor::getErrorCode() {
 if(exception == nullptr) {
    return nullptr;
 }
 return exception->getErrorCode();
}

void XsltProcessor::setSourceFromXdmNode(XdmNode * value) {
    if(value != nullptr){
      value->incrementRefCount();
      parameters["node"] = value;
    }
}

void XsltProcessor::setSourceFromFile(const char * ifile) {
	if(ifile != nullptr) {
		setProperty("s", ifile);
	}
}

void XsltProcessor::setOutputFile(const char * ofile) {
	setProperty("o", ofile);
}

void XsltProcessor::setBaseOutputURI(const char * baseURI) {
	if(baseURI != nullptr) {
  	    setProperty("baseoutput", baseURI);
	}
}

void XsltProcessor::setParameter(const char* name, XdmValue * value) {
	if(value != nullptr && name != nullptr){
		value->incrementRefCount();
		int s = parameters.size();
		std::string skey = "param:"+std::string(name);
		parameters[skey] = value;
		if(s == parameters.size()) {
            std::map<std::string, XdmValue*>::iterator it;
            it = parameters.find(skey);
            if (it != parameters.end()) {
                XdmValue * valuei = it->second;
                valuei->decrementRefCount();
                if(valuei != nullptr && valuei->getRefCount() < 1){
                    delete value;
                }
                parameters.erase(skey);
                parameters[skey] = value;
            }
		}
	} 
}

XdmValue* XsltProcessor::getParameter(const char* name) {
        std::map<std::string, XdmValue*>::iterator it;
        it = parameters.find("param:"+std::string(name));
        if (it != parameters.end())
          return it->second;
        return nullptr;
}

bool XsltProcessor::removeParameter(const char* name) {
	return (bool)(parameters.erase("param:"+std::string(name)));
}

void XsltProcessor::setJustInTimeCompilation(bool jit){
    jitCompilation = jit;
}

void XsltProcessor::setProperty(const char* name, const char* value) {	
	if(name != nullptr) {
	    int s = properties.size();
		std::string skey = std::string(name);
		properties.insert(std::pair<std::string, std::string>(skey, std::string((value == nullptr ? "" : value))));

		if(s == properties.size()) {
            std::map<std::string, std::string>::iterator it;
            it = properties.find(skey);
            if (it != properties.end()) {
                properties.erase(skey);
                properties[skey] = std::string((value == nullptr ? "" : value));
            }
		}
	}
}

const char* XsltProcessor::getProperty(const char* name) {
        std::map<std::string, std::string>::iterator it;
        it = properties.find(std::string(name));
        if (it != properties.end())
          return it->second.c_str();
	return nullptr;
}

void XsltProcessor::clearParameters(bool delValues) {
	if(delValues){
       		for(std::map<std::string, XdmValue*>::iterator itr = parameters.begin(); itr != parameters.end(); itr++){
			
			XdmValue * value = itr->second;
			value->decrementRefCount();
#ifdef DEBUG
			std::cout<<"clearParameter() - XdmValue refCount="<<value->getRefCount()<<std::endl;
#endif
			if(value != nullptr && value->getRefCount() < 1){
	        		delete value;
			}
        	}
		
	} else {
for(std::map<std::string, XdmValue*>::iterator itr = parameters.begin(); itr != parameters.end(); itr++){
		
			XdmValue * value = itr->second;
			value->decrementRefCount();
		
        	}

	}
	parameters.clear();
}

void XsltProcessor::clearProperties() {
	properties.clear();
}



std::map<std::string,XdmValue*>& XsltProcessor::getParameters(){
	std::map<std::string,XdmValue*>& ptr = parameters;
	return ptr;
}

std::map<std::string,std::string>& XsltProcessor::getProperties(){
	std::map<std::string,std::string> &ptr = properties;
	return ptr;
}

void XsltProcessor::exceptionClear(){
 if(exception != nullptr) {
 	delete exception;
 	exception = nullptr;
	SaxonProcessor::sxn_environ->env->ExceptionClear();
 }
  
 }

   void XsltProcessor::setcwd(const char* dir){
    if(dir != nullptr) {
        cwdXT = std::string(dir);
    }
   }

const char* XsltProcessor::checkException() {
	/*if(proc->exception == nullptr) {
	 proc->exception = proc->checkForException(environi, cpp);
	 }
	 return proc->exception;*/
	return proc->checkException(cppXT);
}


    void XsltProcessor::compileFromXdmNodeAndSave(XdmNode * node, const char* filename) {
	static jmethodID cAndSNodemID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"compileFromXdmNodeAndSave",
					"(Ljava/lang/String;Ljava/lang/Object;Ljava/lang/String;)V");
	if (!cAndSNodemID) {
		std::cerr << "Error: "<<getDllname() << ".compileFromStringAndSave"
				<< " not found\n" << std::endl;

	} else {

		
		SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, cAndSNodemID,
						SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
						node->getUnderlyingValue(), 							SaxonProcessor::sxn_environ->env->NewStringUTF(filename));
		
		exception = proc->checkAndCreateException(cppClass);

    }



}

    void XsltProcessor::compileFromStringAndSave(const char* stylesheetStr, const char* filename){
	static jmethodID cAndSStringmID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"compileFromStringAndSave",
					"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!cAndSStringmID) {
		std::cerr << "Error: "<<getDllname() << ".compileFromStringAndSave"
				<< " not found\n" << std::endl;

	} else {

		
		SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, cAndSStringmID,
						SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
						SaxonProcessor::sxn_environ->env->NewStringUTF(stylesheetStr), 							SaxonProcessor::sxn_environ->env->NewStringUTF(filename));
		
		exception = proc->checkAndCreateException(cppClass);

    }
}



    void XsltProcessor::compileFromFileAndSave(const char* xslFilename, const char* filename){
	static jmethodID cAndFStringmID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"compileFromFileAndSave",
					"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	if (!cAndFStringmID) {
		std::cerr << "Error: "<<getDllname() << ".compileFromFileAndSave"
				<< " not found\n" << std::endl;

	} else {

		
		SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, cAndFStringmID,
						SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
						SaxonProcessor::sxn_environ->env->NewStringUTF(xslFilename),SaxonProcessor::sxn_environ->env->NewStringUTF(filename));
		
		exception = proc->checkAndCreateException(cppClass);


     }
}

void XsltProcessor::compileFromString(const char* stylesheetStr) {
	static jmethodID cStringmID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"createStylesheetFromString",
					"(Ljava/lang/String;Ljava/lang/String;)Lnet/sf/saxon/s9api/XsltExecutable;");
	if (!cStringmID) {
		std::cerr << "Error: "<<getDllname() << "createStylesheetFromString"
				<< " not found\n" << std::endl;

	} else {
		releaseStylesheet();
	    if(jitCompilation) {
	        setProperty("jit:", "yes");
	    } else {
            setProperty("jit:", "no");
	    }
		stylesheetObject = (jobject)(
				SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, cStringmID,
						SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
						SaxonProcessor::sxn_environ->env->NewStringUTF(stylesheetStr)));
		if (!stylesheetObject) {
			exception = proc->checkAndCreateException(cppClass);
		}
	}

}

void XsltProcessor::compileFromXdmNode(XdmNode * node) {
	static jmethodID cNodemID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"createStylesheetFromFile",
					"(Ljava/lang/String;Lnet/sf/saxon/s9api/XdmNode;)Lnet/sf/saxon/s9api/XsltExecutable;");
	if (!cNodemID) {
		std::cerr << "Error: "<< getDllname() << "createStylesheetFromXdmNode"
				<< " not found\n" << std::endl;

	} else {
		releaseStylesheet();
	    if(jitCompilation) {
	        setProperty("jit:", "yes");
	    } else {
            setProperty("jit:", "no");
	    }
		stylesheetObject = (jobject)(
				SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, cNodemID,
						SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
						node->getUnderlyingValue()));
		if (!stylesheetObject) {
			exception = proc->checkAndCreateException(cppClass);
			//cout << "Error in compileFromXdmNode" << endl;
		}
	}

}

void XsltProcessor::compileFromFile(const char* stylesheet) {
	static jmethodID cFilemID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"createStylesheetFromFile",
					"(Ljava/lang/String;Ljava/lang/String;)Lnet/sf/saxon/s9api/XsltExecutable;");
	if (!cFilemID) {
		std::cerr << "Error: "<<getDllname() << "createStylesheetFromFile"
				<< " not found\n" << std::endl;

	} else {
		releaseStylesheet();

		stylesheetObject = (jobject)(
				SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, cFilemID,
						SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
						SaxonProcessor::sxn_environ->env->NewStringUTF(stylesheet)));
		if (!stylesheetObject) {
			exception = proc->checkAndCreateException(cppClass);
     		
		}
		//SaxonProcessor::sxn_environ->env->NewGlobalRef(stylesheetObject);
	}

}

void XsltProcessor::releaseStylesheet() {

	stylesheetObject = nullptr;
	
}



XdmValue * XsltProcessor::transformFileToValue(const char* sourcefile,
		const char* stylesheetfile) {

	if(exceptionOccurred()) {
		//Possible error detected in the compile phase. Processor not in a clean state.
		//Require clearing exception.
		return nullptr;
	}

	if(sourcefile == nullptr && stylesheetfile == nullptr && !stylesheetObject){
	
		return nullptr;
	}


	static jmethodID mID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"transformToNode",
					"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)Lnet/sf/saxon/s9api/XdmNode;");
	if (!mID) {
		std::cerr << "Error: "<< getDllname() << "transformtoNode" << " not found\n"
				<< std::endl;

	} else {
		jobjectArray stringArray = nullptr;
		jobjectArray objectArray = nullptr;
		jclass objectClass = lookForClass(SaxonProcessor::sxn_environ->env,
				"java/lang/Object");
		jclass stringClass = lookForClass(SaxonProcessor::sxn_environ->env,
				"java/lang/String");

		int size = parameters.size() + properties.size();
		if (size > 0) {
			objectArray = SaxonProcessor::sxn_environ->env->NewObjectArray((jint) size,
					objectClass, 0);
			stringArray = SaxonProcessor::sxn_environ->env->NewObjectArray((jint) size,
					stringClass, 0);
			int i = 0;
			for (std::map<std::string, XdmValue*>::iterator iter =
					parameters.begin(); iter != parameters.end(); ++iter, i++) {
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->first).c_str()));
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
						(iter->second)->getUnderlyingValue());
			}
			for (std::map<std::string, std::string>::iterator iter =
					properties.begin(); iter != properties.end(); ++iter, i++) {
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->first).c_str()));
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->second).c_str()));
			}
		}
		jobject result = (jobject)(
				SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, mID,
						SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
						(sourcefile != nullptr ?
								SaxonProcessor::sxn_environ->env->NewStringUTF(sourcefile) :
								nullptr),
						(stylesheetfile != nullptr ?
								SaxonProcessor::sxn_environ->env->NewStringUTF(
										stylesheetfile) :
								nullptr), stringArray, objectArray));
		if (size > 0) {
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(stringArray);
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(objectArray);
		}
		if (result) {
			XdmNode * node = new XdmNode(result);
			node->setProcessor(proc);
			XdmValue * value = new XdmValue();
			value->addXdmItem(node);
			return value;
		}else {
	
			exception = proc->checkAndCreateException(cppClass);
	   		
     		}
	}
	return nullptr;

}


void XsltProcessor::transformFileToFile(const char* source,
		const char* stylesheet, const char* outputfile) {

	if(exceptionOccurred()) {
		//Possible error detected in the compile phase. Processor not in a clean state.
		//Require clearing exception.
		return;	
	}
	if(source == nullptr && outputfile == nullptr && !stylesheetObject){
		
		return;
	}

	jmethodID mID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"transformToFile",
					"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)V");
	if (!mID) {
		std::cerr << "Error: "<<getDllname() << "transformToFile" << " not found\n"
				<< std::endl;

	} else {
		jobjectArray stringArray = nullptr;
		jobjectArray objectArray = nullptr;
		jclass objectClass = lookForClass(SaxonProcessor::sxn_environ->env,
				"java/lang/Object");
		jclass stringClass = lookForClass(SaxonProcessor::sxn_environ->env,
				"java/lang/String");

		int size = parameters.size() + properties.size();
#ifdef DEBUG
		std::cerr<<"Properties size: "<<properties.size()<<std::endl;
		std::cerr<<"Parameter size: "<<parameters.size()<<std::endl;
		std::cerr<<"size:"<<size<<std::endl;
#endif
		if (size > 0) {
			objectArray = SaxonProcessor::sxn_environ->env->NewObjectArray((jint) size,
					objectClass, 0);
			stringArray = SaxonProcessor::sxn_environ->env->NewObjectArray((jint) size,
					stringClass, 0);
		
			int i = 0;
			for (std::map<std::string, XdmValue*>::iterator iter =
					parameters.begin(); iter != parameters.end(); ++iter, i++) {

#ifdef DEBUG
				std::cerr<<"map 1"<<std::endl;
				std::cerr<<"iter->first"<<(iter->first).c_str()<<std::endl;
#endif
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->first).c_str()));
#ifdef DEBUG
				std::string s1 = typeid(iter->second).name();
				std::cerr<<"Type of itr:"<<s1<<std::endl;
				jobject xx = (iter->second)->getUnderlyingValue();
				if(xx == nullptr) {
					std::cerr<<"value failed"<<std::endl;
				} else {

					std::cerr<<"Type of value:"<<(typeid(xx).name())<<std::endl;
				}
				if((iter->second)->getUnderlyingValue() == nullptr) {
					std::cerr<<"(iter->second)->getUnderlyingValue() is nullptr"<<std::endl;
				}
#endif
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
						(iter->second)->getUnderlyingValue());

			}

			for (std::map<std::string, std::string>::iterator iter =
					properties.begin(); iter != properties.end(); ++iter, i++) {
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->first).c_str()));
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->second).c_str()));
			}
		}
		SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, mID,
								SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
								(source != nullptr ?
										SaxonProcessor::sxn_environ->env->NewStringUTF(
												source) :
										nullptr),
								SaxonProcessor::sxn_environ->env->NewStringUTF(stylesheet),nullptr,
								stringArray, objectArray);
		if (size > 0) {
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(stringArray);
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(objectArray);
		}
		
	}

	proc->checkAndCreateException(cppClass);
}

void XsltProcessor::setupXslMessage(bool show, const char* filename) {
    if(show) {
        if(filename == nullptr) {
            setProperty("m", "on");
        } else {
            setProperty("m", filename);
        }
    } else {
        setProperty("m", "off");
    }
}


const char * XsltProcessor::transformFileToString(const char* source,
		const char* stylesheet) {

	if(exceptionOccurred()) {
		//Possible error detected in the compile phase. Processor not in a clean state.
		//Require clearing exception.
		return nullptr;
	}
	if(source == nullptr && stylesheet == nullptr && !stylesheetObject){
		std::cerr<< "Error: The most recent StylesheetObject failed. Please check exceptions"<<std::endl;
		return nullptr;
	}

	jmethodID mID =
			(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass,
					"transformToString",
					"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;");
	if (!mID) {
		std::cerr << "Error: "<<getDllname() << "transformFileToString" << " not found\n"
				<< std::endl;

	} else {
		jobjectArray stringArray = nullptr;
		jobjectArray objectArray = nullptr;
		jclass objectClass = lookForClass(SaxonProcessor::sxn_environ->env,
				"java/lang/Object");
		jclass stringClass = lookForClass(SaxonProcessor::sxn_environ->env,
				"java/lang/String");

		int size = parameters.size() + properties.size();
#ifdef DEBUG
		std::cerr<<"Properties size: "<<properties.size()<<std::endl;
		std::cerr<<"Parameter size: "<<parameters.size()<<std::endl;
#endif
		if (size > 0) {
			objectArray = SaxonProcessor::sxn_environ->env->NewObjectArray((jint) size,
					objectClass, 0);
			stringArray = SaxonProcessor::sxn_environ->env->NewObjectArray((jint) size,
					stringClass, 0);
			int i = 0;
			for (std::map<std::string, XdmValue*>::iterator iter =
					parameters.begin(); iter != parameters.end(); ++iter, i++) {

#ifdef DEBUG
				std::cerr<<"map 1"<<std::endl;
				std::cerr<<"iter->first"<<(iter->first).c_str()<<std::endl;
#endif
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->first).c_str()));
#ifdef DEBUG
				std::string s1 = typeid(iter->second).name();
				std::cerr<<"Type of itr:"<<s1<<std::endl;


				jobject xx = (iter->second)->getUnderlyingValue();

				if(xx == nullptr) {
					std::cerr<<"value failed"<<std::endl;
				} else {

					std::cerr<<"Type of value:"<<(typeid(xx).name())<<std::endl;
				}
				if((iter->second)->getUnderlyingValue() == nullptr) {
					std::cerr<<"(iter->second)->getUnderlyingValue() is nullptr"<<std::endl;
				}
#endif

				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
						(iter->second)->getUnderlyingValue());

			}

			for (std::map<std::string, std::string>::iterator iter =
					properties.begin(); iter != properties.end(); ++iter, i++) {
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->first).c_str()));
				SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
						SaxonProcessor::sxn_environ->env->NewStringUTF(
								(iter->second).c_str()));
			}
		}

	jstring result = nullptr;
	jobject obj =
				(
						SaxonProcessor::sxn_environ->env->CallObjectMethod(cppXT, mID,
								SaxonProcessor::sxn_environ->env->NewStringUTF(cwdXT.c_str()),
								(source != nullptr ?
										SaxonProcessor::sxn_environ->env->NewStringUTF(
												source) :
										nullptr),
								SaxonProcessor::sxn_environ->env->NewStringUTF(stylesheet),
								stringArray, objectArray));
		if(obj) {
			result = (jstring)obj;
		}		
		if (size > 0) {
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(stringArray);
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(objectArray);
		}
		if (result) {
			const char * str = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result,
					nullptr);
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(obj);
			return str;
		} else  {
			exception = proc->checkAndCreateException(cppClass);
	   		
     		}
	}
	return nullptr;
}


   const char * XsltProcessor::transformToString(){
	if(!stylesheetObject){
		std::cerr<< "Error: No styleheet found. Please compile stylsheet before calling transformToString or check exceptions"<<std::endl;
		return nullptr;
	}
	return transformFileToString(nullptr, nullptr);
   }


    XdmValue * XsltProcessor::transformToValue(){
	if(!stylesheetObject){
		std::cerr<< "Error: No styleheet found. Please compile stylsheet before calling transformToValue or check exceptions"<<std::endl;
		return nullptr;
	}
	return transformFileToValue(nullptr, nullptr);
   }

    void XsltProcessor::transformToFile(){
	if(!stylesheetObject){
		std::cerr<< "Error: No styleheet found. Please compile stylsheet before calling transformToFile or check exceptions"<<std::endl;
		return;
	}
	transformFileToFile(nullptr, nullptr, nullptr);
   }

const char * XsltProcessor::getErrorMessage(){
 	if(exception == nullptr) {
 	    return nullptr;
 	}
 	return exception->getErrorMessage();
 }

