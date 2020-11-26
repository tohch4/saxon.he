// SaxonApiException.cpp : Defines the exported functions for the DLL application.
//

#include "SaxonApiException.h"



  const char * SaxonApiException::what () {
        return message.c_str();
  }

  SaxonApiException::SaxonApiException(){
	    message = "";
	    lineNumber = -1;
	    errorCode = "";
	    systemId = "";
    }


	SaxonApiException::SaxonApiException(const SaxonApiException &ex){
		message = ex.message;
		lineNumber = ex.lineNumber;
		errorCode = ex.errorCode;
		systemId = ex.systemId;
	}


	SaxonApiException::SaxonApiException(const char * m, const char * ec, const char * sysId, int linenumber){
		if(m != nullptr) {
		    message = std::string(m);
		} else {
		    message = "";
		}

        lineNumber = linenumber;

        if (ec != nullptr) {
            errorCode = std::string(ec);
        } else {
            errorCode = "";
        }

        if(sysId != nullptr) {
            systemId = sysId;
        } else {
            systemId = "";
        }
	}




    /**
     * A destructor.
     */
	SaxonApiException::~SaxonApiException() noexcept {
        message.clear();
        errorCode.clear();
        systemId.clear();
	}

    /**
     * Get the error code associated with the ith exception in the vector, if there is one
     * @param i - ith exception in the vector
     * @return the associated error code, or nullptr if no error code is available
     */
	const char * SaxonApiException::getErrorCode(){
		return errorCode.c_str();
	}


	int SaxonApiException::getLineNumber(){
        return lineNumber;
	}


    const char * SaxonApiException::getSystemId() {
        return systemId.c_str();
    }

	const char * SaxonApiException::getMessage(){
	    return message.c_str();

	}