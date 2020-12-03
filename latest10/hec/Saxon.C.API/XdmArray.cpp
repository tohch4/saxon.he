#include "XdmArray.h"



XdmArray::XdmArray(): XdmFunctionItem(){


}

XdmArray::XdmArray(const XdmArray &d): XdmFunctionItem(d) {
}



XdmArray::XdmArray(jobject obj) : XdmFunctionItem(obj){}

int XdmArray::arrayLength() const {
    jclass xdmArrayClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmArray");
    static jmethodID arLenmID = nullptr;

    if(arLenmID == nullptr) {

        arLenmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmArrayClass, "arrayLength",
                                                                          "()I");
    }

    if (!arLenmID) {
        std::cerr << "Error: Saxonc.XdmArray." << "arrayLength"
                  << " not found\n" << std::endl;
        return 0;
    } else {
        jint valuei = (SaxonProcessor::sxn_environ->env->CallIntMethod(value, arLenmID));
        return valuei;
    }
}

XdmValue* XdmArray::get(int n){
    jclass xdmArrayClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmArray");
    static jmethodID tbmID = nullptr;

    if(tbmID == nullptr) {

        tbmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmArrayClass, "get",
    					"(I)Lnet/sf/saxon/s9api/XdmValue;");
    }

    if (!tbmID) {
    			std::cerr << "Error: Saxonc.XdmArray." << "get"
    				<< " not found\n" << std::endl;
    			return nullptr;
    } else {
    			jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, tbmID, n));
    			if(valueObj) {
    				auto * newValue = new XdmValue((proc == nullptr ? nullptr : proc));
    				newValue->addUnderlyingValue(valueObj);
    				return newValue;
    			}
    			return nullptr;
    }
}

XdmArray* XdmArray::put(int n, XdmValue * valuei) {
    jclass xdmArrayClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmArray");
    static jmethodID tbpmID = nullptr;

    if(tbpmID == nullptr) {
        tbpmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmArrayClass, "put",
    					"(I;Lnet/sf/saxon/s9api/XdmValue;)Lnet/sf/saxon/s9api/XdmArray;");
    }
    if (!tbpmID) {
    			std::cerr << "Error: Saxonc.XdmArray." << "put"
    				<< " not found\n" << std::endl;
    			return nullptr;
    } else {
    			jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, tbpmID, n, valuei->getUnderlyingValue()));
    			if(valueObj) {
    				auto * newValue = new XdmArray(valueObj);
    				return newValue;
    			}
    			return nullptr;
    }


}

XdmArray* XdmArray::addMember(XdmValue* valuei) {
    jclass xdmArrayClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmArray");
    static jmethodID tbamID = nullptr;

    if(tbamID) {
        tbamID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmArrayClass, "concat",
    					"(Lnet/sf/saxon/s9api/XdmValue;)Lnet/sf/saxon/s9api/XdmArray;");

    }

    if (!tbamID) {
    			std::cerr << "Error: Saxonc.XdmArray." << "concat"
    				<< " not found\n" << std::endl;
    			return nullptr;
    } else {
    			jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, tbamID, valuei->getUnderlyingValue()));
    			if(valueObj) {
    				auto * newValue = new XdmArray(valueObj);
    				return newValue;
    			}
    			return nullptr;
    }
}

XdmArray* XdmArray::concat(XdmArray* valuei) {
    jclass xdmArrayClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmArray");
    static jmethodID tbcaID = nullptr;


    if(tbcaID == nullptr) {
        tbcaID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmArrayClass, "concat",
    					"(Lnet/sf/saxon/s9api/XdmArray;)Lnet/sf/saxon/s9api/XdmArray;");
    }

    if (!tbcaID) {
    			std::cerr << "Error: Saxonc.XdmArray." << "concat"
    				<< " not found\n" << std::endl;
    			return nullptr;
    } else {
    			jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value, tbcaID, valuei->getUnderlyingValue()));
    			if(valueObj) {
    				auto * newValue = new XdmArray(valueObj);
    				return newValue;
    			}
    			std::cerr << "Error: Saxonc.XdmArray." << "concat" << " failed\n" << std::endl;
    			return nullptr;
    }

}

std::list<XdmValue *> XdmArray::asList() {
  std::list<XdmValue *> arr;

	jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
	static jmethodID xmID = nullptr;


	if(xmID == nullptr) {
	    xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"convertXdmArrayToArrayObject",
					"(Lnet/sf/saxon/s9api/XdmArray;)[Lnet/sf/saxon/s9api/XdmValue;");
	}

	if (!xmID) {
			std::cerr << "Error: SaxonDll." << "convertXdmArrayToArrayObject"
				<< " not found\n" << std::endl;
			return arr;
		}


	auto results = (jobjectArray)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID,value));
	if(results) {
	   int sizex = SaxonProcessor::sxn_environ->env->GetArrayLength(results);


       		XdmValue * tempValue;
       		for (int p=0; p < sizex; ++p){
       			jobject resulti = SaxonProcessor::sxn_environ->env->GetObjectArrayElement(results, p);
       			tempValue = new XdmValue();
       			tempValue->addUnderlyingValue(resulti);
       			arr.push_back(tempValue);

       		}
       	return arr;

	} else {
	    return arr;
	}


}
