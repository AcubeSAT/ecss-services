#ifndef ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
#define ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP

#include <cstdint>
#include <iostream> // USED BY THE SAMPLE FUNCTIONS ONLY!!
#include <utility>
#include <typeinfo>

#include "etl/map.h"
#include "etl/String.hpp"
#include "Message.hpp"
#include "Service.hpp"

#define FUNCMAPSIZE        128     // size of the function map in bytes (temporary, arbitrary)
#define MAXFUNCNAMELENGTH  32      // max length of the function name (temporary, arbitrary)
#define MAXARGLENGTH       32      // maximum argument byte string length (temporary, arbitrary)

/**
 * @todo: Undef TESTMODE before flight!!!!
 */
#define TESTMODE                   // REMOVE BEFORE FLIGHT!(used by Catch to gain visibility @ test)

/**
 * Implementation of the ST[08] function management service
 *
 * This class implements a skeleton framework for the ST[08] service as described in
 * ECSS-E-ST-70-41C, pages 157-159. Final implementation is dependent on subsystem requirements
 * which are, as of this writing, undefined yet.
 *
 * Caveats:
 * 1) Function names shall be exactly MAXFUNCNAMELENGTH-lengthed in order to be properly read
 * and stored!  (not sure if this is a caveat though, as ECSS-E-ST-70-41C stipulates for ST[08]
 * that all function names must be fixed-length character strings)
 *
 * You have been warned.
 *
 * @author Grigoris Pavlakis <grigpavl@ece.auth.gr>
 */

typedef String<MAXFUNCNAMELENGTH> functionName;
typedef etl::map<functionName, void(*)(String<MAXARGLENGTH>), FUNCMAPSIZE>
PointerMap;

class FunctionManagementService : public Service {
	/**
	 * Map of the function names to their respective pointers. Size controlled by FUNCMAPSIZE
	 */
#ifdef TESTMODE
public: PointerMap funcPtrIndex;
#else
	PointerMap funcPtrIndex;
#endif

public:
	/**
	 * Constructs the function pointer index with all the necessary functions at initialization time
	 * These functions need to be in scope. Uncomment when needed.
	 *
	 * @param None
	 */
	//FunctionManagementService();

	/**
	 * Calls the function described in the TC[8,1] message *msg*, passing the arguments contained
	 * and, if non-existent, generates a failed start of execution notification.
	 * @param msg A TC[8,1] message
	 */
	 #ifdef TESTMODE
	int call(Message msg);
	 #else
	void call(Message msg);
	 #endif

	/**
	 * Includes a new function in the pointer map. This enables it to be called by way of a valid
	 * TC [8,1] message.
	 *
	 * @param funcName the function's name. Max. length is MAXFUNCNAMELENGTH bytes.
	 * @param ptr pointer to a function of void return type and a MAXARGLENGTH-lengthed byte
	 * string as argument (which contains the actual arguments of the function)
	 */
#ifdef TESTMODE
	int include(String<MAXFUNCNAMELENGTH> funcName, void(*ptr)(String<MAXARGLENGTH>));
#else
	void include(String<MAXFUNCNAMELENGTH> funcName, void(*ptr)(String<MAXARGLENGTH>));
#endif
};

#endif //ECSS_SERVICES_FUNCTIONMANAGEMENTSERVICE_HPP
