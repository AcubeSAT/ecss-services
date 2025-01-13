#pragma once
#include "Message.hpp"
#include "Services/HousekeepingService.hpp"
#include "Services/EventReportService.hpp"

namespace ForwardingAndPacketHelper {
    static inline ApplicationProcessId applications[] = {1};
    static inline ServiceTypeNum services[] = {3, 5};
    static inline ServiceTypeNum allServices[] = {1, 3, 4, 5, 6, 11, 13, 17, 19, 20};
    static inline ServiceTypeNum redundantServices[] = {1, 3, 4, 5, 6, 11, 13, 17, 19, 20, 1, 3};
    static inline MessageTypeNum messages1[] = {HousekeepingService::MessageType::HousekeepingPeriodicPropertiesReport,
                                HousekeepingService::MessageType::DisablePeriodicHousekeepingParametersReport};
    static inline MessageTypeNum messages2[] = {EventReportService::MessageType::InformativeEventReport,
                                EventReportService::MessageType::DisabledListEventReport};


    static void validReportTypes(Message& request);

    static void duplicateReportTypes(Message& request);

    static void validInvalidReportTypes(Message& request);

    static void validAllReportsOfService(Message& request);

    static void validInvalidAllReportsOfService(Message& request);
    
    static void validAllReportsOfApp(Message& request);
    
    static void validInvalidAllReportsOfApp(Message& request);
}