#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"

#include <functional>

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
 * Forward declaration to help
 */
template <typename V, typename E>
class Result;

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

namespace Detail {
	template <class T>
	struct IsResult : public std::false_type {};

	template <class V, class E>
	struct IsResult<Result<V, E>> : public std::true_type {};

	template <class T>
	inline constexpr bool IsResultV = IsResult<T>::value;
} // namespace Detail


/**
 * Result class
 * @tparam V Value type
 * @tparam E Error type
 */
template <typename V, typename E>
class Result {
public:
	using ValueType = V;
	using ErrorType = E;

	/**
	 * @name Constructors
	 * @{
	 */
	Result(V&& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(std::forward<V>(value)) {}

	template<typename = std::enable_if<!std::is_same_v<V, E>>>
	Result(E&& error) noexcept(std::is_nothrow_constructible_v<E>) : success(false), storedError(std::forward<E>(error)) {}


	Result(const Ok<V>& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(value.value) {}

	Result(Ok<V>& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(value.value) {}

	Result(Ok<V>&& value) noexcept(std::is_nothrow_constructible_v<V>) : success(true), storedValue(std::forward<V>(value.value)) {}

	Result(Err<E>&& error) noexcept(std::is_nothrow_constructible_v<V>) : success(false), storedError(std::forward<E>(error.value)) {}

	~Result() {
		if (success) {
			storedValue.~StoredValue();
		} else {
			storedError.~StoredError();
		}
	}

	/**
	 * @}
	 */

	/**
	 * @name Copy and move operators
	 * @{
	 */

	/**
	 * Copy constructor from any other Result
	 */
	 template<typename OtherValue, typename OtherError>
	constexpr Result(const Result<OtherValue, OtherError>& input) noexcept {
		success = input.success;
		if (success) {
			storedValue = input.storedValue;
		} else {
			storedError = input.storedError;
		}
	}

	/**
	 * Copy assignment operator from any other Result
	 * @param input
	 * @return
	 */
//	template<typename OtherValue, typename OtherError>
	Result& operator=(const Result& input) {
		if (success != input.success) {
			if (success) {
				storedValue.~StoredValue();
				new(&storedError) StoredError(input.storedError);
			} else {
				storedError.~StoredError();
				new(&storedValue) StoredValue(input.storedValue);
			}

			success = input.success;
		} else {
			if (success) {
				storedValue = input.storedValue;
			} else {
				storedError = input.storedError;
			}
		}

		return *this;
	}

	/**
	 * catchall copy assignment operator
	 * @todo
	 */
	template<typename T>
	Result& operator=(const T& input) {
		new(this) Result<V,E>(input);
		return *this;
	}

	/**
	 * Move assignment operator
	 */
//	template<typename OtherValue, typename OtherError>
//	Result& operator=(const Result<OtherValue, OtherError>&& data) {
//		//TODO!!!
//		return *this;
//	}

	/**
	 * @}
	 */

	/**
	 * @name Result access
	 * @{
	 */

	constexpr Result<V, E>& then(std::function<void(const V&)> function) const noexcept {
		if (success) {
			function(storedValue);
		}

		return *this;
	}

	constexpr Result<V, E>& then(std::function<void(V&)> function) noexcept {
		if (success) {
			function(storedValue);
		}

		return *this;
	}

	template <typename Function, typename Return = std::invoke_result_t<Function, V>, typename = std::enable_if_t<!Detail::IsResultV<Return>>>
	constexpr Result<Return, E> map(Function function) const noexcept {
		if (success) {
			return Ok(function(storedValue));
		} else {
			return Err(storedError);
		}
	}

	template <typename Function, typename Return = std::invoke_result_t<Function, V>, typename = std::enable_if_t<Detail::IsResultV<Return>>>
	constexpr Return map(Function function) const noexcept {
		static_assert(std::is_same_v<ErrorType, typename Return::ErrorType> || std::is_constructible_v<typename Return::ErrorType>, "Couldn't convert input error type to output error type. Please write a conversion function or provide a default error constructor for the output.");

		if (success) {
			return function(storedValue);
		} else {
			if constexpr (std::is_same_v<ErrorType, typename Return::ErrorType>) {
				return Return(Err(storedError));
			} else if constexpr (std::is_convertible_v<ErrorType, typename Return::ErrorType>) {
				return Return(Err(typename Return::ErrorType(storedError)));
			} else {
				return Return(Err(typename Return::ErrorType()));
			}
		}
	}

	constexpr V& operator*() noexcept {
		return storedValue;
	}

	constexpr V unwrap() const noexcept {
		// TODO see what's up with moving stuff and std::forward

		return storedValue;
	}

	constexpr V valueOr() const noexcept {
		static_assert(std::is_constructible_v<V>, "You can use the empty Result::valueOr() function only if there is an empty constructor");

		if (success) {
			return storedValue;
		} else {
			return V();
		}
	}

	constexpr V&& valueOr(V&& otherValue) noexcept {
		if (success) {
			return std::forward<V>(storedValue);
		} else {
			return std::forward<V>(otherValue);
		}
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
