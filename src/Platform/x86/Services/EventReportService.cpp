#include "EventReportService.hpp"

void EventReportService::initializeEventDefinitionSeverityMap() {
    eventDefinitionSeverityMap.insert({Event::UnknownEvent, EventReportSeverity::Informative});
    eventDefinitionSeverityMap.insert({Event::WWDGReset, EventReportSeverity::Low});
    eventDefinitionSeverityMap.insert({Event::AssertionFail, EventReportSeverity::Medium});
    eventDefinitionSeverityMap.insert({Event::MCUStart, EventReportSeverity::High});
    eventDefinitionSeverityMap.insert({Event::FailedStartOfExecution, EventReportSeverity::Low});
    eventDefinitionSeverityMap.insert({Event::UnexpectedValuePMON, EventReportSeverity::Low});
    eventDefinitionSeverityMap.insert({Event::BelowLowLimitPMON, EventReportSeverity::Low});
    eventDefinitionSeverityMap.insert({Event::AboveHighLimitPMON, EventReportSeverity::Low});
    eventDefinitionSeverityMap.insert({Event::BelowLowThresholdPMON, EventReportSeverity::Low});
    eventDefinitionSeverityMap.insert({Event::AboveHighThresholdPMON, EventReportSeverity::Low});
}