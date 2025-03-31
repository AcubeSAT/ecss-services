#include "Helpers/FilepathValidators.hpp"

namespace FilepathValidators {
	etl::optional<size_t> findWildcardPosition(const Filesystem::Path& path) {
		auto wildcardPosition = path.find(FileManagementService::Wildcard, 0);

		if (wildcardPosition == Filesystem::Path::npos) {
			return {};
		}

		return wildcardPosition;
	}

	etl::expected<void, FilePatternError> validateSearchPattern(const etl::string<Filesystem::ObjectPathSize>&pattern) {
		if (pattern.empty()) {
			return etl::unexpected(FilePatternError::EmptyPattern);
		}
		constexpr std::array<char, 8> ILLEGAL_CHARS = {'<', '>', ':', '"', '/', '\\', '|', '?'};

		// Use find_first_of() with a cast to const char*
		if (pattern.find_first_of(ILLEGAL_CHARS.data()) != etl::string<Filesystem::ObjectPathSize>::npos) {
			return etl::unexpected(FilePatternError::IllegalCharacter);
		}

		return {};
	}
} // namespace FilepathValidators
