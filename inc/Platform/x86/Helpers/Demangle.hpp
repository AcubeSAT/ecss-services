#ifndef _MSC_VER
#include <cxxabi.h>
#endif
namespace Demangler {
	template<typename ErrorType>
	inline auto demangle() {
		#ifndef _MSC_VER
		return abi::__cxa_demangle(typeid(ErrorType).name(), nullptr, nullptr, nullptr)
		#else
		return typeid(ErrorType).name();
		#endif
	}
}
