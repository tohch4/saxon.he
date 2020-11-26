
#include "XdmNode.h"


/*
const char * baseURI;
	const char * nodeName;
	XdmNode ** children; //caches child nodes when getChildren method is first called;
	int childCount;
	XdmNode * parent;
	XdmNode ** attrValues;//caches attribute nodes when getAttributeNodes method is first called;
	int attrCount;
	XDM_NODE_KIND nodeKind;
*/

XdmNode::XdmNode(jobject obj): XdmItem(obj), baseURI(nullptr), nodeName(nullptr), children(nullptr), childCount(-1), parent(nullptr), typedValue(nullptr), attrValues(nullptr), attrCount(-1), nodeKind(UNKNOWN){

}

XdmNode::XdmNode(XdmNode * p, jobject obj, XDM_NODE_KIND kind): XdmItem(obj), baseURI(nullptr), nodeName(nullptr), children(nullptr),  childCount(-1), parent(p), typedValue(nullptr), attrValues(nullptr),  attrCount(-1), nodeKind(kind){}

bool XdmNode::isAtomic() {
	return false;
} 
    
    XDM_NODE_KIND XdmNode::getNodeKind(){
	if(nodeKind == UNKNOWN) {
            jclass xdmNodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "Lnet/sf/saxon/s9api/XdmNode;");
            static jmethodID nodeKindMID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmNodeClass,
                                                                                                     "getNodeKind",
                                                                                                     "()Lnet/sf/saxon/s9api/XdmNodeKind;");
            if (!nodeKindMID) {
                std::cerr << "Error: MyClassInDll." << "getNodeKind" << " not found\n"
                          << std::endl;
                return UNKNOWN;
            }

            jobject nodeKindObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, nodeKindMID));
            if (!nodeKindObj) {

                return UNKNOWN;
            }
            jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "Lnet/sf/saxon/option/cpp/XdmUtils;");

            jmethodID mID2 = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,
                                                                                             "convertNodeKindType",
                                                                                             "(Lnet/sf/saxon/s9api/XdmNodeKind;)I");

            if (!mID2) {
                std::cerr << "Error: MyClassInDll." << "convertNodeKindType" << " not found\n"
                          << std::endl;
                return UNKNOWN;
            }

            int kvalue = (int) (SaxonProcessor::sxn_environ->env->CallStaticIntMethod(xdmUtilsClass, mID2, nodeKindObj));

		nodeKind = static_cast<XDM_NODE_KIND>(kvalue);
	} 
	return nodeKind;

    }

    const char * XdmNode::getNodeName(){
	
	if(nodeName != nullptr) {
		return nodeName;
	}
	XDM_NODE_KIND kind = getNodeKind();
 	jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
	jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getNodeName",
					"(Lnet/sf/saxon/s9api/XdmNode;)Ljava/lang/String;");
	switch (kind) {
            case DOCUMENT:
            case TEXT:
            case COMMENT:
                return nullptr;
            case PROCESSING_INSTRUCTION:
            case NAMESPACE:
            case ELEMENT:
            case ATTRIBUTE:
               
		if (!xmID) {
			std::cerr << "Error: MyClassInDll." << "getNodeName"<< " not found\n" << std::endl;
			return nullptr;
		} else {
			jstring result = (jstring)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID, value));
			if(!result) {
				return nullptr;
			} else {
				nodeName = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result, nullptr);
				return nodeName;
			} 
		}
            default:
                return nullptr;
        }
	

    }

    XdmValue * XdmNode::getTypedValue(){
    	if(typedValue == nullptr) {
    		jclass xdmNodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
    		jmethodID tbmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmNodeClass,
    					"getTypedValue",
    					"()Lnet/sf/saxon/s9api/XdmValue;");
    		if (!tbmID) {
    			std::cerr << "Error: Saxonc." << "getTypedValue"
    				<< " not found\n" << std::endl;
    			return nullptr;
    		} else {
    			jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, tbmID));
    			if(valueObj) {
    				typedValue = new XdmValue((proc == nullptr ? nullptr : proc));
    				typedValue->addUnderlyingValue(valueObj);
    				return typedValue;
    			}
    			return nullptr;
    		}
    	} else {
    		return typedValue;
    	}


    }

    const char * XdmNode::getStringValue(){
   		return XdmItem::getStringValue();
    }

    const char * XdmNode::toString(){
       	if(stringValue.empty()) {
        		jclass xdmNodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
        		jmethodID strbMID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmNodeClass,
        					"toString",
        					"()Ljava/lang/String;");
        		if (!strbMID) {
        			std::cerr << "Error: Saxonc." << "toString"
        				<< " not found\n" << std::endl;
        			return nullptr;
        		} else {
        			jstring result = (jstring) (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, strbMID));
        			if(result) {
                       const char * str = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result, nullptr);
                       stringValue = str;
                  		return str;
                }
                   return nullptr;
        		}
        	} else {
        		return stringValue.c_str();
        	}
    }

    
    const char* XdmNode::getBaseUri(){

	if(baseURI != nullptr) {
		return baseURI;
	}

	jclass xdmNodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
	jmethodID bmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmNodeClass,
					"getBaseURI",
					"()Ljava/net/URI;");
	if (!bmID) {
		std::cerr << "Error: MyClassInDll." << "getBaseURI"
				<< " not found\n" << std::endl;
		return nullptr;
	} else {
		jobject nodeURIObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, bmID));
		if(!nodeURIObj){
			return nullptr;
		} else {
			jclass URIClass = lookForClass(SaxonProcessor::sxn_environ->env, "java/net/URI");
			jmethodID strMID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(URIClass,
					"toString",
					"()Ljava/lang/String;");
			if(strMID){
				jstring result = (jstring)(
				SaxonProcessor::sxn_environ->env->CallObjectMethod(nodeURIObj, strMID));
				baseURI = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result,
					nullptr);
			
				return baseURI;
			}	
		}
	}
	return nullptr;
    }
    
    




    XdmNode* XdmNode::getParent(){
	if(parent == nullptr && proc!= nullptr) {
		jclass xdmNodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
		jmethodID bmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmNodeClass,
					"getParent",
					"()Lnet/sf/saxon/s9api/XdmNode;");
		if (!bmID) {
			std::cerr << "Error: MyClassInDll." << "getParent"
				<< " not found\n" << std::endl;
			return nullptr;
		} else {
			jobject nodeObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, bmID));
			if(nodeObj) {
				parent = new XdmNode(nullptr, nodeObj, UNKNOWN);
				parent->setProcessor(proc);
				//parent->incrementRefCount();
				return parent;
			}
			return nullptr;
		}
	} else {
		return parent;
	}
	
    }
    
    const char* XdmNode::getAttributeValue(const char *str){

	if(str == nullptr) { return nullptr;}
	jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
	jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getAttributeValue",
					"(Lnet/sf/saxon/s9api/XdmNode;Ljava/lang/String;)Ljava/lang/String;");
	if (!xmID) {
			std::cerr << "Error: SaxonDll." << "getAttributeValue"
				<< " not found\n" << std::endl;
			return nullptr;
		}
	if(str == nullptr) {
		return nullptr;
	}
	jstring eqname = SaxonProcessor::sxn_environ->env->NewStringUTF(str);

	jstring result = (jstring)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID,value, eqname));
	SaxonProcessor::sxn_environ->env->DeleteLocalRef(eqname);
	//failure = checkForException(sxn_environ,  (jobject)result);//Remove code
	if(result) {
		const char * stri = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result,
					nullptr);
		
		//SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);

		return stri;
	} else {

		return nullptr;
	}

    }

    XdmNode** XdmNode::getAttributeNodes(){
	if(attrValues == nullptr) {
		jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
		jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getAttributeNodes",
					"(Lnet/sf/saxon/s9api/XdmNode;)[Lnet/sf/saxon/s9api/XdmNode;");
		jobjectArray results = (jobjectArray)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID, 
		value));
		if(results == nullptr) {
			return nullptr;
		}
		int sizex = SaxonProcessor::sxn_environ->env->GetArrayLength(results);
		attrCount = sizex;
		if(sizex>0) {	
			attrValues =  new XdmNode*[sizex];
			XdmNode * tempNode =nullptr;
			for (int p=0; p < sizex; ++p){
				jobject resulti = SaxonProcessor::sxn_environ->env->GetObjectArrayElement(results, p);
				tempNode = new XdmNode(this, resulti, ATTRIBUTE);
				tempNode->setProcessor(proc);
				this->incrementRefCount();
				attrValues[p] = tempNode;
			}
		}
	} 
	return attrValues;
    }

    int XdmNode::getAttributeCount(){
	if(attrCount == -1 && proc!= nullptr) {
		jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
		jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getAttributeCount",
					"(Lnet/sf/saxon/s9api/XdmNode;)I");
		
		if (!xmID) {
			std::cerr << "Error: SaxonDll." << "getAttributeCount"
				<< " not found\n" << std::endl;
			return 0;
		}
		jint result = (jlong)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID,
		value));

		attrCount =(int)result;
	}
	return attrCount;
    }

    int XdmNode::getChildCount(){
	if(childCount == -1 && proc!= nullptr) {
		jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
		jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getChildCount",
					"(Lnet/sf/saxon/s9api/XdmNode;)I");
		
		if (!xmID) {
			std::cerr << "Error: SaxonDll." << "getchildCount"
				<< " not found\n" << std::endl;
			return 0;
		}
		jint result = (jlong)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID,
		value));

		childCount =(int)result;
	}
	return childCount;
    }
    
    XdmNode** XdmNode::getChildren(){

	if(children == nullptr && proc!= nullptr) {
		jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
		jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getChildren",
					"(Lnet/sf/saxon/s9api/XdmNode;)[Lnet/sf/saxon/s9api/XdmNode;");
		
		if (!xmID) {
			std::cerr << "Error: SaxonDll." << "getchildren"
				<< " not found\n" << std::endl;
			return nullptr;
		}
		jobjectArray results = (jobjectArray)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID, 
		value));
		int sizex = SaxonProcessor::sxn_environ->env->GetArrayLength(results);
		childCount = sizex;	
		children =  new XdmNode*[sizex];
		XdmNode * tempNode = nullptr;
		for (int p=0; p < sizex; ++p){
			jobject resulti = SaxonProcessor::sxn_environ->env->GetObjectArrayElement(results, p);
			tempNode = new XdmNode(this, resulti, UNKNOWN);
			tempNode->setProcessor(proc);
			//tempNode->incrementRefCount();
			children[p] = tempNode;
		}
	}
	return children;

    }
  
