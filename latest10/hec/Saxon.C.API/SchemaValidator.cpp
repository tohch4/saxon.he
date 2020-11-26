#include "SchemaValidator.h"
#include "XdmNode.h"
#include "XdmValue.h"
#include "XdmItem.h"

//#define DEBUG

SchemaValidator::SchemaValidator() {
	SaxonProcessor *p = new SaxonProcessor(true);
	SchemaValidator(p, "");
}

SchemaValidator::SchemaValidator(SaxonProcessor* p, std::string curr){
	proc = p;
	/*
	 * Look for class.
	 */
	cppClass = lookForClass(SaxonProcessor::sxn_environ->env,
			"net/sf/saxon/option/cpp/SchemaValidatorForCpp");
	if ((proc->proc) == nullptr) {
		std::cerr << "Processor is nullptr" << std::endl;
	}

	cppV = createSaxonProcessor2(SaxonProcessor::sxn_environ->env, cppClass,
			"(Lnet/sf/saxon/s9api/Processor;)V", proc->proc);

#ifdef DEBUG
	jmethodID debugMID = SaxonProcessor::sxn_environ->env->GetStaticMethodID(cppClass, "setDebugMode", "(Z)V");
	SaxonProcessor::sxn_environ->env->CallStaticVoidMethod(cppClass, debugMID, (jboolean)true);
#endif    

	
	if(!(proc->cwd.empty()) && curr.empty()){
		cwdV = proc->cwd;
	} else {
		cwdV = curr;
	}
	exception = proc->checkAndCreateException(cppClass);

}

   void SchemaValidator::setcwd(const char* dir){
	if(dir==nullptr) {
    		cwdV = std::string(dir);
	}
   }

  void SchemaValidator::setOutputFile(const char * sourceFile){
	outputFile = std::string(sourceFile);
  }

  XdmNode * SchemaValidator::getValidationReport(){
	jmethodID mID =
		(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass, "getValidationReport", "()Lnet/sf/saxon/s9api/XdmNode;");
	if (!mID) {
		std::cerr << "Error: libsaxon." << "validate.getValidationReport()" << " not found\n"
			<< std::endl;
	} else {
		jobject result = (jobject)(
			SaxonProcessor::sxn_environ->env->CallObjectMethod(cppV, mID));
		
		if (result) {
			XdmNode * node = new XdmNode(result);
			node->setProcessor(proc);
			return node;
		}
		exception = proc->checkAndCreateException(cppClass);
	}
	return nullptr;
}

  void SchemaValidator::registerSchemaFromFile(const char * sourceFile){
	if (sourceFile == nullptr) {
		std::cerr << "Error:: sourceFile string cannot be empty or nullptr" << std::endl;
	     return;
        }
	
	jmethodID mID =
		(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass, "registerSchema",
				"(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)V");
	if (!mID) {
		std::cerr << "Error: libsaxon." << "validate" << " not found\n"
			<< std::endl;
	} else {
	jobjectArray stringArray = nullptr;
	jobjectArray objectArray = nullptr;
	jclass objectClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/Object");
	jclass stringClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/String");

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
		for (std::map<std::string, XdmValue*>::iterator iter = parameters.begin();
				iter != parameters.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					(iter->second)->getUnderlyingValue());
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
		}
		for (std::map<std::string, std::string>::iterator iter = properties.begin();
				iter != properties.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->second).c_str()));
		}
	}
	
			SaxonProcessor::sxn_environ->env->CallVoidMethod(cppV, mID,
					SaxonProcessor::sxn_environ->env->NewStringUTF(cwdV.c_str()),
					SaxonProcessor::sxn_environ->env->NewStringUTF(sourceFile), stringArray, objectArray);

	if (size > 0) {
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(stringArray);
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(objectArray);
	}

}
	exception = proc->checkAndCreateException(cppClass);
     		
	
 }

 void SchemaValidator::exportSchema(const char * fileName) {
      if (fileName == nullptr) {
      		std::cerr << "Error:: fileName string cannot be empty or nullptr" << std::endl;
      	     return;
              }

      	jmethodID mID =
      		(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass, "exportSchema",
      				"(Ljava/lang/String;Ljava/lang/String;)V");
      	if (!mID) {
      		std::cerr << "Error: libsaxon." << "exportSchema" << " not found\n"
      			<< std::endl;
      	} else {


      			SaxonProcessor::sxn_environ->env->CallVoidMethod(cppV, mID,
      					SaxonProcessor::sxn_environ->env->NewStringUTF(cwdV.c_str()),
      					SaxonProcessor::sxn_environ->env->NewStringUTF(fileName));



      }
      exception = proc->checkAndCreateException(cppClass);

 }

  void SchemaValidator::registerSchemaFromString(const char * sourceStr){

	
	if (sourceStr == nullptr) {
		std::cerr << "Error:: Schema string cannot be empty or nullptr" << std::endl;
	     return;
        }
	jmethodID mID =
		(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass, "registerSchemaString",
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)V");

	if (!mID) {
		std::cerr << "Error: libsaxon." << "registerSchemaString" << " not found\n"
			<< std::endl;
	} else {
	jobjectArray stringArray = nullptr;
	jobjectArray objectArray = nullptr;
	jclass objectClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/Object");
	jclass stringClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/String");

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
		for (std::map<std::string, XdmValue*>::iterator iter = parameters.begin();
				iter != parameters.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					(iter->second)->getUnderlyingValue());
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
		}
		for (std::map<std::string, std::string>::iterator iter = properties.begin();
				iter != properties.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->second).c_str()));
		}
	}

	
			SaxonProcessor::sxn_environ->env->CallVoidMethod(cppV, mID,
					SaxonProcessor::sxn_environ->env->NewStringUTF(cwdV.c_str()),
					(sourceStr != nullptr ? SaxonProcessor::sxn_environ->env->NewStringUTF(sourceStr) : nullptr), nullptr, stringArray, objectArray);

	if (size > 0) {
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(stringArray);
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(objectArray);
	}

    }
	exception = proc->checkAndCreateException(cppClass);
    
}

  void SchemaValidator::validate(const char * sourceFile){
	/*if (sourceFile == nullptr) {
		std::cerr << "Error:: sourceFile string cannot be empty or nullptr" << std::endl;
	     return;
        }*/

jmethodID mID =
		(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass, "validate",
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)V");
if (!mID) {
	std::cerr << "Error: libsaxon." << "validate" << " not found\n"
			<< std::endl;

} else {
	jobjectArray stringArray = nullptr;
	jobjectArray objectArray = nullptr;
	jclass objectClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/Object");
	jclass stringClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/String");

	int size = parameters.size() + properties.size();
	if(lax) {
      size++;
     }
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
        if(lax){
            SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
                        SaxonProcessor::sxn_environ->env->NewStringUTF("lax"));
                        SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
                        booleanValue(SaxonProcessor::sxn_environ, lax));
            i++;
        }
		for (std::map<std::string, XdmValue*>::iterator iter = parameters.begin();
				iter != parameters.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					(iter->second)->getUnderlyingValue());

#ifdef DEBUG
				std::string s1 = typeid(iter->second).name();
				std::cerr<<"param-name:"<<(iter->first)<<",  "<<"Type of itr:"<<s1<<std::endl;
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
		}
		for (std::map<std::string, std::string>::iterator iter = properties.begin();
				iter != properties.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->second).c_str()));

		}
	}
	
			SaxonProcessor::sxn_environ->env->CallVoidMethod(cppV, mID,
					SaxonProcessor::sxn_environ->env->NewStringUTF(cwdV.c_str()), 
					(sourceFile != nullptr ? SaxonProcessor::sxn_environ->env->NewStringUTF(sourceFile) : nullptr), (outputFile.empty() ? nullptr : outputFile.c_str() ), stringArray, objectArray);

	if (size > 0) {
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(stringArray);
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(objectArray);
	}
	exception = proc->checkAndCreateException(cppClass);
				
	}	
}
   
XdmNode * SchemaValidator::validateToNode(const char * sourceFile){
	/*if (sourceFile == nullptr) {
		std::cerr << "Error:: source file string cannot be empty or nullptr" << std::endl;
	     return nullptr;
        }*/

jmethodID mID =
		(jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(cppClass, "validateToNode",
				"(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/Object;)Lnet/sf/saxon/s9api/XdmNode;");
if (!mID) {
	std::cerr << "Error: libsaxon." << "validate" << " not found\n"
			<< std::endl;

} else {
	jobjectArray stringArray = nullptr;
	jobjectArray objectArray = nullptr;
	jclass objectClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/Object");
	jclass stringClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/lang/String");

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
		for (std::map<std::string, XdmValue*>::iterator iter = parameters.begin();
				iter != parameters.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					(iter->second)->getUnderlyingValue());
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
		}
		for (std::map<std::string, std::string>::iterator iter = properties.begin();
				iter != properties.end(); ++iter, i++) {
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(stringArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->first).c_str()));
			SaxonProcessor::sxn_environ->env->SetObjectArrayElement(objectArray, i,
					SaxonProcessor::sxn_environ->env->NewStringUTF((iter->second).c_str()));
		}
	}
	jobject result = (jobject)(
			SaxonProcessor::sxn_environ->env->CallObjectMethod(cppV, mID,
					SaxonProcessor::sxn_environ->env->NewStringUTF(cwdV.c_str()),
					(sourceFile != nullptr ? SaxonProcessor::sxn_environ->env->NewStringUTF(sourceFile) : nullptr), stringArray, objectArray));
	if (size > 0) {
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(stringArray);
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(objectArray);
	}
	if (result) {
		XdmNode * node = new XdmNode(result);
		node->setProcessor(proc);
		return node;
	}

	exception = proc->checkAndCreateException(cppClass);

}
	return nullptr;
}

void SchemaValidator::exceptionClear(){
 if(exception != nullptr) {
 	delete exception;
 	exception = nullptr;
 }
   SaxonProcessor::sxn_environ->env->ExceptionClear();
 }

/*const char * SchemaValidator::getErrorCode(int i) {
	if(proc->exception == nullptr) {return nullptr;}
	return proc->exception->getErrorCode(i);
}

const char * SchemaValidator::getErrorMessage(int i ){
 if(proc->exception == nullptr) {return nullptr;}
 return proc->exception->getErrorMessage(i);
 } */

bool SchemaValidator::exceptionOccurred() {
	return proc->exceptionOccurred() || exception != nullptr;
}

const char* SchemaValidator::checkException() {
	return proc->checkException(cppV);
}

/*int SchemaValidator::exceptionCount(){
 if(proc->exception != nullptr){
 return proc->exception->count();
 }
 return 0;
 }   */

void SchemaValidator::setSourceNode(XdmNode * value) {
    if(value != nullptr){
      value->incrementRefCount();
      parameters["node"] = (XdmValue *)value;
    }

	
}

void SchemaValidator::setParameter(const char * name, XdmValue*value) {
	if(value != nullptr){
		value->incrementRefCount();
		parameters["param:"+std::string(name)] = value;
	}
}

bool SchemaValidator::removeParameter(const char * name) {
	return (bool)(parameters.erase("param:"+std::string(name)));
}

void SchemaValidator::setProperty(const char * name, const char * value) {
#ifdef DEBUG	
	if(value == nullptr) {
		std::cerr<<"Validator setProperty is nullptr"<<std::endl;
	}
#endif
	properties.insert(std::pair<std::string, std::string>(std::string(name), std::string((value== nullptr ? "" : value))));
}

void SchemaValidator::clearParameters(bool delVal) {
	if(delVal){
       		for(std::map<std::string, XdmValue*>::iterator itr = parameters.begin(); itr != parameters.end(); itr++){
			XdmValue * value = itr->second;
			value->decrementRefCount();
#ifdef DEBUG
			std::cout<<"SchemaValidator.clearParameter() - XdmValue refCount="<<value->getRefCount()<<std::endl;
#endif
			if(value != nullptr && value->getRefCount() < 1){
	        		delete value;
			}
        	}
		parameters.clear();
	}
}

void SchemaValidator::clearProperties() {
	properties.clear();
}

std::map<std::string,XdmValue*>& SchemaValidator::getParameters(){
	std::map<std::string,XdmValue*>& ptr = parameters;
	return ptr;
}

std::map<std::string,std::string>& SchemaValidator::getProperties(){
	std::map<std::string,std::string> &ptr = properties;
	return ptr;
}

