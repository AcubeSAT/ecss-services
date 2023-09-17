#pragma once

#include <cstdint>
#include <etl/optional.h>
#include "Services/FileManagementService.hpp"
#include "etl/String.hpp"

namespace FilepathValidators {
	/**
     * If a wildcard is encountered, then it returns its position in the string (starting from 0).
     * @param path : The path passed as a String.
     * @return Optionally, the position of the wildcard.
     */
	etl::optional<size_t> findWildcardPosition(const Filesystem::Path& path);

	/**
	 * Returns the Message data as a String, if a Variable String Terminator is found within an
	 * acceptable length. Returns a null optional otherwise.
	 * @param message A message to extract the string from.
	 * @return Optionally, a String of type Filesystem::Path.
	 */
	etl::optional<Filesystem::Path> getStringUntilTerminator(Message& message);
} //namespace FilepathValidators
