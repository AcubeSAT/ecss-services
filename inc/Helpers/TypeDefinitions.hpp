#pragma once
#include <cstdint>

using StepId = uint8_t;
using ParameterReportStructureId = uint8_t;
/**
 * @deprecated Ought to be changed to use our custom class.
 * Used for the various timestamps that exist in the code and previously made use of the uint32_t type.
 */
using TimeStamps = uint32_t;
using EventDefinitionId = uint16_t;
using ParameterId = uint16_t;
using ParameterSampleCount = uint16_t;
using SamplingInterval = uint16_t;
using StartAddress = uint64_t;
using MemoryId = uint8_t;
using ReadData = uint8_t;
using MemoryManagementChecksum = uint16_t;
/**
 * Length of the written, loaded or dumped data used in the Memory Management Service.
 */
using MemoryDataLength = uint16_t;
/**
 * Parameter Monitoring Identifier as used in the On-board Monitoring Service.
 */
using PMONId = uint16_t;
using LargeMessageTransactionId = uint16_t;
using PartSequenceNum = uint16_t;
using PacketStoreSize = uint16_t;
using PacketStoreType = uint8_t;
/**
 * Virtual channel used in the Storage and Retrieval Service.
 */
using VirtualChannel = uint8_t;
using NumOfPacketStores = uint16_t;
using ApplicationProcessId = uint16_t;
/**
 * The types used for the numerical representation of service and message types.
 */
using ServiceTypeNum = uint8_t;
using MessageTypeNum = uint8_t;

using SourceId = uint16_t;
using SequenceCount = uint16_t;
/**
 * Filling percentages of the packet stores, either total or from the open retrieval start time tag.
 */
using PercentageFilled = uint16_t;
/**
 * The time interval at which the parameters are collected, expressed as units of the minimum sampling interval as per 6.3.3.2.c.5 #NOTE-2.
 */
using CollectionInterval = uint32_t;
using ErrorCode = uint16_t;
/**
 * The types used for the three Check Types and their parameters in OnBoardMonitoringService.
 */
 using LowLimit = double;
 using BelowLowLimitEvent = uint16_t;
 using HighLimit = double;
 using AboveHighLimitEvent = uint16_t;
 using ExpectedValue = double;
 using Mask = uint64_t;
 using UnexpectedValueEvent = uint16_t;
 using NumberOfConsecutiveDeltaChecks = uint16_t;
 using LowDeltaThreshold = double;
 using BelowLowThresholdEvent = uint16_t;
 using HighDeltaThreshold = double;
 using AboveHighThresholdEvent = uint16_t;