#ifndef ECSS_SERVICES_TYPEDEFDEFINITIONS_HPP
#define ECSS_SERVICES_TYPEDEFDEFINITIONS_HPP

typedef uint8_t StructureIdSize; //also in housekeepingstructure.hpp

typedef uint16_t EventIdSize;

typedef uint8_t StepIdSize; //also in errorhandler.hpp

typedef uint16_t ParameterIdSize; //also in parameter service
typedef uint16_t SamplesSize;
typedef uint16_t IntervalSize;

typedef uint64_t StartAddressSize;
typedef uint8_t MemoryIdSize;
typedef uint8_t ReadDataSize;
typedef uint16_t IterationSize;
typedef uint16_t ChecksumSize;
typedef uint16_t LengthSize;

typedef uint16_t IterationTimeSize;

typedef uint16_t NumOfPMONDefSize;
typedef uint16_t PMONIdSize;//also in pmonbase.hpp

typedef uint16_t LargeMessageTransactionIdSize;
typedef uint16_t PartSequenceNumSize;
typedef uint16_t DataSizeSize; //Both in "Message.hpp and LPTS.hpp

---------

typedef uint8_t ApplicationIdSize;
typedef uint8_t ServiceTypeSize;//numOfMessages, numOfServices

---------
typedef uint16_t NumOfPacketStoresSize;
typedef auto PacketStoreIdSize;
typedef uint32_t TimeSize; //also in packetstore.hpp and housekeepingservice
--------
--------
//make a question regarding application id and why it is different in size for realtimeforwarding and eventactionservice
typedef uint16_t EventDefinitionIdSize;
typedef uint16_t ApplicationIdSize2; //also in event action service

typedef uint8_t MessageTypeSize;
typedef uint8_t ServiceTypeSize;

typedef uint8_t SourceIdSize;
//notetoself: last left off at whether there could be a different type for i in LargePacketTransferService.cpp line 62
#endif //ECSS_SERVICES_TYPEDEFDEFINITIONS_HPP
