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
  /** @brief Writes to the specified 32-bit register of the PCI device */
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

  /** @brief Common header for PCI capability registers */
  union CapabilityHeader {
    uint32_t data;
    struct {
      uint32_t cap_id : 8;
      uint32_t next_ptr : 8;
      uint32_t cap : 16;
    } __attribute__((packed)) bits;
  } __attribute__((packed));

  const uint8_t kCapabilityMSI = 0x05;
  const uint8_t kCapabilityMSIX = 0x11;

  /** @brief Reads the specified capability register of the specified PCI device
   *
   * @param dev  PCI device from which to read the capability
   * @param addr  Configuration space address of the capability register
   */
  CapabilityHeader ReadCapabilityHeader(const Device& dev, uint8_t addr);

  /** @brief MSI capability structure
   *
   * The MSI capability structure has many variants depending on 64-bit support etc.
   * This struct defines members to match the largest variant to support all variants.
   */
  struct MSICapability {
    union {
      uint32_t data;
      struct {
        uint32_t cap_id : 8;
        uint32_t next_ptr : 8;
        uint32_t msi_enable : 1;
        uint32_t multi_msg_capable : 3;
        uint32_t multi_msg_enable : 3;
        uint32_t addr_64_capable : 1;
        uint32_t per_vector_mask_capable : 1;
        uint32_t : 7;
      } __attribute__((packed)) bits;
    } __attribute__((packed)) header ;

    uint32_t msg_addr;
    uint32_t msg_upper_addr;
    uint32_t msg_data;
    uint32_t mask_bits;
    uint32_t pending_bits;
  } __attribute__((packed));

  /** @brief Configures MSI or MSI-X interrupt
   *
   * @param dev  Target PCI device to configure
   * @param msg_addr  Address to write message to when interrupt occurs
   * @param msg_data  Message value to write when interrupt occurs
   * @param num_vector_exponent  Number of vectors to allocate (specify n for 2^n)
   */
  Error ConfigureMSI(const Device& dev, uint32_t msg_addr, uint32_t msg_data,
                     unsigned int num_vector_exponent);

  enum class MSITriggerMode {
    kEdge = 0,
    kLevel = 1
  };

  enum class MSIDeliveryMode {
    kFixed          = 0b000,
    kLowestPriority = 0b001,
    kSMI            = 0b010,
    kNMI            = 0b100,
    kINIT           = 0b101,
    kExtINT         = 0b111,
  };

  Error ConfigureMSIFixedDestination(
      const Device& dev, uint8_t apic_id,
      MSITriggerMode trigger_mode, MSIDeliveryMode delivery_mode,
      uint8_t vector, unsigned int num_vector_exponent);
}
