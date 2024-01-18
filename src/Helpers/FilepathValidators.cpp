#include "Helpers/FilepathValidators.hpp"

namespace FilepathValidators {
	etl::optional<size_t> findWildcardPosition(const Filesystem::Path& path) {
		auto wildcardPosition = path.find(FileManagementService::Wildcard, 0);

		if (wildcardPosition == Filesystem::Path::npos) {
			return {};
		}

		return wildcardPosition;
	}
} // namespace FilepathValidators