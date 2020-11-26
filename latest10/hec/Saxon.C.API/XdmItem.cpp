

#include "XdmItem.h"

XdmItem::XdmItem(): XdmValue(){
	value = nullptr;
}

    XdmItem::XdmItem(const XdmItem &other): XdmValue(other){
        value = other.value;
	    xdmSize =1;
	    refCount = other.refCount;
    }


XdmItem::XdmItem(jobject obj){
	value = obj;
	xdmSize =1;
	refCount =1;
}

bool XdmItem::isAtomic(){
	return false;
}




   XdmItem * XdmItem::getHead(){ return this;}

  XdmItem * XdmItem::itemAt(int n){
	if (n < 0 || n >= size()) {
		return nullptr;	
	}
	return this;
  }



 int XdmItem::size(){
	return 1;	
   }

jobject XdmItem::getUnderlyingValue(){
#ifdef DEBUG
	std::cerr<<std::endl<<"XdmItem-getUnderlyingValue:"<<std::endl; 
#endif 
	if(value == nullptr) {
		return nullptr;	
	}
	return value;
}

    const char * XdmItem::getStringValue(){
        if(stringValue.empty()) {
    		jclass xdmItemClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmItem");
    		jmethodID sbmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmItemClass,
    					"getStringValue",
    					"()Ljava/lang/String;");
    		if (!sbmID) {
    			std::cerr << "Error: Saxonc." << "getStringValue"
    				<< " not found\n" << std::endl;
    			return nullptr;
    		} else {
    			jstring result = (jstring)(SaxonProcessor::sxn_environ->env->CallObjectMethod(value, sbmID));
    			if(result) {
    					const char * str = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result, nullptr);
    					stringValue = std::string(str);
                        return stringValue.c_str();
    			}
    			return nullptr;
    		}
    	} else {
    		return stringValue.c_str();
    	}
   }

	/**
	* Get the type of the object
	*/
	XDM_TYPE XdmItem::getType(){
		return XDM_ITEM;
	}

XdmItem::~XdmItem() {
    //std::cerr<<std::endl<<"XdmItem destructor called, refCount"<<getRefCount()<<std::endl;
    if(getRefCount()<1){
        if(value !=nullptr && proc != nullptr && SaxonProcessor::jvmCreatedCPP>0) {
            SaxonProcessor::sxn_environ->env->DeleteGlobalRef(value);
        }
        free(value);
        if(stringValue.empty()) {
            stringValue.clear();
        }
    }
}

void XdmItem::incrementRefCount() {
    refCount++;
    //std::cerr<<"refCount-inc-xdmItem="<<refCount<<" ob ref="<<(this)<<std::endl;
}

void XdmItem::decrementRefCount() {
    if (refCount > 0)
        refCount--;
    //std::cerr<<"refCount-dec-xdmItem="<<refCount<<" ob ref="<<(this)<<std::endl;
}
