#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

#include <optional>

template <typename T, int Tag>
struct ValueWrapper {
	explicit constexpr ValueWrapper(T const& val) noexcept(std::is_nothrow_copy_constructible_v<T>)
	    : value{val} {}

	explicit constexpr ValueWrapper(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>)
	    : value{std::forward<T>(val)} {}

	T value;
};

//TODO void value wrapper

template <typename T>
struct Ok {
	explicit constexpr Ok(T const& val) noexcept(std::is_nothrow_copy_constructible_v<T>)
	    : value{val} {}

	explicit constexpr Ok(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>)
	    : value{std::forward<T>(val)} {}

//	template<typename OtherType>
//	constexpr Ok(OtherType const& val) noexcept(std::is_nothrow_copy_constructible_v<OtherType>)
//	    : value(val) {}
//
//	template<typename OtherType>
//	constexpr Ok(OtherType&& val) noexcept(std::is_nothrow_move_constructible_v<OtherType>)
//	    : value(std::forward<OtherType>(val)) {}

	T value;
};

template <typename T>
struct Err {
	constexpr Err(T const& val) noexcept(std::is_nothrow_copy_constructible_v<T>)
	    : value{val} {}

	constexpr Err(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>)
	    : value{std::forward<T>(val)} {}

	T value;
};

//template <typename T>
//using Ok = ValueWrapper<T, 0>;
//
//template <typename T>
//using Err = ValueWrapper<T, 1>;


/**
 * Syntactic sugar to return successful Result
 * Note: this function is a producer / constructor of type T
 *
 * @return type::Ok<T> that can be converted into a successful Result<T, E>
 */
template <typename T, typename CleanT = typename std::decay<T>::type>
inline Ok<CleanT> Ocake(T&& val) {
	return Ok<CleanT>{std::forward<T>(val)};
}


/**
 * Result class
 * @tparam V Value type
 * @tparam E Error type
 */
template <typename V, typename E>
class Result {
public:
	/**
	 * @name Constructors
	 * @{
	 */
	Result(V&& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(std::forward<V>(value)) {}

	Result(E&& error) noexcept(std::is_nothrow_constructible_v<E>) : success(false), storedError(std::forward<E>(error)) {}


	Result(const Ok<V>& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(value.value) {}

	Result(Ok<V>& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(value.value) {}

	Result(Ok<V>&& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(std::forward<V>(value.value)) {}

	Result(Err<E>&& error) noexcept(std::is_nothrow_constructible_v<V>) : success(false), storedError(std::forward<E>(error.value)) {}


	/**
	 * @}
	 */

	/**
	 * @name Result access
	 * @{
	 */

	constexpr std::optional<V>& operator*() noexcept {
		return storedValue;
	}

	constexpr V&& unwrap() const noexcept {
		return std::forward<V>(storedValue);
	}

	constexpr V valueOr() const noexcept {
		static_assert(std::is_constructible_v<V>, "You can use the empty Result::valueOr() function only if there is an empty constructor");

		if (success) {
			return storedValue;
		}

		return V();
	}

	/**
	 * @}
	 */

	/**
	 * @name Error access
	 * @{
	 */

	/**
	 * @todo see what's going on with references
	 * @return
	 */
	constexpr E error() const noexcept {
		if constexpr (std::is_constructible_v<E>) {
			if (success) {
				return E();
			}
		}

		return storedError;
	}

	/**
	 * @}
	 */

	/**
	 * The default bool operator allows checking if the result is successful
	 *
	 * @code
	 * Result<V,E> result = ...;
	 * if (result) {
	 *     // success!!
	 * }
	 * @endcode
	 */
	[[nodiscard]] constexpr explicit operator bool() const noexcept {
		return success;
	}

	/**
	 * @return True if the result is successful, false if there is an error
	 */
	[[nodiscard]] constexpr bool has_value() const noexcept {
		return success;
	}

	/**
	 * @return True if there is an error, false if the result is successful
	 */
	[[nodiscard]] constexpr bool has_error() const noexcept {
		return !success;
	}

private:
	template <typename DT, typename DE>
	friend class Result;

	using StoredValue = V;
	using StoredError = E;

	bool success;
	union {
		StoredValue storedValue;
		StoredError storedError;
	};
};

//template<typename V, typename E>
//Result(Ok<V>) -> Result<typename V, typename E>

#pragma clang diagnostic pop
