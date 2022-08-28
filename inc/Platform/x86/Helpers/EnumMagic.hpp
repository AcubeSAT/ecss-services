#pragma once

#include <string>
#include <utility>

#if !defined(__GNUC__) && !defined(__clang__)
#define __PRETTY_FUNCTION__ "E V = Sorry, feature not supported!"
#endif

/**
 * A simplified version of https://github.com/Neargye/magic_enum, this file uses some gcc compiler options to provide an @ref enumName function.
 * This function converts an enum value to a string. It's used for debugging purposes only, to improve the readability of values that are hidden
 * behind enums.
 *
 * Use with care. NOT intended for a microcontroller.
 *
 * @note This functionality is designed for simple, sequence-like enums. Their values should start from 0 and increase 1-by-1 until the end. It will probably
 * result in slow compilation or incorrect results if used for other, "weirder" enums.
 *
 * This hack aims to be safe from errors by using `constexpr` lavishly. Enum values which are out of bounds (e.g. not defined or beyond
 * EnumMagic_::MaximumValues) will not throw errors or cause unexpected events, but will probably display some kind of string.
 *
 * @license MIT, Daniil Goncharov
 */
namespace EnumMagic_ {
	/**
	 * The highest enum value that can be handled.
	 *
	 * Values higher than this will just result in a number or malformed text, but _not_ a compilation or runtime error.
	 */
	constexpr auto MaximumValues = 256;

	/**
	 * Returns the identifier of an enum value.
	 *
	 * For example, if you define an enum:
	 * @code
	 * enum Something {
	 *     First = 0,
	 *     Second = 1
	 * }
	 * @endcode
	 * then calling this function with `Something::First` will return the string `"Something::First"`.
	 *
	 * Internally, this function uses the [`__PRETTY_FUNCTION__`](https://gcc.gnu.org/onlinedocs/gcc/Function-Names.html)
	 * magic constant defined by GCC. The constant contains the template arguments of the function in a string,
	 * which themselves contain the identifier of the enum value.
	 *
	 * @note Tested with GCC version 11 only, other versions or compilers might return incorrect values
	 *
	 * @tparam E Enum type
	 * @tparam V Enum value (of type E)
	 * @return The identifier of the enum value
	 */
	template <typename E, E V>
	constexpr std::basic_string_view<char> enumHack() {
		static_assert(std::is_enum_v<E>, "enumHack() requires enum type.");

		std::string_view function = __PRETTY_FUNCTION__;
		auto pos = function.find("E V = ");
		std::string_view output = function.substr(pos + 6);
		output.remove_suffix(1);
		return output;
	}

	/**
	 * Shortcut definition of enumHack for easier access
	 */
	template <typename Enum, Enum Value>
	inline constexpr auto enumNameV = enumHack<Enum, Value>();

	/**
	 * Given an enum type and a sequence of values, this function returns a list
	 *
	 * @tparam Indexes A list of potential enum values in numerical format
	 * @return An std::array of names for every enum value in the indexes
	 */
	template <typename Enum, std::size_t... Indexes>
	constexpr auto names(std::index_sequence<Indexes...>) noexcept {
		static_assert(std::is_enum_v<Enum>, "names() requires enum type.");

		return std::array<std::string_view, sizeof...(Indexes)>{{enumNameV<Enum, static_cast<Enum>(Indexes)>...}};
	}

	/**
	 * For the specified Enum enumeration, this variable will contain a list of the names of all its values
	 */
	template <typename Enum>
	inline constexpr std::array namesV = names<Enum>(std::make_index_sequence<MaximumValues>{});

	/**
	 * Given a nested name (e.g. `Services::Parameter::value`), this function returns the innermost name (e.g. `value`)
	 */
	constexpr std::string_view removeNamespace(std::string_view in) {
		auto pos = in.find_last_of("::");
		return in.substr(pos + 1);
	}
} // namespace EnumMagic_

/**
 * Get the name of an enum value, as defined in the source code.
 */
template <typename Enum>
std::string enumName(Enum value) {
	using namespace EnumMagic_;

	auto index = static_cast<size_t>(value);

	if (index >= namesV<Enum>.size()) {
		return "(Unknown)";
	}

	return std::string(removeNamespace(namesV<Enum>[index]));
}
