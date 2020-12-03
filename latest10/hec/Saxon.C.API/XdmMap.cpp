#include "XdmMap.h"
#include <map>


XdmMap::XdmMap(): XdmFunctionItem() {

}

XdmMap::XdmMap(const XdmMap &d) {
      arity = d.arity;
      data = d.data; //TODO check if copy works
    }

XdmMap::XdmMap(jobject obj): XdmFunctionItem(obj) {}




    int XdmMap::mapSize(){
        return  data.size();
    }

    XdmValue * XdmMap::get(XdmAtomicValue* key) {
    jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
    static jmethodID tbmapID = nullptr;

    if(tbmapID == nullptr) {
    tbmapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "get",
    					"(Lnet/sf/saxon/s9api/XdmAtomicValue;)Lnet/sf/saxon/s9api/XdmValue;");

    }
    if (!tbmapID) {
    			std::cerr << "Error: Saxonc.XdmMap." << "get"
    				<< " not found\n" << std::endl;
    			return nullptr;
    } else {
    			jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, tbmapID, ((XdmItem *)key)->getUnderlyingValue()));
    			if(valueObj) {
    				XdmValue * newValue = new XdmValue((proc == nullptr ? nullptr : proc));
    				newValue->addUnderlyingValue(valueObj);
    				return newValue;
    			}
    			return nullptr;
    }

    }

    XdmValue * XdmMap::get(const char * key) {
        jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
        static jmethodID tbsmapID = nullptr;

        if(tbsmapID == nullptr) {
        tbsmapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "get",
        					"(Ljava/lang/String;)Lnet/sf/saxon/s9api/XdmValue;");

        }
        if (!tbsmapID) {
        			std::cerr << "Error: Saxonc.XdmMap." << "get"
        				<< " not found\n" << std::endl;
        			return nullptr;
        } else {
                jobject strObj = getJavaStringValue(SaxonProcessor::sxn_environ, key);
        		jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, tbsmapID, strObj));
        		if(valueObj) {
        			XdmValue * newValue = new XdmValue((proc == nullptr ? nullptr : proc));
       				newValue->addUnderlyingValue(valueObj);
       				return newValue;
       			}
       			return nullptr;
        }


    }

    XdmValue * XdmMap::get(int key) {
        jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
        static jmethodID tbimapID = nullptr;

        if(tbimapID == nullptr) {
        tbimapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "get",
        					"(I)Lnet/sf/saxon/s9api/XdmValue;");

        }
        if (!tbimapID) {
        			std::cerr << "Error: Saxonc.XdmMap." << "get"
        				<< " not found\n" << std::endl;
        			return nullptr;
        } else {
        		jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, tbimapID, key));
        		if(valueObj) {
        			XdmValue * newValue = new XdmValue((proc == nullptr ? nullptr : proc));
       				newValue->addUnderlyingValue(valueObj);
       				return newValue;
       			}
       			return nullptr;
        }


    }

    XdmValue *  XdmMap::get(double key) {
        jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
        static jmethodID tbdmapID = nullptr;

        if(tbdmapID == nullptr) {
            tbdmapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "get",
        					"(D)Lnet/sf/saxon/s9api/XdmValue;");

        }
        if (!tbdmapID) {
        			std::cerr << "Error: Saxonc.XdmMap." << "get"
        				<< " not found\n" << std::endl;
        			return nullptr;
        } else {
        		jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, tbdmapID, key));
        		if(valueObj) {
        			XdmValue * newValue = new XdmValue((proc == nullptr ? nullptr : proc));
       				newValue->addUnderlyingValue(valueObj);
       				return newValue;
       			}
       			return nullptr;
        }


    }

    XdmValue * XdmMap::get(long key) {
        jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
        static jmethodID tbdmapID = nullptr;

        if(tbdmapID == nullptr) {
            tbdmapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "get",
        					"(J)Lnet/sf/saxon/s9api/XdmValue;");

        }
        if (!tbdmapID) {
        			std::cerr << "Error: Saxonc.XdmMap." << "get"
        				<< " not found\n" << std::endl;
        			return nullptr;
        } else {
        		jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, tbdmapID, key));
        		if(valueObj) {
        			XdmValue * newValue = new XdmValue((proc == nullptr ? nullptr : proc));
       				newValue->addUnderlyingValue(valueObj);
       				return newValue;
       			}
       			return nullptr;
        }

    }

//TODO test this method
    XdmMap * XdmMap::put(XdmAtomicValue* key, XdmValue * valuei) {
        if(key == nullptr || valuei == nullptr) {
            std::cerr << "Error: Saxonc.XdmMap." << "key or value is nullptr" << std::endl;
            return nullptr;
        }
        jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
        static jmethodID putmapID = nullptr;

        if(putmapID == nullptr) {
                    putmapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "put",
                					"(Lnet/sf/saxon/s9api/XdmAtomicValue;Lnet/sf/saxon/s9api/XdmValue;)Lnet/sf/saxon/s9api/XdmMap;");

        }
        if (!putmapID) {
            std::cerr << "Error: Saxonc.XdmMap." << "put"
                	  << " not found\n" << std::endl;
            return nullptr;
        } else {
                		jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, putmapID, ((XdmItem *)key)->getUnderlyingValue(), valuei->getUnderlyingValue()));
                		if(valueObj) {
                			XdmMap * newValue = new XdmMap(valueObj);
               				return newValue;
               			}
               			return nullptr;
                }

    }

    XdmMap * XdmMap::remove(XdmAtomicValue* key){
                jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
                static jmethodID removemapID = nullptr;

                if(removemapID == nullptr) {
                    removemapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "remove",
                					"(Lnet/sf/saxon/s9api/XdmAtomicValue;)Lnet/sf/saxon/s9api/XdmMap;");

                }
                if (!removemapID) {
                			std::cerr << "Error: Saxonc.XdmMap." << "remove"
                				<< " not found\n" << std::endl;
                			return nullptr;
                } else {
                		jobject valueObj = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, removemapID, ((XdmItem *)key)->getUnderlyingValue()));
                		if(valueObj) {
                			XdmMap * newValue = new XdmMap(valueObj);
               				return newValue;
               			}
               			return nullptr;
                }



    }

    std::set<XdmAtomicValue*> XdmMap::keySet(){
        std::set<XdmAtomicValue *> myset;

        jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
        static jmethodID keySetmID = nullptr;

        if(keySetmID == nullptr) {
            keySetmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getXdmMapKeys",
                        					"(Lnet/sf/saxon/s9api/XdmMap)[Lnet/sf/saxon/s9api/XdmAtomicValue;");
            }
            if (!keySetmID) {
                std::cerr << "Error: SaxonDll." << "makeXdmMap"<< " not found\n" << std::endl;
                return myset;
            }


            jobjectArray results = (jobjectArray)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, keySetmID,value->xdmvalue));
            if(results) {
            	   int sizex = SaxonProcessor::sxn_environ->env->GetArrayLength(results);


                   	XdmAtomicValue * tempValue = nullptr;
                   		for (int p=0; p < sizex; ++p){
                   			jobject resulti = SaxonProcessor::sxn_environ->env->GetObjectArrayElement(results, p);
                   			tempValue = new XdmAtomicValue();
                   			tempValue->addUnderlyingValue(resulti);
                   			myset.push_back(tempValue);
                   		}
        }



         return myset;
    }

    //std::map<XdmAtomicValue*, XdmValue*> XdmMap::asMap();

    bool XdmMap::isEmpty() {
                jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
                static jmethodID isEmptyMapID = nullptr;

                if(isEmptyMapID == nullptr) {
                    isEmptyMapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "isEmpty",
                					"()Z");

                }
                if (!isEmptyMapID) {
                			std::cerr << "Error: Saxonc.XdmMap." << "isEmpty"
                				<< " not found\n" << std::endl;
                			return false;
                } else {
                		jboolean result = (SaxonProcessor::sxn_environ->env->CallBooleanMethod(value->xdmvalue, isEmptyMapID));
                		return result;
                }
    }

    bool XdmMap::containsKey(XdmAtomicValue * key) {
                jclass xdmMapClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/s9api/XdmMap");
                static jmethodID containsMapID = nullptr;

                if(containsMapID == nullptr) {
                    containsMapID = (jmethodID) SaxonProcessor::sxn_environ->env->GetMethodID(xdmMapClass, "containsKey",
                					"(Lnet/sf/saxon/s9api/XdmAtomicValue;)Z");

                }
                if (!containsMapID) {
                			std::cerr << "Error: Saxonc.XdmMap." << "containsKey"
                				<< " not found\n" << std::endl;
                			return false;
                } else {
                		jboolean result = (SaxonProcessor::sxn_environ->env->CallObjectMethod(value->xdmvalue, containsMapID, ((XdmItem *)key)->getUnderlyingValue()));
                		return result;
                }



    }

    std::list<XdmValue *> XdmMap::values() {
         std::list<XdmValue *> mylist;

            jclass xdmUtilsClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmUtils");
            jmethodID xmID = (jmethodID) SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmUtilsClass,"getXdmMapValues",
                        					"(Lnet/sf/saxon/s9api/XdmMap)[Lnet/sf/saxon/s9api/XdmValue;");
            if (!xmID) {
                std::cerr << "Error: SaxonDll." << "makeXdmMap"<< " not found\n" << std::endl;
                return mylist;
            }


            jobjectArray results = (jobjectArray)(SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmUtilsClass, xmID,value->xdmvalue));
            if(results) {
            	   int sizex = SaxonProcessor::sxn_environ->env->GetArrayLength(results);


                   	XdmValue * tempValue = nullptr;
                   	for (int p=0; p < sizex; ++p){
                   			jobject resulti = SaxonProcessor::sxn_environ->env->GetObjectArrayElement(results, p);
                   			tempValue = new XdmValue();
                   			tempValue->addUnderlyingValue(resulti);
                   			mylist.push_back(tempValue);

                   		}

            }


         return mylist;
    }