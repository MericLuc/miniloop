#include "Loop.h"

#include <event2/event.h>

namespace loop {

const int Loop::IO::READ = EV_READ;
const int Loop::IO::WRITE = EV_WRITE;

Loop::IO::IO(int fd, Loop &loop) noexcept
    : loop(loop), event(event_new(loop.event_base, fd, 0, &trampoline, this)) {}

Loop::IO::~IO() noexcept {
  if (event) {
    event_del(event);
    event_free(event);
  }
}

void Loop::IO::onEvent(std::function<void(int)> callback) noexcept {
  this->callback = callback;
}

int Loop::IO::getFd() const noexcept { return event_get_fd(event); }

void Loop::IO::setRequestedEvents(int events) noexcept {
  if (events != requested_events) {
    requested_events = events;
    event_del(event);
    event_assign(event, loop.event_base, getFd(),
                 static_cast<short>(requested_events | EV_PERSIST), &trampoline,
                 this);
    if (requested_events)
      event_add(event, nullptr);
  }
}

int Loop::IO::getRequestedEvents() const noexcept { return requested_events; }

void Loop::IO::trampoline(int /* fd */, short events, void *data) noexcept {
  if (reinterpret_cast<Loop::IO *>(data)->callback)
    reinterpret_cast<Loop::IO *>(data)->callback(events);
}
} // namespace loop
