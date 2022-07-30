#include "Loop.h"

#include <event2/event.h>

namespace loop {

Loop::UNIX_SIGNAL::UNIX_SIGNAL(int id, Loop &loop) noexcept
    : loop(loop), requested_id(id),
      event(evsignal_new(loop.event_base, requested_id, &trampoline, this)) {
  if (nullptr != event && 0 != requested_id) {
    event_add(event, nullptr);
  }
}

Loop::UNIX_SIGNAL::~UNIX_SIGNAL() noexcept {
  if (event) {
    evsignal_del(event);
    event_free(event);
  }
}

void Loop::UNIX_SIGNAL::onEvent(std::function<void(int)> callback) noexcept {
  this->callback = callback;
}

void Loop::UNIX_SIGNAL::setRequestedSignal(int id) noexcept {
  if (id != requested_id) {
    requested_id = id;
    evsignal_del(event);
    evsignal_assign(event, loop.event_base, requested_id, &trampoline, this);

    if (0 != requested_id)
      evsignal_add(event, nullptr);
  }
}

int Loop::UNIX_SIGNAL::getRequestedSignal() const noexcept {
  return requested_id;
}

int Loop::UNIX_SIGNAL::getFd() const noexcept { return event_get_fd(event); }

void Loop::UNIX_SIGNAL::trampoline(int /* fd */, short events,
                                   void *data) noexcept {
  if (reinterpret_cast<Loop::UNIX_SIGNAL *>(data)->callback)
    reinterpret_cast<Loop::UNIX_SIGNAL *>(data)->callback(events);
}

} // namespace loop
