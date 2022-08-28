#pragma

/**
 * Result class
 * @tparam V Value type
 * @tparam E Error type
 */
template<typename V, typename E>
class Result {
public:
	/**
	 * @name Constructors
	 * @{
	 */
	explicit Result(V&& value) noexcept(std::is_nothrow_constructible<V>::value) : success(true), storedValue(std::forward<V>(value)) { }

	explicit Result(E&& error) noexcept(std::is_nothrow_constructible<E>::value) : success(false), storedError(std::forward<E>(error)) {}
	/**
	 * @}
	 */

	/**
	 * @name Result access
	 * @{
	 */

	constexpr V& operator*() const noexcept {
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
	template<typename DT, typename DE>
	friend class Result;

	using StoredValue = V;
	using StoredError = E;

	bool success;
	union {
		StoredValue storedValue;
		StoredError storedError;
	};
};
