#ifndef ECSS_SERVICES_LARGEPACKETTRANSFERSERVICE_HPP
#define ECSS_SERVICES_LARGEPACKETTRANSFERSERVICE_HPP

#include <etl/String.hpp>
#include "Service.hpp"

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
	inline static constexpr ServiceTypeNum ServiceType = 13;


	enum MessageType : uint8_t {
		FirstDownlinkPartReport = 1,
		InternalDownlinkPartReport = 2,
		LastDownlinkPartReport = 3,
	};

	/**
	 * Default constructor since only functions will be used.
	 */
	LargePacketTransferService() {
		serviceType = ServiceType;
	}

	/**
	 * TM[13,1] Function that handles the first part of the download report
	 * @param largeMessageTransactionIdentifier The identifier of the large packet
	 * @param partSequenceNumber The identifier of the part of the large packet
	 * @param string The data contained in this part of the large packet
	 */
	void firstDownlinkPartReport(LargeMessageTransactionId largeMessageTransactionIdentifier, PartSequenceNum partSequenceNumber,
	                             const String<ECSSMaxFixedOctetStringSize>& string);

	/**
	 * TM[13,2] Function that handles the n-2 parts of tbe n-part download report
	 * @param largeMessageTransactionIdentifier The identifier of the large packet
	 * @param partSequenceNumber The identifier of the part of the large packet
	 * @param string The data contained in this part of the large packet
	 */
	void intermediateDownlinkPartReport(LargeMessageTransactionId largeMessageTransactionIdentifier, PartSequenceNum partSequenceNumber,
	                                    const String<ECSSMaxFixedOctetStringSize>& string);

	/**
	 * TM[13,3] Function that handles the last part of the download report
	 * @param largeMessageTransactionIdentifier The identifier of the large packet
	 * @param partSequenceNumber The identifier of the part of the large packet
	 * @param string The data contained in this part of the large packet
	 */
	void lastDownlinkPartReport(LargeMessageTransactionId largeMessageTransactionIdentifier, PartSequenceNum partSequenceNumber,
	                            const String<ECSSMaxFixedOctetStringSize>& string);

	// The three uplink functions should handle a TC request to "upload" data. Since there is not
	// a composeECSS function ready, I just return the given string.
	// @TODO (#220): Modify these functions properly
	/**
	 * TC[13,9] Function that handles the first part of the uplink request
	 * @param string This will change when these function will be modified
	 */
	static String<ECSSMaxFixedOctetStringSize> firstUplinkPart(const String<ECSSMaxFixedOctetStringSize>& string);

	/**
	 * TC[13,10] Function that handles the n-2 parts of the n-part uplink request
	 * @param string This will change when these function will be modified
	 */
	static String<ECSSMaxFixedOctetStringSize>
	intermediateUplinkPart(const String<ECSSMaxFixedOctetStringSize>& string);

	/**
	 * TC[13,11] Function that handles the last part of the uplink request
	 * @param string This will change when these function will be modified
	 */
	static String<ECSSMaxFixedOctetStringSize> lastUplinkPart(const String<ECSSMaxFixedOctetStringSize>& string);

	/**
	 * Function that splits large messages
	 * @param message that is exceeds the standards and has to be split down
	 * @param largeMessageTransactionIdentifier that is a value we assign to this splitting of the large message
	 */
	void split(const Message& message, LargeMessageTransactionId largeMessageTransactionIdentifier);
};

#endif // ECSS_SERVICES_LARGEPACKETTRANSFERSERVICE_HPP
