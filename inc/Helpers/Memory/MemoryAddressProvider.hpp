#ifndef MEMORYADDRESSPROVIDER_HPP
#define MEMORYADDRESSPROVIDER_HPP

#include "ECSS_Definitions.hpp"
#include "Memory.hpp"
#include "etl/unordered_map.h"

/**
 * Maximum number of entries in Valid Memory IDs set
 */
inline constexpr uint32_t MaxValidMemoryIdsSize = 8;

namespace MemoryAddressProvider {
	/**
	*
	*/
	extern const etl::unordered_map<MemoryId, Memory*, MaxValidMemoryIdsSize> memoryMap;

} // namespace MemoryAddressProvider

#endif //MEMORYADDRESSPROVIDER_HPP
