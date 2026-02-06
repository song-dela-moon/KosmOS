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

  // #@@range_begin(class_code)
  /** @brief Class code of a PCI device */
  struct ClassCode {
    uint8_t base, sub, interface;

    /** @brief Returns true if the base class is equal */
    bool Match(uint8_t b) { return b == base; }
    /** @brief Returns true if the base and subclass are equal */
    bool Match(uint8_t b, uint8_t s) { return Match(b) && s == sub; }
    /** @brief Returns true if the base, sub, and interface classes are all equal */
    bool Match(uint8_t b, uint8_t s, uint8_t i) {
      return Match(b, s) && i == interface;
    }
  };

  /** @brief Stores basic data for operating a PCI device
   *
   * Bus number, device number, and function number are required to identify a device.
   * Other information is added only for convenience.
   * */
  struct Device {
    uint8_t bus, device, function, header_type;
    ClassCode class_code;
  };
  // #@@range_end(class_code)

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
  /** @brief Reads the class code register (common to all header types) */
  ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

  inline uint16_t ReadVendorId(const Device& dev) {
    return ReadVendorId(dev.bus, dev.device, dev.function);
  }

  /** @brief Reads the specified 32-bit register of a PCI device */
  uint32_t ReadConfReg(const Device& dev, uint8_t reg_addr);

  void WriteConfReg(const Device& dev, uint8_t reg_addr, uint32_t value);

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
  constexpr uint8_t CalcBarAddress(unsigned int bar_index) {
    return 0x10 + 4 * bar_index;
  }

  WithError<uint64_t> ReadBar(Device& device, unsigned int bar_index);
}
