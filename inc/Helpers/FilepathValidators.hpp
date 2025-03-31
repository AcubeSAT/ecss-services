#pragma once

#include <cstdint>
#include <etl/optional.h>
#include "Services/FileManagementService.hpp"

namespace FilepathValidators {

	enum class FilePatternError : uint8_t {
		EmptyPattern = 0,
		IllegalCharacter = 1
	};

	/**
     * If a wildcard is encountered, then it returns its position in the string (starting from 0).
     * @param path The path passed as a String.
     * @return Optionally, the position of the wildcard.
     */
	etl::optional<size_t> findWildcardPosition(const Filesystem::Path& path);

	/**
		 * Validates the input search pattern. If the pattern is empty, or contains illegal characters an
		 * appropriate error is returned.
		 * @param pattern the pattern to validate.
		 * @return an error if the pattern is invalid, nothing otherwise.
		 */
	etl::expected<void, FilePatternError> validateSearchPattern(const etl::string<Filesystem::ObjectPathSize>& pattern);
} //namespace FilepathValidators
