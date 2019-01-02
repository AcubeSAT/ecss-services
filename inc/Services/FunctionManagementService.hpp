#ifndef ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP

#include <cstdint>
#include <iostream> // USED BY THE SAMPLE FUNCTIONS ONLY!!
#include <utility>
#include <typeinfo>

#include "etl/map.h"
#include "etl/String.hpp"
#include "Message.hpp"

#define FUNCMAPSIZE        128     // size of the function map in bytes (temporary, arbitrary)
#define MAXFUNCNAMELENGTH  32      // max length of the function name (temporary, arbitrary)
#define MAXARGLENGTH       32      // maximum argument byte string length (temporary, arbitrary)

/**
 * Implementation of the ST[08] function management service
 *
 * This class implements a skeleton framework for the ST[08] service as described in
 * ECSS-E-ST-70-41C, pages 157-159. Final implementation is dependent on subsystem requirements
 * which are, as of this writing, undefined yet.
 *
 * Caveats:
 * 1) Any string handling in this class involves **non-null-terminated strings**.
 *
 * You have been warned.
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

typedef String<MAXFUNCNAMELENGTH> functionName;
typedef etl::map<functionName, void(*)(String<MAXARGLENGTH>), (const size_t) FUNCMAPSIZE>
PointerMap;

class FunctionManagementService {
	/**
	 * Map of the function names to their respective pointers. Size controlled by FUNCMAPSIZE
	 */
	 PointerMap funcPtrIndex;

public:
	/**
	 * Constructs the function pointer index with all the necessary functions at initialization time
	 * These functions need to be in scope.
	 * @param None
	 */
	FunctionManagementService();

	/**
	 * Calls the function described in the TC[8,1] message *msg*, passing the arguments contained
	 * WARNING: Do not include any spaces in the arguments, they are ignored and replaced with NULL
	 * @param msg A TC[8,1] message
	 */
	void call(Message msg);
};

#endif //ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
