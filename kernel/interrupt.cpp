/**
 * @file interrupt.cpp
 *
 * A collection of programs for handling interrupts.
 */

#include "interrupt.hpp"

#include "asmfunc.h"
#include "segment.hpp"
#include "timer.hpp"
#include "task.hpp"

// #@@range_begin(idt_array)
std::array<InterruptDescriptor, 256> idt;
// #@@range_end(idt_array)

// #@@range_begin(set_idt_entry)
void SetIDTEntry(InterruptDescriptor& desc,
                 InterruptDescriptorAttribute attr,
                 uint64_t offset,
                 uint16_t segment_selector) {
  desc.attr = attr;
  desc.offset_low = offset & 0xffffu;
  desc.offset_middle = (offset >> 16) & 0xffffu;
  desc.offset_high = offset >> 32;
  desc.segment_selector = segment_selector;
}
// #@@range_end(set_idt_entry)

// #@@range_begin(notify_eoi)
void NotifyEndOfInterrupt() {
  volatile auto end_of_interrupt = reinterpret_cast<uint32_t*>(0xfee000b0);
  *end_of_interrupt = 0;
}
// #@@range_end(notify_eoi)

// #@@range_begin(int_handler)
namespace {

  // #@@range_begin(inthandler_xhci)
  __attribute__((interrupt))
  void IntHandlerXHCI(InterruptFrame* frame) {
    task_manager->SendMessage(1, Message{Message::kInterruptXHCI});
    NotifyEndOfInterrupt();
  }
  // #@@range_end(inthandler_xhci)

  __attribute__((interrupt))
  void IntHandlerLAPICTimer(InterruptFrame* frame) {
    LAPICTimerOnInterrupt();
  }
}
// #@@range_end(int_handler)

// #@@range_begin(register_handler)
// #@@range_begin(init_int)
void InitializeInterrupt() {
// #@@range_end(init_int)

  SetIDTEntry(idt[InterruptVector::kXHCI],
              MakeIDTAttr(DescriptorType::kInterruptGate, 0),
              reinterpret_cast<uint64_t>(IntHandlerXHCI),
              kKernelCS);
  SetIDTEntry(idt[InterruptVector::kLAPICTimer],
              MakeIDTAttr(DescriptorType::kInterruptGate, 0),
              reinterpret_cast<uint64_t>(IntHandlerLAPICTimer),
              kKernelCS);
  LoadIDT(sizeof(idt) - 1, reinterpret_cast<uintptr_t>(&idt[0]));
}
// #@@range_end(register_handler)
