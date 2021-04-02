#ifndef ECSS_SERVICES_LARGEPACKETTRANSFERSERVICE_HPP
#define ECSS_SERVICES_LARGEPACKETTRANSFERSERVICE_HPP

#include "Service.hpp"
#include <etl/String.hpp>

/**
 * Implementation of the ST[13] large packet transfer service
 * The goal of this service is to help in splitting data packages that exceed the standard's
 * maximum data size
 *
 * Note: More information can be found in the standards' manual, in p. 526-528 and in p. 229-236
 *
 * @ingroup Services
 */

class LargePacketTransferService : public Service {
public:

	/*
* ST[13] Large-Packet-Transfer Service and Sub-Service Macros, for readability purpose
*/

	inline static const uint8_t ServiceType = 13;


	enum MessageType : uint8_t {
		FirstDownlinkPartReport = 1,
		InternalDownlinkPartReport = 2,
		LastDownlinkPartReport = 3,
	};

	/**
	 * Default constructor since only functions will be used.
	 */
	LargePacketTransferService() {
		serviceType = 13;
	}

	/**
	 * Function that handles the first part of the download report
	 * @param largeMessageTransactionIdentifier The identifier of the large packet
	 * @param partSequenceNumber The identifier of the part of the large packet
	 * @param string The data contained in this part of the large packet
	 */
	void firstDownlinkPartReport(uint16_t largeMessageTransactionIdentifier, uint16_t partSequenceNumber,
	                             const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string);

	/**
	 * Function that handles the n-2 parts of tbe n-part download report
	 * @param largeMessageTransactionIdentifier The identifier of the large packet
	 * @param partSequenceNumber The identifier of the part of the large packet
	 * @param string The data contained in this part of the large packet
	 */
	void intermediateDownlinkPartReport(uint16_t largeMessageTransactionIdentifier, uint16_t partSequenceNumber,
	                                    const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string);

	/**
	 * Function that handles the last part of the download report
	 * @param largeMessageTransactionIdentifier The identifier of the large packet
	 * @param partSequenceNumber The identifier of the part of the large packet
	 * @param string The data contained in this part of the large packet
	 */
	void lastDownlinkPartReport(uint16_t largeMessageTransactionIdentifier, uint16_t partSequenceNumber,
	                            const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string);

	// The three uplink functions should handle a TC request to "upload" data. Since there is not
	// a composeECSS function ready, I just return the given string.
	// @TODO: Modify these functions properly
	/**
	 * Function that handles the first part of the uplink request
	 * @param string This will change when these function will be modified
	 */
	String<ECSS_MAX_FIXED_OCTET_STRING_SIZE> firstUplinkPart(const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string);

	/**
	 * Function that handles the n-2 parts of tbe n-part uplink request
	 * @param string This will change when these function will be modified
	 */
	String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>
	intermediateUplinkPart(const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string);

	/**
	 * Function that handles the last part of the uplink request
	 * @param string This will change when these function will be modified
	 */
	String<ECSS_MAX_FIXED_OCTET_STRING_SIZE> lastUplinkPart(const String<ECSS_MAX_FIXED_OCTET_STRING_SIZE>& string);

	/**
	 * Function that splits large messages
	 * @param message that is exceeds the standards and has to be split down
	 * @param largeMessageTransactionIdentifier that is a value we assign to this splitting of the large message
	 */
	void split(Message& message, uint16_t largeMessageTransactionIdentifier);
};

#endif // ECSS_SERVICES_LARGEPACKETTRANSFERSERVICE_HPP
