#ifndef ECSS_SERVICES_ENVIRONMENTALTESTINGSERVICE_HPP
#define ECSS_SERVICES_ENVIRONMENTALTESTINGSERVICE_HPP

#include "CAN/ApplicationLayer.hpp"
#include "Service.hpp"

/**
 * This is a service used during the Environmental Testing Campaign of the OBC/ADCS Board.
 * The primary function of this service is to receive custom TCs that execute functions to test various components
 * of our design.
 */
class EnvironmentalTestingService : public Service {
public:
    inline static const uint8_t ServiceType = 129;

    enum MessageType : uint8_t {
        BusSwitch = 1,
    };

    EnvironmentalTestingService() {
        serviceType = ServiceType;
    }

    void executeBusSwitch(Message message) {
        auto newBus = static_cast<CAN::Driver::ActiveBus>(message.data[0]);
        CAN::Application::sendBusSwitchoverMessage(newBus);
    }
};

#endif
