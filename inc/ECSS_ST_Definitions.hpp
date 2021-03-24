//
// Created by user on 23/3/21.
//

#ifndef ECSS_SERVICES_ECSS_ST_DEFINITIONS_HPP
#define ECSS_SERVICES_ECSS_ST_DEFINITIONS_HPP

/*
 * List with Service and Subservicce types with their corresponding values.
 * ex ST[19] refers to Event-Action
 * The goal of this document is to make the services code more readable
 */

/*
 * ST[19] Event-Action-Service
 */

#define EventAction 19

//Message types

#define AddEventAction 1
#define DeleteEventAction 2
#define DeleteAllEventAction 3
#define EnableEventAction 4
#define DisableEventAction 5
#define ReportStatusOfEachEventAction 6
#define EventActionStatusReport 7
#define EnableEventActionFunction 8
#define DisableEventActionFunction 9

/*
 * ST[5] Event-Report-Service
 */

#define EventReport 5

//Message types

#define InformativeEventReport 1
#define LowSeverityAnomalyReport 2
#define MediumSeverityAnomalyReport 3
#define HighSeverityAnomalyReport 4
#define EnableReportGenerationOfEvents 5
#define DisableReportGenerationOfEvents 6
#define ReportListOfDisabledEvent 7
#define DisabledEventListReport 8

/*
 * ST[8] Function-Management
 */

#define FunctionManagement 8

//Message types

#define PerformFunction 1

/*
 * ST[13] Large-Packet-Transfer
 */

#define LargePacketTransfer 13

//Message types

#define FirstDownlinkPartReport 1
#define IntermediateDownlinkPartReport 2
#define LastDownlinkPartReport 3

/*
 * ST[6] Memory-Management
 */

#define MemoryManagement 6

//Message types

#define LoadRawMemoryDataAreas 2
#define DumpRawMemoryData 5
#define DumpedRawMemoryDataReport 6
#define CheckRawMemoryData 9
#define CheckedRawMemoryDataReport 10

/*
 * ST[20] Parameter-Management
 */

#define ParameterManagement 20

//Message types

#define ReportParameterValues 1
#define ParameterValueReport 2
#define SetParameterValues 3

/*
 * ST[1] Request-Verification
 */

#define RequestVerification 1

//Message types

#define SuccessfulAcceptanceReport 1
#define FailedAcceptanceReport 2
#define SuccessfulStartOfExecution 3
#define FailedStartOfExecution 4
#define SuccessfulProgressOfExecution 5
#define FailedProgressOfExecution 6
#define SuccessfulCompletionOfExecution 7
#define FailedCompletionOfExecution 8
#define FailedRoutingReport 10

/*
 * ST[17] Test
 */

#define Test 17

//Message types

#define AreYouAliveTest 1
#define AreYouALiveTestReport 2
#define OnBoardConnectionTest 3
#define OnBoardConnectionTestReport 4

/*
 * ST[11] Time-Based-Scheduling
 */

#define TimeBasedScheduling 11

//Message types

#define EnableTimeBasedScheduleExecutionFunction 1
#define DisableTimeBasedScheduleExecutionFunction 2
#define ResetTimeBasedSchedule 3
#define InsertActivities 4
#define DeleteActivitiesByIdentifier 5
#define TimeShiftActivitiesByIdentifier 7
#define DetailReportActivitiesByIdentifier 9
#define TimeBasedScheduleDetailReport 10
#define ActivitiesSummaryReportByIdentifier 12
#define TimeBasedScheduleSummaryReport 13
#define TimeShiftAllScheduledActivities 15
#define DetailReportAllScheduledActivities 16

/*
 * ST[9] Time-Management
 */

#define TimeManagement 9

//Message types

#define CDSTimeReport 3

#endif // ECSS_SERVICES_ECSS_ST_DEFINITIONS_HPP
