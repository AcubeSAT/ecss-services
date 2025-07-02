#ifndef FILECOPYOPERATIONIDGENERATOR_HPP
#define FILECOPYOPERATIONIDGENERATOR_HPP

#pragma once

#include "etl/unordered_set.h"

namespace Filesystem {

	using OperationId = uint32_t;
	constexpr OperationId InvalidOperationId = 0;
	// TODO (#317): Discuss and determine a maximum for this value if the implementation does not change
	constexpr size_t MaxConcurrentFileOperations = 16;

	class OperationIdGenerator {
	public:
		static OperationId next() {
			while (true) {
				currentId++;
				if (activeIds.find(currentId) == activeIds.end()) {
					break;
				}
			}
			return currentId;
		}

		static void markInUse(const OperationId id) {
			activeIds.insert(id);
		}

		static void release(const OperationId id) {
			activeIds.erase(id);
		}

		[[nodiscard]] static bool isInUse(const OperationId id) {
			return activeIds.find(id) != activeIds.end();
		}

	private:
		inline static OperationId currentId = InvalidOperationId;
		inline static etl::unordered_set<OperationId, MaxConcurrentFileOperations> activeIds; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
	};

} // namespace Filesystem

#endif //FILECOPYOPERATIONIDGENERATOR_HPP
