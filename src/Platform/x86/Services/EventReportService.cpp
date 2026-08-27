#include "Services/EventReportService.hpp"

void EventReportService::initializeEventDefinitionSeverityArray() {
    eventDefinitionSeverityArray.at(Event::UnknownEvent - 1) = EventReportSeverity::Informative;
    eventDefinitionSeverityArray.at(Event::WWDGReset - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::AssertionFail - 1) = EventReportSeverity::Medium;
    eventDefinitionSeverityArray.at(Event::MCUStart - 1) = EventReportSeverity::High;
    eventDefinitionSeverityArray.at(Event::FailedStartOfExecution - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::ParameterOutOfLimits - 1) = EventReportSeverity::Low;
}
