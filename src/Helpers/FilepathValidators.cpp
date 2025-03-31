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

		auto isIllegalCharacter = [&ILLEGAL_CHARS](char c) {
			return std::any_of(ILLEGAL_CHARS.begin(), ILLEGAL_CHARS.end(), [c](const char illegal) {
				return c == illegal;
			});
		};

		for (const char c : pattern) {
			if (isIllegalCharacter(c)) {
				return etl::unexpected(FilePatternError::IllegalCharacter);
			}
		}

		return {};
	}
} // namespace FilepathValidators
