/**
 * @file memory_manager.hpp
 *
 * A collection of memory manager class and related functionality.
 */

#pragma once

#include <array>
#include <limits>

#include "error.hpp"

// #@@range_begin(frame_id)
namespace {
  constexpr unsigned long long operator""_KiB(unsigned long long kib) {
    return kib * 1024;
  }

  constexpr unsigned long long operator""_MiB(unsigned long long mib) {
    return mib * 1024_KiB;
  }

  constexpr unsigned long long operator""_GiB(unsigned long long gib) {
    return gib * 1024_MiB;
  }
}

/** @brief Size of one physical memory frame in bytes */
static const auto kBytesPerFrame{4_KiB};

class FrameID {
 public:
  explicit FrameID(size_t id) : id_{id} {}
  size_t ID() const { return id_; }
  void* Frame() const { return reinterpret_cast<void*>(id_ * kBytesPerFrame); }

 private:
  size_t id_;
};

static const FrameID kNullFrame{std::numeric_limits<size_t>::max()};
// #@@range_end(frame_id)

/** @brief Class that manages memory in frame units using a bitmap array.
 *
 * One bit corresponds to one frame; free frames are managed by the bitmap.
 * Each bit of the alloc_map array corresponds to a frame; 0 means free, 1 means in use.
 * The physical address for the m-th bit of alloc_map[n] is given by:
 *   kFrameBytes * (n * kBitsPerMapLine + m)
 */
// #@@range_begin(bitmap_memory_manager)
class BitmapMemoryManager {
 public:
  /** @brief Maximum physical memory size (in bytes) that this memory manager can handle */
  static const auto kMaxPhysicalMemoryBytes{128_GiB};
  /** @brief Number of frames required to cover physical memory up to kMaxPhysicalMemoryBytes */
  static const auto kFrameCount{kMaxPhysicalMemoryBytes / kBytesPerFrame};

  /** @brief Element type of the bitmap array */
  using MapLineType = unsigned long;
  /** @brief Number of bits in one element of the bitmap array == number of frames */
  static const size_t kBitsPerMapLine{8 * sizeof(MapLineType)};

  /** @brief Initializes the instance. */
  BitmapMemoryManager();

  /** @brief Allocates the requested number of frames and returns the leading frame ID */
  WithError<FrameID> Allocate(size_t num_frames);
  Error Free(FrameID start_frame, size_t num_frames);
  void MarkAllocated(FrameID start_frame, size_t num_frames);

  /** @brief Sets the memory range managed by this memory manager.
   * After this call, memory allocation via Allocate will only be performed within the set range.
   *
   * @param range_begin  Start of the memory range
   * @param range_end    End of the memory range (the frame after the last frame).
   */
  void SetMemoryRange(FrameID range_begin, FrameID range_end);

 private:
  std::array<MapLineType, kFrameCount / kBitsPerMapLine> alloc_map_;
  /** @brief Start of the memory range managed by this memory manager. */
  FrameID range_begin_;
  /** @brief End of the memory range managed by this memory manager (the frame after the last frame). */
  FrameID range_end_;

  bool GetBit(FrameID frame) const;
  void SetBit(FrameID frame, bool allocated);
};
// #@@range_end(bitmap_memory_manager)
