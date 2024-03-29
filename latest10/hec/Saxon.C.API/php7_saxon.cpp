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
zend_object_handlers xslt30Processor_object_handlers;
zend_object_handlers xqueryProcessor_object_handlers;
zend_object_handlers xpathProcessor_object_handlers;
zend_object_handlers schemaValidator_object_handlers;
zend_object_handlers xdmValue_object_handlers;
zend_object_handlers xdmItem_object_handlers;
zend_object_handlers xdmNode_object_handlers;
zend_object_handlers xdmAtomicValue_object_handlers;

zend_class_entry *saxonProcessor_ce;
zend_class_entry *xsltProcessor_ce;
zend_class_entry *xslt30Processor_ce;
zend_class_entry *xqueryProcessor_ce;
zend_class_entry *xpathProcessor_ce;
zend_class_entry *schemaValidator_ce;
zend_class_entry *xdmValue_ce;
zend_class_entry *xdmItem_ce;
zend_class_entry *xdmNode_ce;
zend_class_entry *xdmAtomicValue_ce;

void SaxonProcessor_free_storage(zend_object *object)
{

//std::cerr<<"_free_Storage called php-saxonProc"<<std::endl;
 	
    saxonProcessor_object *obj;
	
	obj =  (saxonProcessor_object *)((char *)object - XtOffsetOf(saxonProcessor_object, std));

 SaxonProcessor * saxonProc= obj->saxonProcessor;
    if(saxonProc != nullptr) {
	//SaxonProcessor::release();    
	delete saxonProc;
    }
 zend_object_std_dtor(object);

    efree(obj);
}

void SaxonProcessor_destroy_storage(zend_object *pobj)
{

	//php_error(E_WARNING,"_destroy_Storage called php-saxonProc");
 	 //  saxonProcessor_object *obj;
	//zend_object* pobj = Z_OBJ_P(getThis()); 
 /*  saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

    SaxonProcessor * saxonProc= obj->saxonProcessor;
    if(saxonProc != nullptr) {
	SaxonProcessor::release();    
	delete saxonProc;
    }*/
	
    zend_objects_destroy_object(pobj);

    
}

zend_object *saxonProcessor_create_handler(zend_class_entry *type)
{
    zval *tmp;
    zend_object retval;
    saxonProcessor_object *obj = (saxonProcessor_object *)ecalloc(1, sizeof(saxonProcessor_object) + zend_object_properties_size(type));
 

    zend_object_std_init(&obj->std, type); /* take care of the zend_object also ! */
    object_properties_init(&obj->std, type);

    obj->std.handlers = &saxonProcessor_object_handlers;

    return &obj->std;
}

PHP_METHOD(SaxonProcessor, __construct)
{
    if (ZEND_NUM_ARGS()>2) {
        WRONG_PARAM_COUNT;
    }

    char * cwdi = nullptr;
   bool license = false;
    size_t len1;
    if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS(), "b", &license) == FAILURE) {
        RETURN_NULL();
    } 


    if (ZEND_NUM_ARGS()>1 && zend_parse_parameters(ZEND_NUM_ARGS(), "bs", &license, &cwdi, &len1) == FAILURE) {
        RETURN_NULL();
    }


    zval *object = getThis();
    SaxonProcessor * saxonProc = nullptr;
    zend_object * zobj = Z_OBJ_P(object);

    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)zobj - XtOffsetOf(saxonProcessor_object, std));

	saxonProc = new SaxonProcessor(license); //TODO: add license flag to PHP function argument
		
    
	obj->saxonProcessor = saxonProc;
//std::cerr<<"SaxonProcessorConstructor cp1"<<std::endl;
    if(cwdi==nullptr) {
//std::cerr<<"SaxonProcessorConstructor cp1-1"<<std::endl;
#if !(defined (__linux__) || (defined (__APPLE__) && defined(__MACH__)))
	    TCHAR s[256];

            // --
            DWORD a = GetCurrentDirectory(256, s);
	    const size_t newsize = wcslen(s)*2;
	    char* cwd = new char[newsize];
	    wcstombs_s(0, cwd, newsize, s, _TRUNCATE);
	    // -- code above returns the apache installation directory as the CWD

	    char* cwd2;

	    //php_error(E_WARNING,cwd2);

	    saxonProc->setcwd(cwd);
	    // -- code above tries to use VCWD_GETCWD but there is a linkage error
#else
	    char cwd[256];

	    VCWD_GETCWD(cwd, sizeof(cwd));
	    if(cwd == nullptr) {
	     //php_error(E_WARNING,"cwd is nullptrXXXXXXXXXXXXXXXXXXXXXXX");
	   }else {
           //  php_error(E_WARNING,cwd);
 
	    saxonProc->setcwd(cwd);

          }
#endif

    } else {
        saxonProc->setcwd(cwdi);
    }

}

PHP_METHOD(SaxonProcessor, __destruct)
{
    //php_error(E_WARNING,"__destruct called php-saxonProc");
    //std::cerr<<"__destruct call saxonProc"<<std::endl;
	zend_object* pobj = Z_OBJ_P(getThis()); 
   saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

    SaxonProcessor * saxonProc= obj->saxonProcessor;
    if(saxonProc != nullptr) {
	//SaxonProcessor::release();    
	//delete saxonProc;
    }
}


PHP_METHOD(SaxonProcessor, release)
{
//php_error(E_WARNING,"__destruct called php-saxonProc");
  //  std::cerr<<"__destruct call saxonProc"<<std::endl;
	zend_object* pobj = Z_OBJ_P(getThis()); 
   saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

    SaxonProcessor * saxonProc= obj->saxonProcessor;
    if(saxonProc != nullptr) {
	SaxonProcessor::release();    
    }
}

PHP_METHOD(SaxonProcessor, setResourcesDirectory)
{
    SaxonProcessor *saxonProcessor;
    char * dirStr;
    size_t len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &dirStr, &len) == FAILURE) {
        RETURN_NULL();
    }
    
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
       
        if(dirStr != nullptr) {
            saxonProcessor->setResourcesDirectory(dirStr);
        }
    }
}


PHP_METHOD(SaxonProcessor, isSchemaAware)
{
    SaxonProcessor *saxonProcessor;
    
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
       
         if(saxonProcessor->isSchemaAwareProcessor()){

		RETURN_TRUE;
 	}
    }
    RETURN_FALSE;
}

PHP_METHOD(SaxonProcessor, setCatalog)
{
    SaxonProcessor *saxonProcessor;
    char * catalogFile;
    size_t len;
    bool trace = false;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "sb", &catalogFile, &len, &trace) == FAILURE) {
        RETURN_NULL();
    }
    
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
       
        if(catalogFile != nullptr) {
            saxonProcessor->setCatalog(catalogFile, trace);
        }
    }
}



PHP_METHOD(SaxonProcessor, setcwd)
{
    SaxonProcessor *saxonProcessor;
    char * cwdStr;
    size_t len;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &cwdStr, &len) == FAILURE) {
        RETURN_NULL();
    }
    
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
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
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &source, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    assert (obj != nullptr);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
        XdmNode* node = saxonProcessor->parseXmlFromString(source);
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                zend_object* vobj = Z_OBJ_P(return_value);
 		xdmNode_object * xdmNObj = (xdmNode_object *)((char *)vobj - XtOffsetOf(xdmNode_object, std));
                assert (xdmNObj != nullptr);
                xdmNObj->xdmNode = node;
            }
        } else {
            if(obj->saxonProcessor->exceptionOccurred()){
		//TODO throw exception
	    }
        }
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, parseXmlFromFile)
{
    SaxonProcessor * saxonProcessor;
    char * source;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &source, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    assert (obj != nullptr);
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
        XdmNode* node = saxonProcessor->parseXmlFromFile(source);//TODO this needs to be XdmNode object
        if(node != nullptr) {
      
            if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
          
            zend_object* vvobj = Z_OBJ_P(return_value);
            xdmNode_object * vobj = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
                
                assert (vobj != nullptr);
                vobj->xdmNode = node;
            }
        } else {
           // obj->xsltProcessor->checkException();//TODO
        }
    } else {
  //  std::cerr<<"parseXmlFromFile cp2"<<std::endl;
        RETURN_NULL();
    }
   // std::cerr<<"parseXmlFromFile cp3"<<std::endl;
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
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "z",&zvalue) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
	switch (Z_TYPE_P(zvalue)) {
            case IS_FALSE:
	    xdmValue = proc->makeBooleanValue(false);
	    case IS_TRUE:
                xdmValue = proc->makeBooleanValue(true);
            break;
            case IS_LONG:
                iVal = Z_LVAL_P(zvalue);
		 xdmValue = proc->makeIntegerValue((int)iVal);
            break;
            case IS_STRING:
                sVal = Z_STRVAL_P(zvalue);
                len = Z_STRLEN_P(zvalue);
		
                xdmValue = proc->makeStringValue((const char*)sVal);
		//efree(sVal);
            break;
            case IS_NULL:
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
                RETURN_NULL();
        }
//std::cerr<<"createAtomicValue cp0"<<std::endl;
        if(xdmValue == nullptr) {
            RETURN_NULL();
        }
        if (object_init_ex(return_value, xdmAtomicValue_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
            //struct xdmAtomicValue_object* vobj = (struct xdmAtomicValue_object *)Z_OBJ_P(return_value);
 	    zend_object* vvobj = Z_OBJ_P(return_value);
	   xdmAtomicValue_object * vobj = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
            assert (vobj != nullptr);
            vobj->xdmAtomicValue = xdmValue;
        }
    } else {
       
        RETURN_NULL();
    }
}


PHP_METHOD(SaxonProcessor, newXPathProcessor)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XPathProcessor * xpathProcessor = nullptr;

    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
if (object_init_ex(return_value, xpathProcessor_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	  // struct xpathProcessor_object* vobji = (struct xpathProcessor_object *)Z_OBJ_P(return_value TSRMLS_CC);
	   zend_object* vvobj = Z_OBJ_P(return_value);
	   xpathProcessor_object * vobj = (xpathProcessor_object *)((char *)vvobj - XtOffsetOf(xpathProcessor_object, std));
            assert (vobj != nullptr);
	    xpathProcessor = proc->newXPathProcessor();
            vobj->xpathProcessor = xpathProcessor;
	}
    } else {
       
        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, newXsltProcessor)
{
//std::cerr<<"SaxonProcessor newXsltproc point 1"<<std::endl;
    //php_error(E_WARNING,"new xsltProc 1");
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XsltProcessor * xsltProcessor = nullptr;

      zend_object* pobj = Z_OBJ_P(getThis()); 
   saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

   // saxonProcessor_object *obj = (saxonProcessor_object *)Z_OBJ_P(getThis());
    assert (obj != nullptr);
    proc = obj->saxonProcessor;

    assert (proc != nullptr);
    if (proc != nullptr) {
    if (object_init_ex(return_value, xsltProcessor_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	xsltProcessor = proc->newXsltProcessor();
	   zend_object* vobj = Z_OBJ_P(return_value);
	   xsltProcessor_object * xproc_object = (xsltProcessor_object *)((char *)vobj - XtOffsetOf(xsltProcessor_object, std));
            assert (vobj != nullptr);
	    
            xproc_object->xsltProcessor = xsltProcessor;

    }
    } else {
       
        RETURN_NULL();
    }
}


PHP_METHOD(SaxonProcessor, newXslt30Processor)
{
//std::cerr<<"SaxonProcessor newXslt30proc point 1"<<std::endl;
   // php_error(E_WARNING,"new xslt30Proc 1");
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    Xslt30Processor * xslt30Processor = nullptr;

      zend_object* pobj = Z_OBJ_P(getThis());
   saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

   // saxonProcessor_object *obj = (saxonProcessor_object *)Z_OBJ_P(getThis());
    assert (obj != nullptr);
    proc = obj->saxonProcessor;

    assert (proc != nullptr);
    if (proc != nullptr) {
    if (object_init_ex(return_value, xslt30Processor_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	xslt30Processor = proc->newXslt30Processor();	
	   zend_object* vobj = Z_OBJ_P(return_value);
	   xslt30Processor_object * xproc_object = (xslt30Processor_object *)((char *)vobj - XtOffsetOf(xslt30Processor_object, std));
            assert (vobj != nullptr);

            xproc_object->xslt30Processor = xslt30Processor;

    }
    } else {

        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, newXQueryProcessor)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    XQueryProcessor * xqueryProcessor = nullptr;
     zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
	if (object_init_ex(return_value, xqueryProcessor_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	  xqueryProcessor = proc->newXQueryProcessor();
	  zend_object* vvobj = Z_OBJ_P(return_value);
	   xqueryProcessor_object * vobj = (xqueryProcessor_object *)((char *)vvobj - XtOffsetOf(xqueryProcessor_object, std));
            assert (vvobj != nullptr);
	    
            vobj->xqueryProcessor = xqueryProcessor;
     }
    } else {
       
        RETURN_NULL();
    }
}

PHP_METHOD(SaxonProcessor, newSchemaValidator)
{
   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    SaxonProcessor * proc;
    SchemaValidator * schemaValidator = nullptr;
   
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    assert (obj != nullptr);
    proc = obj->saxonProcessor;
    assert (proc != nullptr);
    if (proc != nullptr) {
	if (object_init_ex(return_value, schemaValidator_ce) != SUCCESS) {
            RETURN_NULL();
        } else {
	   //struct schemaValidator_object* vobj = (struct schemaValidator_object *)Z_OBJ_P(return_value TSRMLS_CC);
	   zend_object* vvobj = Z_OBJ_P(return_value);
	   schemaValidator_object * vobj = (schemaValidator_object *)((char *)vvobj - XtOffsetOf(schemaValidator_object, std));
            assert (vobj != nullptr);
	    schemaValidator = proc->newSchemaValidator();
	    if(schemaValidator == nullptr){
		RETURN_NULL();
	    }
            vobj->schemaValidator = schemaValidator;
	}
    } else {
       
        RETURN_NULL();
    }
}


PHP_METHOD(SaxonProcessor, version)
{
    SaxonProcessor *saxonProcessor;

   
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));
    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr) {
	const char * verStr = saxonProcessor->version();
        //char *str = estrdup(saxonProcessor->version());
        _RETURN_STRING(verStr);
//efree(verStr);
    }
    RETURN_NULL();
}



PHP_METHOD(SaxonProcessor, setConfigurationProperty)
{
    SaxonProcessor *saxonProcessor;
    char * name;
    char * value;
    size_t len1, len2;
    if (ZEND_NUM_ARGS()!= 2) {
        WRONG_PARAM_COUNT;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr && name != nullptr && value != nullptr) {
        saxonProcessor->setConfigurationProperty(name, value);
    }
    
}

PHP_METHOD(SaxonProcessor, registerPHPFunctions)
{
    SaxonProcessor *saxonProcessor;
    char * libName;
    size_t len1;
 //std::cerr<<"checkpoint in registerPHPFunction start"<<std::endl;
    if (ZEND_NUM_ARGS()!= 1) {
        WRONG_PARAM_COUNT;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &libName, &len1) == FAILURE) {
        RETURN_NULL();
    }
//std::cerr<<"checkpoint in registerPHPFunction cp2"<<std::endl;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    saxonProcessor_object * obj = (saxonProcessor_object *)((char *)pobj - XtOffsetOf(saxonProcessor_object, std));

    saxonProcessor = obj->saxonProcessor;
    if (saxonProcessor != nullptr && libName != nullptr) {
        saxonProcessor->setConfigurationProperty("extc", libName);
    }
saxonProcessor->registerNativeMethods(SaxonProcessor::sxn_environ->env, "com/saxonica/functions/extfn/cpp/PHPFunctionSet$PHPFunction",
    phpMethods, sizeof(phpMethods) / sizeof(phpMethods[0]));
   // std::cerr<<"checkpoint in registerPHPFunction end"<<std::endl;
}

/*     ============== XSLT10/20/30/31: PHP Interface of   XsltProcessor class =============== */

void XsltProcessor_free_storage(zend_object *object)
{

    zend_object_std_dtor(object);
}

void XsltProcessor_destroy_storage(zend_object *object)
{
    xsltProcessor_object *obj;

    zend_objects_destroy_object(object);
}

zend_object * xsltProcessor_create_handler(zend_class_entry *type)
{
   

    xsltProcessor_object *obj = (xsltProcessor_object *)ecalloc(1, sizeof(xsltProcessor_object)+ zend_object_properties_size(type));
   
    
   zend_object_std_init(&obj->std,type);
    object_properties_init(&obj->std, type);
    
    obj->std.handlers = &xsltProcessor_object_handlers;

    return &obj->std;
}



PHP_METHOD(XsltProcessor, __destruct)
{

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));


    XsltProcessor * xsltProcessor= obj->xsltProcessor;
    if(xsltProcessor != nullptr){
    	delete xsltProcessor;
     }
 
    
}

PHP_METHOD(XsltProcessor, transformFileToFile)
{
    XsltProcessor *xsltProcessor;
    char * outfileName;
    char * infilename;
    char * styleFileName;
    size_t len1, len2, len3;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "sss", &infilename, &len1, &styleFileName, &len2, &outfileName, &len3) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
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
   size_t len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    
    if (xsltProcessor != nullptr) {

        XdmValue * node = xsltProcessor->transformFileToValue(infilename, styleFileName);
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
		zend_object *ooth =  Z_OBJ_P(return_value);
                //struct xdmValue_object* vobj = (struct xdmValue_object *)Z_OBJ_P(return_value TSRMLS_CC);
		xdmValue_object* vobj  = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmValue = node;
            }
        } else {
            if(obj->xsltProcessor->exceptionOccurred()){
  		//TODO
	    }
        }
    }else {
        RETURN_NULL();
    }
}


PHP_METHOD(XsltProcessor, transformFileToString)
{
    XsltProcessor *xsltProcessor;
    char * infilename;
    char * styleFileName;
    size_t len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

        const char * result = xsltProcessor->transformFileToString(infilename, styleFileName);
	if(result != nullptr) {
            //char *str = estrdup(result);
            _RETURN_STRING(result);
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
//	std::cerr<<"phpNative called"<<std::endl;
	char *nativeString = (char *)senv->GetStringUTFChars(funcName, nullptr);
	std::string nativeString2 = std::string(nativeString);
	if(nativeString == nullptr) {
		return nullptr;
	}
	int nativeStrLen = strlen(nativeString);
//std::cerr<<"phpNative called. nativeString="<<nativeString<<", length="<<nativeStrLen<<std::endl;
	zval function_name;
	zval retval;
	
	int argLength = 0;
	zvalArr * php_argv= nullptr;
	if(arguments != nullptr) {
		argLength = (int)senv->GetArrayLength(arguments);
		php_argv = new zvalArr[argLength];
	}
	zval *params;
	if(argLength>0) {
		//(*params) = (zval**)malloc(sizeof(zval*) * argLength);
		params =  new zval[argLength];
	} else {
		params = nullptr;
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
		
//std::cerr<<"phpNative called cp2"<<std::endl;

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
		zend_object* zend_vobj;
		XdmNode * node = nullptr;
		std::map<std::string, saxonTypeEnum>::iterator it = typeMap.find(str);
		if (it != typeMap.end()){
			switch (it->second)
			{
				case enumNode:
					if(!nprocessor->proc){
						nprocessor->proc = (jobject)SaxonProcessor::sxn_environ->env->CallStaticObjectMethod(xdmValueForCppClass, procForNodeMID, argObj);
					}
					if (object_init_ex(php_argv[i]._val, xdmNode_ce) != SUCCESS) {
						//error
						
						//php_error(E_WARNING,"error phpNative xdmNode creation failed");
            					break;
        				} 
					node = new XdmNode(argObj);
					node->setProcessor(nprocessor);

					//MAKE_STD_ZVAL(php_argv[i]._val);
					zend_vobj = Z_OBJ_P(php_argv[i]._val);
	   				vobj = (xdmNode_object *)((char *)zend_vobj - XtOffsetOf(xdmNode_object, std));
            				assert (vobj != nullptr);
	    
                			vobj->xdmNode = node;
					break;
				case enumString:
					stri = senv->GetStringUTFChars((jstring)argObj, nullptr);
					//ZVAL_STRING(php_argv[i]._val);
					_ZVAL_STRING(php_argv[i]._val, stri);
					break;
				case enumInteger:
					sresult->xdmvalue = argObj; 
					lnumber = getLongValue(SaxonProcessor::sxn_environ, *sresult, 0);

					//MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_LONG(php_argv[i]._val, lnumber);					
					break;
				case enumDouble:
					sresult->xdmvalue = argObj; 
					dnumber = getDoubleValue(SaxonProcessor::sxn_environ, *sresult, 0);
					//MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_DOUBLE(php_argv[i]._val, dnumber);
					break;
				case enumFloat:
					sresult->xdmvalue = argObj; 
					fnumber = getFloatValue(SaxonProcessor::sxn_environ, *sresult, 0);
					//MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_DOUBLE(php_argv[i]._val, fnumber);					
					break;
				case enumBool:
					sresult->xdmvalue = argObj; 
					bvalue = getBooleanValue(SaxonProcessor::sxn_environ, *sresult);
					//MAKE_STD_ZVAL(php_argv[i]._val);
					ZVAL_BOOL(php_argv[i]._val, bvalue);						
					break;
				case enumArrXdmValue:
					//TODO - not currently supported
					argLength--;
					break;
			}
			senv->ReleaseStringUTFChars(argType, str);
		} 

}

	//TODO should free sresult but it causes memory corruption	
		
// array of zvals to execute
    
//std::cerr<<"phpNative called cp3"<<std::endl;
 
    	// convert all the values
   	 for(int i = 0; i < argLength; i++) { 

		params[i] = *php_argv[i]._val; }

	//std::cerr<<"phpNative called cp3-1"<<std::endl;
	//note: no implicit type conversion.

	zval *argvv = nullptr;//xparams;
	zval* callOnObj = nullptr;
	//MAKE_STD_ZVAL(function_name);
	//nativeString[nativeStrLen] = '\0';

	ZVAL_STRING(&function_name, nativeString);
//std::cerr<<"phpNative called cp3-2, argumentLen="<<argLength<<std::endl;
	if(call_user_function_ex(CG(function_table), (zval*)callOnObj, &function_name, &retval, argLength, params, 0, nullptr) != SUCCESS)
	{
 	   zend_error(E_ERROR, "Function call failed");
	}

	if(Z_TYPE(retval) ==0){
		zend_error(E_ERROR, "Function returned nullptr");
	}

	char * sVal = nullptr;
	int len = 0;
	jobject obj = nullptr;
//TODO handle XdmValue wrapped object
const char * objName = nullptr;
xdmNode_object* ooth = nullptr;
zend_object* zend_vobj2;
bool bVal;
	switch (Z_TYPE_P(&retval)) {
            case IS_FALSE:
		obj= booleanValue(SaxonProcessor::sxn_environ, false);
                break;
	    case IS_TRUE:
                obj= booleanValue(SaxonProcessor::sxn_environ, true);
                break;
            
            case IS_LONG:
                obj= longValue(SaxonProcessor::sxn_environ, Z_LVAL_P(&retval));
                break;
            case IS_STRING:
                sVal = Z_STRVAL_P(&retval);
                len = Z_STRLEN_P(&retval);
		obj = getJavaStringValue(SaxonProcessor::sxn_environ,estrndup(sVal, len));
                break;
            break;
            case IS_NULL:
                
            	break;
            case IS_DOUBLE:
		obj = doubleValue(SaxonProcessor::sxn_environ, (double)Z_DVAL_P(&retval));
		 break;
            
            case IS_ARRAY:
            //break;
            case IS_OBJECT:
		
            	objName =ZSTR_VAL(Z_OBJCE_P(&retval)->name);
      

      		if(strcmp(objName, "Saxon\\XdmNode")==0) {
		
			zend_vobj2 =  Z_OBJ_P(&retval);
			ooth = (xdmNode_object *)((char *)zend_vobj2 - XtOffsetOf(xdmNode_object, std));

        		if(ooth != nullptr) {
            			obj = ooth->xdmNode->getUnderlyingValue();
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


    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {

        const char * result = xsltProcessor->transformToString();
        if(result != nullptr) {
            _RETURN_STRING(result);
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
    RETURN_NULL();
}

PHP_METHOD(XsltProcessor, transformToValue)
{
    XsltProcessor *xsltProcessor;

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

	XdmValue * node = xsltProcessor->transformToValue();
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                //struct xdmValue_object* vobj = (struct xdmValue_object *)Z_OBJ_P(return_value);
		zend_object *vvobj =  Z_OBJ_P(return_value);
		xdmValue_object* vobj  = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmValue = node;
            }
        } else if(xsltProcessor->exceptionOccurred()){
            xsltProcessor->checkException();
	    RETURN_NULL();
        }
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(XsltProcessor, transformToFile)
{
    XsltProcessor *xsltProcessor;
 
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

	xsltProcessor->transformToFile();
        if(xsltProcessor->exceptionOccurred()) {
           //TODO
            const char * exStr = xsltProcessor->checkException();
        }
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(XsltProcessor, compileFromFile)
{
    XsltProcessor *xsltProcessor;
    char * name;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->compileFromFile(name);
    }
}

PHP_METHOD(XsltProcessor, compileFromString)
{
    XsltProcessor *xsltProcessor;
    char * stylesheetStr;
    size_t len1, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &stylesheetStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->compileFromString(stylesheetStr);
    }
}

PHP_METHOD(XsltProcessor, compileFromStringAndSave)
{
    XsltProcessor *xsltProcessor;
    char * stylesheetStr;
    char * filename;
    size_t len1, len2, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &stylesheetStr, &filename, &len1, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->compileFromStringAndSave(stylesheetStr, filename);
    }
}

PHP_METHOD(XsltProcessor, compileFromFileAndSave)
{
    XsltProcessor *xsltProcessor;
    char * stylesheetFilename;
    char * filename;
    size_t len1, len2, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &stylesheetFilename, &filename, &len1, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->compileFromStringAndSave(stylesheetFilename, filename);
    }
}



PHP_METHOD(XsltProcessor, compileFromValue)
{
    XsltProcessor *xsltProcessor;
   zval* oth;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "O", &oth, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
	zend_object* vvobj = Z_OBJ_P(oth);
	xdmValue_object* ooth = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
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

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {

    if(!oth) {
	php_error(E_WARNING, "Error setting source value");
	return;
    } else {
	if(Z_TYPE_P(oth) ==IS_NULL){
		php_error(E_WARNING, "Error setting source value");
		return;
	}
	
      const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object * nobj = Z_OBJ_P(oth);

	xdmNode_object* ooth = (xdmNode_object *)((char *)nobj - XtOffsetOf(xdmNode_object, std));//(xdmNode_object*)Z_OBJ_P(oth);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
		
	        xsltProcessor->setSourceFromXdmNode(value);

            }
        }
      } /*else if(strcmp(objName, "Saxon\\XdmValue")==0) {
	zend_object* vvobj = Z_OBJ_P(oth);
	xdmValue_object* ooth = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr && value) {
	        xsltProcessor->setSourceFromXdmNode((XdmNode*)value);
            }
        }
      }  */

        
    }
  }
}


PHP_METHOD(XsltProcessor, setJustInTimeCompilation)
{
    XsltProcessor *xsltProcessor;
    bool jit = false;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "b", &jit) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
	 xsltProcessor->setJustInTimeCompilation(jit);
    }
}

PHP_METHOD(XsltProcessor, setOutputFile)
{
    XsltProcessor *xsltProcessor;
    char * outputFilename;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &outputFilename, &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr && outputFilename != nullptr) {
        
	 xsltProcessor->setOutputFile(outputFilename);
            
        
    }
}


PHP_METHOD(XsltProcessor, setBaseOutputURI)
{
    XsltProcessor *xsltProcessor;
    char * baseOutputURI = nullptr;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &baseOutputURI, &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr && baseOutputURI != nullptr) {

	 xsltProcessor->setBaseOutputURI(baseOutputURI);


    }
}


int size_t2int(size_t val) {
    return (val <= INT_MAX) ? (int)((ssize_t)val) : -1;
}

PHP_METHOD(XsltProcessor, setSourceFromFile)
{
    XsltProcessor *xsltProcessor;
    char * inFilename;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &inFilename, &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
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
    size_t len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
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
   size_t len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
	if(Z_TYPE_P(oth) ==IS_NULL){
		php_error(E_WARNING, "Error setting source value - value is nullptr");
		return;
	}

      const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object* ooth = Z_OBJ_P(oth);
	xdmNode_object * nobj = (xdmNode_object *)((char *)ooth - XtOffsetOf(xdmNode_object, std));
        if(nobj != nullptr) {
            XdmNode * value = nobj->xdmNode;
            if(value != nullptr) {
	        xsltProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	zend_object* ooth = Z_OBJ_P(oth);
	xdmValue_object * vobj = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
        if(vobj != nullptr) {
            XdmValue * value = vobj->xdmValue;
            if(value != nullptr) {
		
                xsltProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	zend_object* ooth = Z_OBJ_P(oth);
	xdmAtomicValue_object * aobj = (xdmAtomicValue_object *)((char *)ooth - XtOffsetOf(xdmAtomicValue_object, std));
        if(aobj != nullptr) {
            XdmAtomicValue * value = aobj->xdmAtomicValue;
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

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
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

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->clearProperties();
    }
}



PHP_METHOD(XsltProcessor, setupXslMessage)
{
    XsltProcessor *xsltProcessor;
    bool show = false;
    char * filename = nullptr;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "bs", &show, &filename &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;

    if (xsltProcessor != nullptr) {

	    xsltProcessor->setupXslMessage(show, filename);
	}

}



PHP_METHOD(XsltProcessor, exceptionOccurred)
{
    XsltProcessor *xsltProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
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
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
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
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        const char * errCode = xsltProcessor->getErrorCode((int)index);
        if(errCode != nullptr) {
            //char *str = estrdup(errCode);
            _RETURN_STRING(errCode);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XsltProcessor, getErrorMessage)
{
    XsltProcessor *xsltProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        const char * errStr = xsltProcessor->getErrorMessage((int)index);
        if(errStr != nullptr) {
            //char *str = estrdup(errStr);
            _RETURN_STRING(errStr);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(XsltProcessor, exceptionClear)
{
    XsltProcessor *xsltProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xsltProcessor_object *obj = (xsltProcessor_object *)((char *)pobj - XtOffsetOf(xsltProcessor_object, std));
    xsltProcessor = obj->xsltProcessor;
    if (xsltProcessor != nullptr) {
        xsltProcessor->exceptionClear();
    }
}



/*     ============== XSLT30: PHP Interface of  Xslt30Processor class =============== */

void Xslt30Processor_free_storage(zend_object *object)
{

    zend_object_std_dtor(object);

}

void Xslt30Processor_destroy_storage(zend_object *object)
{
    xslt30Processor_object *obj;

    zend_objects_destroy_object(object);
}

zend_object * xslt30Processor_create_handler(zend_class_entry *type)
{


    xslt30Processor_object *obj = (xslt30Processor_object *)ecalloc(1, sizeof(xslt30Processor_object)+ zend_object_properties_size(type));


   zend_object_std_init(&obj->std,type);
    object_properties_init(&obj->std, type);

    obj->std.handlers = &xslt30Processor_object_handlers;

    return &obj->std;
}



PHP_METHOD(Xslt30Processor, __destruct)
{

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));


    Xslt30Processor * xslt30Processor= obj->xslt30Processor;
    if(xslt30Processor != nullptr){
    	delete xslt30Processor;
     }


}


PHP_METHOD(Xslt30Processor, setBaseOutputURI)
{
    Xslt30Processor *xslt30Processor;
    char * baseOutputURI = nullptr;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &baseOutputURI, &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr && baseOutputURI != nullptr) {

	 xslt30Processor->setBaseOutputURI(baseOutputURI);


    }
}



PHP_METHOD(Xslt30Processor, callFunctionReturningValue)
{
//arguments: const char * stylesheetFilename, const char* functionName, XdmValue * arguments, int argument_length

    Xslt30Processor *xslt30Processor;
    HashTable *arr_hash;
    char* functionName;
    zval * arguments_zval;
    zval * val;
    long num_key;
    XdmValue ** arguments = nullptr;
    int argument_length=0;
    zend_string *key;

    char * infilename;
    char * styleFileName = nullptr;
    size_t len1, len2;

    if (ZEND_NUM_ARGS()==2 && zend_parse_parameters(ZEND_NUM_ARGS() , "sa", &functionName, &len2, &arguments_zval) == FAILURE) {
        RETURN_NULL();
    } else if (ZEND_NUM_ARGS()==3 && zend_parse_parameters(ZEND_NUM_ARGS() , "ssa", &styleFileName, &len1, &functionName, &len2, &arguments_zval) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    arr_hash = Z_ARRVAL_P(arguments_zval);
    argument_length = zend_hash_num_elements(arr_hash);
   
    arguments = new XdmValue*[argument_length];
      
    if (xslt30Processor != nullptr) {
      ZEND_HASH_FOREACH_KEY_VAL(arr_hash, num_key, key, val) {
        if(Z_TYPE_P(val) != nullptr) {
	const char * objName =ZSTR_VAL(Z_OBJCE_P(val)->name);
      /*std::cerr<<"test type:"<<(Z_OBJCE_P(val)->name)<<std::endl;
	php_printf("num_key %d =>", num_key);
	php_printf("key %d =>", key);
	*/
	if(strcmp(objName, "Saxon\\XdmNode")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmNode_object * nobj = (xdmNode_object *)((char *)ooth - XtOffsetOf(xdmNode_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmNode;
            		arguments[num_key] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmAtomicValue_object * nobj = (xdmAtomicValue_object *)((char *)ooth - XtOffsetOf(xdmAtomicValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmAtomicValue;
            		arguments[num_key] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmValue_object * nobj = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = nobj->xdmValue;
            		arguments[num_key] = value;
        	}
	}
	else {//TODO error warning}
         }}else {
		//TODO error warning
        }
      } ZEND_HASH_FOREACH_END();


	//php_printf(" argument_length= %d",argument_length);
        XdmValue * result = xslt30Processor->callFunctionReturningValue(styleFileName, functionName, arguments, argument_length);
	if(result != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
		zend_object *ooth =  Z_OBJ_P(return_value);
                //struct xdmValue_object* vobj = (struct xdmValue_object *)Z_OBJ_P(return_value TSRMLS_CC);
		xdmValue_object* vobj  = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmValue = result;
            }
        } else {
            if(obj->xslt30Processor->exceptionOccurred()){
  		//TODO
	    }
	    RETURN_NULL();
        }
    }


}

PHP_METHOD(Xslt30Processor, callFunctionReturningString){
    Xslt30Processor *xslt30Processor;
    HashTable *arr_hash;
    char* functionName;
    zval * arguments_zval;
    zval * val;
    long num_key;
    XdmValue ** arguments = nullptr;
    int argument_length=0;
    zend_string *key;

    char * infilename;
    char * styleFileName = nullptr;
    size_t len1, len2;

     if (ZEND_NUM_ARGS()==2 && zend_parse_parameters(ZEND_NUM_ARGS() , "sa", &functionName, &len2, &arguments_zval) == FAILURE) {
        RETURN_NULL();
      }else if (ZEND_NUM_ARGS()==3 && zend_parse_parameters(ZEND_NUM_ARGS() , "ssa", &styleFileName, &len1, &functionName, &len2, &arguments_zval) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    arr_hash = Z_ARRVAL_P(arguments_zval);
    argument_length = zend_hash_num_elements(arr_hash);
   
    arguments = new XdmValue*[argument_length];
      
    if (xslt30Processor != nullptr) {
      ZEND_HASH_FOREACH_KEY_VAL(arr_hash, num_key, key, val) {
        if(Z_TYPE_P(val) != nullptr) {
	const char * objName =ZSTR_VAL(Z_OBJCE_P(val)->name);
      /*std::cerr<<"test type:"<<(Z_OBJCE_P(val)->name)<<std::endl;
	php_printf("num_key %d =>", num_key);
	php_printf("key %d =>", key);
	*/
	if(strcmp(objName, "Saxon\\XdmNode")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmNode_object * nobj = (xdmNode_object *)((char *)ooth - XtOffsetOf(xdmNode_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmNode;
            		arguments[num_key] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmAtomicValue_object * nobj = (xdmAtomicValue_object *)((char *)ooth - XtOffsetOf(xdmAtomicValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmAtomicValue;
            		arguments[num_key] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmValue_object * nobj = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = nobj->xdmValue;
            		arguments[num_key] = value;
        	}
	}
	else {//TODO error warning}
         }}else {
		//TODO error warning
        }
      } ZEND_HASH_FOREACH_END();

        const char * result = xslt30Processor->callFunctionReturningString(styleFileName, functionName, arguments, argument_length);
	if(result != nullptr) {
            //char *str = estrdup(result);
            _RETURN_STRING(result);
        } else if(xslt30Processor->exceptionOccurred()){
            //TODO: xslt30Processor->checkException();
            const char * errStr = xslt30Processor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xslt30Processor->getErrorCode(0);
                if(errorCode!=nullptr) {
                    // TODO: throw exception
                }
            }
        }
    }



}


    PHP_METHOD(Xslt30Processor, callFunctionReturningFile){
    HashTable *arr_hash;
    Xslt30Processor *xslt30Processor;
    char* functionName;
    zval * arguments_zval;
    zval * val;
    long num_key;
    XdmValue ** arguments = nullptr;
    int argument_length=0;
    zend_string *key;

    char * outfilename;
    char * styleFileName = nullptr;
    size_t len1, len2, len3;


    if (ZEND_NUM_ARGS()==3 && zend_parse_parameters(ZEND_NUM_ARGS() , "sas", &functionName, &len2, &arguments_zval, &outfilename, &len3) == FAILURE) {
        RETURN_NULL();
    } else if (ZEND_NUM_ARGS()==4 && zend_parse_parameters(ZEND_NUM_ARGS() , "ssas", &styleFileName, &len1, &functionName, &len2, &arguments_zval, &outfilename, &len3) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    arr_hash = Z_ARRVAL_P(arguments_zval);
    argument_length = zend_hash_num_elements(arr_hash);
   
    arguments = new XdmValue*[argument_length];
      
    if (xslt30Processor != nullptr) {
      ZEND_HASH_FOREACH_KEY_VAL(arr_hash, num_key, key, val) {
        if(Z_TYPE_P(val) != nullptr) {
	const char * objName =ZSTR_VAL(Z_OBJCE_P(val)->name);
      /*std::cerr<<"test type:"<<(Z_OBJCE_P(val)->name)<<std::endl;
	php_printf("num_key %d =>", num_key);
	php_printf("key %d =>", key);
	*/
	if(strcmp(objName, "Saxon\\XdmNode")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmNode_object * nobj = (xdmNode_object *)((char *)ooth - XtOffsetOf(xdmNode_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmNode;
            		arguments[num_key] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmAtomicValue_object * nobj = (xdmAtomicValue_object *)((char *)ooth - XtOffsetOf(xdmAtomicValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmAtomicValue;
            		arguments[num_key] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmValue_object * nobj = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = nobj->xdmValue;
            		arguments[num_key] = value;
        	}
	}
	else {//TODO error warning}
         }}else {
		//TODO error warning
        }
      } ZEND_HASH_FOREACH_END();


        xslt30Processor->callFunctionReturningFile(styleFileName, functionName, arguments, argument_length, outfilename);
	if(xslt30Processor->exceptionOccurred()){
            //TODO: xslt30Processor->checkException();
            const char * errStr = xslt30Processor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xslt30Processor->getErrorCode(0);
            }
        }
    }

}

    PHP_METHOD(Xslt30Processor, callTemplateReturningValue){

  Xslt30Processor *xslt30Processor;
    char* templateName = nullptr;
   
    char * styleFileName = nullptr;
    size_t len1, len2;


    if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS() , "s", &templateName, &len2) == FAILURE) {
        RETURN_NULL();
    } else if (ZEND_NUM_ARGS()>1 && zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &styleFileName, &len1, &templateName, &len2) == FAILURE) {
        RETURN_NULL();
    }


    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
      
    if (xslt30Processor != nullptr) {
   

	//php_printf(" argument_length= %d",argument_length);
        XdmValue * result = xslt30Processor->callTemplateReturningValue(styleFileName, templateName);
	if(result != nullptr) {
 
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) { 
               RETURN_NULL();
            } else {
		zend_object *ooth =  Z_OBJ_P(return_value);
              
		xdmValue_object* vobj  = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmValue = result;
		
            }
        } else {
            if(obj->xslt30Processor->exceptionOccurred()){
  		//TODO
	    }
	    RETURN_NULL();
        }
    }
   }


    PHP_METHOD(Xslt30Processor, callTemplateReturningString){
    Xslt30Processor *xslt30Processor;
    char* templateName = nullptr;
   
    char * styleFileName = nullptr;
    size_t len1, len2;

    if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS() , "s", &templateName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    if (ZEND_NUM_ARGS()>1 && zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &styleFileName, &len1, &templateName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
      
    if (xslt30Processor != nullptr) {
   

	//php_printf(" argument_length= %d",argument_length);
        const char * result = xslt30Processor->callTemplateReturningString(styleFileName, templateName);
	if(result != nullptr) {
            _RETURN_STRING(result); 
        } else {
            if(xslt30Processor->exceptionOccurred()){
            xslt30Processor->checkException();
            const char * errStr = xslt30Processor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xslt30Processor->getErrorCode(0);
                if(errorCode!=nullptr) {
                    // TODO: throw exception
                }
            }
           }
        }
    }

     RETURN_NULL();
    }

    PHP_METHOD(Xslt30Processor, callTemplateReturningFile){
   Xslt30Processor *xslt30Processor;
    char* templateName = nullptr;
    char * filename = nullptr;
    char * styleFileName = nullptr;
    size_t len1, len2, len3;

    if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS() , "s", &templateName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    if (ZEND_NUM_ARGS()>1 && zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &styleFileName, &len1, &templateName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
      
    if (xslt30Processor != nullptr) {
   	xslt30Processor->callTemplateReturningFile(styleFileName, templateName, filename);
    }

  }


PHP_METHOD(Xslt30Processor, applyTemplatesReturningValue){
   Xslt30Processor *xslt30Processor;
    char * styleFileName = nullptr;
    size_t len1;

    if (ZEND_NUM_ARGS()>0) {

      if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &styleFileName, &len1) == FAILURE) {
          RETURN_NULL();
      }
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
      
    if (xslt30Processor != nullptr) {
   	XdmValue * result = xslt30Processor->applyTemplatesReturningValue(styleFileName);
    
	if(result != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
		zend_object *ooth =  Z_OBJ_P(return_value);
        	xdmValue_object* vobj  = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmValue = result;
            }
        } else {
            if(obj->xslt30Processor->exceptionOccurred()){
  		//TODO
	    }
	    RETURN_NULL();
        }



    } else {
     RETURN_NULL();
    }
}


PHP_METHOD(Xslt30Processor, applyTemplatesReturningString){
 Xslt30Processor *xslt30Processor;

    char * styleFileName = nullptr;
    size_t len1;


    if (ZEND_NUM_ARGS()>0) {
    
    	if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &styleFileName, &len1) == FAILURE) {
    	    RETURN_NULL();
    	}
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
      
    if (xslt30Processor != nullptr) {
        const char * result = xslt30Processor->applyTemplatesReturningString(styleFileName);
   
	if(result != nullptr) {
            _RETURN_STRING(result); 
        } else {
            if(xslt30Processor->exceptionOccurred()){
            xslt30Processor->checkException();
            const char * errStr = xslt30Processor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xslt30Processor->getErrorCode(0);
                if(errorCode!=nullptr) {
                    // TODO: throw exception
                }
            }
           }
        }
    }

     RETURN_NULL();
   


}



PHP_METHOD(Xslt30Processor, applyTemplatesReturningFile){
 Xslt30Processor *xslt30Processor;
    char * filename = nullptr;
    char * styleFileName = nullptr;
    size_t len1, len2;

    if (ZEND_NUM_ARGS()==2 && zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &styleFileName, &len1, &filename, &len2) == FAILURE) {
          RETURN_NULL();
    }

   if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS() , "s", &filename, &len2) == FAILURE) {
          RETURN_NULL();
    }
    

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
      
    if (xslt30Processor != nullptr) {
   	xslt30Processor->applyTemplatesReturningFile(styleFileName, filename);
    }



}


/*PHP_METHOD(Xslt30Processor, addPackages){
    HashTable *arr_hash;
    Xslt30Processor *xslt30Processor;
    char* functionName = nullptr;
    zval * arguments_zval;
    zval * val;
    long num_key;
    const char ** arguments = nullptr;
    int argument_length=0;
    zend_string *key;

    char * infilename = nullptr;
    char * styleFileName = nullptr;
    size_t len1, len2;

        if (zend_parse_parameters(ZEND_NUM_ARGS() , "a", &arguments_zval) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    arr_hash = Z_ARRVAL_P(arguments_zval);
    argument_length = zend_hash_num_elements(arr_hash);
   
    arguments = new const char*[argument_length];

  if (xslt30Processor != nullptr) {
      ZEND_HASH_FOREACH_KEY_VAL(arr_hash, num_key, key, val) {
        if(Z_TYPE_P(val) != nullptr && Z_TYPE_P(val)== IS_STRING) {

		arguments[num_key] = (const char*)Z_STRVAL_P(val);
     		
        } 
    }  ZEND_HASH_FOREACH_END();

        xslt30Processor->addPackages(arguments, argument_length);
	if(xslt30Processor->exceptionOccurred()){
            //TODO: xslt30Processor->checkException();
            const char * errStr = xslt30Processor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xslt30Processor->getErrorCode(0);
            }
        }
    }
}    */


    PHP_METHOD(Xslt30Processor,setInitialTemplateParameters){

  Xslt30Processor *xslt30Processor;
    HashTable *arr_hash;
    zval * arguments_zval;
    zval * val;
    long num_key;
    XdmValue ** arguments;
    int argument_length=0;
    zend_string *key;

    bool tunnel = false;
    if (ZEND_NUM_ARGS()==1 && zend_parse_parameters(ZEND_NUM_ARGS() , "a", &arguments_zval) == FAILURE) {
        RETURN_NULL();
    }

    if (ZEND_NUM_ARGS()==2 && zend_parse_parameters(ZEND_NUM_ARGS() , "ab", &arguments_zval, &tunnel) == FAILURE) {
        RETURN_NULL();
    }


    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    arr_hash = Z_ARRVAL_P(arguments_zval);
    argument_length = zend_hash_num_elements(arr_hash);
   
    arguments = new XdmValue*[argument_length];
    std::map<std::string,XdmValue*> parameters;
    if (xslt30Processor != nullptr) {
      ZEND_HASH_FOREACH_KEY_VAL(arr_hash, num_key, key, val) {
        if(Z_TYPE_P(val) != nullptr) {
	const char * objName =ZSTR_VAL(Z_OBJCE_P(val)->name);
      /*std::cerr<<"test type:"<<(Z_OBJCE_P(val)->name)<<std::endl;
	php_printf("num_key %d =>", num_key);
	php_printf("key %d =>", key);
	*/
	if(strcmp(objName, "Saxon\\XdmNode")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmNode_object * nobj = (xdmNode_object *)((char *)ooth - XtOffsetOf(xdmNode_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmNode;
            		parameters[ZSTR_VAL(key)] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmAtomicValue_object * nobj = (xdmAtomicValue_object *)((char *)ooth - XtOffsetOf(xdmAtomicValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = (XdmValue *) nobj->xdmAtomicValue;
            		parameters[ZSTR_VAL(key)] = value;
        	}
	}
	else if(strcmp(objName, "Saxon\\XdmValue")==0) {
		zend_object* ooth = Z_OBJ_P(val);
		xdmValue_object * nobj = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
        	if(nobj != nullptr) {
            		XdmValue * value = nobj->xdmValue;
            		parameters[ZSTR_VAL(key)] = value;
        	}
	}
	else {//TODO error warning}
         }}else {
		//TODO error warning
        }
      } ZEND_HASH_FOREACH_END();


	//php_printf(" argument_length= %d",argument_length);
        xslt30Processor->setInitialTemplateParameters(parameters, tunnel);
	
    }

    }


PHP_METHOD(Xslt30Processor, setInitialMatchSelection){
   Xslt30Processor *xslt30Processor;
    zval* oth = nullptr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {

    if(!oth) {
	php_error(E_WARNING, "Error setting source value");
	return;
    } else {
	if(Z_TYPE_P(oth) ==IS_NULL){
		php_error(E_WARNING, "Error setting source value - nullptr found");
		return;
	}

      const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      
      if(strcmp(objName, "Saxon\\XdmValue")==0) {
	zend_object * nobj = Z_OBJ_P(oth);

	xdmValue_object* ooth = (xdmValue_object *)((char *)nobj - XtOffsetOf(xdmValue_object, std));//(xdmNode_object*)Z_OBJ_P(oth);
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		xslt30Processor->setInitialMatchSelection(value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object * nobj = Z_OBJ_P(oth);

	xdmNode_object* ooth = (xdmNode_object *)((char *)nobj - XtOffsetOf(xdmNode_object, std));//(xdmNode_object*)Z_OBJ_P(oth);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
		XdmValue  *valueX = (XdmValue*)value;
	        xslt30Processor->setInitialMatchSelection(valueX);

            } else {

php_error(E_WARNING, "XdmNode - seInitialMatchSelection - cp1");
}
        }
      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0) {
	zend_object* vvobj = Z_OBJ_P(oth);

	xdmAtomicValue_object* ooth = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
	        xslt30Processor->setInitialMatchSelection((XdmValue*)value);
            }
        }
      } else {
	//TODO raise warning
	}


    }
  }




}
    

PHP_METHOD(Xslt30Processor, setGlobalContextItem)
{
    Xslt30Processor *xslt30Processor;
    zval* oth = nullptr;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {

    if(!oth) {
	php_error(E_WARNING, "Error setting source value");
	return;
    } else {
	if(Z_TYPE_P(oth) ==IS_NULL){
		php_error(E_WARNING, "Error setting source value");
		return;
	}

      const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object * nobj = Z_OBJ_P(oth);

	xdmNode_object* ooth = (xdmNode_object *)((char *)nobj - XtOffsetOf(xdmNode_object, std));//(xdmNode_object*)Z_OBJ_P(oth);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
		XdmItem  *valueX = (XdmItem*)value;
	        xslt30Processor->setGlobalContextItem(valueX);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0) {
	zend_object* vvobj = Z_OBJ_P(oth);
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
	        xslt30Processor->setGlobalContextItem((XdmItem*)value);
            }
        }
      } else {
	//TODO raise warning
	}


    }
  }
}


PHP_METHOD(Xslt30Processor, setGlobalContextFromFile)
{
    Xslt30Processor *xslt30Processor;
    char * inFilename = nullptr;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &inFilename, &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr && inFilename != nullptr) {
	 xslt30Processor->setGlobalContextFromFile(inFilename);


    }
}


PHP_METHOD(Xslt30Processor, setInitialMatchSelectionAsFile)
{
    Xslt30Processor *xslt30Processor;
    char * inFilename = nullptr;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &inFilename, &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr && inFilename != nullptr) {
	 xslt30Processor->setInitialMatchSelectionAsFile(inFilename);


    }
}


PHP_METHOD(Xslt30Processor, transformFileToFile)
{
    Xslt30Processor *xslt30Processor;
    char * outfileName = nullptr;
    char * infilename = nullptr;
    char * styleFileName = nullptr;
    size_t len1, len2, len3;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "sss", &infilename, &len1, &styleFileName, &len2, &outfileName, &len3) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;

    if (xslt30Processor != nullptr) {

        xslt30Processor->transformFileToFile(infilename, styleFileName, outfileName);
        if(xslt30Processor->exceptionOccurred()) {
     	  // TODO: throw exception
        }
    }
}

PHP_METHOD(Xslt30Processor, transformFileToValue)
{
    Xslt30Processor *xslt30Processor;
    char * infilename = nullptr;
    char * styleFileName = nullptr;
   size_t len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;

    if (xslt30Processor != nullptr) {

        XdmValue * node = xslt30Processor->transformFileToValue(infilename, styleFileName);
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                //struct xdmValue_object* vobj = (struct xdmValue_object *)Z_OBJ_P(return_value TSRMLS_CC);
		zend_object *ooth =  Z_OBJ_P(return_value);
		xdmValue_object* vobj  = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmValue = node;
            }
        } else {
            if(obj->xslt30Processor->exceptionOccurred()){
  		//TODO
	    }
        }
    }else {
        RETURN_NULL();
    }
}


PHP_METHOD(Xslt30Processor, transformFileToString)
{
    Xslt30Processor *xslt30Processor;
    char * infilename = nullptr;
    char * styleFileName = nullptr;
    size_t len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &infilename, &len1, &styleFileName, &len2) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;

    if (xslt30Processor != nullptr) {

        const char * result = xslt30Processor->transformFileToString(infilename, styleFileName);
	if(result != nullptr) {
            //char *str = estrdup(result);
            _RETURN_STRING(result);
        } else if(xslt30Processor->exceptionOccurred()){
            //TODO: xslt30Processor->checkException();
            const char * errStr = xslt30Processor->getErrorMessage(0);
            if(errStr != nullptr) {
                const char * errorCode = xslt30Processor->getErrorCode(0);
                if(errorCode!=nullptr) {
                    // TODO: throw exception
                }
            }
        }
    }
}



PHP_METHOD(Xslt30Processor, transformToString)
{
    Xslt30Processor *xslt30Processor;

    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }


    zval* oth;
   

    if (ZEND_NUM_ARGS() == 1 && zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }


    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {


     if(ZEND_NUM_ARGS() == 0) {
        const char * result = xslt30Processor->transformToString();
        if(result != nullptr) {
            _RETURN_STRING(result);
            return;

        }


     } else {
        if(!oth) {
            php_error(E_WARNING, "Error setting source value");
            return;
        }

	    if(Z_TYPE_P(oth) ==IS_NULL){
		    php_error(E_WARNING, "Error setting source value");
		    return;
	    }

        const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
        if(strcmp(objName, "Saxon\\XdmNode")==0) {
	        zend_object * nobj = Z_OBJ_P(oth);

	        xdmNode_object* ooth = (xdmNode_object *)((char *)nobj - XtOffsetOf(xdmNode_object, std));//(xdmNode_object*)Z_OBJ_P(oth);
            if(ooth != nullptr) {
                XdmNode * value = ooth->xdmNode;
                if(value != nullptr) {
        	        const char * result = xslt30Processor->transformToString(value);
                    if(result != nullptr) {
                        _RETURN_STRING(result);
                    }
                }

            }
        }
     }
    }

    RETURN_NULL();
}

PHP_METHOD(Xslt30Processor, transformToValue)
{
    Xslt30Processor *xslt30Processor;

    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }


    zval* oth;
   

    if (ZEND_NUM_ARGS() == 1 && zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;

    if (xslt30Processor != nullptr) {
      if(!oth) {
	php_error(E_WARNING, "Error setting source value");
	return;
      } else {
        XdmValue * node = nullptr;
        if(ZEND_NUM_ARGS() == 0) {
            XdmValue * node = xslt30Processor->transformToValue();

        } else {

            if(Z_TYPE_P(oth) == IS_NULL){
                php_error(E_WARNING, "Error setting source value");
                return;
            }

            const char *objName = ZSTR_VAL(Z_OBJCE_P(oth)->name);

            if(strcmp(objName,"Saxon\\XdmNode")==0) {
                zend_object *nobj = Z_OBJ_P(oth);

                xdmNode_object *ooth = (xdmNode_object *) ((char *) nobj -
                                           XtOffsetOf(xdmNode_object, std));//(xdmNode_object*)Z_OBJ_P(oth);
                if(ooth != nullptr) {
                    XdmNode *value = ooth->xdmNode;
                    if(value != nullptr) {
                        node = xslt30Processor->transformToValue(value);
                    }
                }
            }
        }
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
            } else {
                zend_object *vvobj =  Z_OBJ_P(return_value);
		        xdmValue_object* vobj  = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
		        assert (vobj != nullptr);
		        vobj->xdmValue = node;
		        return;
            }
        }
      }



    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(Xslt30Processor, transformToFile)
{
    Xslt30Processor *xslt30Processor;

    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }


    zval* oth;
   

    if (ZEND_NUM_ARGS() == 1 && zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;

    if (xslt30Processor != nullptr) {
      if(!oth) {
	php_error(E_WARNING, "Error setting source value");
	return;
      } else {
        if(ZEND_NUM_ARGS() == 0) {
            xslt30Processor->transformToFile();
        } else {
            if(Z_TYPE_P(oth) == mIS_NULL){
                php_error(E_WARNING,"Error setting source value");
                return;
            }

            const char *objName = ZSTR_VAL(Z_OBJCE_P(oth)->name);

            if(strcmp(objName,"Saxon\\XdmNode")==0) {
                zend_object *nobj = Z_OBJ_P(oth);

                xdmNode_object *ooth = (xdmNode_object *) ((char *) nobj -
                                           XtOffsetOf(xdmNode_object, std));//(xdmNode_object*)Z_OBJ_P(oth);
                if(ooth != nullptr) {
                    XdmNode *value = ooth->xdmNode;
                    if(value != nullptr) {
                        xslt30Processor->transformToFile(value);
                    }
                }
            }

        }
      }
    }
}

PHP_METHOD(Xslt30Processor, compileFromFile)
{
    Xslt30Processor *xslt30Processor;
    char * name = nullptr;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->compileFromFile(name);
    }
}


PHP_METHOD(Xslt30Processor, compileFromAssociatedFile)
{
    Xslt30Processor *xslt30Processor;
    char * name = nullptr;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->compileFromAssociatedFile(name);
    }
}


PHP_METHOD(Xslt30Processor, compileFromString)
{
    Xslt30Processor *xslt30Processor;
    char * stylesheetStr;
    size_t len1, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &stylesheetStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->compileFromString(stylesheetStr);
    }
}

PHP_METHOD(Xslt30Processor, compileFromStringAndSave)
{
    Xslt30Processor *xslt30Processor;
    char * stylesheetStr;
    char * filename = nullptr;
    size_t len1, len2, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &stylesheetStr, filename, &len1, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->compileFromStringAndSave(stylesheetStr, filename);
    }
}

PHP_METHOD(Xslt30Processor, compileFromFileAndSave)
{
    Xslt30Processor *xslt30Processor;
    char * stylesheetFilename = nullptr;
    char * filename = nullptr;
    size_t len1, len2, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &stylesheetFilename, filename, &len1, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->compileFromStringAndSave(stylesheetFilename, filename);
    }
}



PHP_METHOD(Xslt30Processor, compileFromValue)
{
    Xslt30Processor *xslt30Processor;
   zval* oth;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "O", &oth, xdmValue_ce) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
	zend_object* vvobj = Z_OBJ_P(oth);
	xdmValue_object* ooth = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr && value->size() == 1 && (value->getHead())->getType() == 3) {
        	xslt30Processor->compileFromXdmNode((XdmNode*)(value->getHead()));
	    }
	}
    }
}



PHP_METHOD(Xslt30Processor, setResultAsRawValue)
{
    Xslt30Processor *xslt30Processor;
    bool raw = false;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "b", &raw) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
	 xslt30Processor->setResultAsRawValue(raw);
    }

}



PHP_METHOD(Xslt30Processor, setJustInTimeCompilation)
{
    Xslt30Processor *xslt30Processor;
    bool jit = false;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "b", &jit) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
	 xslt30Processor->setJustInTimeCompilation(jit);
    }
}

PHP_METHOD(Xslt30Processor, setOutputFile)
{
    Xslt30Processor *xslt30Processor;
    char * outputFilename = nullptr;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &outputFilename, &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr && outputFilename != nullptr) {

	 xslt30Processor->setOutputFile(outputFilename);


    }
}




PHP_METHOD(Xslt30Processor, setProperty)
{
    Xslt30Processor *xslt30Processor;
    char * name = nullptr;
    char * value;
    size_t len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->setProperty(name, value);
    }
}

PHP_METHOD(Xslt30Processor, setParameter)
{

   Xslt30Processor *xslt30Processor;
   char * name;
   zval* oth;
   size_t len1, len2, myint;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
	if(Z_TYPE_P(oth) ==IS_NULL){
		php_error(E_WARNING, "Error setting source value - value is nullptr");
		return;
	}

      const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object* ooth = Z_OBJ_P(oth);
	xdmNode_object * nobj = (xdmNode_object *)((char *)ooth - XtOffsetOf(xdmNode_object, std));
        if(nobj != nullptr) {
            XdmNode * value = nobj->xdmNode;
            if(value != nullptr) {
	        xslt30Processor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	zend_object* ooth = Z_OBJ_P(oth);
	xdmValue_object * vobj = (xdmValue_object *)((char *)ooth - XtOffsetOf(xdmValue_object, std));
        if(vobj != nullptr) {
            XdmValue * value = vobj->xdmValue;
            if(value != nullptr) {

                xslt30Processor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	zend_object* ooth = Z_OBJ_P(oth);
	xdmAtomicValue_object * aobj = (xdmAtomicValue_object *)((char *)ooth - XtOffsetOf(xdmAtomicValue_object, std));
        if(aobj != nullptr) {
            XdmAtomicValue * value = aobj->xdmAtomicValue;
            if(value != nullptr) {
                xslt30Processor->setParameter(name, (XdmValue *)value);
            }
        }



      }

    }
}

PHP_METHOD(Xslt30Processor, clearParameters)
{
    Xslt30Processor *xslt30Processor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->clearParameters(true);
    }
}

PHP_METHOD(Xslt30Processor, clearProperties)
{
    Xslt30Processor *xslt30Processor;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->clearProperties();
    }
}


PHP_METHOD(Xslt30Processor, setupXslMessage)
{
    Xslt30Processor *xslt30Processor;
    bool show = false;
    char * filename = nullptr;
    size_t len1;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "bs", &show, &filename &len1) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;

    if (xslt30Processor != nullptr) {

	    xslt30Processor->setupXslMessage(show, filename);
	}

}



PHP_METHOD(Xslt30Processor, exceptionOccurred)
{
    Xslt30Processor *xslt30Processor;
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        bool result = xslt30Processor->exceptionOccurred();
        RETURN_BOOL(result);
    }
    RETURN_BOOL(false);
}

PHP_METHOD(Xslt30Processor, getExceptionCount)
{
    Xslt30Processor *xslt30Processor;
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        int count = xslt30Processor->exceptionCount();
        RETURN_LONG(count);
    }
    RETURN_LONG(0);
}

PHP_METHOD(Xslt30Processor, getErrorCode)
{
    Xslt30Processor *xslt30Processor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        const char * errCode = xslt30Processor->getErrorCode((int)index);
        if(errCode != nullptr) {
            //char *str = estrdup(errCode);
            _RETURN_STRING(errCode);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(Xslt30Processor, getErrorMessage)
{
    Xslt30Processor *xslt30Processor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        const char * errStr = xslt30Processor->getErrorMessage((int)index);
        if(errStr != nullptr) {
            //char *str = estrdup(errStr);
            _RETURN_STRING(errStr);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(Xslt30Processor, exceptionClear)
{
    Xslt30Processor *xslt30Processor;
    zend_object* pobj = Z_OBJ_P(getThis());
    xslt30Processor_object *obj = (xslt30Processor_object *)((char *)pobj - XtOffsetOf(xslt30Processor_object, std));
    xslt30Processor = obj->xslt30Processor;
    if (xslt30Processor != nullptr) {
        xslt30Processor->exceptionClear();
    }
}

/*     ============== XQuery10/30/31: PHP Interface of   XQueryProcessor =============== */

void xqueryProcessor_free_storage(zend_object *object)
{
    zend_object_std_dtor(object);
}

void xqueryProcessor_destroy_storage(zend_object *object)
{
    zend_objects_destroy_object(object);
}

zend_object *xqueryProcessor_create_handler(zend_class_entry *type)
{

    xqueryProcessor_object *obj = (xqueryProcessor_object *)ecalloc(1, sizeof(xqueryProcessor_object)+ zend_object_properties_size(type));
    zend_object_std_init(&obj->std, type); 
    object_properties_init(&obj->std, type);
    obj->std.handlers = &xqueryProcessor_object_handlers;

    return &obj->std;
}

PHP_METHOD(XQueryProcessor, __destruct)
{
    zend_object* pobj = Z_OBJ_P(getThis());
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    XQueryProcessor * xqueryProcessor= obj->xqueryProcessor;
    if(xqueryProcessor != nullptr){
    	delete xqueryProcessor;
    }
}


PHP_METHOD(XQueryProcessor, runQueryToValue)
{
    XQueryProcessor *xqueryProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != nullptr) {
        XdmValue * node = xqueryProcessor->runQueryToValue();
        if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                //struct xdmValue_object* vobj = (struct xdmValue_object *)Z_OBJ_P(return_value TSRMLS_CC);
		zend_object *vvobj =  Z_OBJ_P(return_value);
		xdmValue_object* vobj  = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmValue = node;
                return;
            }
        }
        xqueryProcessor->checkException();//TODO
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(XQueryProcessor, runQueryToString)
{
    XQueryProcessor *xqueryProcessor;
   

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    xqueryProcessor = obj->xqueryProcessor;

    if (xqueryProcessor != nullptr) {
        const char * result = xqueryProcessor->runQueryToString();
        if(result != nullptr) {
            //char *str = estrdup(result);
            _RETURN_STRING(result);
	    return;
        } else {
          xqueryProcessor->checkException(); //TODO
	}
    }
   RETURN_NULL();
}

PHP_METHOD(XQueryProcessor, runQueryToFile)
{

     char * ofilename;
    size_t len1 =0;
    if (ZEND_NUM_ARGS()!= 1) {
        WRONG_PARAM_COUNT;
    }
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &ofilename, &len1) == FAILURE) {
        RETURN_NULL();
    }
    XQueryProcessor *xqueryProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));



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
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &queryStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std)); 
    if(queryStr != nullptr) {
      obj->xqueryProcessor->setProperty("qs", queryStr);
   }
}

PHP_METHOD(XQueryProcessor, setQueryFile)
{
   char * fileName;
   size_t len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &fileName, &len1) == FAILURE) {
        RETURN_NULL();
    }
    if(fileName != nullptr) {
    	zend_object* pobj = Z_OBJ_P(getThis()); 
    	xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setQueryFile(fileName);
    }
    	
}

PHP_METHOD(XQueryProcessor, setQueryBaseURI)
{
   char * base;
   size_t len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &base, &len1) == FAILURE) {
        RETURN_NULL();
    }
    if(base != nullptr) {
    	zend_object* pobj = Z_OBJ_P(getThis()); 
    	xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setQueryBaseURI(base);
    }
    	
}

PHP_METHOD(XQueryProcessor, declareNamespace)
{
   char * prefix;
   char * ns;
   size_t len1, len2;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &prefix, &len1, &ns, &len2) == FAILURE) {
        RETURN_NULL();
    }
    if(prefix != nullptr && ns != nullptr) {
    	zend_object* pobj = Z_OBJ_P(getThis()); 
    	xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
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

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &oth) == FAILURE) {
        RETURN_NULL();
    }
    if(oth != nullptr) {
    	zend_object* pobj = Z_OBJ_P(getThis()); 
    	xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    	xqueryProcessor = obj->xqueryProcessor;
    const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name); 
	
      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object *vvobj =  Z_OBJ_P(oth);
	xdmNode_object* ooth  = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
	//xdmNode_object* ooth = (xdmNode_object*)Z_OBJ_P(oth TSRMLS_CC);
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xqueryProcessor->setContextItem((XdmItem *)value);
	       
	        return;
            }
        }
      } else if(strcmp(objName, "Saxon\\XdmItem")==0){
	zend_object *vvobj =  Z_OBJ_P(oth);
	xdmItem_object* ooth  = (xdmItem_object *)((char *)vvobj - XtOffsetOf(xdmItem_object, std));
        if(ooth != nullptr) {
            XdmItem * value = ooth->xdmItem;
	    if(value != nullptr) {
                xqueryProcessor->setContextItem(value);
               
		return;
	    }
         }
        



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	zend_object *vvobj =  Z_OBJ_P(oth);
	xdmAtomicValue_object* ooth  = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                xqueryProcessor->setContextItem((XdmItem *)value);
      
		return;
            }
        }



      } 


    }
	//throw exception
	
    	
}

PHP_METHOD(XQueryProcessor, setContextItemFromFile)
{
   char * cfilename;
   size_t len1;
    XQueryProcessor *xqueryProcessor;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &cfilename, &len1) == FAILURE) {
        RETURN_NULL();
    }
    if(cfilename != nullptr) {
     	zend_object* pobj = Z_OBJ_P(getThis()); 
    	xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    	xqueryProcessor = obj->xqueryProcessor;
    	xqueryProcessor->setContextItemFromFile(cfilename);
    }
    	
}


PHP_METHOD(XQueryProcessor, setProperty)
{
    XQueryProcessor *xqueryProcessor;
    char * name;
    char * value;
    size_t len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
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
   size_t len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
	if(Z_TYPE_P(oth) ==IS_NULL){
		php_error(E_WARNING, "Error setting source value - value is nullptr");
		return;
	}
             const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;
//Z_ADDREF_P(oth);
      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object *vvobj =  Z_OBJ_P(oth);
	xdmNode_object* ooth  = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xqueryProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	zend_object *vvobj =  Z_OBJ_P(oth);
	xdmValue_object* ooth  = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		
                xqueryProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	zend_object *vvobj =  Z_OBJ_P(oth);
	xdmAtomicValue_object* ooth  = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
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

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
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

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        xqueryProcessor->clearProperties();
    }
}

PHP_METHOD(XQueryProcessor, exceptionOccurred)
{
    XQueryProcessor *xqueryProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
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
     zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
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
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        const char * errCode = xqueryProcessor->getErrorCode((int)index);
        if(errCode != nullptr) {
            //char *str = estrdup(errCode);
            _RETURN_STRING(errCode);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XQueryProcessor, getErrorMessage)
{
    XQueryProcessor *xqueryProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        const char * errStr = xqueryProcessor->getErrorMessage((int)index);
        if(errStr != nullptr) {
            //char *str = estrdup(errStr);
            _RETURN_STRING(errStr);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(XQueryProcessor, exceptionClear)
{
    XQueryProcessor *xqueryProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xqueryProcessor_object *obj = (xqueryProcessor_object *)((char *)pobj - XtOffsetOf(xqueryProcessor_object, std));
    xqueryProcessor = obj->xqueryProcessor;
    if (xqueryProcessor != nullptr) {
        xqueryProcessor->exceptionClear();
    }
}

/*     ============== PHP Interface of XPath2.0/3.0  XPathProcessor =============== */

void xpathProcessor_free_storage(zend_object *object)
{
    zend_object_std_dtor(object);
}

void xpathProcessor_destroy_storage(zend_object *object)
{
    zend_objects_destroy_object(object);
}

PHP_METHOD(XPathProcessor, __destruct)
{

   zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));


   XPathProcessor * xpathProc= obj->xpathProcessor;
    if(xpathProc != nullptr){
    	delete xpathProc;
     }

   
    
}

zend_object *xpathProcessor_create_handler(zend_class_entry *type)
{
    xpathProcessor_object *obj = (xpathProcessor_object *)ecalloc(1, sizeof(xpathProcessor_object)+ zend_object_properties_size(type));
    
    zend_object_std_init(&obj->std, type); 
    
    object_properties_init(&obj->std, type);
    
    obj->std.handlers = &xpathProcessor_object_handlers;

    return &obj->std;
}



PHP_METHOD(XPathProcessor, setProperty)
{
    XPathProcessor *xpathProcessor;
    char * name;
    char * value;
    size_t len1, len2;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
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
   size_t len1, len2;	
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
            const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	
        zend_object* vvobj = Z_OBJ_P(oth);
        xdmNode_object * ooth = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xpathProcessor->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	
        zend_object* vvobj = Z_OBJ_P(oth);
        xdmValue_object * ooth = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		
                xpathProcessor->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	
        zend_object* vvobj = Z_OBJ_P(oth);
        xdmAtomicValue_object * ooth = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
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

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &prefix, &len1, &ns, &len2) == FAILURE) {
        RETURN_NULL();
    }
    if(prefix != nullptr && ns != nullptr) {
    	zend_object* pobj = Z_OBJ_P(getThis()); 
        xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    	xpathProcessor = obj->xpathProcessor;
    	xpathProcessor->declareNamespace(prefix, ns);
    }
    	
}



PHP_METHOD(XPathProcessor, setBackwardsCompatible){

    XPathProcessor *xpathProcessor;
    bool allow = false;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "b", &allow) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
	    xpathProcessor->setBackwardsCompatible(allow);
	 }
}

PHP_METHOD(XPathProcessor, setCaching){
    XPathProcessor *xpathProcessor;
    bool isCaching = false;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "b", &isCaching) == FAILURE) {
        RETURN_NULL();
    }

    zend_object* pobj = Z_OBJ_P(getThis());
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
	    xpathProcessor->setCaching(isCaching);
	 }
}

PHP_METHOD(XPathProcessor, importSchemaNamespace){
      char * name;
      int len1;
      XPathProcessor *xpathProcessor;

       if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &name, &len1) == FAILURE) {
           RETURN_NULL();
       }
       if(name != nullptr) {
       	zend_object* pobj = Z_OBJ_P(getThis());
           xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
       	xpathProcessor = obj->xpathProcessor;
       	xpathProcessor->importSchemaNamespace(name);
       }
}

PHP_METHOD(XPathProcessor, effectiveBooleanValue)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   size_t len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &xpathStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
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
   size_t len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &xpathStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr && xpathStr != nullptr) {
        
        XdmValue * node = xpathProcessor->evaluate(xpathStr);
	if(node != nullptr) {
            if (object_init_ex(return_value, xdmValue_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		zend_object* vvobj = Z_OBJ_P(return_value);
        		
		if(node->getType() == XDM_NODE) {
			xdmNode_object * vobj = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
             	   	assert (vobj != nullptr);
               	   	vobj->xdmNode = (XdmNode *)node;
                	return;

		} else if (node->getType() == XDM_ATOMIC_VALUE) {
        	xdmAtomicValue_object * vobj = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmAtomicValue = (XdmAtomicValue *)node;
                return;

		} else {
        	  xdmValue_object * vobj = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
                  assert (vobj != nullptr);
                  vobj->xdmValue = node;
                  return;
		}
            }
        }
        xpathProcessor->checkException();//TODO
    } 
    RETURN_NULL();
    
}

PHP_METHOD(XPathProcessor, evaluateSingle)
{

   XPathProcessor *xpathProcessor;
   char * xpathStr;
   zval* oth;
   size_t len1, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &xpathStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;

    if(xpathStr == nullptr) {
	RETURN_NULL();
	return;
	}


    if (xpathProcessor != nullptr) {
        
        XdmItem * node = xpathProcessor->evaluateSingle(xpathStr);
	if(node != nullptr) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                //struct xdmItem_object* vobj = (struct xdmItem_object *)Z_OBJ_P(return_value TSRMLS_CC);
		zend_object* vvobj = Z_OBJ_P(return_value);
		if(node->getType() == XDM_NODE) {
	        	xdmNode_object * vobj = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
	                assert (vobj != nullptr);
	                vobj->xdmNode = (XdmNode *)node;
	                return;

		} else if (node->getType() == XDM_ATOMIC_VALUE) {

        	xdmAtomicValue_object * vobj = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
                assert (vobj != nullptr);
                vobj->xdmAtomicValue = (XdmAtomicValue *)node;
                return;

		} else {
        		xdmItem_object * vobj = (xdmItem_object *)((char *)vvobj - XtOffsetOf(xdmItem_object, std));
                	assert (vobj != nullptr);
                	vobj->xdmItem = node;
                	return;
                }
            }
        } 
        xpathProcessor->checkException();//TODO
    } 
    RETURN_NULL();
}

PHP_METHOD(XPathProcessor, setContextItem)
{

   XPathProcessor *xpathProcessor;

   zval* oth;
	//TODO this should be relaxed to accept item/atomic/node as well as Value

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
	if(!oth) {
		php_error(E_WARNING, "Error setting source value");
		return;
         } 
        const char * objName = ZSTR_VAL(Z_OBJCE_P(oth)->name);
        
       if(strcmp(objName, "Saxon\\XdmNode")==0) {
        zend_object* vvobj = Z_OBJ_P(oth);
        xdmNode_object * ooth = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xpathProcessor->setContextItem((XdmItem *)value);
		value->incrementRefCount();

            }
        }
      }  else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	zend_object* vvobj = Z_OBJ_P(oth);
        xdmAtomicValue_object * ooth = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
        if(ooth != nullptr) {
            XdmAtomicValue * value = ooth->xdmAtomicValue;
            if(value != nullptr) {
		
                xpathProcessor->setContextItem((XdmItem *)value);
		value->incrementRefCount();
            }
        }
     }   else if(strcmp(objName, "Saxon\\XdmItem")==0){
	zend_object* vvobj = Z_OBJ_P(oth);
        xdmItem_object * ooth = (xdmItem_object *)((char *)vvobj - XtOffsetOf(xdmItem_object, std));
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
   size_t len1;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &uriStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
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
   size_t len1;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
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

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
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

    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        xpathProcessor->clearProperties();
    }
}


PHP_METHOD(XPathProcessor, exceptionOccurred)
{
   XPathProcessor *xpathProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
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
   zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
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
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        const char * errCode = xpathProcessor->getErrorCode((int)index);
        if(errCode != nullptr) {
           // char *str = estrdup(errCode);
            _RETURN_STRING(errCode);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XPathProcessor, getErrorMessage)
{
    XPathProcessor *xpathProcessor;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        const char * errStr = xpathProcessor->getErrorMessage((int)index);
        if(errStr != nullptr) {
            //char *str = estrdup(errStr);
            _RETURN_STRING(errStr);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(XPathProcessor, exceptionClear)
{
    XPathProcessor *xpathProcessor;
    zend_object* pobj = Z_OBJ_P(getThis()); 
    xpathProcessor_object *obj = (xpathProcessor_object *)((char *)pobj - XtOffsetOf(xpathProcessor_object, std));
    xpathProcessor = obj->xpathProcessor;
    if (xpathProcessor != nullptr) {
        xpathProcessor->exceptionClear();
    }
}

/*     ============== PHP Interface of   SchemaValidator =============== */

void schemaValidator_free_storage(zend_object *object)
{
    zend_object_std_dtor(object);
}

void schemaValidator_destroy_storage(zend_object *object)
{
    zend_objects_destroy_object(object);
}

zend_object *schemaValidator_create_handler(zend_class_entry *type)
{
    zval *tmp;
    zend_object retval;
    schemaValidator_object *obj = (schemaValidator_object *)ecalloc(1, sizeof(schemaValidator_object)+ zend_object_properties_size(type));

    zend_object_std_init(&obj->std, type);    
    object_properties_init(&obj->std, type);

    obj->std.handlers = &schemaValidator_object_handlers;

    return &obj->std;
}



PHP_METHOD(SchemaValidator, __destruct)
{
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));


    SchemaValidator * schemaValidator= obj->schemaValidator;
    if(schemaValidator != nullptr) {
    	delete schemaValidator;
    }
    
}



PHP_METHOD(SchemaValidator, registerSchemaFromFile)
{
    SchemaValidator *schemaValidator;
    char * name;
    size_t len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (name != nullptr && schemaValidator != nullptr) {
        schemaValidator->registerSchemaFromFile(name);
    }
}

PHP_METHOD(SchemaValidator, registerSchemaFromString)
{
    char * schemaStr;
    size_t len1;
    SchemaValidator *schemaValidator;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &schemaStr, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaStr!= nullptr && schemaValidator != nullptr) {
        schemaValidator->registerSchemaFromString(schemaStr);
    }
}

PHP_METHOD(SchemaValidator, validate)
{
    char * name = nullptr;
    size_t len1;
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }
    if (ZEND_NUM_ARGS()>0 && zend_parse_parameters(ZEND_NUM_ARGS() , "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->validate(name);
    }
}

PHP_METHOD(SchemaValidator, validateToNode)
{
    char * name = nullptr;
   size_t len1;
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>1) {
        WRONG_PARAM_COUNT;
    }
    if (ZEND_NUM_ARGS()>0 && zend_parse_parameters(ZEND_NUM_ARGS() , "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        XdmNode * node = schemaValidator->validateToNode(name);
	if(node != nullptr) {
	    if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                //struct xdmNode_object* vobj = (struct xdmNode_object *)Z_OBJ_P(return_value TSRMLS_CC);
		zend_object* vvobj = Z_OBJ_P(return_value);
 		xdmNode_object * vobj = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
                assert (vobj != nullptr);
                vobj->xdmNode = node;
                return;
            }
	} 
    	schemaValidator->checkException();//TODO
    } 
    RETURN_NULL();
}


PHP_METHOD(SchemaValidator, getValidationReport)
{

    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }

    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        XdmNode * node = schemaValidator->getValidationReport();
	if(node != nullptr) {
	    if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                zend_object* vvobj = Z_OBJ_P(return_value);
 		xdmNode_object * vobj = (xdmNode_object *)((char *)vvobj - XtOffsetOf(xdmNode_object, std));
                assert (vobj != nullptr);
                vobj->xdmNode = node;
                return;
            }
	} 
    	schemaValidator->checkException();//TODO
    } 
    RETURN_NULL();
}


PHP_METHOD(SchemaValidator, setSourceNode)
{
    SchemaValidator *schemaValidator;

    zval* oth;
   

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
	const char * objName = ZSTR_VAL(Z_OBJCE_P(oth)->name);
	if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object * nobj = Z_OBJ_P(oth);
	xdmNode_object* ooth = (xdmNode_object *)((char *)nobj - XtOffsetOf(xdmNode_object, std));
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

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->setOutputFile(name);
    }
}


PHP_METHOD(SchemaValidator, exportSchema)
{
    SchemaValidator *schemaValidator;
    char * name;
    int len1;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis());
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->exportSchema(name);
    }
}


PHP_METHOD(SchemaValidator, setProperty)
{
    SchemaValidator *schemaValidator;
    char * name;
    char * value;
    size_t len1, len2, myint;

    if (zend_parse_parameters(ZEND_NUM_ARGS() , "ss", &name, &len1, &value, &len2) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
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
   size_t len1, len2, myint;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "sz", &name, &len2, &oth) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
      const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object * nobj = Z_OBJ_P(oth);

	xdmNode_object* ooth = (xdmNode_object *)((char *)nobj - XtOffsetOf(xdmNode_object, std));
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        schemaValidator->setParameter(name, (XdmValue *)value);

            }
        }
      } else if(strcmp(objName, "Saxon\\XdmValue")==0){
	zend_object* vvobj = Z_OBJ_P(oth);
	xdmValue_object* ooth = (xdmValue_object *)((char *)vvobj - XtOffsetOf(xdmValue_object, std));
        if(ooth != nullptr) {
            XdmValue * value = ooth->xdmValue;
            if(value != nullptr) {
		
                schemaValidator->setParameter(name, value);
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	zend_object* vvobj = Z_OBJ_P(oth);
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object *)((char *)vvobj - XtOffsetOf(xdmAtomicValue_object, std));
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

    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
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
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
       
                schemaValidator->clearParameters(true);
		schemaValidator->exceptionClear();
        }
    }

PHP_METHOD(SchemaValidator, exceptionOccurred)
{
    SchemaValidator *schemaValidator;
    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
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

    if (ZEND_NUM_ARGS()>0) {
        WRONG_PARAM_COUNT;
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
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
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        const char * errCode = schemaValidator->getErrorCode((int)index);
        if(errCode != nullptr) {
            //char *str = estrdup(errCode);
            _RETURN_STRING(errCode);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(SchemaValidator, getErrorMessage)
{
    SchemaValidator *schemaValidator;
    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "l", &index) == FAILURE) {
        RETURN_NULL();
    }
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        const char * errStr = schemaValidator->getErrorMessage((int)index);
        if(errStr != nullptr) {
            //char *str = estrdup(errStr);
            _RETURN_STRING(errStr);
        }
    }
    RETURN_NULL();
}
PHP_METHOD(SchemaValidator, exceptionClear)
{
    SchemaValidator * schemaValidator;
    zend_object* pobj = Z_OBJ_P(getThis()); 
   schemaValidator_object * obj = (schemaValidator_object *)((char *)pobj - XtOffsetOf(schemaValidator_object, std));
    schemaValidator = obj->schemaValidator;
    if (schemaValidator != nullptr) {
        schemaValidator->exceptionClear();
    }
}

/*     ============== PHP Interface of   XdmValue =============== */
void xdmValue_free_storage(zend_object *object)
{
    zend_object_std_dtor(object);
}

zend_object *xdmValue_create_handler(zend_class_entry *type)
{
    zval *tmp;
    zend_object retval;
    xdmValue_object *obj = (xdmValue_object *)ecalloc(1, sizeof(xdmValue_object)+ zend_object_properties_size(type));
    zend_object_std_init(&obj->std, type); /* take care of the zend_object also ! */
    object_properties_init(&obj->std, type);
    
    obj->std.handlers = &xdmValue_object_handlers;

    return &obj->std;
}

void XdmValue_destroy_storage(zend_object *object)
{
    zend_objects_destroy_object(object);
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
    //xdmValue_object *obj = (xdmValue_object *) Z_OBJ_P(getThis() TSRMLS_CC);
    /*if (zend_parse_parameters(ZEND_NUM_ARGS() , "z",&zvalue) == SUCCESS) {
        switch (Z_TYPE_P(zvalue)) {
            case IS_FALSE:
	    case IS_TRUE:
                bVal = Z_BVAL_P(zvalue);
                xdmValue = new XdmValue(bVal);
                obj = (xdmValue_object *)Z_OBJ_P(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_LONG:
                iVal = Z_LVAL_P(zvalue);
                xdmValue = new XdmValue((int)iVal);
                obj = (xdmValue_object *)Z_OBJ_P(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_STRING:
                sVal = Z_STRVAL_P(zvalue);
                len = Z_STRLEN_P(zvalue);
                xdmValue = new XdmValue("string", sVal);
                obj = (xdmValue_object *)Z_OBJ_P(getThis() TSRMLS_CC);
                obj->xdmValue = xdmValue;
            break;
            case IS_NULL:
                xdmValue = new XdmValue();
                obj = (xdmValue_object *)Z_OBJ_P(getThis() TSRMLS_CC);
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

     zend_object *oobj = Z_OBJ_P(getThis());
    xdmValue_object* obj = (xdmValue_object *)((char *)oobj - XtOffsetOf(xdmValue_object, std));
    XdmValue * xdmValue= obj->xdmValue;
   if(xdmValue != nullptr) {
    	xdmValue->decrementRefCount();
    	if(xdmValue!= nullptr && xdmValue->getRefCount()< 1){
    		delete xdmValue;
   	 } 
    }
}

PHP_METHOD(XdmValue, __toString)
    {
    	 XdmValue *xdmValue;
         zend_object *oobj = Z_OBJ_P(getThis());
         xdmValue_object* obj = (xdmValue_object *)((char *)oobj - XtOffsetOf(xdmValue_object, std));
         xdmValue = obj->xdmValue;
         if (xdmValue != nullptr) {
        	const char * value = xdmValue->toString();
          	if(value != nullptr) {
          	    _RETURN_STRING(value);
          	 } else {
          	 _RETURN_STRING("");

          	 }
         	}else {
      		_RETURN_STRING("");
      	}
    }

PHP_METHOD(XdmValue,  getHead){
    XdmValue *xdmValue;
     zend_object *oobj = Z_OBJ_P(getThis());
    xdmValue_object* obj = (xdmValue_object *)((char *)oobj - XtOffsetOf(xdmValue_object, std));
    xdmValue = obj->xdmValue;
    if (xdmValue != nullptr) {
	XdmItem * item = xdmValue->getHead();
	if(item != nullptr) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
		RETURN_NULL();
              } else {
		item->incrementRefCount();
		zend_object * oobj = Z_OBJ_P(return_value);
		xdmItem_object* vobj = (xdmItem_object *)((char *)oobj - XtOffsetOf(xdmItem_object, std));
                assert (vobj != nullptr);
                vobj->xdmItem = item;
                
            }
        } else {
		RETURN_NULL();
	}
        
    } else {
	RETURN_NULL();
    }
}


PHP_METHOD(XdmValue,  itemAt){
    XdmValue *xdmValue;

    long index;
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "l", &index) == FAILURE) {
        RETURN_NULL();
    }

     zend_object *oobj = Z_OBJ_P(getThis());
    xdmValue_object* obj = (xdmValue_object *)((char *)oobj - XtOffsetOf(xdmValue_object, std));
    xdmValue = obj->xdmValue;
    if (xdmValue != nullptr) {
	XdmItem * item = xdmValue->itemAt((unsigned int)index);
	if(item != nullptr) {
            if (object_init_ex(return_value, xdmItem_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		item->incrementRefCount();
		zend_object * oobj = Z_OBJ_P(return_value);
		xdmItem_object* vobj = (xdmItem_object *)((char *)oobj - XtOffsetOf(xdmItem_object, std));
               
                assert (vobj != nullptr);
                vobj->xdmItem = item;
                return;
            }
        }
        
    } else {
	RETURN_NULL();
    }
}


PHP_METHOD(XdmValue,  size){
    XdmValue *xdmValue;
    zend_object *oobj = Z_OBJ_P(getThis());
    xdmValue_object* obj = (xdmValue_object *)((char *)oobj - XtOffsetOf(xdmValue_object, std));
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
   	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "z", &oth) == FAILURE) {
        RETURN_NULL();
    }

    xdmValue_object *obj = (xdmValue_object *)Z_OBJ_P(getThis() TSRMLS_CC);
    xdmValue = obj->xdmValue;
    if (xdmValue != nullptr) {
     const char * objName =ZSTR_VAL(Z_OBJCE_P(oth)->name);
      //std::cout<<"test type:"<<(Z_OBJCE_P(oth)->name)<<std::endl;

      if(strcmp(objName, "Saxon\\XdmNode")==0) {
	zend_object * nobj = Z_OBJ_P(oth);
	xdmNode_object* ooth = (xdmNode_object *)((char *)nobj - XtOffsetOf(xdmNode_object, std));
        if(ooth != nullptr) {
            XdmNode * value = ooth->xdmNode;
            if(value != nullptr) {
	        xdmValue->addXdmItem((XdmItem *)value);
		return;
            }
        }
      } else if(strcmp(objName, "Saxon\\XdmItem")==0){
	zend_object * iobj = Z_OBJ_P(oth);
	xdmItem_object* ooth = (xdmItem_object *)((char *)iobj - XtOffsetOf(xdmItem_object, std));
        if(ooth != nullptr) {
            XdmItem * value = ooth->xdmItem;
            if(value != nullptr) {
		xdmValue->addXdmItem(value);
		return;
            }
        }



      } else if(strcmp(objName, "Saxon\\XdmAtomicValue")==0){
	zend_object * aobj = Z_OBJ_P(oth);
	xdmAtomicValue_object* ooth = (xdmAtomicValue_object *)((char *)aobj - XtOffsetOf(xdmAtomicValue_object, std));
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

void xdmItem_free_storage(zend_object *object)
{
    zend_object_std_dtor(object);
}

zend_object *xdmItem_create_handler(zend_class_entry *type)
{
    zval *tmp;
    zend_object retval;
    xdmItem_object *obj = (xdmItem_object *)ecalloc(1, sizeof(xdmItem_object)+ zend_object_properties_size(type));
    
    object_properties_init(&obj->std, type);
    zend_object_std_init(&obj->std, type); /* take care of the zend_object also ! */
    
    obj->std.handlers = &xdmItem_object_handlers;

    return &obj->std;
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
    
   zval *object = getThis();
   
    zend_object * zobj = Z_OBJ_P(object);

    xdmItem_object * obj = (xdmItem_object *)((char *)zobj - XtOffsetOf(xdmItem_object, std));
    //saxonProc =  obj->saxonProcessor;
 
}

void XdmItem_destroy_storage(zend_object *object)
{
    zend_objects_destroy_object(object);
    
}

PHP_METHOD(XdmItem, __destruct)
{
     zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmItem_object * obj = (xdmItem_object *)((char *)zobj - XtOffsetOf(xdmItem_object, std));

    XdmItem * xdmItem= obj->xdmItem;
    xdmItem->decrementRefCount();
    if(xdmItem != nullptr && xdmItem->getRefCount()< 1){
    	delete xdmItem;
    }
    
}

PHP_METHOD(XdmItem, getStringValue)
{
    XdmItem *xdmItem;
     zval *object = getThis();
    zend_object * zobj = Z_OBJ_P(object);
    xdmItem_object * obj = (xdmItem_object *)((char *)zobj - XtOffsetOf(xdmItem_object, std));
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr) {
        const char * valueStr = xdmItem->getStringValue();
        if(valueStr != nullptr) {
            _RETURN_STRING(valueStr);
        }
    }
    RETURN_NULL();
}

    /* {{{ __toString()
       Returns the string content */
PHP_METHOD(XdmItem, __toString)
    {
    	 XdmItem *xdmItem;
         zend_object *oobj = Z_OBJ_P(getThis());
         xdmItem_object* obj = (xdmItem_object *)((char *)oobj - XtOffsetOf(xdmItem_object, std));
         xdmItem = obj->xdmItem;
         if (xdmItem != nullptr) {
        	const char * value = xdmItem->toString();
          	if(value != nullptr) {
          	    _RETURN_STRING(value);
          	 } else {
          	 _RETURN_STRING("");

          	 }
         	}else {
      		_RETURN_STRING("");
      	}
    }

PHP_METHOD(XdmItem, isAtomic)
{
    XdmItem *xdmItem;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmItem_object * obj = (xdmItem_object *)((char *)zobj - XtOffsetOf(xdmItem_object, std));
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
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmItem_object * obj = (xdmItem_object *)((char *)zobj - XtOffsetOf(xdmItem_object, std));
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr && xdmItem->getType() == XDM_NODE) {
        RETURN_TRUE;
    }
    RETURN_FALSE;
}

PHP_METHOD(XdmItem, getAtomicValue)
{
    XdmItem *xdmItem;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmItem_object * obj = (xdmItem_object *)((char *)zobj - XtOffsetOf(xdmItem_object, std));
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr) {
	  if(!xdmItem->isAtomic()) {
		RETURN_NULL();
		return;
	  }
          if (object_init_ex(return_value, xdmAtomicValue_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		xdmItem->incrementRefCount();
//Z_ADDREF_P(return_value);
   		zend_object * zobj = Z_OBJ_P(return_value);
    		xdmAtomicValue_object * vobj = (xdmAtomicValue_object *)((char *)zobj - XtOffsetOf(xdmAtomicValue_object, std));
                //struct xdmAtomicValue_object* vobj = (struct xdmAtomicValue_object *)Z_OBJ_P(return_value TSRMLS_CC);
                assert (vobj != nullptr);
                vobj->xdmAtomicValue = (XdmAtomicValue *)xdmItem;
                return;
            }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmItem, getNodeValue)
{
    XdmItem *xdmItem;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmItem_object * obj = (xdmItem_object *)((char *)zobj - XtOffsetOf(xdmItem_object, std));
    xdmItem = obj->xdmItem;

    if (xdmItem != nullptr) {
	  if(xdmItem->isAtomic()) {
		RETURN_NULL();
		return;
	  }
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
                //struct xdmNode_object* vobj = (struct xdmNode_object *)Z_OBJ_P(return_value TSRMLS_CC);
   		zend_object * zobj = Z_OBJ_P(return_value);
    		xdmNode_object * vobj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
                assert (vobj != nullptr);
                vobj->xdmNode = (XdmNode *)xdmItem;
		vobj->xdmNode->incrementRefCount();

                return;
            }
    }
    RETURN_NULL();
}



/*     ============== PHP Interface of   XdmNode =============== */

void xdmNode_free_storage(zend_object *object)
{
   zend_object_std_dtor(object);

    
}

zend_object *xdmNode_create_handler(zend_class_entry *type)
{
    zval *tmp;
    zend_object retval;
    xdmNode_object *obj = (xdmNode_object *)ecalloc(1, sizeof(xdmNode_object)+ zend_object_properties_size(type));
    zend_object_std_init(&obj->std, type); /* take care of the zend_object also ! */
    object_properties_init(&obj->std, type);
    obj->std.handlers = &xdmNode_object_handlers;
	
    return &obj->std;
}

PHP_METHOD(XdmNode, __construct)
{
    //xdmNode_object *obj = (xdmNode_object *) Z_OBJ_P(getThis() TSRMLS_CC);
}

void XdmNode_destroy_storage(zend_object *object)
{
 
 zend_objects_destroy_object(object);
   
}

PHP_METHOD(XdmNode, __destruct)
{
    zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));

    if(obj != nullptr) {
    XdmNode * xdmNode= obj->xdmNode;
    if(xdmNode != nullptr) {
    	xdmNode->decrementRefCount();
    	if(xdmNode->getRefCount()< 1){
    		delete xdmNode;
    	} 
    }
    }
}

PHP_METHOD(XdmNode, getStringValue)
{
    XdmNode *xdmNode;
    zval *object = getThis();
    zend_object * zobj = Z_OBJ_P(object);

    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;

   
    if (xdmNode != nullptr) {
        const char * valueStr = xdmNode->getStringValue();
        if(valueStr != nullptr) {
            _RETURN_STRING(valueStr);
	    return;
        }
    } 
    RETURN_NULL();
    
}


    /* {{{ __toString()
       Returns the string content */
PHP_METHOD(XdmNode, __toString)
    {
    	 XdmNode *xdmNode;
         zend_object *oobj = Z_OBJ_P(getThis());
         xdmNode_object* obj = (xdmNode_object *)((char *)oobj - XtOffsetOf(xdmNode_object, std));
         xdmNode = obj->xdmNode;
         if (xdmNode != nullptr) {
        	const char * value = xdmNode->toString();
          	if(value != nullptr) {
          	    _RETURN_STRING(value);
          	 } else {
          	 _RETURN_STRING("");

          	 }
         	}else {
      		_RETURN_STRING("");
      	}
    }

PHP_METHOD(XdmNode, getTypedValue)
{
    XdmNode *xdmNode;
    zval *object = getThis();
    zend_object * zobj = Z_OBJ_P(object);

    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;


    if (xdmNode != nullptr) {
        XdmValue * typedValue = xdmNode->getTypedValue();
        if(typedValue == nullptr) {
        			RETURN_NULL();
        			return;
        	}
                  if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                        RETURN_NULL();
                        return;
                    } else {
        		typedValue->incrementRefCount();

        		zend_object * zobj = Z_OBJ_P(return_value);
            		xdmValue_object * vobj = (xdmValue_object *)((char *)zobj - XtOffsetOf(xdmValue_object, std));
                       	assert (vobj != nullptr);
                       	vobj->xdmValue = typedValue;
                       	return;
                    }
            }
            RETURN_NULL();
}

PHP_METHOD(XdmNode, getNodeName)
{
    XdmNode *xdmNode;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
        const char * valueStr = xdmNode->getNodeName();
        if(valueStr != nullptr) {
            //char *str = estrdup(valueStr);
            _RETURN_STRING(valueStr);
        }
    } 
    RETURN_NULL();
}

PHP_METHOD(XdmNode, getNodeKind)
{
    XdmNode *xdmNode;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;

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
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;

    int nodeChildCount = 0;
    if (xdmNode != nullptr) {
        nodeChildCount = xdmNode->getChildCount();
        
    }
     RETURN_LONG(nodeChildCount);
}   

PHP_METHOD(XdmNode,  getAttributeCount){
    XdmNode *xdmNode;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;

    int nodeAttrCount = 0;
    if (xdmNode != nullptr) {
        nodeAttrCount = xdmNode->getAttributeCount();
        
    }
     RETURN_LONG(nodeAttrCount);

} 

PHP_METHOD(XdmNode,  getChildNode){
    long indexi;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "l",&indexi) == FAILURE) {
       RETURN_NULL();
    }

    XdmNode *xdmNode;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
	 int count = xdmNode->getChildCount();
 	 if(count==0) {
		RETURN_NULL();
		return;
	  }	
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
               RETURN_NULL();
                return;
            } else {
		
		if(indexi>=0 && indexi < count) {
			XdmNode ** childNodes = xdmNode->getChildren();
			if(childNodes == nullptr) {
				RETURN_NULL();
				return;
			}
			XdmNode * childNode = childNodes[indexi];
			if(childNode != nullptr) {
				childNode->incrementRefCount();
				zend_object * zobj = Z_OBJ_P(return_value);
    				xdmNode_object * vobj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
                		assert (vobj != nullptr);
                		vobj->xdmNode = childNode;
                		return;
			}
		}
            }
    }			
    RETURN_NULL();
}

PHP_METHOD(XdmNode,  getParent){
    XdmNode *xdmNode;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
	XdmNode * parent = xdmNode->getParent();
	if(parent == nullptr) {
			RETURN_NULL();
			return;
	}
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		parent->incrementRefCount();
               	//struct xdmNode_object* vobj = (struct xdmNode_object *)Z_OBJ_P(return_value TSRMLS_CC);
		zend_object * zobj = Z_OBJ_P(return_value);
    		xdmNode_object * vobj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
               	assert (vobj != nullptr);
               	vobj->xdmNode = parent;
               	return;
            }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmNode,  getAttributeNode){
    long indexi;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "l",&indexi) == FAILURE) {
        RETURN_NULL();
    }

    XdmNode *xdmNode;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;

    if (xdmNode != nullptr) {
	  int count = xdmNode->getAttributeCount();
	  if(count > 0) {
          if (object_init_ex(return_value, xdmNode_ce) != SUCCESS) {
                RETURN_NULL();
                return;
            } else {
		
		if(indexi < count) {
			XdmNode * attNode = xdmNode->getAttributeNodes()[indexi];
			if(attNode != nullptr) {
				attNode->incrementRefCount();
                		//struct xdmNode_object* vobj = (struct xdmNode_object *)Z_OBJ_P(return_value TSRMLS_CC);
				zend_object * zobj = Z_OBJ_P(return_value);
    				xdmNode_object * vobj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
                		assert (vobj != nullptr);
                		vobj->xdmNode = attNode;

                		return;
			}
		}
            }
	}
    }
    RETURN_NULL();

}

PHP_METHOD(XdmNode,  getAttributeValue){
   char * name;
   long len1;	
    if (zend_parse_parameters(ZEND_NUM_ARGS() , "s", &name, &len1) == FAILURE) {
        RETURN_NULL();
    }
    XdmNode *xdmNode;
    zend_object * zobj = Z_OBJ_P(getThis());
    xdmNode_object * obj = (xdmNode_object *)((char *)zobj - XtOffsetOf(xdmNode_object, std));
    xdmNode = obj->xdmNode;
    if (xdmNode != nullptr && name != nullptr) {
	
        const char * valueStr = xdmNode->getAttributeValue(name);
        if(valueStr != nullptr) {
            //char *str = estrdup(valueStr);
            _RETURN_STRING(valueStr);
	    return;
        }
    }
    RETURN_NULL();


}

/*     ============== PHP Interface of   XdmAtomicValue =============== */

void xdmAtomicValue_free_storage(zend_object *object)
{
    zend_object_std_dtor(object);
}

void XdmAtomicValue_destroy_storage(zend_object *object)
{
    zend_objects_destroy_object(object);
    
}

zend_object *xdmAtomicValue_create_handler(zend_class_entry *type)
{

    xdmAtomicValue_object *obj = (xdmAtomicValue_object *)ecalloc(1, sizeof(xdmAtomicValue_object)+ zend_object_properties_size(type));

    zend_object_std_init(&obj->std, type); /* take care of the zend_object also ! */
    object_properties_init(&obj->std, type);

    obj->std.handlers = &xdmAtomicValue_object_handlers;

    return &obj->std;
}

PHP_METHOD(XdmAtomicValue, __construct)
{
   /* XdmAtomicValue *xdmValue = nullptr;
    bool bVal;
    char * sVal;
    int len;
    long iVal;
    double dVal;
    zval *zvalue;*/

   // xdmAtomicValue_object *obj = (xdmAtomicValue_object *) Z_OBJ_P(getThis() TSRMLS_CC);

}

PHP_METHOD(XdmAtomicValue, __destruct)
{
     zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmAtomicValue_object * obj = (xdmAtomicValue_object *)((char *)zobj - XtOffsetOf(xdmAtomicValue_object, std));

    XdmAtomicValue * xdmValue= obj->xdmAtomicValue;
    if(xdmValue!= nullptr && xdmValue->getRefCount()< 1){
	xdmValue->decrementRefCount();
    	delete xdmValue;
    }
    
}

PHP_METHOD(XdmAtomicValue, getBooleanValue)
{
    XdmAtomicValue *xdmAtomicValue;
    zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmAtomicValue_object * obj = (xdmAtomicValue_object *)((char *)zobj - XtOffsetOf(xdmAtomicValue_object, std));
    xdmAtomicValue = obj->xdmAtomicValue;


    bool resultb = false;
    if (xdmAtomicValue != nullptr) {
         resultb = xdmAtomicValue->getBooleanValue();
        
    }
    RETURN_BOOL(resultb);
}


PHP_METHOD(XdmAtomicValue, getDoubleValue)
{
    XdmAtomicValue *xdmAtomicValue;
   zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmAtomicValue_object * obj = (xdmAtomicValue_object *)((char *)zobj - XtOffsetOf(xdmAtomicValue_object, std));
    xdmAtomicValue = obj->xdmAtomicValue;


    double resultb = 0;
    if (xdmAtomicValue != nullptr) {
         resultb = xdmAtomicValue->getDoubleValue();
        
    }
    RETURN_DOUBLE(resultb);
}

PHP_METHOD(XdmAtomicValue, getLongValue)
{
    XdmAtomicValue *xdmAtomicValue;
   zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmAtomicValue_object * obj = (xdmAtomicValue_object *)((char *)zobj - XtOffsetOf(xdmAtomicValue_object, std));
    xdmAtomicValue = obj->xdmAtomicValue;

    long result = 0;
    if (xdmAtomicValue != nullptr) {
         result = xdmAtomicValue->getLongValue();
        
    }
    RETURN_LONG(result);
}

PHP_METHOD(XdmAtomicValue, getStringValue)
{
    XdmAtomicValue *xdmAtomicValue;
    zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmAtomicValue_object * obj = (xdmAtomicValue_object *)((char *)zobj - XtOffsetOf(xdmAtomicValue_object, std));
    xdmAtomicValue = obj->xdmAtomicValue;

    SaxonProcessor * saxonProc;
     zend_object * zzobj = Z_OBJ_P(getThis());

    saxonProcessor_object * obj2 = (saxonProcessor_object *)((char *)zzobj - XtOffsetOf(xdmAtomicValue_object, std));
    
    saxonProc =  obj2->saxonProcessor;

    if (xdmAtomicValue != nullptr) {
        const char * valueStr = xdmAtomicValue->getStringValue();
        if(valueStr != nullptr) {
            //char *str = estrdup(valueStr);
            _RETURN_STRING(valueStr);
        }
    }
    RETURN_NULL();
}

PHP_METHOD(XdmAtomicValue, getPrimitiveTypeName)
{
    XdmAtomicValue *xdmAtomicValue;
    zval *object = getThis();
     zend_object * zobj = Z_OBJ_P(object);

    xdmAtomicValue_object * obj = (xdmAtomicValue_object *)((char *)zobj - XtOffsetOf(xdmAtomicValue_object, std));
    xdmAtomicValue = obj->xdmAtomicValue;

    if (xdmAtomicValue != nullptr) {
        const char * valueStr = xdmAtomicValue->getPrimitiveTypeName();
        if(valueStr != nullptr) {
            //char *str = estrdup(valueStr);
            _RETURN_STRING(valueStr);
        }
    }
    RETURN_NULL();
}




    /* {{{ __toString()
       Returns the string content */
PHP_METHOD(XdmAtomicValue, __toString)
    {
    	 XdmAtomicValue *xdmAtomicValue;
         zend_object *oobj = Z_OBJ_P(getThis());
         xdmAtomicValue_object* obj = (xdmAtomicValue_object *)((char *)oobj - XtOffsetOf(xdmAtomicValue_object, std));
         xdmAtomicValue = obj->xdmAtomicValue;
         if (xdmAtomicValue != nullptr) {
        	const char * value = xdmAtomicValue->getStringValue();
        	if(value != nullptr) {
        	    _RETURN_STRING(value);
        	 } else {
        	 _RETURN_STRING("");

        	 }
       	}else {
    		_RETURN_STRING("");
    	}
    }


PHP_METHOD(XdmAtomicValue, isAtomic)
{

    RETURN_TRUE;
}


/*void php_saxonc_initialize(void){
   if(SaxonProcessor::jvmCreatedCPP == 0){
	SaxonProcessor::jvmCreatedCPP=1;
 	SaxonProcessor::sxn_environ= (sxnc_environment *)malloc(sizeof(sxnc_environment));

    SaxonProcessor::sxn_environ->myDllHandle = loadDefaultDll ();

    initDefaultJavaRT (SaxonProcessor::sxn_environ); 
    }
} */


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
    PHP_ME(SaxonProcessor,  newXslt30Processor,     nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newXQueryProcessor,     nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  newSchemaValidator,     nullptr, ZEND_ACC_PUBLIC)
//    PHP_ME(SaxonProcessor,  importDocument,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  setResourcesDirectory,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  setCatalog,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor, setConfigurationProperty,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  registerPHPFunctions,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  version,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  isSchemaAware,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(SaxonProcessor,  release,      nullptr, ZEND_ACC_PUBLIC)
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
    PHP_ME(XsltProcessor, compileFromStringAndSave, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor, compileFromFileAndSave, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setOutputFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setSourceFromFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setSourceFromXdmValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setJustInTimeCompilation, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setParameter, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setProperty, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  clearParameters, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  clearProperties, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  setupXslMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  exceptionOccurred, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  exceptionClear, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getErrorCode, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getErrorMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XsltProcessor,  getExceptionCount, nullptr, ZEND_ACC_PUBLIC)
{nullptr, nullptr, nullptr}
};


zend_function_entry Xslt30Processor_methods[] = {
    PHP_ME(Xslt30Processor,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(Xslt30Processor, callFunctionReturningValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, callFunctionReturningString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, callFunctionReturningFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, callTemplateReturningValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, callTemplateReturningString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, callTemplateReturningFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, applyTemplatesReturningValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, applyTemplatesReturningString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, applyTemplatesReturningFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, addPackages, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, setInitialTemplateParameters, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, setInitialMatchSelection, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, setInitialMatchSelectionAsFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, setGlobalContextItem, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, setGlobalContextFromFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  transformFileToFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  transformFileToString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  transformFileToValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  transformToString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  transformToValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  transformToFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, compileFromFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, compileFromValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, compileFromString, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, compileFromStringAndSave, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, compileFromFileAndSave, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor, compileFromAssociatedFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  setOutputFile, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  setJustInTimeCompilation, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  setResultAsRawValue, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  setParameter, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  setProperty, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  clearParameters, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  clearProperties, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  setupXslMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  exceptionOccurred, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  exceptionClear, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  getErrorCode, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  getErrorMessage, nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(Xslt30Processor,  getExceptionCount, nullptr, ZEND_ACC_PUBLIC)
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
    PHP_ME(XdmValue,  __toString,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue,  getHead,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue,  itemAt,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue,  size,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmValue, addXdmItem,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

zend_function_entry xdmItem_methods[] = {
    PHP_ME(XdmItem,  __construct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmItem,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmItem,  __toString,      nullptr, ZEND_ACC_PUBLIC)
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
    PHP_ME(XdmNode,  __toString,      nullptr, ZEND_ACC_PUBLIC)
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
    PHP_ME(XdmNode,  getTypedValue,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

zend_function_entry xdmAtomicValue_methods[] = {
    PHP_ME(XdmAtomicValue,  __construct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(XdmAtomicValue,  __destruct,     nullptr, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_ME(XdmAtomicValue,  __toString,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getStringValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  isAtomic,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getBooleanValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getDoubleValue,      nullptr, ZEND_ACC_PUBLIC)
    PHP_ME(XdmAtomicValue,  getLongValue,      nullptr, ZEND_ACC_PUBLIC)
        PHP_ME(XdmAtomicValue,  getPrimitiveTypeName,      nullptr, ZEND_ACC_PUBLIC)
    {nullptr, nullptr, nullptr}
};

PHP_MINIT_FUNCTION(saxon)
{


    //php_saxonc_initialize(); - commented out code which causes PHP to hang see bug issue #4371

    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Saxon\\SaxonProcessor", SaxonProcessor_methods);
    saxonProcessor_ce = zend_register_internal_class(&ce);
    saxonProcessor_ce->create_object = saxonProcessor_create_handler;
    memcpy(&saxonProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(saxonProcessor_object_handlers));//zend_object_handlers
    saxonProcessor_object_handlers.offset = XtOffsetOf(saxonProcessor_object, std);
    saxonProcessor_object_handlers.free_obj = SaxonProcessor_free_storage;
    saxonProcessor_object_handlers.dtor_obj = SaxonProcessor_destroy_storage;
   
   // saxonProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XSLTProcessor", XsltProcessor_methods);
    xsltProcessor_ce = zend_register_internal_class(&ce);
    xsltProcessor_ce->create_object = xsltProcessor_create_handler;
    memcpy(&xsltProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(xsltProcessor_object_handlers));
    xsltProcessor_object_handlers.offset = XtOffsetOf(xsltProcessor_object, std);
    xsltProcessor_object_handlers.free_obj = XsltProcessor_free_storage;
    xsltProcessor_object_handlers.dtor_obj = XsltProcessor_destroy_storage;
    //xsltProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XSLT30Processor", Xslt30Processor_methods);
    xslt30Processor_ce = zend_register_internal_class(&ce);
    xslt30Processor_ce->create_object = xslt30Processor_create_handler;
    memcpy(&xslt30Processor_object_handlers, zend_get_std_object_handlers(), sizeof(xslt30Processor_object_handlers));
    xslt30Processor_object_handlers.offset = XtOffsetOf(xslt30Processor_object, std);
    xslt30Processor_object_handlers.free_obj = Xslt30Processor_free_storage;
    xslt30Processor_object_handlers.dtor_obj = Xslt30Processor_destroy_storage;
    //xslt30Processor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XQueryProcessor", XQueryProcessor_methods);
    xqueryProcessor_ce = zend_register_internal_class(&ce);
    xqueryProcessor_ce->create_object = xqueryProcessor_create_handler;
    memcpy(&xqueryProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(xqueryProcessor_object_handlers));
    xqueryProcessor_object_handlers.offset = XtOffsetOf(xqueryProcessor_object, std);
    xqueryProcessor_object_handlers.free_obj = xqueryProcessor_free_storage;
    xqueryProcessor_object_handlers.dtor_obj = xqueryProcessor_destroy_storage;    

	//xqueryProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XPathProcessor", XPathProcessor_methods);
    xpathProcessor_ce = zend_register_internal_class(&ce);
    xpathProcessor_ce->create_object = xpathProcessor_create_handler;
    memcpy(&xpathProcessor_object_handlers, zend_get_std_object_handlers(), sizeof(xpathProcessor_object_handlers));
    xpathProcessor_object_handlers.offset = XtOffsetOf(xpathProcessor_object, std);
    xpathProcessor_object_handlers.free_obj = xpathProcessor_free_storage;
    xpathProcessor_object_handlers.dtor_obj = xpathProcessor_destroy_storage; 
    //xpathProcessor_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\SchemaValidator", SchemaValidator_methods);
    schemaValidator_ce = zend_register_internal_class(&ce);
    schemaValidator_ce->create_object = schemaValidator_create_handler;
    memcpy(&schemaValidator_object_handlers, zend_get_std_object_handlers(), sizeof(schemaValidator_object_handlers));
    schemaValidator_object_handlers.offset = XtOffsetOf(schemaValidator_object, std);
    schemaValidator_object_handlers.free_obj = schemaValidator_free_storage;
    schemaValidator_object_handlers.dtor_obj = schemaValidator_destroy_storage; 
    //schemaValidator_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmValue", xdmValue_methods);
    xdmValue_ce = zend_register_internal_class(&ce);
    xdmValue_ce->create_object = xdmValue_create_handler;
    memcpy(&xdmValue_object_handlers, zend_get_std_object_handlers(), sizeof(xdmValue_object_handlers));
    xdmValue_object_handlers.offset = XtOffsetOf(xdmValue_object, std);
    xdmValue_object_handlers.free_obj = xdmValue_free_storage;
    xdmValue_object_handlers.dtor_obj = XdmValue_destroy_storage;
    //xdmValue_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmItem", xdmItem_methods);
    xdmItem_ce = zend_register_internal_class(&ce);
    xdmItem_ce->create_object = xdmItem_create_handler;
    memcpy(&xdmItem_object_handlers, zend_get_std_object_handlers(), sizeof(xdmItem_object_handlers));
    xdmItem_object_handlers.offset = XtOffsetOf(xdmItem_object, std);
    xdmItem_object_handlers.free_obj = xdmItem_free_storage;
    xdmItem_object_handlers.dtor_obj = XdmItem_destroy_storage;
    //xdmItem_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmNode", xdmNode_methods);
    xdmNode_ce = zend_register_internal_class(&ce);
    xdmNode_ce->create_object = xdmNode_create_handler;
    memcpy(&xdmNode_object_handlers, zend_get_std_object_handlers(), sizeof(xdmNode_object_handlers));
    xdmNode_object_handlers.offset = XtOffsetOf(xdmNode_object, std);
    xdmNode_object_handlers.free_obj = xdmNode_free_storage;
    xdmNode_object_handlers.dtor_obj = XdmNode_destroy_storage;
 
    //xdmNode_object_handlers.clone_obj = nullptr;

    INIT_CLASS_ENTRY(ce, "Saxon\\XdmAtomicValue", xdmAtomicValue_methods);
    xdmAtomicValue_ce = zend_register_internal_class(&ce);
    xdmAtomicValue_ce->create_object = xdmAtomicValue_create_handler;
    memcpy(&xdmAtomicValue_object_handlers, zend_get_std_object_handlers(), sizeof(xdmAtomicValue_object_handlers));
    xdmAtomicValue_object_handlers.offset = XtOffsetOf(xdmAtomicValue_object, std);
    xdmAtomicValue_object_handlers.free_obj = xdmAtomicValue_free_storage;
    xdmAtomicValue_object_handlers.dtor_obj = XdmAtomicValue_destroy_storage;
    //REGISTER_INI_ENTRIES();
    return SUCCESS;
}

PHP_MINFO_FUNCTION(saxon)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "saxonc", "enabled");
    php_info_print_table_row(2, "Saxon/C EXT version", "1.2.1");
    php_info_print_table_row(2, "Saxon", "9.9.1.5");
    php_info_print_table_row(2, "Excelsior JET", "15.3 MP1");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}

PHP_MSHUTDOWN_FUNCTION(saxon) {
//std::cerr<<"MSHUTDOWN called -start"<<std::endl;
//SaxonProcessor *  proc = new SaxonProcessor();
 SaxonProcessor::release();   
//delete proc;
 UNREGISTER_INI_ENTRIES();
   
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(saxon) {
   // std::cerr<<"RSHUTDOWN called -start"<<std::endl;
//    SaxonProcessor::release();
    return SUCCESS;
}

PHP_RINIT_FUNCTION(saxon) {
    //SaxonProcessor::release();
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

#ifdef COMPILE_DL_SAXONC
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(saxonc)
#endif

