//
// Created by kyriakum on 19/2/2025.
//

#ifndef MEMORYADDRESSPROVIDER_HPP
#define MEMORYADDRESSPROVIDER_HPP

#include <memory>
#include "ErrorHandler.hpp"
#include "Helpers/CRCHelper.hpp"
#include "MemoryAddressLimits.hpp"
#include "etl/unordered_map.h"
#include "ECSS_Definitions.hpp"
#include "Memory.hpp"


/**
 * Maximum number of entries in Memory Limits Map
 */
inline constexpr uint32_t MaxMemoryLimitsMapSize = 8;

/**
 * Maximum number of entries in Valid Memory IDs set
 */
inline constexpr uint32_t MaxValidMemoryIdsSize = 8;

namespace MemoryAddressProvider {
	/**
	 * Helper struct to define upper and lower limits of different memories
	 */
/**	struct MemoryLimits {
		uint32_t lowerLim;
		uint32_t upperLim;
	};*/

	// Memory type ID's
	//enum MemoryID {
		/*DTCMRAM = 0,
		RAM_D1,
		RAM_D2,
		RAM_D3,
		ITCMRAM,
		FLASH_MEMORY,
		EXTERNAL,*/
	//};

	/**
	*
	*/
	extern const etl::unordered_map<MemoryId, Memory*, MaxValidMemoryIdsSize> memoryMap;

	/**
	 * Map containing all the different types of memory limits
	 */
	//extern constexpr etl::unordered_map<MemoryId, MemoryLimits, MaxMemoryLimitsMapSize> memoryLimitsMap;
	/* = {
		{MemoryManagementService::MemoryID::DTCMRAM, {DTCMRAMLowerLim, DTCMRAMUpperLim}},
		{MemoryManagementService::MemoryID::ITCMRAM, {ITCMRAMLowerLim, ITCMRAMUpperLim}},
		{MemoryManagementService::MemoryID::RAM_D1, {RAMD1LowerLim, RAMD1UpperLim}},
		{MemoryManagementService::MemoryID::RAM_D2, {RAMD2LowerLim, RAMD2UpperLim}},
		{MemoryManagementService::MemoryID::RAM_D3, {RAMD3LowerLim, RAMD3UpperLim}},
		{MemoryManagementService::MemoryID::FLASH_MEMORY, {FlashLowerLim, FlashUpperLim}}};*/

	/**
	 * Data structure containing all the valid memory IDs
	 * TODO remove this since it's useless
	 */
 /*	constexpr etl::unordered_set<MemoryID, MaxValidMemoryIdsSize> validMemoryIds = {
		MemoryManagementService::MemoryID::RAM_D1,
		MemoryManagementService::MemoryID::RAM_D2,
		MemoryManagementService::MemoryID::RAM_D3,
		MemoryManagementService::MemoryID::DTCMRAM,
		MemoryManagementService::MemoryID::ITCMRAM,
		MemoryManagementService::MemoryID::FLASH_MEMORY,
		MemoryManagementService::MemoryID::EXTERNAL};*/

};

#endif //MEMORYADDRESSPROVIDER_HPP

