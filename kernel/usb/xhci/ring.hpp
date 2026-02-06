/**
 * @file usb/xhci/ring.hpp
 *
 * Event Ring, Command Ring, Transfer Ring classes and related functionality.
 */

#pragma once

#include <cstdint>
#include <vector>

#include "error.hpp"
#include "usb/memory.hpp"
#include "usb/xhci/registers.hpp"
#include "usb/xhci/trb.hpp"

namespace usb::xhci {
  /** @brief Class representing Command/Transfer Ring. */
  class Ring {
   public:
    Ring() = default;
    Ring(const Ring&) = delete;
    ~Ring();
    Ring& operator=(const Ring&) = delete;

    /** @brief Allocates ring memory and initializes members. */
    Error Initialize(size_t buf_size);

    /** @brief Appends a TRB to the ring end with cycle bit set.
     *
     * @return Pointer to the appended (ring) TRB.
     */
    template <typename TRBType>
    TRB* Push(const TRBType& trb) {
      return Push(trb.data);
    }

    TRB* Buffer() const { return buf_; }

   private:
    TRB* buf_ = nullptr;
    size_t buf_size_ = 0;

    /** @brief Bit representing producer cycle state */
    bool cycle_bit_;
    /** @brief Next write position on the ring */
    size_t write_index_;

    /** @brief Writes a TRB to the ring end with cycle bit set.
     *
     * write_index_ is not modified.
     */
    void CopyToLast(const std::array<uint32_t, 4>& data);

    /** @brief Appends a TRB to the ring end with cycle bit set.
     *
     * Increments write_index_. When write_index_ reaches the ring end,
     * places LinkTRB appropriately, resets write_index_ to 0, and toggles cycle bit.
     *
     * @return Pointer to the appended (ring) TRB.
     */
    TRB* Push(const std::array<uint32_t, 4>& data);
  };

  union EventRingSegmentTableEntry {
    std::array<uint32_t, 4> data;
    struct {
      uint64_t ring_segment_base_address;  // 64-byte alignment

      uint32_t ring_segment_size : 16;
      uint32_t : 16;

      uint32_t : 32;
    } __attribute__((packed)) bits;
  };

  class EventRing {
   public:
    Error Initialize(size_t buf_size, InterrupterRegisterSet* interrupter);

    TRB* ReadDequeuePointer() const {
      return reinterpret_cast<TRB*>(interrupter_->ERDP.Read().Pointer());
    }

    void WriteDequeuePointer(TRB* p);

    bool HasFront() const {
      return Front()->bits.cycle_bit == cycle_bit_;
    }

    TRB* Front() const {
      return ReadDequeuePointer();
    }

    void Pop();

   private:
    TRB* buf_;
    size_t buf_size_;

    bool cycle_bit_;
    EventRingSegmentTableEntry* erst_;
    InterrupterRegisterSet* interrupter_;
  };
}
