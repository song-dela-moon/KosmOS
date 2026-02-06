/**
 * @file usb/memory.hpp
 *
 * Dynamic memory management for USB driver
 */

#pragma once

#include <cstddef>

namespace usb {
  /** @brief Maximum memory pool size for dynamic allocation (bytes) */
  static const size_t kMemoryPoolSize = 4096 * 32;

  /** @brief Allocates memory of the specified size and returns a pointer to it.
   *
   * Allocates memory aligned to the specified alignment.
   * If size <= boundary, guarantees the allocated region does not cross boundary.
   * Typically use 4096 for boundary to avoid crossing page boundaries.
   *
   * @param size        Size of memory region to allocate (bytes)
   * @param alignment   Alignment constraint. 0 means no constraint.
   * @param boundary    Allocated region must not cross this boundary. 0 means no constraint.
   * @return nullptr if allocation failed
   */
  void* AllocMem(size_t size, unsigned int alignment, unsigned int boundary);

  template <class T>
  T* AllocArray(size_t num_obj, unsigned int alignment, unsigned int boundary) {
    return reinterpret_cast<T*>(
        AllocMem(sizeof(T) * num_obj, alignment, boundary));
  }

  /** @brief Frees the specified memory region. Actual deallocation is not guaranteed. */
  void FreeMem(void* p);

  /** @brief Memory allocator for standard containers */
  template <class T, unsigned int Alignment = 64, unsigned int Boundary = 4096>
  class Allocator {
   public:
    using size_type = size_t;
    using pointer = T*;
    using value_type = T;

    Allocator() noexcept = default;
    Allocator(const Allocator&) noexcept = default;
    template <class U> Allocator(const Allocator<U>&) noexcept {}
    ~Allocator() noexcept = default;
    Allocator& operator=(const Allocator&) = default;

    pointer allocate(size_type n) {
      return AllocArray<T>(n, Alignment, Boundary);
    }

    void deallocate(pointer p, size_type num) {
      FreeMem(p);
    }
  };
}
