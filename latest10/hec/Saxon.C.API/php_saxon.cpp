#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "php_saxon.h"

#ifdef COMPILE_DL_SAXON
    extern "C" {
        ZEND_GET_MODULE(saxon)
    }
#endif

JNINativeMethod phpMethods[] =
{
    {
         "_phpCall",
         "(Ljava/lang/String;[Ljava/lang/Object;[Ljava/lang/String;)Ljava/lang/Object;",
         (void *)&phpNativeCall
    }
};

zend_object_handlers saxonProcessor_object_handlers;
zend_object_handlers xsltProcessor_object_handlers;
zend_object_handlers xqueryProcessor_object_handlers;
zend_object_handlers xpathProcessor_object_handlers;
zend_object_handlers schemaValidator_object_handlers;
zend_object_handlers xdmValue_object_handlers;
zend_object_handlers xdmItem_object_handlers;
zend_object_handlers xdmNode_object_handlers;
zend_object_handlers xdmAtomicValue_object_handlers;

zend_class_entry *saxonProcessor_ce;
zend_class_entry *xsltProcessor_ce;
zend_class_entry *xqueryProcessor_ce;
zend_class_entry *xpathProcessor_ce;
zend_class_entry *schemaValidator_ce;
zend_class_entry *xdmValue_ce;
zend_class_entry *xdmItem_ce;
zend_class_entry *xdmNode_ce;
zend_class_entry *xdmAtomicValue_ce;

void SaxonProcessor_free_storage(void *object TSRMLS_DC)
{
 
    saxonProcessor_object *obj = (saxonProcessor_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value saxonProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    saxonProcessor_object *obj = (saxonProcessor_object *)emalloc(sizeof(saxonProcessor_object));
    memset(obj, 0, sizeof(saxonProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, nullptr, SaxonProcessor_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &saxonProcessor_object_handlers;

    return retval;
}

PHP_METHOD(SaxonProcessor, __construct)
{

    if (ZEND_NUM_ARGS()>2) {
        WRONG_PARAM_COUNT;
    }

    char * cwdi = nullptr;
   bool license = false;
    int len1;
    if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &license) == FAILURE) {
        php_error(E_WARNING,"Wrong SaxonProcessor argument");
        RETURN_nullptr();
    } 


    if (ZEND_NUM_ARGS()>1 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "bs", &license, &cwdi, &len1) == FAILURE) {
        php_error(E_WARNING,"Wrong SaxonProcessor arguments");
        RETURN_nullptr();
    }
 

    zval *object = getThis();
    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj = (saxonProcessor_object *)zend_object_store_get_object(object TSRMLS_CC);
    saxonProc =  obj->saxonProcessor;
    if(saxonProc == nullptr) {

	saxonProc = new SaxonProcessor(license); //TODO: add license flag to PHP function argument

	obj->saxonProcessor = saxonProc;
    }

    if(cwdi==nullptr) {
#if !(defined (__linux__) || (defined (__APPLE__) && defined(__MACH__)))
	    TCHAR s[256];

            // --
            DWORD a = GetCurrentDirectory(256, s);
	    const size_t newsize = wcslen(s)*2;
	    char* cwd = new char[newsize];
	    wcstombs_s(0, cwd, newsize, s, _TRUNCATE);
	    // -- code above returns the apache installtion directory as the CWD

	    char* cwd2;

	    //php_error(E_WARNING,cwd2);

	    saxonProc->setcwd(cwd2);
	    // -- code above tries to use VCWD_GETCWD but there is a linkage error
#else

	    char cwd[256];

	    VCWD_GETCWD(cwd, sizeof(cwd));
	    if(cwd == nullptr) {
	     //php_error(E_WARNING,"cwd is nullptr");
	   }else {
             //php_error(E_WARNING,cwd);

	    saxonProc->setcwd(cwd);
          }
#endif

    } else {
        saxonProc->setcwd(cwdi);
    }

}

PHP_METHOD(SaxonProcessor, __destruct)
{
    
    saxonProcessor_object *obj = (saxonProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    SaxonProcessor * saxonProc= obj->saxonProcessor;

    delete saxonProc;
}

PHP_METHOD(SaxonProcessor, setResourcesDirectory)
{
    SaxonProcessor *saxonProcessor;
    char * dirStr;
    int len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dirStr, &len) == FAILURE) {
        RETURN_nullptr();
    }
    
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
       
        if(dirStr != nullptr) {
            saxonProcessor->setResourcesDirectory(dirStr);
        }
    }
}

PHP_METHOD(SaxonProcessor, setcwd)
{
    SaxonProcessor *saxonProcessor;
    char * cwdStr;
    int len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cwdStr, &len) == FAILURE) {
        RETURN_nullptr();
    }
    
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
        
        if(cwdStr != nullptr) {
            saxonProcessor->setcwd(cwdStr);
        }
    }
}


PHP_METHOD(SaxonProcessor, parseXmlFromString)
{
    SaxonProcessor * saxonProcessor;
    char * source;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != nullptr);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
        XdmNode* node = saxonProcessor->parseXmlFromString(source);
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_nullptr();
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmNode = node;
            }
        } else {
            if(obj->saxonProcessor->exceptionOccurred()){
		//TODO throw exception
	    }
        }
    } else {
        RETURN_nullptr();
    }
}

PHP_METHOD(SaxonProcessor, parseXmlFromFile)
{
    SaxonProcessor * saxonProcessor;
    char * source;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != nullptr);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
        XdmValue* node = (XdmValue*)saxonProcessor->parseXmlFromFile(source);//TODO this needs to be XdmNode object
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_nullptr();
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmValue = node;
            }
        } else {
           // obj->xsltProcessor->checkException();//TODO
        }
    } else {
        RETURN_nullptr();
    }
}


PHP_METHOD(SaxonProcessor, createAtomicValue)
{
    XdmAtomicValue * xdmValue = nullptr;
    SaxonProcessor * proc;
    char * source;
    int len1;
    zval *zvalue;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&zvalue) == FAILURE) {
        RETURN_nullptr();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
	switch (Z_TYPE_P(zvalue)) {
            case IS_BOOL:
                bVal = Z_BVAL_P(zvalue);
                xdmValue = proc->makeBooleanValue((bool)bVal);
            break;
            case IS_LONG:
                iVal = Z_LVAL_P(zvalue);
		 xdmValue = proc->makeIntegerValue((int)iVal);
            break;
            case IS_STRING:
                sVal = Z_STRVAL_P(zvalue);
                len = Z_STRLEN_P(zvalue);
                xdmValue = proc->makeStringValue((const char*)sVal);
            break;
            case IS_nullptr:
                xdmValue = new XdmAtomicValue();
            break;
            case IS_DOUBLE:
                dVal = (double)Z_DVAL_P(zvalue);
		xdmValue = proc->makeDoubleValue((double)iVal);
                break;
            case IS_ARRAY:
                // TODO: Should not be expected. Do this some other way
                //break;
            case IS_OBJECT:
                // TODO: implement this
                //break;
            default:
                obj = nullptr;
                zend_throw_exception(zend_exception_get_default(TSRMLS_C), "unknown type specified in XdmValue", 0 TSRMLS_CC); 
                RETURN_nullptr();
        }
        if(xdmValue == nullptr) {
            RETURN_nullptr();
        }
        if (object_init_ex(return_value, xdmAtomicValue_ce) != SUCCESS) {
            RETURN_nullptr();
        } else {
            struct xdmAtomicValue_object* vobj = (struct xdmAtomicValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
            assert (vobj != nullptr);
            vobj->xdmAtomicValue = xdmValue;
        }
    } else {
       
        RETURN_nullptr();
    }
}


PHP_METHOD(SaxonProcessor, newXPathProcessor)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XPathProcessor * xpathProcessor = nullptr;
    if((saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC) == nullptr) {
      
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
if (object_init_ex(return_value, xpathProcessor_ce) != SUCCESS) {
            RETURN_nullptr();
        } else {
	   struct xpathProcessor_object* vobji = (struct xpathProcessor_object *)zend_object_store_get_object(return_value TSRMLS_CC);
            assert (vobji != nullptr);
	    xpathProcessor = proc->newXPathProcessor();
            vobji->xpathProcessor = xpathProcessor;
	}
    } else {
       
        RETURN_nullptr();
    }
}

PHP_METHOD(SaxonProcessor, newXsltProcessor)
{
    //php_error(E_WARNING,"new xsltProc 1");
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XsltProcessor * xsltProcessor = nullptr;
if((saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC) == nullptr) {
   
}

    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
    if (object_init_ex(return_value, xsltProcessor_ce) != SUCCESS) {
            RETURN_nullptr();
        } else {
	   struct xsltProcessor_object* vobj = (struct xsltProcessor_object *)zend_object_store_get_object(return_value TSRMLS_CC);
	
            assert (vobj != nullptr);
	    xsltProcessor = proc->newXsltProcessor();
            vobj->xsltProcessor = xsltProcessor;
    }
    } else {
       
        RETURN_nullptr();
    }
}

PHP_METHOD(SaxonProcessor, newXQueryProcessor)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XQueryProcessor * xqueryProcessor = nullptr;
   if((saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC) == nullptr) {
   
   }
   
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
	if (object_init_ex(return_value, xqueryProcessor_ce) != SUCCESS) {
            RETURN_nullptr();
        } else {
	   struct xqueryProcessor_object* vobj = (struct xqueryProcessor_object *)zend_object_store_get_object(return_value TSRMLS_CC);

            assert (vobj != nullptr);
	    xqueryProcessor = proc->newXQueryProcessor();
            vobj->xqueryProcessor = xqueryProcessor;
     }
    } else {
       
        RETURN_nullptr();
    }
}

PHP_METHOD(SaxonProcessor, newSchemaValidator)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    SchemaValidator * schemaValidator = nullptr;
   
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
	if (object_init_ex(return_value, schemaValidator_ce) != SUCCESS) {
            RETURN_nullptr();
        } else {
	   struct schemaValidator_object* vobj = (struct schemaValidator_object *)zend_object_store_get_object(return_value TSRMLS_CC);
            assert (vobj != nullptr);
	    schemaValidator = proc->newSchemaValidator();
	    if(schemaValidator == nullptr){
		RETURN_nullptr();
	    }
            vobj->schemaValidator = schemaValidator;
	}
    } else {
       
        RETURN_nullptr();
    }
}


PHP_METHOD(SaxonProcessor, version)
{
    SaxonProcessor *saxonProcessor;
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
        char *str = estrdup(saxonProcessor->version());
        RETURN_STRING(str, 0);
    }
    RETURN_nullptr();
}


PHP_METHOD(SaxonProcessor, setConfigurationProperty)
{
    SaxonProcessor *saxonProcessor;
    char * name;
    int len1;
    char * value;
    int len2;
    if (ZEND_NUM_ARGS()!= 2) {
        WRONG_PARAM_COUNT;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_nullptr();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr && name != nullptr && value != nullptr) {
        saxonProcessor->setConfigurationProperty(name, value);
    }
    
}

PHP_METHOD(SaxonProcessor, registerPHPFunction)
{
    SaxonProcessor *saxonProcessor;
    char * libName;
    int len1;
 //std::cerr<<"checkpoint in registerPHPFunction start"<<std::endl;
    if (ZEND_NUM_ARGS()!= 1) {
        WRONG_PARAM_COUNT;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &libName, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    saxonProcessor_object *obj = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr && libName != nullptr) {
        saxonProcessor->setConfigurationProperty("extc", libName);
    }
saxonProcessor->registerNativeMethods(SaxonProcessor::sxn_environ->env, "com/saxonica/functions/extfn/cpp/PHPFunctionSet$PhpFunction",
    phpMethods, sizeof(phpMethods) / sizeof(phpMethods[0]));
   // std::cerr<<"checkpoint in registerPHPFunction end"<<std::endl;
}

/*     ============== XSLT10/20/30/31: PHP Interface of   XsltProcessor class =============== */

void XsltProcessor_free_storage(void *object TSRMLS_DC)
{

    xsltProcessor_object *obj = (xsltProcessor_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);

}

zend_object_value xsltProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;

    xsltProcessor_object *obj = (xsltProcessor_object *)emalloc(sizeof(xsltProcessor_object));
    memset(obj, 0, sizeof(xsltProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, nullptr, XsltProcessor_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &xsltProcessor_object_handlers;

    return retval;
}



PHP_METHOD(XsltProcessor, __destruct)
{
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    XsltProcessor * xsltProcessor= obj->xsltProcessor;

    delete xsltProcessor;
    
}

PHP_METHOD(XsltProcessor, transformFileToFile)
{
    XsltProcessor *xsltProcessor;
    char * outfileName;
    char * infilename;
    char * styleFileName;
    int len1, len2, len3;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &infilename, &len1, &styleFileName, &len2, &outfileName, &len3) == FAILURE) {
        RETURN_nullptr();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
 
    if (xsltProcessor != nullptr) {

        xsltProcessor->transformFileToFile(infilename, styleFileName, outfileName);
        if(xsltProcessor->exceptionOccurred()) {
     	  // TODO: throw exception
        }
    }
}

PHP_METHOD(XsltProcessor, transformFileToValue)
{
    XsltProcessor *xsltProcessor;
    char * infilename;
    char * styleFileName;
    int len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_nullptr();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    
    if (xsltProcessor != nullptr) {

        XdmValue * node = xsltProcessor->transformFileToValue(infilename, styleFileName);
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_nullptr();
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmValue = node;
            }
        } else {
            if(obj->xsltProcessor->exceptionOccurred()){
  		//TODO
	    }
        }
    }else {
        RETURN_nullptr();
    }
}


PHP_METHOD(XsltProcessor, transformFileToString)
{
    XsltProcessor *xsltProcessor;
    char * infilename;
    char * styleFileName;
    int len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_nullptr();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

        const char * result = xsltProcessor->transformFileToString(infilename, styleFileName);
	if(result != nullptr) {
            char *str = estrdup(result);
            RETURN_STRING(str, 0);
        } else if(xsltProcessor->exceptionOccurred()){
            //TODO: xsltProcessor->checkException();
            const char * errStr = xsltProcessor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xsltProcessor->getErrorCode(0);
                if(errorCode!=nullptr) {
                    // TODO: throw exception
                }
            }
        }
    }
}

/*enum saxonTypeEnum
{
	enumNode,
	enumString,
	enumInteger,
	enumDouble,
	enumFloat,
	enumBool,
	enumArrXdmValue
};*/

jobject JNICALL phpNativeCall
  (JNIEnv *env, jobject object, jstring funcName, jobjectArray arguments, jobjectArray argTypes){
	JNIEnv *senv = SaxonProcessor::sxn_environ->env;

	const char *nativeString = senv->GetStringUTFChars(funcName, nullptr);

	if(nativeString == nullptr) {
		return nullptr;
	}

	zval *function_name;
	zval *retval;
	
	int argLength = 0;
	zvalArr * php_argv= nullptr;
	if(arguments != nullptr) {
		argLength = (int)senv->GetArrayLength(arguments);
		php_argv = new zvalArr[argLength];
	}
	zval **params[argLength];
	if(argLength>0) {
		(*params) = (zval**)malloc(sizeof(zval*) * argLength);
	} else {
		(*params) = nullptr;
	}
	std::map<std::string, saxonTypeEnum> typeMap;
	typeMap["node"] = enumNode;
	typeMap["string"] = enumString;
	typeMap["integer"] = enumInteger;
	typeMap["double"] = enumDouble;
	typeMap["float"] = enumFloat;
	typeMap["boolean"] = enumBool;
	typeMap["[xdmvalue"] = enumArrXdmValue;
	sxnc_value* sresult = (sxnc_value *)malloc(sizeof(sxnc_value));
	SaxonProcessor * nprocessor = new SaxonProcessor(true); //processor object created for XdmNode
	jclass xdmValueForCppClass = lookForClass(SaxonProcessor::sxn_environ->env, "net/sf/saxon/option/cpp/XdmValueForCpp");
	jmethodID procForNodeMID = SaxonProcessor::sxn_environ->env->GetStaticMethodID(xdmValueForCppClass, "getProcessor", "(Ljava/lang/Object;)Lnet/sf/saxon/s9api/Processor;");
	
	for(int i=0; i<argLength;i++){

		jstring argType = (jstring)senv->GetObjectArrayElement(argTypes, i);
		jobject argObj = senv->GetObjectArrayElement(arguments, i);

		const char * str = senv->GetStringUTFChars(argType,nullptr);
		const char *stri = nullptr;
		double dnumber = 0;
		long lnumber = 0;
		bool bvalue = false;
		float fnumber = 0;
		

         
		struct xdmNode_object* vobj;
		XdmNode * node = nullptr;
		std::map<std::string, saxonTypeEnum>::iterator it = typeMap.find(str);
		if (it != typeMap.end()){
			switch (it->second)
			{
				case enumNode:
					if(!nprocessor->proc){
						nprocessor->proc = (jobject)SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmValueForCppClass, procForNodeMID, argObj);
					}
					node = new XdmNode(argObj);
					node->setProcessor(nprocessor);

					MAKE_STD_ZVAL(php_argv[i]._val);
					object_init_ex(php_argv[i]._val, xdmNode_ce);
   					vobj = (struct xdmNode_object *)zend_object_store_get_object(php_argv[i]._val TSRMLS_CC);
                			assert (vobj != nullptr);
                			vobj->xdmNode = node;
					break;
				case enumString:
					stri = senv->GetStringUTFChars((jstring)argObj, nullptr);
					MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_STRING(php_argv[i]._val, stri, 1);
					break;
				case enumInteger:
					sresult->xdmvalue = argObj; 
					lnumber = getLongValue(*SaxonProcessor::sxn_environ, *sresult, 0);

					MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_LONG(php_argv[i]._val, lnumber);					
					break;
				case enumDouble:
					sresult->xdmvalue = argObj; 
					dnumber = getDoubleValue(*SaxonProcessor::sxn_environ, *sresult, 0);
					MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_DOUBLE(php_argv[i]._val, dnumber);
					break;
				case enumFloat:
					sresult->xdmvalue = argObj; 
					fnumber = getFloatValue(*SaxonProcessor::sxn_environ, *sresult, 0);
					MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_DOUBLE(php_argv[i]._val, fnumber);					
					break;
				case enumBool:
					sresult->xdmvalue = argObj; 
					bvalue = getBooleanValue(*SaxonProcessor::sxn_environ, *sresult);
					MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_BOOL(php_argv[i]._val, bvalue);						
					break;
				case enumArrXdmValue:
					//TODO - not currently supported
					argLength--;
					break;
			}
			senv->ReleaseStringUTFChars(argType, str);
		} 

	//TODO should free sresult but it causes memory corruption	
		
// array of zvals to execute
    

 
    	// convert all the values
   	 for(int i = 0; i < argLength; i++) { params[i] = &php_argv[i]._val; }

	}
	//note: no implicit type conversion.

	zval *argvv = nullptr;//xparams;
	MAKE_STD_ZVAL(function_name);
	ZVAL_STRING(function_name, nativeString, 1);
	if(call_user_function_ex(CG(function_table), nullptr, function_name, &retval, argLength, params, 0, nullptr TSRMLS_CC) != SUCCESS)
	{
 	   zend_error(E_ERROR, "Function call failed");
	}



	if(Z_TYPE(*retval) ==0){
		zend_error(E_ERROR, "Function returned nullptr");
	}

	char * sVal = nullptr;
	int len = 0;
	jobject obj = nullptr;
	std::cerr<<" Return type="<<Z_TYPE_P(retval)<<std::endl;
//TODO handle XdmValue wrapped object
const char * objName = nullptr;
xdmNode_object* ooth = nullptr;
	switch (Z_TYPE_P(retval)) {
            case IS_BOOL:
                obj= booleanValue(*SaxonProcessor::sxn_environ, Z_BVAL_P(retval));
                break;
            
            case IS_LONG:
                obj= longValue(*SaxonProcessor::sxn_environ, Z_LVAL_P(retval));
                break;
            case IS_STRING:
                sVal = Z_STRVAL_P(retval);
                len = Z_STRLEN_P(retval);
		obj = getJavaStringValue(*SaxonProcessor::sxn_environ,estrndup(sVal, len)); 
                break;
            break;
            case IS_nullptr:
                
            	break;
            case IS_DOUBLE:
		obj = doubleValue(*SaxonProcessor::sxn_environ, (double)Z_DVAL_P(retval));		
		 break;
            
            case IS_ARRAY:
            //break;
            case IS_OBJECT:
            	objName =Z_OBJCE_P(retval)->name;
      

      		if(strcmp(objName, "Saxon\\XdmNode")==0) {
			ooth = (xdmNode_object*)zend_object_store_get_object(retval TSRMLS_CC);
        		if(ooth != nullptr) {
            			obj = ooth->xdmNode->getUnderlyingValue(nullptr);
            	
        		}
      		}
		break;
            default:
                obj = nullptr;
                zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Unknown type specified in extension function", 0 TSRMLS_CC);
        }

	//zend_printf("We have %i as type<br>", retval->type);
	//*return_value = *retval;
	//zval_copy_ctor(return_value);
	//zval_ptr_dtor(&retval);
	/*int cstrlen = Z_STRLEN_P(retval);
	char * str = estrndup(Z_STRVAL_P(retval), cstrlen);
	
	jstring jstrBuf = SaxonProcessor::sxn_environ->env->NewStringUTF(str);*/
	zval_ptr_dtor(&retval);
	return obj;
}



PHP_METHOD(XsltProcessor, transformToString)
{
    XsltProcessor *xsltProcessor;
 
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
////
/*zval *function_name;
zval *retval;

char * str = "userFunction";


MAKE_STD_ZVAL(function_name);
ZVAL_STRING(function_name, str, 1);
if(call_user_function_ex(CG(function_table), nullptr, function_name, &retval, 0, nullptr, 0, nullptr TSRMLS_CC) != SUCCESS)
{
    zend_error(E_ERROR, "Function call failed");
}

if(Z_TYPE(*retval) ==0){
zend_error(E_ERROR, "DATAYYY is nullptr");
}else {
str = Z_STRVAL_P(retval);
zend_printf("DATAYYY= %i <br>", str);
} 

zend_printf("We have %i as type<br>", retval->type);
*return_value = *retval;
zval_copy_ctor(return_value);
zval_ptr_dtor(&retval);*/


////

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

        const char * result = xsltProcessor->transformToString();
        if(result != nullptr) {
            char *str = estrdup(result);
            RETURN_STRING(str, 0);
        } else if(xsltProcessor->exceptionOccurred()){
            xsltProcessor->checkException();
            const char * errStr = xsltProcessor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xsltProcessor->getErrorCode(0);
                if(errorCode!=nullptr) {
                    // TODO: throw exception
                }
            }
        }
    }
    RETURN_nullptr();
}

PHP_METHOD(XsltProcessor, transformToValue)
{
    XsltProcessor *xsltProcessor;

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

	XdmValue * node = xsltProcessor->transformToValue();
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_nullptr();
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmValue = node;
            }
        } else if(xsltProcessor->exceptionOccurred()){
            xsltProcessor->checkException();
        }
    } else {
        RETURN_nullptr();
    }
}

PHP_METHOD(XsltProcessor, transformToFile)
{
    XsltProcessor *xsltProcessor;
 
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

	xsltProcessor->transformToFile();
        if(xsltProcessor->exceptionOccurred()) {
           //TODO
            const char * exStr = xsltProcessor->checkException();
        }
    } else {
        RETURN_nullptr();
    }
}

PHP_METHOD(XsltProcessor, compileFromFile)
{
    XsltProcessor *xsltProcessor;
    char * name;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->compileFromFile(name);
    }
}

PHP_METHOD(XsltProcessor, compileFromString)
{
    XsltProcessor *xsltProcessor;
    char * stylesheetStr;
    int len1, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &stylesheetStr, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->compileFromString(stylesheetStr);
    }
}

PHP_METHOD(XsltProcessor, compileFromValue)
{
    XsltProcessor *xsltProcessor;
   zval* oth;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &oth, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
 	xdmValue_object* ooth = (xdmValue_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr && value->size() == 1 && (value->getHead())->getType() == 3) {
        	xsltProcessor->compileFromXdmNode((XdmNode*)(value->getHead()));
	    }
	}
    }
}




PHP_METHOD(XsltProcessor, setSourceFromXdmValue)
{
    XsltProcessor *xsltProcessor;
    zval* oth = nullptr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_nullptr();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {

    if(!oth) {
	php_error(E_WARNING, "Error setting source value");
	return;
    } else {

      const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
		XdmItem  *valueX = (XdmItem*)value;
	        xsltProcessor->setSourceFromXdmValue(valueX);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0) {
	xdmValue_object* ooth = (xdmValue_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
	        xsltProcessor->setSourceFromXdmValue((XdmItem*)value);
            }
        }
      }  

        
    }
  }
}

PHP_METHOD(XsltProcessor, setOutputFile)
{
    XsltProcessor *xsltProcessor;
    char * outputFilename;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &outputFilename, &len1, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr && outputFilename != nullptr) {
        
	 xsltProcessor->setOutputFile(outputFilename);
            
        
    }
}

PHP_METHOD(XsltProcessor, setSourceFromFile)
{
    XsltProcessor *xsltProcessor;
    char * inFilename;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &inFilename, &len1, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }

    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr && inFilename != nullptr) {
        
	 xsltProcessor->setSourceFromFile(inFilename);
            
        
    }
}


PHP_METHOD(XsltProcessor, setProperty)
{
    XsltProcessor *xsltProcessor;
    char * name;
    char * value;
    int len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_nullptr();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->setProperty(name, value);
    }
}

PHP_METHOD(XsltProcessor, setParameter)
{

   XsltProcessor *xsltProcessor;
   char * name;
   zval* oth;
   int len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_nullptr();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {


      const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xsltProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		
                xsltProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                xsltProcessor->setParameter(name, (XdmValue *)value);
            }
        }



      }

    }
}

PHP_METHOD(XsltProcessor, clearParameters)
{
    XsltProcessor *xsltProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->clearParameters(true);
    }
}

PHP_METHOD(XsltProcessor, clearProperties)
{
    XsltProcessor *xsltProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->clearProperties();
    }
}

PHP_METHOD(XsltProcessor, exceptionOccurred)
{
    XsltProcessor *xsltProcessor;
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        bool result = xsltProcessor->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XsltProcessor, getExceptionCount)
{
    XsltProcessor *xsltProcessor;
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        int count = xsltProcessor->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(XsltProcessor, getErrorCode)
{
    XsltProcessor *xsltProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        const char * errCode = xsltProcessor->getErrorCode((int)index);
        if(errCode != nullptr) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}

PHP_METHOD(XsltProcessor, getErrorMessage)
{
    XsltProcessor *xsltProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    xsltProcessor_object *obj = (xsltProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        const char * errStr = xsltProcessor->getErrorMessage((int)index);
        if(errStr != nullptr) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}
PHP_METHOD(XsltProcessor, exceptionClear)
{
    XsltProcessor *xsltProcessor;
    xsltProcessor_object *obj = (xsltProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->exceptionClear();
    }
}




/*     ============== XQuery10/30/31: PHP Interface of   XQueryProcessor =============== */

void xqueryProcessor_free_storage(void *object TSRMLS_DC)
{
    xqueryProcessor_object *obj = (xqueryProcessor_object *)object;
    //obj->xqueryProcessor->clearParameters(true);
    //obj->xqueryProcessor->clearProperties();
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xqueryProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xqueryProcessor_object *obj = (xqueryProcessor_object *)emalloc(sizeof(xqueryProcessor_object));
    memset(obj, 0, sizeof(xqueryProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);
    retval.handle = zend_objects_store_put(obj, nullptr, xqueryProcessor_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &xqueryProcessor_object_handlers;

    return retval;
}

PHP_METHOD(XQueryProcessor, __destruct)
{
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    XQueryProcessor * xqueryProcessor= obj->xqueryProcessor;

    delete xqueryProcessor;
    
}


PHP_METHOD(XQueryProcessor, runQueryToValue)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != nullptr) {
        XdmValue * node = xqueryProcessor->runQueryToValue();
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmValue = node;
                return;
            }
        }
        xqueryProcessor->checkException();//TODO
    } else {
        RETURN_nullptr();
    }
}

PHP_METHOD(XQueryProcessor, runQueryToString)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != nullptr) {
        const char * result = xqueryProcessor->runQueryToString();
        if(result != nullptr) {
            char *str = estrdup(result);
            RETURN_STRING(str, 0);
	    return;
        } else {
          xqueryProcessor->checkException(); //TODO
	}
    }
   RETURN_nullptr();
}

PHP_METHOD(XQueryProcessor, runQueryToFile)
{

     char * ofilename;
     int len1 =0;
    if (ZEND_NUM_ARGS()!= 1) {
        WRONG_PARAM_COUNT;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ofilename, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);



    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != nullptr) {
	if(ofilename != nullptr) {
		xqueryProcessor->setOutputFile(ofilename);	
	}
        xqueryProcessor->runQueryToFile(); 
    }

}

PHP_METHOD(XQueryProcessor, setQueryContent)
{
    char * queryStr;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &queryStr, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC); 
    if(queryStr != nullptr) {
      obj->xqueryProcessor->setProperty("qs", queryStr);
   }
}

PHP_METHOD(XQueryProcessor, setQueryFile)
{
   char * fileName;
   int len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &fileName, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    if(fileName != nullptr) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setQueryFile(fileName);
    }
    	
}

PHP_METHOD(XQueryProcessor, setQueryBaseURI)
{
   char * base;
   int len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &base, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    if(base != nullptr) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setQueryBaseURI(base);
    }
    	
}

PHP_METHOD(XQueryProcessor, declareNamespace)
{
   char * prefix;
   char * ns;
   int len1, len2;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &prefix, &len1, &ns, &len2) == FAILURE) {
        RETURN_nullptr();
    }
    if(prefix != nullptr && ns != nullptr) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->declareNamespace(prefix, ns);
    }
    	
}



PHP_METHOD(XQueryProcessor, setContextItem)
{
   char * context;
   int len1;
   zval* oth;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_nullptr();
    }
    if(oth != nullptr) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    const char * objName =Z_OBJCE_P(oth)->name;
      //std::cerr<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xqueryProcessor->setContextItem((XdmItem *)value);
	        return;
            }
        }
      } else if(strcmp(objName, "Saxon\\XdmItem")==0){
	xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmItem * value = ooth->xdmItem;
	    if(value != nullptr) {
                xqueryProcessor->setContextItem(value);
		return;
	    }
         }
        



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                xqueryProcessor->setContextItem((XdmItem *)value);
		return;
            }
        }



      } 




	/*xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmItem * value = ooth->xdmItem;
            if(value != nullptr) {
    		xqueryProcessor->setContextItem(value);
	    }
	}*/
    }
	//throw exception
	php_error(E_WARNING,"contextItem not set");
	
    	
}

PHP_METHOD(XQueryProcessor, setContextItemFromFile)
{
   char * cfilename;
   int len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cfilename, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    if(cfilename != nullptr) {
    	xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setContextItemFromFile(cfilename);
    }
    	
}


PHP_METHOD(XQueryProcessor, setProperty)
{
    XQueryProcessor *xqueryProcessor;
    char * name;
    char * value;
    int len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_nullptr();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        xqueryProcessor->setProperty(name, value);
    }
}

PHP_METHOD(XQueryProcessor, setParameter)
{

   XQueryProcessor *xqueryProcessor;
   char * name;
   zval* oth;
   int len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_nullptr();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
             const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xqueryProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		
                xqueryProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                xqueryProcessor->setParameter(name, (XdmValue *)value);
            } 
        }



      }

    }
}




PHP_METHOD(XQueryProcessor, clearParameters)
{
    XQueryProcessor *xqueryProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        xqueryProcessor->clearParameters(true);
    }
}

PHP_METHOD(XQueryProcessor, clearProperties)
{
    XQueryProcessor *xqueryProcessor;

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        xqueryProcessor->clearProperties();
    }
}

PHP_METHOD(XQueryProcessor, exceptionOccurred)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        bool result = xqueryProcessor->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XQueryProcessor, getExceptionCount)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        int count = xqueryProcessor->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(XQueryProcessor, getErrorCode)
{
    XQueryProcessor *xqueryProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        const char * errCode = xqueryProcessor->getErrorCode((int)index);
        if(errCode != nullptr) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}

PHP_METHOD(XQueryProcessor, getErrorMessage)
{
    XQueryProcessor *xqueryProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    xqueryProcessor_object *obj = (xqueryProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        const char * errStr = xqueryProcessor->getErrorMessage((int)index);
        if(errStr != nullptr) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}
PHP_METHOD(XQueryProcessor, exceptionClear)
{
    XQueryProcessor *xqueryProcessor;
    xqueryProcessor_object *obj = (xqueryProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        xqueryProcessor->exceptionClear();
    }
}

/*     ============== PHP Interface of XPath2.0/3.0  XPathProcessor =============== */

void xpathProcessor_free_storage(void *object TSRMLS_DC)
{
    xpathProcessor_object *obj = (xpathProcessor_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

PHP_METHOD(XPathProcessor, __destruct)
{
   xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
   XPathProcessor * xpathProc= obj->xpathProcessor;


    delete xpathProc;
    
}

zend_object_value xpathProcessor_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xpathProcessor_object *obj = (xpathProcessor_object *)emalloc(sizeof(xpathProcessor_object));
    memset(obj, 0, sizeof(xpathProcessor_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, nullptr, xpathProcessor_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &xpathProcessor_object_handlers;

    return retval;
}



PHP_METHOD(XPathProcessor, setProperty)
{
    XPathProcessor *xpathProcessor;
    char * name;
    char * value;
    int len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        xpathProcessor->setProperty(name, value);
    }
}

PHP_METHOD(XPathProcessor, setParameter)
{

   XPathProcessor *xpathProcessor;
   char * name;
   zval* oth;
   int len1, len2;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
            const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xpathProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		
                xpathProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                xpathProcessor->setParameter(name, (XdmValue *)value);
            }
        }



      }

    }
}

PHP_METHOD(XPathProcessor, declareNamespace)
{
   char * prefix;
   char * ns;
   int len1, len2;
   XPathProcessor *xpathProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &prefix, &len1, &ns, &len2) == FAILURE) {
        RETURN_nullptr();
    }
    if(prefix != nullptr && ns != nullptr) {
    	xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    	xpathProcessor = obj->xpathProcessor;
    	xpathProcessor->declareNamespace(prefix, ns);
    }
    	
}


PHP_METHOD(XPathProcessor, setBackwardsCompatible){

    XPathProcessor *xpathProcessor;
    bool allow = false;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &allow) == FAILURE) {
        RETURN_nullptr();
    }

     xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
     xpathProcessor = obj->xpathProcessor;
     if (xpathProcessor != nullptr ) {
	    xpathProcessor->setBackwardsCompatible(allow);
	 }
}

PHP_METHOD(XPathProcessor, setCaching){
    XPathProcessor *xpathProcessor;
    bool isCaching = false;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &isCaching) == FAILURE) {
        RETURN_nullptr();
    }

     xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
     xpathProcessor = obj->xpathProcessor;
     if (xpathProcessor != nullptr) {
	    xpathProcessor->setCaching(isCaching);
	 }
}

PHP_METHOD(XPathProcessor, importSchemaNamespace){
      char * name;
      int len1;
      XPathProcessor *xpathProcessor;

       if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
           RETURN_nullptr();
       }
       xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
       xpathProcessor = obj->xpathProcessor;
       if (xpathProcessor != nullptr && name != nullptr) {
            xpathProcessor->importSchemaNamespace(name);
       }

}

PHP_METHOD(XPathProcessor, effectiveBooleanValue)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   int len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &xpathStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr && xpathStr != nullptr) {
        
                bool result = xpathProcessor->effectiveBooleanValue(xpathStr);
		RETURN_BOOL(result);
    }
}

PHP_METHOD(XPathProcessor, evaluate)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   int len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &xpathStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr && xpathStr != nullptr) {
        
        XdmValue * node = xpathProcessor->evaluate(xpathStr);
	if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
                struct xdmValue_object* vobj = (struct xdmValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmValue = node;
                return;
            }
        }
        xpathProcessor->checkException();//TODO
    } 
    RETURN_nullptr();
    
}

PHP_METHOD(XPathProcessor, evaluateSingle)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   int len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &xpathStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;

    if(xpathStr == nullptr) {
	RETURN_nullptr();
	return;
	}


    if (xpathProcessor != nullptr) {
        
        XdmItem * node = xpathProcessor->evaluateSingle(xpathStr);
	if(node != nullptr) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
                struct xdmItem_object* vobj = (struct xdmItem_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmItem = node;
                return;
            }
        } 
        xpathProcessor->checkException();//TODO
    } 
    RETURN_nullptr();
}

PHP_METHOD(XPathProcessor, setContextItem)
{

   XPathProcessor *xpathProcessor;

   zval* oth;
	//TODO this should be relaxed to accept item/atomic/node as well as Value

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
	if(!oth) {
		php_error(E_WARNING, "Error setting source value");
		return;
         } 
        const char * objName =Z_OBJCE_P(oth)->name;
        xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
       if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xpathProcessor->setContextItem((XdmItem *)value);
		value->incrementRefCount();

            }
        }
      }  else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                xpathProcessor->setContextItem((XdmItem *)value);
		value->incrementRefCount();
            }
        }
     }   else if(strcmp(objName, "Saxon\\XdmItem")==0){
	xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmItem * value = ooth->xdmItem;
            if(value != nullptr) {
		
                xpathProcessor->setContextItem(value);
		value->incrementRefCount();
            }
        }

      }
    }
}

PHP_METHOD(XPathProcessor, setBaseURI)
{

   XPathProcessor *xpathProcessor;

   char * uriStr;
   int len1;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &uriStr, &len1, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        
        if(uriStr != nullptr) {
           
                xpathProcessor->setBaseURI(uriStr);
            
        }
    }
}

PHP_METHOD(XPathProcessor, setContextFile)
{

   XPathProcessor *xpathProcessor;

   char * name;
   int len1;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1, xdmValue_ce) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        
        if(name != nullptr) {
           
                xpathProcessor->setContextFile(name);
            
        }
    }
}

PHP_METHOD(XPathProcessor, clearParameters)
{
    XPathProcessor *xpathProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        xpathProcessor->clearParameters(true);
    }
}

PHP_METHOD(XPathProcessor, clearProperties)
{
     XPathProcessor *xpathProcessor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        xpathProcessor->clearProperties();
    }
}


PHP_METHOD(XPathProcessor, exceptionOccurred)
{
   XPathProcessor *xpathProcessor;
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        bool result = xpathProcessor->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XPathProcessor, getExceptionCount)
{
    XPathProcessor *xpathProcessor;
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        int count = xpathProcessor->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(XPathProcessor, getErrorCode)
{
    XPathProcessor *xpathProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        const char * errCode = xpathProcessor->getErrorCode((int)index);
        if(errCode != nullptr) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}

PHP_METHOD(XPathProcessor, getErrorMessage)
{
    XPathProcessor *xpathProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    xpathProcessor_object *obj = (xpathProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        const char * errStr = xpathProcessor->getErrorMessage((int)index);
        if(errStr != nullptr) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}
PHP_METHOD(XPathProcessor, exceptionClear)
{
    XPathProcessor *xpathProcessor;
    xpathProcessor_object *obj = (xpathProcessor_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        xpathProcessor->exceptionClear();
    }
}

/*     ============== PHP Interface of   SchemaValidator =============== */

void schemaValidator_free_storage(void *object TSRMLS_DC)
{
    schemaValidator_object *obj = (schemaValidator_object *)object;

    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value schemaValidator_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    schemaValidator_object *obj = (schemaValidator_object *)emalloc(sizeof(schemaValidator_object));
    memset(obj, 0, sizeof(schemaValidator_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, nullptr, schemaValidator_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &schemaValidator_object_handlers;

    return retval;
}



PHP_METHOD(SchemaValidator, __destruct)
{
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    SchemaValidator * schemaValidator= obj->schemaValidator;

    delete schemaValidator;
    
}



PHP_METHOD(SchemaValidator, registerSchemaFromFile)
{
    SchemaValidator *schemaValidator;
    char * name;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (name != nullptr && schemaValidator != nullptr) {
        schemaValidator->registerSchemaFromFile(name);
    }
}

PHP_METHOD(SchemaValidator, registerSchemaFromString)
{
    char * schemaStr;
    int len1;
    SchemaValidator *schemaValidator;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &schemaStr, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaStr!= nullptr && schemaValidator != nullptr) {
        schemaValidator->registerSchemaFromString(schemaStr);
    }
}

PHP_METHOD(SchemaValidator, validate)
{
    char * name = nullptr;
    int len1;
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }
    if (ZEND_NUM_ARGS()>0 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->validate(name);
    }
}

PHP_METHOD(SchemaValidator, validateToNode)
{
    char * name = nullptr;
    int len1;
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }
    if (ZEND_NUM_ARGS()>0 && zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        XdmNode * node = schemaValidator->validateToNode(name);
	if(node != nullptr) {
	    if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmNode = node;
                return;
            }
	} 
    	schemaValidator->checkException();//TODO
    } 
    RETURN_nullptr();
}


PHP_METHOD(SchemaValidator, getValidationReport)
{

    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        XdmNode * node = schemaValidator->getValidationReport();
	if(node != nullptr) {
	    if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmNode = node;
                return;
            }
	} 
    	schemaValidator->checkException();//TODO
    } 
    RETURN_nullptr();
}


PHP_METHOD(SchemaValidator, setSourceNode)
{
    SchemaValidator *schemaValidator;

    zval* oth;
   

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
	const char * objName =Z_OBJCE_P(oth)->name;
	if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
		schemaValidator->setSourceNode(value);

            }
        }
      
        
      }
    }
}

PHP_METHOD(SchemaValidator, setOutputFile)
{
    SchemaValidator *schemaValidator;
    char * name;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->setOutputFile(name);
    }
}


PHP_METHOD(SchemaValidator, setProperty)
{
    SchemaValidator *schemaValidator;
    char * name;
    char * value;
    int len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->setProperty(name, value);
    }
}

PHP_METHOD(SchemaValidator, setParameter)
{

   SchemaValidator *schemaValidator;
   char * name;
   zval* oth;
   int len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
      const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        schemaValidator->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	xdmValue_object* ooth = (xdmValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		
                schemaValidator->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                schemaValidator->setParameter(name, (XdmValue *)value);
            }
        }



      }

    }
}

PHP_METHOD(SchemaValidator, clearProperties)
{
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->clearProperties();
	schemaValidator->exceptionClear();
    }
}

PHP_METHOD(SchemaValidator, clearParameters)
{

   SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
       
                schemaValidator->clearParameters(true);
		schemaValidator->exceptionClear();
        }
    }

PHP_METHOD(SchemaValidator, exceptionOccurred)
{
    SchemaValidator *schemaValidator;
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        bool result = schemaValidator->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(SchemaValidator, getExceptionCount)
{
    SchemaValidator *schemaValidator;
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        int count = schemaValidator->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(SchemaValidator, getErrorCode)
{
    SchemaValidator *schemaValidator;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        const char * errCode = schemaValidator->getErrorCode((int)index);
        if(errCode != nullptr) {
            char *str = estrdup(errCode);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}

PHP_METHOD(SchemaValidator, getErrorMessage)
{
    SchemaValidator *schemaValidator;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }
    schemaValidator_object *obj = (schemaValidator_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        const char * errStr = schemaValidator->getErrorMessage((int)index);
        if(errStr != nullptr) {
            char *str = estrdup(errStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}
PHP_METHOD(SchemaValidator, exceptionClear)
{
    SchemaValidator * schemaValidator;
    schemaValidator_object *obj = (schemaValidator_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->exceptionClear();
    }
}

/*     ============== PHP Interface of   XdmValue =============== */
void xdmValue_free_storage(void *object TSRMLS_DC)
{
    xdmValue_object *obj = (xdmValue_object *)object;
    //obj->xdmValue->releaseXdmValue(nullptr);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmValue_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmValue_object *obj = (xdmValue_object *)emalloc(sizeof(xdmValue_object));
    memset(obj, 0, sizeof(xdmValue_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, nullptr, xdmValue_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &xdmValue_object_handlers;

    return retval;
}

PHP_METHOD(XdmValue, __construct)
{
    XdmValue *xdmValue = nullptr;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    SaxonProcessor *proc= nullptr;
    xdmValue_object *obj = (xdmValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    /*if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z",&zvalue) == SUCCESS) {
        switch (Z_TYPE_P(zvalue)) {
            case IS_BOOL:
                bVal = Z_BVAL_P(zvalue);
                xdmValue = new XdmValue(bVal);
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_LONG:
                iVal = Z_LVAL_P(zvalue);
                xdmValue = new XdmValue((int)iVal);
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_STRING:
                sVal = Z_STRVAL_P(zvalue);
                len = Z_STRLEN_P(zvalue);
                xdmValue = new XdmValue("string", sVal);
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_nullptr:
                xdmValue = new XdmValue();
                obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_DOUBLE:
                // TODO: implement this
                //index = (long)Z_DVAL_P(zvalue);
            //break;
            case IS_ARRAY:
            //break;
            case IS_OBJECT:
            //break;
            default:
                obj = nullptr;
                zend_throw_exception(zend_exception_get_default(TSRMLS_C), "unknown type specified in XdmValue", 0 TSRMLS_CC);
        }
    }*/
}

PHP_METHOD(XdmValue, __destruct)
{
    xdmValue_object *obj = (xdmValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
    XdmValue * xdmValue= obj->xdmValue;
    xdmValue->decrementRefCount();
    if(xdmValue->getRefCount()< 1){
    	delete xdmValue;
    } 
    
    
}

PHP_METHOD(XdmValue,  getHead){
    XdmValue *xdmValue;
    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    if (xdmValue != nullptr) {
	XdmItem * item = xdmValue->getHead();
	if(item != nullptr) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
                struct xdmItem_object* vobj = (struct xdmItem_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmItem = item;
                return;
            }
        }
        
    } else {
	RETURN_nullptr();
    }
}


PHP_METHOD(XdmValue,  itemAt){
    XdmValue *xdmValue;

    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
        RETURN_nullptr();
    }

    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    if (xdmValue != nullptr) {
	XdmItem * item = xdmValue->itemAt((int)index);
	if(item != nullptr) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
		item->incrementRefCount();
                struct xdmItem_object* vobj = (struct xdmItem_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmItem = item;
                return;
            }
        }
        
    } else {
	RETURN_nullptr();
    }
}


PHP_METHOD(XdmValue,  size){
    XdmValue *xdmValue;
    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    int sizei = 0;
    if (xdmValue != nullptr) {
	sizei = xdmValue->size();
    }
     RETURN_LONG(sizei);
}


PHP_METHOD(XdmValue, addXdmItem){
    XdmValue *xdmValue;
    zval* oth;
   	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &oth) == FAILURE) {
        RETURN_nullptr();
    }

    xdmValue_object *obj = (xdmValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    if (xdmValue != nullptr) {
     const char * objName =Z_OBJCE_P(oth)->name;
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	xdmNode_object* ooth = (xdmNode_object*)zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xdmValue->addXdmItem((XdmItem *)value);
		return;
            }
        }
      } else if(strcmp(objName, "Saxon\\XdmItem")==0){
	xdmItem_object* ooth = (xdmItem_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmItem * value = ooth->xdmItem;
            if(value != nullptr) {
		xdmValue->addXdmItem(value);
		return;
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object*) zend_object_store_get_object(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		xdmValue->addXdmItem((XdmItem *)value);
		return;
            }
        }

      } else {
		//TODO exception
	}
    }
}



/*     ============== PHP Interface of   XdmItem =============== */

void xdmItem_free_storage(void *object TSRMLS_DC)
{
    xdmItem_object *obj = (xdmItem_object *)object;
    //obj->xdmItem->releaseXdmValue(nullptr);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmItem_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmItem_object *obj = (xdmItem_object *)emalloc(sizeof(xdmItem_object));
    memset(obj, 0, sizeof(xdmItem_object));
    obj->std.ce = type;

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, nullptr, xdmItem_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &xdmItem_object_handlers;

    return retval;
}

PHP_METHOD(XdmItem, __construct)
{
    XdmItem *xdmItem = nullptr;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    SaxonProcessor *proc= nullptr;
    xdmItem_object *obj = (xdmItem_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

}

PHP_METHOD(XdmItem, __destruct)
{
    xdmItem_object *obj = (xdmItem_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    XdmItem * xdmItem= obj->xdmItem;
    xdmItem->decrementRefCount();
    if(xdmItem->getRefCount()< 1){
    	delete xdmItem;
    }
    
}

PHP_METHOD(XdmItem, getStringValue)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;

    if (xdmItem != nullptr) {
        const char * valueStr = saxonProc->getStringValue(xdmItem);
        if(valueStr != nullptr) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}

PHP_METHOD(XdmItem, isAtomic)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr) {
        bool isAtomic = xdmItem->isAtomic();
        RETURN_BOOL(isAtomic);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(XdmItem, isNode)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr && xdmItem->getType() == XDM_NODE) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(XdmItem, getAtomicValue)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr) {
	  if(!xdmItem->isAtomic()) {
		RETURN_nullptr();
		return;
	  }
          if (object_init_ex(return_value, xdmAtomicValue_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
		xdmItem->incrementRefCount();
                struct xdmAtomicValue_object* vobj = (struct xdmAtomicValue_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmAtomicValue = (XdmAtomicValue *)xdmItem;
                return;
            }
    }
    RETURN_nullptr();
}

PHP_METHOD(XdmItem, getNodeValue)
{
    XdmItem *xdmItem;
    xdmItem_object *obj = (xdmItem_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr) {
	  if(xdmItem->isAtomic()) {
		RETURN_nullptr();
		return;
	  }
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
                struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmNode = (XdmNode *)xdmItem;
		vobj->xdmNode->incrementRefCount();

                return;
            }
    }
    RETURN_nullptr();
}



/*     ============== PHP Interface of   XdmNode =============== */

void xdmNode_free_storage(void *object TSRMLS_DC)
{
    xdmNode_object *obj = (xdmNode_object *)object;
    //obj->xdmItem->releaseXdmValue(nullptr);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmNode_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmNode_object *obj = (xdmNode_object *)emalloc(sizeof(xdmNode_object));
    memset(obj, 0, sizeof(xdmNode_object));
   // type->name ="XdmNode";
	 
    obj->std.ce = type;	

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, nullptr, xdmNode_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &xdmNode_object_handlers;

    return retval;
}

PHP_METHOD(XdmNode, __construct)
{
    XdmNode *xdmNode = nullptr;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    SaxonProcessor *proc= nullptr;
    xdmNode_object *obj = (xdmNode_object *) zend_object_store_get_object(getThis() TSRMLS_CC);
}

PHP_METHOD(XdmNode, __destruct)
{
    xdmNode_object *obj = (xdmNode_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

    XdmNode * xdmNode= obj->xdmNode;
   // if(xdmNode != nullptr) {
    	xdmNode->decrementRefCount();
    	if(xdmNode->getRefCount()< 1){
    		delete xdmNode;
    	}
    //}
    
}

PHP_METHOD(XdmNode, getStringValue)
{
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;

    if (xdmNode != nullptr) {
        const char * valueStr = xdmNode->getStringValue(saxonProc);
        if(valueStr != nullptr) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
	    return;
        }
    } 
    RETURN_nullptr();
    
}

PHP_METHOD(XdmNode, getNodeName)
{
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
        const char * valueStr = xdmNode->getNodeName();
        if(valueStr != nullptr) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
        }
    } 
    RETURN_nullptr();
}

PHP_METHOD(XdmNode, getNodeKind)
{
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    int nodeKind = 0;
    if (xdmNode != nullptr) {
        nodeKind = xdmNode->getNodeKind();
        
    }
     RETURN_LONG(nodeKind);
}

PHP_METHOD(XdmNode, isAtomic)
{

    RETURN_FALSE;
}


PHP_METHOD(XdmNode,  getChildCount){
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    int nodeChildCount = 0;
    if (xdmNode != nullptr) {
        nodeChildCount = xdmNode->getChildCount();
        
    }
     RETURN_LONG(nodeChildCount);
}   

PHP_METHOD(XdmNode,  getAttributeCount){
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    int nodeAttrCount = 0;
    if (xdmNode != nullptr) {
        nodeAttrCount = xdmNode->getAttributeCount();
        
    }
     RETURN_LONG(nodeAttrCount);

} 

PHP_METHOD(XdmNode,  getChildNode){
    int indexi;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&indexi) == FAILURE) {
        RETURN_nullptr();
    }

    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
	 int count = xdmNode->getChildCount();
	 if(count==0) {
		RETURN_nullptr();
		return;
	  }	
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
		
		if(indexi>=0 && indexi < count) {
			XdmNode ** childNodes = xdmNode->getChildren();
			if(childNodes == nullptr) {
				RETURN_nullptr();
				return;
			}
			XdmNode * childNode = childNodes[indexi];
			if(childNode != nullptr) {
				childNode->incrementRefCount();
                		struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                		assert (vobj != nullptr);
                		vobj->xdmNode = childNode;
                		return;
			}
		}
            }
    }
    RETURN_nullptr();
}

PHP_METHOD(XdmNode,  getParent){
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
	XdmNode * parent = xdmNode->getParent();
	if(parent == nullptr) {
			RETURN_nullptr();
			return;
	}
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
		parent->incrementRefCount();
               	struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
               	assert (vobj != nullptr);
               	vobj->xdmNode = parent;
               	return;
            }
    }
    RETURN_nullptr();
}

PHP_METHOD(XdmNode,  getAttributeNode){
    int indexi;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",&indexi) == FAILURE) {
        RETURN_nullptr();
    }

    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
	  int count = xdmNode->getAttributeCount();
	  if(count > 0) {
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_nullptr();
                return;
            } else {
		
		if(indexi < count) {
			XdmNode * attNode = xdmNode->getAttributeNodes()[indexi];
			if(attNode != nullptr) {
				attNode->incrementRefCount();
                		struct xdmNode_object* vobj = (struct xdmNode_object *)zend_object_store_get_object(return_value TSRMLS_CC);
                		assert (vobj != nullptr);
                		vobj->xdmNode = attNode;

                		return;
			}
		}
            }
	}
    }
    RETURN_nullptr();

}

PHP_METHOD(XdmNode,  getAttributeValue){
   char * name;
   int len1;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &len1) == FAILURE) {
        RETURN_nullptr();
    }
    XdmNode *xdmNode;
    xdmNode_object *obj = (xdmNode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmNode = obj->xdmNode;
    if (xdmNode != nullptr && name != nullptr) {
	
        const char * valueStr = xdmNode->getAttributeValue(name);
        if(valueStr != nullptr) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
	    return;
        }
    }
    RETURN_nullptr();


}

/*     ============== PHP Interface of   XdmAtomicValue =============== */

void xdmAtomicValue_free_storage(void *object TSRMLS_DC)
{
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)object;
    //obj->xdmItem->releaseXdmValue(nullptr);
    zend_hash_destroy(obj->std.properties);
    FREE_HASHTABLE(obj->std.properties);
    efree(obj);
}

zend_object_value xdmAtomicValue_create_handler(zend_class_entry *type TSRMLS_DC)
{
    zval *tmp;
    zend_object_value retval;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)emalloc(sizeof(xdmAtomicValue_object));
    memset(obj, 0, sizeof(xdmAtomicValue_object));
   // type->name ="XdmNode";
	 
    obj->std.ce = type;	

    ALLOC_HASHTABLE(obj->std.properties);
    zend_hash_init(obj->std.properties, 0, nullptr, ZVAL_PTR_DTOR, 0);
    object_properties_init(&obj->std, type);

    retval.handle = zend_objects_store_put(obj, nullptr, xdmNode_free_storage, nullptr TSRMLS_CC);
    retval.handlers = &xdmAtomicValue_object_handlers;

    return retval;
}

PHP_METHOD(XdmAtomicValue, __construct)
{
    XdmAtomicValue *xdmValue = nullptr;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;

    xdmAtomicValue_object *obj = (xdmAtomicValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

}

PHP_METHOD(XdmAtomicValue, __destruct)
{
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *) zend_object_store_get_object(getThis() TSRMLS_CC);


    XdmAtomicValue * xdmValue= obj->xdmAtomicValue;
    xdmValue->decrementRefCount();
    if(xdmValue->getRefCount()< 1){
    	delete xdmValue;
    }
    
}

PHP_METHOD(XdmAtomicValue, getBooleanValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    bool resultb = false;
    if (xdmAtomicValue != nullptr) {
         resultb = xdmAtomicValue->getBooleanValue();
        
    }
    RETURN_BOOL(resultb);
}


PHP_METHOD(XdmAtomicValue, getDoubleValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    double resultb = 0;
    if (xdmAtomicValue != nullptr) {
         resultb = xdmAtomicValue->getDoubleValue();
        
    }
    RETURN_DOUBLE(resultb);
}

PHP_METHOD(XdmAtomicValue, getLongValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;
    long result = 0;
    if (xdmAtomicValue != nullptr) {
         result = xdmAtomicValue->getLongValue();
        
    }
    RETURN_LONG(result);
}

PHP_METHOD(XdmAtomicValue, getStringValue)
{
    XdmAtomicValue *xdmAtomicValue;
    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
    saxonProcessor_object * obj2 = (saxonProcessor_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
    saxonProc =  obj2->saxonProcessor;

    if (xdmAtomicValue != nullptr) {
        const char * valueStr = saxonProc->getStringValue(xdmAtomicValue);
        if(valueStr != nullptr) {
            char *str = estrdup(valueStr);
            RETURN_STRING(str, 0);
        }
    }
    RETURN_nullptr();
}

PHP_METHOD(XdmAtomicValue, isAtomic)
{

    RETURN_TRUE;
}


// =============================================================

zend_function_entry SaxonProcessor_methods[] = {
    PHP_ME(SaxonProcessor,  __construct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(SaxonProcessor,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(SaxonProcessor,  createAtomicValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  parseXmlFromString,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  parseXmlFromFile,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  setcwd,     nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newXPathProcessor,     nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newXsltProcessor,     nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newXQueryProcessor,     nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newSchemaValidator,     nullptr, ZEND_ACC_PUBLIC)
//    PHP_ME(SaxonProcessor,  importDocument,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  setResourcesDirectory,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor, setConfigurationProperty,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  registerPHPFunction,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  version,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

zend_function_entry XsltProcessor_methods[] = {
    PHP_ME(XsltProcessor,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XsltProcessor,  transformFileToFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformFileToString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformFileToValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformToString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformToValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  transformToFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor, compileFromFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor, compileFromValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor, compileFromString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setOutputFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setSourceFromFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setSourceFromXdmValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setParameter, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setProperty, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  clearParameters, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  clearProperties, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  exceptionOccurred, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  exceptionClear, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getErrorCode, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getErrorMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getExceptionCount, nullptr, ZEND_ACC_PUBLIC)
{nullptr, nullptr, nullptr}
};

zend_function_entry XQueryProcessor_methods[] = {
    PHP_ME(XQueryProcessor,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
   // PHP_ME(XQueryProcessor,  getErrorCode,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setQueryContent,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setContextItem,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setContextItemFromFile,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setParameter,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  setProperty,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  clearParameters,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  clearProperties,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, runQueryToValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, runQueryToString,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, runQueryToFile,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, setQueryFile,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, setQueryBaseURI,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor, declareNamespace,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  exceptionOccurred, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  exceptionClear, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  getErrorCode, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  getErrorMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XQueryProcessor,  getExceptionCount, nullptr, ZEND_ACC_PUBLIC)
{nullptr, nullptr, nullptr}
};

zend_function_entry XPathProcessor_methods[] = {
    PHP_ME(XPathProcessor,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XPathProcessor,  setContextItem,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  setContextFile,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  effectiveBooleanValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  evaluate,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  evaluateSingle,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  setParameter,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  setProperty,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  clearParameters,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  clearProperties,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  exceptionOccurred, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  exceptionClear, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  getErrorCode, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  getErrorMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor,  getExceptionCount, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor, declareNamespace,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor, setBackwardsCompatible,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor, setCaching,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor, importSchemaNamespace,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XPathProcessor, setBaseURI, nullptr, ZEND_ACC_PUBLIC)
{nullptr, nullptr, nullptr}
};

zend_function_entry SchemaValidator_methods[] = {
    PHP_ME(SchemaValidator,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(SchemaValidator,  setSourceNode,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  setOutputFile,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  validate,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  validateToNode,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  registerSchemaFromFile,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  registerSchemaFromString,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getValidationReport,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  setParameter,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  setProperty,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  clearParameters,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  clearProperties,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  exceptionOccurred, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  exceptionClear, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getErrorCode, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getErrorMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SchemaValidator,  getExceptionCount, nullptr, ZEND_ACC_PUBLIC)
{nullptr, nullptr, nullptr}
};

zend_function_entry xdmValue_methods[] = {
    PHP_ME(XdmValue,  __construct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmValue,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmValue,  getHead,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue,  itemAt,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue,  size,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue, addXdmItem,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

zend_function_entry xdmItem_methods[] = {
    PHP_ME(XdmItem,  __construct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmItem,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmItem,  getStringValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  isAtomic,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  isNode,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  getAtomicValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmItem,  getNodeValue,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

zend_function_entry xdmNode_methods[] = {
    PHP_ME(XdmNode,  __construct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmNode,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmNode,  getStringValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getNodeKind,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getNodeName,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  isAtomic,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getChildCount,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getAttributeCount,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getChildNode,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getParent,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getAttributeNode,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmNode,  getAttributeValue,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

zend_function_entry xdmAtomicValue_methods[] = {
    PHP_ME(XdmAtomicValue,  __construct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmAtomicValue,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmAtomicValue,  getStringValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  isAtomic,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getBooleanValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getDoubleValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getLongValue,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

PHP_MINIT_FUNCTION(saxon)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Saxon\\SaxonProcessor", SaxonProcessor_methods);
    saxonProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    saxonProcessor_ce->create_object = saxonProcessor_create_handler;
    memcpy(&saxonProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    saxonProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XSLTProcessor", XsltProcessor_methods);
    xsltProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xsltProcessor_ce->create_object = xsltProcessor_create_handler;
    memcpy(&xsltProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xsltProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XQueryProcessor", XQueryProcessor_methods);
    xqueryProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xqueryProcessor_ce->create_object = xqueryProcessor_create_handler;
    memcpy(&xqueryProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xqueryProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XPathProcessor", XPathProcessor_methods);
    xpathProcessor_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xpathProcessor_ce->create_object = xpathProcessor_create_handler;
    memcpy(&xpathProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xpathProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\SchemaValidator", SchemaValidator_methods);
    schemaValidator_ce = zend_register_internal_class(&ce TSRMLS_CC);
    schemaValidator_ce->create_object = schemaValidator_create_handler;
    memcpy(&schemaValidator_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    schemaValidator_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmValue", xdmValue_methods);
    xdmValue_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmValue_ce->create_object = xdmValue_create_handler;
    memcpy(&xdmValue_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmValue_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmItem", xdmItem_methods);
    xdmItem_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmItem_ce->create_object = xdmItem_create_handler;
    memcpy(&xdmItem_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmItem_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmNode", xdmNode_methods);
    xdmNode_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmNode_ce->create_object = xdmNode_create_handler;
    memcpy(&xdmNode_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmNode_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmAtomicValue", xdmAtomicValue_methods);
    xdmAtomicValue_ce = zend_register_internal_class(&ce TSRMLS_CC);
    xdmAtomicValue_ce->create_object = xdmAtomicValue_create_handler;
    memcpy(&xdmAtomicValue_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    xdmAtomicValue_object_handlers.clone_obj = nullptr;

    return SUCCESS;
}

PHP_MINFO_FUNCTION(saxon)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "saxonc", "enabled");
    php_info_print_table_row(2, "Saxon/C EXT version", "1.1.0");
    php_info_print_table_row(2, "Saxon", "9.8.0.4");
    php_info_print_table_row(2, "Excelsior JET", "11.3 MP1");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

PHP_MSHUTDOWN_FUNCTION(saxon) {
    UNREGISTER_INI_ENTRIES();
    //SaxonProcessor::release();

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(saxon) {
    //std::cerr<<"RSHUTDOWN called -start"<<std::endl;
    return SUCCESS;
}

PHP_RINIT_FUNCTION(saxon) {
    //std::cerr<<"RINIT called -start"<<std::endl;
    return SUCCESS;
}

zend_module_entry saxon_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_SAXON_EXTNAME,
    nullptr,        /* Functions */
    PHP_MINIT(saxon),        /* MINIT */
    PHP_MSHUTDOWN(saxon),        /* MSHUTDOWN */
    nullptr,        /* RINIT */
    nullptr,        /* RSHUTDOWN */
    PHP_MINFO(saxon),        /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
    PHP_SAXON_EXTVER,
#endif
    STANDARD_MODULE_PROPERTIES
};

