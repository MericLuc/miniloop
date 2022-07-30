#include <cstring>
#include <ctime>
#include <map>
#include <string>

#include "Loop.h"

static void time_now(int &minuteOfTheDay,
                     enum loop::Loop::AlarmClock::Alarm::Days &day) {
  struct tm tm;
  time_t t = time(nullptr);

  localtime_r(&t, &tm);
  minuteOfTheDay = tm.tm_hour * 60 + tm.tm_min;
  switch (tm.tm_wday) {
  case 0:
    day = loop::Loop::AlarmClock::Alarm::Days::DAY_SUNDAY;
    break;
  case 1:
    day = loop::Loop::AlarmClock::Alarm::Days::DAY_MONDAY;
    break;
  case 2:
    day = loop::Loop::AlarmClock::Alarm::Days::DAY_TUESDAY;
    break;
  case 3:
    day = loop::Loop::AlarmClock::Alarm::Days::DAY_WEDNESDAY;
    break;
  case 4:
    day = loop::Loop::AlarmClock::Alarm::Days::DAY_THURSDAY;
    break;
  case 5:
    day = loop::Loop::AlarmClock::Alarm::Days::DAY_FRIDAY;
    break;
  case 6:
    day = loop::Loop::AlarmClock::Alarm::Days::DAY_SATURDAY;
    break;
  }
}

namespace loop {

Loop::AlarmClock::Alarm::Alarm(int days, int time) noexcept
    : days{days}, time{time} {}

bool Loop::AlarmClock::Alarm::matches(int time, int day) const noexcept {
  return this->time == time && (this->days & day);
}

void Loop::AlarmClock::set(int days, int minuteOfTheDay) noexcept {
  alarm = Alarm(days, minuteOfTheDay);
}

void Loop::AlarmClock::onAlarm(std::function<void()> callback) noexcept {
  this->callback = callback;
}

Loop::AlarmClock::AlarmClock() noexcept {
  timeout.onTimeout([this]() {
    int minuteOfTheDay;
    Alarm::Days currentDay = Alarm::Days::DAY_SUNDAY;

    time_now(minuteOfTheDay, currentDay);

    if (alarm.matches(minuteOfTheDay, currentDay)) {
      if (!triggered && callback) {
        triggered = true;
        callback();
      }
    } else
      triggered = false;
  });
}

void Loop::AlarmClock::reset() noexcept { alarm = Alarm(); }

} // namespace loop
