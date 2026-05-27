#pragma once

#include <cstdint>
#include <etl/optional.h>
#include "Services/FileManagementService.hpp"

namespace FilepathValidators {

	/**
	 * Errors returned when a search pattern fails validation
	 */
	enum class FilePatternError : uint8_t {
		EmptyPattern = 0,    ///< The provided pattern string is empty
		IllegalCharacter = 1 ///< The pattern contains a character that is not permitted in a filename
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
