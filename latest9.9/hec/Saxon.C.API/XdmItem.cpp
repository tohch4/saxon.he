

#include "XdmItem.h"

XdmItem::XdmItem(): XdmValue(){
	value = NULL;
    stringValue = NULL;
}

    XdmItem::XdmItem(const XdmItem &other): XdmValue(other){
	value = (sxnc_value *)malloc(sizeof(sxnc_value));
        value->xdmvalue = other.value->xdmvalue;
	xdmSize =1;
	refCount = other.refCount;
	stringValue = NULL;
    }


XdmItem::XdmItem(jobject obj){
	value = (sxnc_value *)malloc(sizeof(sxnc_value));
        value->xdmvalue = obj;
	xdmSize =1;
	refCount =1;
    stringValue = NULL;
}

bool XdmItem::isAtomic(){
	return false;
}




   XdmItem * XdmItem::getHead(){ return this;}

  XdmItem * XdmItem::itemAt(int n){
	if (n < 0 || n >= size()) {
		return NULL;	
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
	if(value == NULL) {
		return NULL;	
	}
	return value->xdmvalue;
}

    const char * XdmItem::getStringValue(){
        if(stringValue == NULL) {
    		jclass xdmItemClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmItem");
    		jmethodID sbmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmItemClass,
    					"getStringValue",
    					"()Ljava/lang/String;");
    		if (!bmID) {
    			std::cerr << "Error: Saxonc." << "getStringValue"
    				<< " not found\n" << std::endl;
    			return NULL;
    		} else {
    			jstring result = (jstring)(SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, sbmID));
    			if(result) {
    					const char * str = (*(environi->env))->GetStringUTFChars(SaxonProcessor::sxn_environ->env, result, NULL);
    					stringValue = str;
                        return str;
    			}
    			return NULL;
    		}
    	} else {
    		return stringValue;
    	}
   }

	/**
	* Get the type of the object
	*/
	XDM_TYPE XdmItem::getType(){
		return XDM_ITEM;
	}
