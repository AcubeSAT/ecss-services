#ifndef FILECOPYOPERATIONIDGENERATOR_HPP
#define FILECOPYOPERATIONIDGENERATOR_HPP

#pragma once

#include "etl/unordered_set.h"

namespace Filesystem {

	using OperationId = uint32_t;
	constexpr OperationId InvalidOperationId = 0;

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
		inline static etl::unordered_set<OperationId, 16> activeIds;
	};

} // namespace Filesystem

#endif //FILECOPYOPERATIONIDGENERATOR_HPP
