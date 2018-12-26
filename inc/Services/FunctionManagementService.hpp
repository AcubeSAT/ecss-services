#ifndef ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP

#include <stdint.h>
#include "etl/map.h"
#include "etl/String.hpp"
#include "Message.hpp"

/**
 * Implementation of the ST[08] function management service
 *
 * This class implements a basic framework for the ST[08] service as described in ECSS-E-ST-70-41C,
 * pages 157-159. Actual implementation is dependent on subsystem requirements which are, as of this
 * writing, undefined yet.
 *
 * <b>WARNING!</b> Due to the class's usage of the custom string class by Konstantinos Kanavouras,
 * <b>the function name passed in the message shall be stripped of its trailing NULL character!</b>
 *
 * @todo Ignore the ending NULL in passed strings
 * @todo Revamp the method used to store the argument types, it's hacky AF and a (memory) space hog
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

#define FUNCMAPSIZE 5       // size of the function map (temporary, arbitrary)
#define MAXFUNCNAMELEN 128  // max length of the function (temporary, arbitrary)
#define MAXFUNCARGS 64      // maximum arguments that a function can hold
#define MAXARGLENGTH 32     // maximum argument name length

typedef struct {
	void (*funcPtr)(void);                      // a generic function pointer
	String<MAXARGLENGTH> args[MAXFUNCARGS];     // the argument types with their original sequence
} Function;

class FunctionManagementService {
	etl::map<String<MAXFUNCNAMELEN>,
	    Function, FUNCMAPSIZE> funcPtrIndex;  // map of function names to their pointers

public:
	/**
	 * Calls the function described in the TC[8,1] message *msg*, passing the arguments contained
	 * @param msg A TC[8,1] message
	 */
	void call(Message msg);
};

#endif //ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
