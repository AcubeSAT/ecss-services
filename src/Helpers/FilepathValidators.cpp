#include "Helpers/FilepathValidators.hpp"

namespace FilepathValidators {
	etl::optional<size_t> findWildcardPosition(const Filesystem::Path& path) {
		auto wildcardPosition = path.find(FileManagementService::Wildcard, 0);

		if (wildcardPosition == Filesystem::Path::npos) {
			return {};
		}

		return wildcardPosition;
	}

	etl::expected<void, FilePatternError> validateSearchPattern(const etl::string<Filesystem::ObjectPathSize>&
		pattern) {
		if (pattern.empty()) {
			return etl::unexpected(FilePatternError::EmptyPattern);
		}

		if (const std::regex invalid_chars(R"([<>:"/\\|?])"); std::regex_search(pattern.data(), invalid_chars)) {
			return etl::unexpected(FilePatternError::IllegalCharacter);
		}

		return {};
	}
} // namespace FilepathValidators