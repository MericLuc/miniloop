#include <event2/event.h>

#include "Loop.h"

loop::Loop &loop::Loop::singleton() noexcept {
  static loop::Loop singleton;
  return singleton;
}

loop::Loop::Loop() noexcept : event_base(event_base_new()) {}

loop::Loop::~Loop() noexcept { event_base_free(event_base); }

void loop::Loop::run() noexcept { event_base_dispatch(event_base); }

void loop::Loop::exit() noexcept { event_base_loopbreak(event_base); }
