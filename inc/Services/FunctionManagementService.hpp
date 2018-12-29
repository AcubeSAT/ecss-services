#ifndef ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP

#include <cstdint>
#include <iostream> // TEMPORARY!
#include <utility>
#include <typeinfo>

#include "etl/map.h"
#include "etl/String.hpp"
#include "Message.hpp"

/**
 * Implementation of the ST[08] function management service
 *
 * This class implements a basic framework for the ST[08] service as described in ECSS-E-ST-70-41C,
 * pages 157-159. Final implementation is dependent on subsystem requirements which are, as of this
 * writing, undefined yet.
 * *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

#define FUNCMAPSIZE     5        // size of the function map (temporary, arbitrary)
#define MAXFUNCNAMELEN  128      // max length of the function (temporary, arbitrary)
#define MAXFUNCARGS     64       // maximum arguments that a function can hold
#define MAXARGLENGTH    32       // maximum argument name length

typedef String<MAXFUNCNAMELEN> funcName;

class FunctionManagementService {
	etl::map<funcName, void*, (const size_t) FUNCMAPSIZE> funcPtrIndex;
	// map of function names to their pointers

public:
	//FunctionManagementService(); // dummy constructor (commented out to pacify clang-tidy)
	/**
	 * Calls the function described in the TC[8,1] message *msg*, passing the arguments contained
	 * @param msg A TC[8,1] message
	 */
	void call(Message msg);
};

#endif //ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
