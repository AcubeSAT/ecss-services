#pragma once

#include <cstdint>
#include <etl/optional.h>
#include "Services/FileManagementService.hpp"
#include "etl/String.hpp"
#include <regex>

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
	 * 
	 * @param pattern 
	 * @return 
	 */
	etl::optional<FilePatternError> validateSearchPattern(const etl::string<Filesystem::ObjectPathSize>& pattern);
} //namespace FilepathValidators
