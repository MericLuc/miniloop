#include "Loop.h"

#include <event2/event.h>

namespace loop {

Loop::Timeout::Timeout(Loop &loop) noexcept
    : loop(loop), callback(nullptr),
      event(evtimer_new(loop.event_base, &trampoline, this)) {}

Loop::Timeout::~Timeout() noexcept {
  event_del(event);
  event_free(event);
}

void Loop::Timeout::onTimeout(std::function<void()> callback) noexcept {
  this->callback = callback;
}

void Loop::Timeout::set(int after_ms) noexcept {
  struct timeval tv;

  tv.tv_sec = after_ms / 1000;
  tv.tv_usec = after_ms % 1000;

  evtimer_add(event, &tv);
}

void Loop::Timeout::cancel() noexcept { event_del(event); }

void Loop::Timeout::trampoline(int /* fd */, short /* events */,
                               void *data) noexcept {
  if (reinterpret_cast<Loop::Timeout *>(data)->callback)
    reinterpret_cast<Loop::Timeout *>(data)->callback();
}

Loop::RecurrentTimeout::RecurrentTimeout(int period, Loop &loop) noexcept
    : period_ms(period), underlyingTimeout(loop) {
  underlyingTimeout.onTimeout([this]() {
    underlyingTimeout.set(period_ms);
    if (callback)
      callback();
  });
  underlyingTimeout.set(period);
}
Loop::RecurrentTimeout::~RecurrentTimeout() noexcept {}

void Loop::RecurrentTimeout::onTimeout(
    std::function<void()> callback) noexcept {
  this->callback = callback;
}

} // namespace loop
