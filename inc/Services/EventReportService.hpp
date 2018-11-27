//
// Created by athanasios on 27/11/2018.
//

#ifndef ECSS_SERVICES_EVENTREPORTSERVICE_HPP
#define ECSS_SERVICES_EVENTREPORTSERVICE_HPP

#include "Service.hpp"
/**
 * Implementation of ST[05] event reporting service
 * @todo add enum event definition id (and maybe some appending?)
 */
#define CSS_EVENTS_MAX_COUNT 16
#define ECSS_EVENTS_BITS 16
class EventReportService: public Service {
public:
	EventReportService(){
		serviceType = 5;
	}
	/**
	 * TM[5,1] informative event report
	 * Send report to inform the respective recipients about an event
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void informativeEventReport(uint16_t eventID, const uint8_t *data, uint8_t length);

	/**
	 * TM[5,2] low severiity anomaly report
	 * Send report when there is an anomaly event of low severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void lowSeverityAnomalyReport(uint16_t eventID, const uint8_t *data, uint8_t length);

	/**
	 * TM[5,3] medium severity anomaly report
	 * Send report when there is an anomaly event of medium severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void mediumSeverityAnomalyReport(uint16_t eventID, const uint8_t *data, uint8_t length);

	/**
	 * TM[5,4] high severity anomaly report
	 * Send report when there is an anomaly event of hgih severity to the respective recipients
	 *
	 * Note: The parameters are defined by the standard, but the event definition id is missing!
	 *
	 * @param eventID event definition ID
	 * @param data the data of the report
	 * @param length the length of the data
	 */
	void highSeverityAnomalyReport(uint16_t eventID, const uint8_t *data, uint8_t length);

};
#endif //ECSS_SERVICES_EVENTREPORTSERVICE_HPP
