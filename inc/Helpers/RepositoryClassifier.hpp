#ifndef REPOSITORYCLASSIFIER_HPP
#define REPOSITORYCLASSIFIER_HPP

#pragma once

#include "Filesystem.hpp"

namespace Filesystem {

	/**
	 * Determines whether a given repository path is local to this filesystem.
	 * The local prefix must be configured at startup or compile-time.
	 */
	class RepositoryClassifier {
	public:
		static Path getLocalPrefix() {
			return localPrefix;
		}

		static void setLocalPrefix(const Path& prefix) {
			localPrefix = prefix;
		}

		static bool isLocal(const Path& path) {
			return path.find(localPrefix) == 0;
		}

	private:
		inline static Path localPrefix = "";
	};

} // namespace Filesystem
#endif //REPOSITORYCLASSIFIER_HPP
