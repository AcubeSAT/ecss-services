#include "Helpers/FilepathValidators.hpp"

namespace FilepathValidators {
	etl::optional<size_t> findWildcardPosition(const Filesystem::Path& path) {
		size_t wildcardPosition = path.find(FileManagementService::Wildcard, 0);

		if (wildcardPosition == -1) {
			return {};
		}

		return wildcardPosition;
	}

	etl::optional<Filesystem::Path> getStringUntilTerminator(Message& message) {
		uint8_t charCounter = 0;
		Filesystem::Path extractedString = "";

		static_assert(sizeof(char) == sizeof(uint8_t));
		char currentChar = static_cast<char>(message.readByte());

		while (currentChar != FileManagementService::VariableStringTerminator) {
			if (charCounter == ECSSMaxStringSize - 1) {
				return etl::nullopt;
			}

			extractedString.append(1, currentChar);
			charCounter++;
			currentChar = static_cast<char>(message.readByte());
		}

		return extractedString;
	}
} // namespace FilepathValidators