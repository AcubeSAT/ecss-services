#ifndef ECSS_SERVICES_MEMORYADDRESSLIMITS_STM32F7_HPP
#define ECSS_SERVICES_MEMORYADDRESSLIMITS_STM32F7_HPP

// Memory limits definitions
inline const uint32_t DTCMRAMLowerLim = 0x20000000UL;
inline const uint32_t DTCMRAMUpperLim = 0x20020000UL;
inline const uint32_t ITCMRAMLowerLim = 0x00000000UL;
inline const uint32_t ITCMRAMUpperLim = 0x00010000UL;
inline const uint32_t RAMD1LowerLim = 0x24000000UL;
inline const uint32_t RAMD1UpperLim = 0x24080000UL;
inline const uint32_t RAMD2LowerLim = 0x30000000UL;
inline const uint32_t RAMD2UpperLim = 0x30048000UL;
inline const uint32_t RAMD3LowerLim = 0x38000000UL;
inline const uint32_t RAMD3UpperLim = 0x38010000UL;
inline const uint32_t FlashLowerLim = 0x08000000UL;
inline const uint32_t FlashUpperLim = 0x08200000UL;

#endif // ECSS_SERVICES_MEMORYADDRESSLIMITS_STM32F7_HPP
