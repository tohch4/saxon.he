
//

#include "XdmFunctionItem.h"

    XdmFunctionItem::XdmFunctionItem():XdmItem(), arity(-1){}


    XdmFunctionItem::XdmFunctionItem(const XdmFunctionItem &aVal): XdmItem(aVal){
        arity = aVal.arity;
    }

   
	

    XdmFunctionItem::XdmFunctionItem(jobject obj):XdmItem(obj), arity(-1){
    }

    const char* XdmFunctionItem::getName(){
          if(fname.empty()) {
             jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
             		static jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getFunctionName",
             					"(Lnet/sf/saxon/s9api/XdmFunctionItem;)Ljava/lang/String;");
             		if (!xmID) {
             			std::cerr << "Error: SaxonC." << "getFunctionName"
             				<< " not found\n" << std::endl;
             			return nullptr;
             		} else {
             			jstring result = (jstring)(SaxonProcessor::sxn_environ->env->CallStaticIntMethod(xdmUtilsClass, xmID, value));
             			if(result) {
                        			const char * stri = SaxonProcessor::sxn_environ->env->GetStringUTFChars(result, nullptr);

                        		    SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                        			fname = std::string(stri);
                        			return stri;
                        }
             		}

          } else {
            return fname.c_str();
          }

    }

    int XdmFunctionItem::getArity(){
          if(arity == -1) {
             jclass xdmFunctionClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmFunctionItem");
             		static jmethodID bfmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmFunctionClass,
             					"getArity",
             					"()I");
             		if (!bfmID) {
             			std::cerr << "Error: SaxonC." << "getArity"
             				<< " not found\n" << std::endl;
             			return false;
             		} else {
             			jint result = (jint)(SaxonProcessor::sxn_environ->env->CallIntMethod(value, bfmID));
             			return (int)result;
             		}

          } else {
            return arity;
          }

    }

    XdmFunctionItem * XdmFunctionItem::getSystemFunction(SaxonProcessor * processor, const char * name, int arity){
        if(processor == nullptr || name == nullptr) {
            std::cerr << "Error in getSystemFunction. Please make sure processor and name are not nullptr." << std::endl;
             return nullptr;
        }
             jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
             static jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getSystemFunction",
             "(Lnet/sf/saxon/s9api/Procesor;Ljava/lang/String;I)Lnet/sf/saxon/s9api/XdmFunctionItem");
             if (!xmID) {
                       std::cerr << "Error: SaxonC." << "getSystemFunction" << " not found\n" << std::endl;
                         			return nullptr;
                         		} else {
                         			jobject result = (jobject)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID, processor->proc, SaxonProcessor::sxn_environ->env->NewStringUTF(name), arity);
                         			if(result) {
                         			    auto functionItem = new XdmFunctionItem(result);
                         			    return functionItem;

                         			} else {
                         			    return nullptr;

                         			}

                         		}



    }

    XdmValue * XdmFunctionItem::call(XdmValue ** arguments, int argument_length) {
          if(argument_length > 0 && arguments == nullptr) {
                      std::cerr << "Error in XdmFunctionItem.call.  nullptr arguments found." << std::endl;
                      return nullptr;
          }
          if(proc != nullptr) {
                       jclass xdmFunctionClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmFunctionItem");
                       static jmethodID xffmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetObjectMethodID(xdmFunctionClass,"call",
                       "(Lnet/sf/saxon/s9api/Procesor;[Lnet/sf/saxon/s9api/XdmValue)Lnet/sf/saxon/s9api/XdmValue");
                       if (!xffmID) {
                                 std::cerr << "Error: SaxonC." << "getSystemFunction" << " not found\n" << std::endl;
                                 return nullptr;
                       } else {

                                 jobjectArray argumentJArray = SaxonProcessor::createJArray(arguments, argument_length);
                                 jobject result = (jobject)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xffmID, processor->proc, argumentJArray);
                                 if(argumentJArray != nullptr) {
                                    	SaxonProcessor::sxn_environ->env->DeleteLocalRef(argumentJArray);


                                 }
                                 if(result) {
                                        jclass atomicValueClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmAtomicValue");
                                        jclass nodeClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmNode");
                                        jclass functionItemClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmFunctionItem");
                                        XdmValue * value = nullptr;
                                        XdmItem * xdmItem = nullptr;


                                        if(SaxonProcessor::sxn_environ->env->IsInstanceOf(result, atomicValueClass)           == JNI_TRUE) {
                                                xdmItem =  new XdmAtomicValue(result);
                                                xdmItem->setProcessor(proc);
                                                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                                                return xdmItem;

                                        } else if(SaxonProcessor::sxn_environ->env->IsInstanceOf(result, nodeClass)           == JNI_TRUE) {
                                                xdmItem =  new XdmNode(result);
                                                xdmItem->setProcessor(proc);
                                                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                                                return xdmItem;
                                        } else if (SaxonProcessor::sxn_environ->env->IsInstanceOf(result, functionItemClass)           == JNI_TRUE) {
                                                xdmItem =  new XdmFunctionItem(result);
                                                xdmItem->setProcessor(proc);
                                                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                                                return xdmItem;
                                        } else {
                                                value = new XdmValue(result, true);
                                                value->setProcessor(proc);
                                                for(int z=0;z<value->size();z++) {
                                                	value->itemAt(z)->setProcessor(proc);
                                                }
                                                SaxonProcessor::sxn_environ->env->DeleteLocalRef(result);
                                                return value;
                                        }

                                 } else {
                                 	    return nullptr

                                 }

                       }
      } else {
         std::cerr << "Error in XdmFunctionItem.call.  Processor not set on the xdmFunctionItem." << std::endl;
         return nullptr;
      }

    }


