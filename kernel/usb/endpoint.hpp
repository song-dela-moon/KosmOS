/**
 * @file usb/endpoint.hpp
 *
 * Endpoint configuration functionality.
 */

#pragma once

#include "error.hpp"

namespace usb {
  enum class EndpointType {
    kControl = 0,
    kIsochronous = 1,
    kBulk = 2,
    kInterrupt = 3,
  };

  class EndpointID {
   public:
    constexpr EndpointID() : addr_{0} {}
    constexpr EndpointID(const EndpointID& ep_id) : addr_{ep_id.addr_} {}
    explicit constexpr EndpointID(int addr) : addr_{addr} {}

    /** Constructs an ID from endpoint number and I/O direction.
     *
     * ep_num is an integer in range 0..15.
     * dir_in must always be true for Control endpoint.
     */
    constexpr EndpointID(int ep_num, bool dir_in) : addr_{ep_num << 1 | dir_in} {}

    EndpointID& operator =(const EndpointID& rhs) {
      addr_ = rhs.addr_;
      return *this;
    }

    /** Endpoint address (0..31) */
    int Address() const { return addr_; }

    /** Endpoint number (0..15) */
    int Number() const { return addr_ >> 1; }

    /** I/O direction. true for Control endpoint */
    bool IsIn() const { return addr_ & 1; }

   private:
    int addr_;
  };

  constexpr EndpointID kDefaultControlPipeID{0, true};

  struct EndpointConfig {
    /** Endpoint ID */
    EndpointID ep_id;

    /** This endpoint type */
    EndpointType ep_type;

    /** Max packet size for this endpoint (bytes) */
    int max_packet_size;

    /** Control period for this endpoint (125*2^(interval-1) microseconds) */
    int interval;
  };
}
