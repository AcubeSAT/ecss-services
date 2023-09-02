#pragma once
#include <cstdint>

using StepId = uint8_t;

using ParameterReportStructureId = uint8_t;
/**
 * \deprecated Ought to be changed to use proper timestamps.
 */
using TimeInt = uint32_t;

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
using DataLength = uint16_t;

using PMONId = uint16_t;

using LargeMessageTransactionId = uint16_t;
using PartSequenceNum = uint16_t;

using NumOfPacketStores = uint16_t;

using ApplicationProcessId = uint16_t;

using ServiceTypeSize = uint8_t;
static_assert(sizeof(ServiceTypeSize) == 1);
using MessageTypeSize = uint8_t;
static_assert(sizeof(MessageTypeSize) == 1);

using SourceId = uint16_t;
