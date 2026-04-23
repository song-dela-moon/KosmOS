#include "timer.hpp"

#include "acpi.hpp"
#include "interrupt.hpp"
#include "task.hpp"

Timer::Timer(unsigned long timeout, int value)
    : timeout_{timeout}, value_{value} {
}

TimerManager::TimerManager() {
  timers_.push(Timer{std::numeric_limits<unsigned long>::max(), -1});
}

void TimerManager::AddTimer(const Timer& timer) {
  timers_.push(timer);
}

bool TimerManager::Tick() {
  ++tick_;

  bool timeout = false;
  while (timers_.top().Timeout() <= tick_) {
    if (timers_.top().Value() == kTaskTimerValue) {
      timeout = true;
      timers_.pop();
      timers_.push(Timer{tick_ + kTaskTimerPeriod, kTaskTimerValue});
      continue;
    }

    Message msg{Message::kTimerTimeout};
    msg.arg.timer.timeout = timers_.top().Timeout();
    msg.arg.timer.value = timers_.top().Value();
    task_manager->SendMessage(1, msg);

    timers_.pop();
  }

  return timeout;
}

TimerManager* timer_manager;
unsigned long lapic_timer_freq;

extern "C" void LAPICTimerOnInterrupt(const TaskContext& ctx_stack) {
  const bool task_timer_timeout = timer_manager->Tick();
  NotifyEndOfInterrupt();

  if (task_timer_timeout) {
    task_manager->SwitchTask(ctx_stack);
  }
}

void StartLAPICTimer() {
  volatile uint32_t* initial_count = reinterpret_cast<uint32_t*>(0xfee00380);
  *initial_count = 0xffffffff; // one-shot: count from max
}

uint32_t LAPICTimerElapsed() {
  volatile uint32_t* current_count = reinterpret_cast<uint32_t*>(0xfee00390);
  return 0xffffffff - *current_count;
}

void StopLAPICTimer() {
  volatile uint32_t* initial_count = reinterpret_cast<uint32_t*>(0xfee00380);
  *initial_count = 0;
}

void InitializeLAPICTimer() {
  timer_manager = new TimerManager;

  // Set one-shot mode to measure actual LAPIC frequency via ACPI PM timer
  volatile uint32_t* lvt_timer = reinterpret_cast<uint32_t*>(0xfee00320);
  *lvt_timer = (0b00 << 17) | 0xff; // One-shot, vector 0xff (masked)

  // Measure for 100ms
  StartLAPICTimer();
  acpi::WaitMilliseconds(100);
  const auto elapsed = LAPICTimerElapsed();
  StopLAPICTimer();

  lapic_timer_freq = static_cast<unsigned long>(elapsed) * 10; // per second

  // Now set periodic mode for the actual timer
  *lvt_timer = (0b001 << 17) | InterruptVector::kLAPICTimer; // Periodic mode

  // Start the periodic timer
  volatile uint32_t* initial_count = reinterpret_cast<uint32_t*>(0xfee00380);
  *initial_count = lapic_timer_freq / kTimerFreq;
}
