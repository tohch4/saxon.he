#include "XdmValue.h"
#include "XdmItem.h"
#include "XdmAtomicValue.h"
#include "XdmNode.h"
#include "XdmFunctionItem.h"
#include "XdmMap.h"
#include "XdmArray.h"

XdmValue::XdmValue(const XdmValue &other) {
	//SaxonProcessor *proc = other.proc; //TODO
	valueType = other.valueType;
	refCount = 1;
	xdmSize = other.xdmSize;
	jValues = other.jValues;
	toStringValue = other.toStringValue;
	values.resize(0);//TODO memory issue might occur here
	toStringValue = other.toStringValue;
	for (int i = 0; i < xdmSize; i++) {
		addXdmItem(other.values[i]);
	}
	
}

const char * XdmValue::toString() {
	if (toStringValue.empty()) {
		jclass xdmValueClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmValue");
		jmethodID strMID2 = (jmethodID)SaxonProcessor::sxn_environ->env->GetMethodID(xdmValueClass,
			"toString",
			"()Ljava/lang/String;");
		if (!strMID2) {
			std::cerr << "Error: Saxonc.XdmValue." << "toString"
				<< " not found\n" << std::endl;
			return nullptr;
		}
		else {
			for(int i=0; i<size();i++) {

			
				jstring result = (jstring)(SaxonProcessor::sxn_environ->env->CallObjectMethod(itemAt(i)->getUnderlyingValue(), strMID2));
				if (result) {
					toStringValue += SaxonProcessor::sxn_environ->env->GetStringUTFChars(result, nullptr);
					SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
				
				}
			}

			return toStringValue.c_str();			
		}
			
		
	}
	else {
		return toStringValue.c_str();
	}

}

void XdmValue::setProcessor(SaxonProcessor *p) { 
		proc = p;
	}


int XdmValue::size() {
	return xdmSize;
}

XdmValue::XdmValue(jobject val) {
	XdmItem * value = new XdmItem(val);
	values.resize(0);//TODO memory issue might occur here
	values.push_back(value);
	xdmSize++;
	jValues = nullptr;
	valueType = nullptr;
}


XdmValue::XdmValue(jobject val, bool arr){
	xdmSize = 0;
	values.resize(0);
	jValues = nullptr;
	valueType = nullptr;
	jclass xdmValueForcppClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmValueForCpp");
	jmethodID xvfMID = SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmValueForcppClass, "makeArrayFromXdmValue", "(Lnet/sf/saxon/s9api/XdmValue;)[Lnet/sf/saxon/s9api/XdmItem;");

	if(!xvfMID){

		std::cerr << "Error: SaxonDll." << "makeArrayFromXdmValue"
				<< " not found\n" << std::endl;
			return ;
	}
	
	jobjectArray results = (jobjectArray) SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmValueForcppClass, xvfMID, val);
	if(results){
	int sizex = SaxonProcessor::sxn_environ->env->GetArrayLength(results);
	if (sizex>0) {
		jclass atomicValueClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmAtomicValue");
		jclass nodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
		jclass functionItemClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmFunctionItem");

		//XdmValue * value = new XdmValue();
		//value->setProcessor(proc);
		XdmItem * xdmItem = nullptr;
		for (int p=0; p < sizex; ++p) 
		{
			jobject resulti = SaxonProcessor::sxn_environ->env->GetObjectArrayElement(results, p);
			//value->addUnderlyingValue(resulti);

			if(SaxonProcessor::sxn_environ->env->IsInstanceOf(resulti, atomicValueClass)           == JNI_TRUE) {
				xdmItem = new XdmAtomicValue(SaxonProcessor::sxn_environ->env->NewGlobalRef(resulti));
				

			} else if(SaxonProcessor::sxn_environ->env->IsInstanceOf(resulti, nodeClass)           == JNI_TRUE) {
				xdmItem = new XdmNode(SaxonProcessor::sxn_environ->env->NewGlobalRef(resulti));


			} else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(resulti, functionItemClass)           == JNI_TRUE) {
			      //Not Supported yet
#if CVERSION_API_NO >= 123
				xdmItem = new XdmFunctionItem(SaxonProcessor::sxn_environ->env->NewGlobalRef(resulti));
#endif
			}
			//xdmItem->setProcessor(proc);
			addXdmItem(xdmItem);
			SaxonProcessor::sxn_environ->env->DeleteLocalRef(resulti);
		}
	}
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(results);
	}	
}


XdmValue::~XdmValue() {
	bool safeToClear = false;
    if(getRefCount()<1){
	//proc->env->ReleaseObject
	for (size_t i = 0; i < values.size(); i++) {
		if (values[i] != nullptr && values[i]->getRefCount() < 1) {
			delete values[i];
			safeToClear = true;
		} else {
			safeToClear = false;
		}
	}
	if(safeToClear) {
		values.clear();
	}
	if (valueType != nullptr) { delete valueType; }
	if (jValues && proc != nullptr) {
		SaxonProcessor::sxn_environ->env->DeleteLocalRef(jValues);  //TODO check - this maybe should be DeleteGlobalRef  - check where it is created
	}
	xdmSize = 0;
	if(!toStringValue.empty()) {
		toStringValue.clear();
	}
    }

}

void XdmValue::addXdmItem(XdmItem* val) {
	if (val != nullptr) {
		values.push_back(val);
		xdmSize++;
		jValues = nullptr; //TODO clear jni array from memory if needed
	}
}


void XdmValue::addUnderlyingValue(jobject val) {
	XdmItem * valuei = new XdmItem(val);
	valuei->setProcessor(proc);
	values.push_back(valuei);
	xdmSize++;
	jValues = nullptr; //TODO clear jni array from memory if needed

}

void XdmValue::incrementRefCount() {
		refCount++;
		//std::cerr<<"refCount-inc-xdmVal="<<refCount<<" ob ref="<<(this)<<std::endl;
	}

void XdmValue::decrementRefCount() {
		if (refCount > 0)
			refCount--;
		//std::cerr<<"refCount-dec-xdmVal="<<refCount<<" ob ref="<<(this)<<std::endl;
	}




XdmItem * XdmValue::getHead() {
	if (values.size() > 0) {
		return values[0];
	}
	else {
		return nullptr;
	}
}

jobject XdmValue::getUnderlyingValue() {
	if (jValues == nullptr) {
		int i;
		JNIEnv *env = SaxonProcessor::sxn_environ->env;
		int count = values.size();
		if (count == 0) {
			return nullptr;
		}
		jclass objectClass = lookForClass(env,
			"net/sf/saxon/s9api/XdmItem");
		jValues = (jobjectArray)env->NewObjectArray((jint)count, objectClass, 0);

		for (i = 0; i < count; i++) {
			env->SetObjectArrayElement(jValues, i, values[i]->getUnderlyingValue());
		}
	} 
	return (jobject)jValues;
}

void XdmValue::releaseXdmValue() {



}

XdmItem * XdmValue::itemAt(int n) {
	if (n >= 0 && (unsigned int)n < values.size()) {
		return values[n];
	}
	return nullptr;
}

/**
* Get the type of the object
*/
XDM_TYPE XdmValue::getType() {
	return XDM_VALUE;
}




