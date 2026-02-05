/**
 * @file pci.hpp
 *
 * A collection of programs for PCI bus control.
 */

#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci {
  // #@@range_begin(config_addr)
  /** @brief IO port address for CONFIG_ADDRESS register */
  const uint16_t kConfigAddress = 0x0cf8;
  /** @brief IO port address for CONFIG_DATA register */
  const uint16_t kConfigData = 0x0cfc;
  // #@@range_end(config_addr)

  /** @brief Writes a specified integer to CONFIG_ADDRESS */
  void WriteAddress(uint32_t address);
  /** @brief Writes a specified integer to CONFIG_DATA */
  void WriteData(uint32_t value);
  /** @brief Reads a 32-bit integer from CONFIG_DATA */
  uint32_t ReadData();

  /** @brief Reads the Vendor ID register (common to all header types) */
  uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);
  /** @brief Reads the Device ID register (common to all header types) */
  uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);
  /** @brief Reads the Header Type register (common to all header types) */
  uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);
  /** @brief Reads the Class Code register (common to all header types)
   *
   * The structure of the returned 32-bit integer is as follows:
   * - 31:24 : Base Class
   * - 23:16 : Subclass
   * - 15:8  : Interface
   * - 7:0   : Revision
   */
  uint32_t ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief Reads the Bus Numbers register (for Header Type 1)
   *
   * The structure of the returned 32-bit integer is as follows:
   * - 23:16 : Subordinate Bus Number
   * - 15:8  : Secondary Bus Number
   * - 7:0   : Revision Number
   */
  uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

  /** @brief Returns true if it is a single-function device. */
  bool IsSingleFunctionDevice(uint8_t header_type);

  /** @brief Stores basic data for operating PCI devices
   *
   * Bus, device, and function numbers are essential to identify the device.
   * Other information is added purely for convenience.
   * */
  struct Device {
    uint8_t bus, device, function, header_type;
  };

  // #@@range_begin(var_devices)
  /** @brief List of PCI devices discovered by ScanAllBus() */
  inline std::array<Device, 32> devices;
  /** @brief Number of valid elements in devices */
  inline int num_device;
  /** @brief Recursively explores all PCI devices and stores them in devices
   *
   * Scans PCI devices starting from Bus 0 and writes them into devices.
   * Sets the number of discovered devices to num_devices.
   */
  Error ScanAllBus();
  // #@@range_end(var_devices)
}
