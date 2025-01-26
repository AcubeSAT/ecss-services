#pragma once

#include "Services/HousekeepingService.hpp"
#include "Services/EventReportService.hpp"

namespace ForwardingAndPacketHelper {
    static inline ApplicationProcessId applications[] = {1};
    static inline ServiceTypeNum services[] = {3, 5};
    static inline ServiceTypeNum allServices[] = {1, 3, 4, 5, 6, 11, 12, 13, 14, 15, 17, 19, 20, 23};
    static inline ServiceTypeNum redundantServices[] = {1, 3, 4, 5, 6, 11, 12, 13, 14, 15, 17, 19, 20, 23, 1, 3};
    static inline MessageTypeNum messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                                HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};
    static inline MessageTypeNum messages2[] = {EventReportService::MessageType::InformativeEventReport,
                                EventReportService::MessageType::DisabledListEventReport};


	void validReportTypes(Message& request);

    void duplicateReportTypes(Message& request);

    void validInvalidReportTypes(Message& request);

    void validAllReportsOfService(Message& request);

    void validInvalidAllReportsOfService(Message& request);
    
    void validAllReportsOfApp(Message& request);
    
    void validInvalidAllReportsOfApp(Message& request);
}