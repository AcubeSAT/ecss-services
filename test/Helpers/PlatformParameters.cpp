#include "Platform/x86/Parameters/PlatformParameters.hpp"
#include "../Services/ServiceTests.hpp"
#include "ECSS_Definitions.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("Getting a reference of a parameter") {
	SECTION("Valid parameter requested") {
		ParameterId parameterId = 7;
		REQUIRE(Services.parameterManagement.getParameter(parameterId)->get().getValueAsUint64() == 55);

		parameterId = 11;
		REQUIRE(Services.parameterManagement.getParameter(parameterId)->get().getValueAsUint64() == 1);
	}

	SECTION("Invalid parameter requested") {
		ParameterId parameterId = ECSSParameterCount + 1;
		REQUIRE(not Services.parameterManagement.getParameter(parameterId));

		parameterId = parameterId + 24;
		REQUIRE(not Services.parameterManagement.getParameter(parameterId));
	}
}