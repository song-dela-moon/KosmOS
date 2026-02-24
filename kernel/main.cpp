// #@@range_begin(includes)
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>

#include <deque>
#include <limits>
#include <numeric>
#include <vector>

#include "frame_buffer_config.hpp"
#include "memory_map.hpp"
#include "graphics.hpp"
#include "mouse.hpp"
#include "font.hpp"
#include "console.hpp"
#include "pci.hpp"
#include "logger.hpp"
#include "usb/xhci/xhci.hpp"
#include "interrupt.hpp"
#include "asmfunc.h"
#include "segment.hpp"
#include "paging.hpp"
#include "memory_manager.hpp"
#include "window.hpp"
#include "layer.hpp"
#include "message.hpp"
#include "timer.hpp"
#include "acpi.hpp"
#include "keyboard.hpp"
// #@@range_end(includes)

int printk(const char* format, ...) {
  va_list ap;
  int result;
  char s[1024];

  va_start(ap, format);
  result = vsprintf(s, format, ap);
  va_end(ap);

  console->PutString(s);
  return result;
}

std::shared_ptr<Window> main_window;
unsigned int main_window_layer_id;
void InitializeMainWindow() {
  main_window = std::make_shared<Window>(
      160, 52, screen_config.pixel_format);
  DrawWindow(*main_window->Writer(), "Hello Window");

  main_window_layer_id = layer_manager->NewLayer()
    .SetWindow(main_window)
    .SetDraggable(true)
    .Move({300, 100})
    .ID();

  layer_manager->UpDown(main_window_layer_id, 2);
}

std::deque<Message>* main_queue;

alignas(16) uint8_t kernel_main_stack[1024 * 1024];


extern "C" void KernelMainNewStack(
    const FrameBufferConfig& frame_buffer_config_ref,
    const MemoryMap& memory_map_ref,
    const acpi::RSDP& acpi_table) {

  MemoryMap memory_map{memory_map_ref};

  InitializeGraphics(frame_buffer_config_ref);
  InitializeConsole();

  SetLogLevel(kWarn);
    printk("Welcome to KosmOS!\n");
    printk(" /$$   /$$                                    /$$$$$$   /$$$$$$ \n");
    printk("| $$  /$$/                                   /$$__  $$ /$$__  $$\n");
    printk("| $$ /$$/   /$$$$$$   /$$$$$$$ /$$$$$$/$$$$ | $$  : $$| $$  :__/\n");
    printk("| $$$$$/   /$$__  $$ /$$_____/| $$_  $$_  $$| $$  | $$|  $$$$$$ \n");
    printk("| $$  $$  | $$  : $$|  $$$$$$ | $$ : $$ : $$| $$  | $$ :____  $$\n");
    printk("| $$:  $$ | $$  | $$ :____  $$| $$ | $$ | $$| $$  | $$ /$$  : $$\n");
    printk("| $$ :  $$|  $$$$$$/ /$$$$$$$/| $$ | $$ | $$|  $$$$$$/|  $$$$$$/\n");
    printk("|__/  :__/ :______/ |_______/ |__/ |__/ |__/ :______/  :______/ \n");
  SetLogLevel(kWarn);

  InitializeSegmentation();
  InitializePaging();
  InitializeMemoryManager(memory_map);
  ::main_queue = new std::deque<Message>(32);
  InitializeInterrupt(main_queue);

  InitializePCI();
  usb::xhci::Initialize();
  
  InitializeLayer();
  InitializeMainWindow();
  InitializeMouse();
  layer_manager->Draw({{0, 0}, ScreenSize()});


  acpi::Initialize(acpi_table);
  InitializeLAPICTimer(*main_queue);

  // #@@range_begin(call_initkb)
  InitializeKeyboard(*main_queue);
  // #@@range_end(call_initkb)

  char str[128];

  // #@@range_begin(event_loop)
  while (true) {
    __asm__("cli");
    const auto tick = timer_manager->CurrentTick();
    __asm__("sti");

    sprintf(str, "%010lu", tick);
    FillRectangle(*main_window->Writer(), {24, 28}, {8 * 10, 16}, {0xc6, 0xc6, 0xc6});
    WriteString(*main_window->Writer(), {24, 28}, str, {0, 0, 0});
    layer_manager->Draw(main_window_layer_id);

    __asm__("cli");
    if (main_queue->size() == 0) {
      __asm__("sti\n\thlt");
      continue;
    }

    Message msg = main_queue->front();
    main_queue->pop_front();
    __asm__("sti");

    // #@@range_begin(process_event)
    switch (msg.type) {
    case Message::kInterruptXHCI:
      usb::xhci::ProcessEvents();
      break;
    case Message::kTimerTimeout:
      break;
    // #@@range_begin(event_handling)
    case Message::kKeyPush:
      if (msg.arg.keyboard.ascii != 0) {
        printk("%c", msg.arg.keyboard.ascii);
      }
      break;
    // #@@range_end(event_handling)
    default:
      Log(kError, "Unknown message type: %d\n", msg.type);
    }
  }
  // #@@range_end(event_loop)
}

extern "C" void __cxa_pure_virtual() {
  while (1) __asm__("hlt");
}
