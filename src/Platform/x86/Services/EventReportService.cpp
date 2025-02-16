#include "Services/EventReportService.hpp"

void EventReportService::initializeEventDefinitionSeverityArray() {
    eventDefinitionSeverityArray.at(Event::UnknownEvent - 1) = EventReportSeverity::Informative;
    eventDefinitionSeverityArray.at(Event::WWDGReset - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::AssertionFail - 1) = EventReportSeverity::Medium;
    eventDefinitionSeverityArray.at(Event::MCUStart - 1) = EventReportSeverity::High;
    eventDefinitionSeverityArray.at(Event::FailedStartOfExecution - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::UnexpectedValuePMON - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::BelowLowLimitPMON - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::AboveHighLimitPMON - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::BelowLowThresholdPMON - 1) = EventReportSeverity::Low;
    eventDefinitionSeverityArray.at(Event::AboveHighThresholdPMON - 1) = EventReportSeverity::Low;
}