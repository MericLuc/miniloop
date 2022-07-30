/**
 * @file : Loop.h
 * @brief : Public header of miniloop library
 * @author : lhm
 */

#ifndef SRC_MINILOOP_LOOP_H_
#define SRC_MINILOOP_LOOP_H_

#include <functional>

struct event;
struct event_base;

namespace loop {

/*!
 * \brief The Loop class is the eventloop for miniloop
 */
class Loop {
public:
  static Loop &singleton() noexcept;

  Loop() noexcept;
  virtual ~Loop() noexcept;

  /*!
   * \brief run start the loop.
   * It will run untill exit() is called.
   */
  void run() noexcept;

  /*!
   * \brief exit Stops the loop.
   * It has no effect if the loop is not running.
   */
  void exit() noexcept;

public:
  /*!
   * \brief The UNIX_SIGNAL class allows to listen to specific UNIX
   *  signals and perform actions when they are emitted.
   */
  class UNIX_SIGNAL {
  public:
    UNIX_SIGNAL(int id, Loop &loop = Loop::singleton()) noexcept;
    virtual ~UNIX_SIGNAL() noexcept;

    /*!
     * \brief onEvent Sets the callback when the requested signal
     * is emitted.
     * \param callback the callback.
     */
    void onEvent(std::function<void(int)> callback) noexcept;

    /*!
     * \brief setRequestedSignal Change the signal of interest
     * \param id The id of the signal (\see signum-generic.h)
     */
    void setRequestedSignal(int id) noexcept;

    /*!
     * \brief getRequestedSignal
     * \return The ID of the requested signal
     */
    int getRequestedSignal() const noexcept;
    int getFd() const noexcept;

    explicit operator bool() const noexcept { return nullptr != event; }

  private:
    Loop &loop;
    std::function<void(int)> callback{nullptr};
    int requested_id;
    struct event *event{nullptr};

    static void trampoline(int fd, short events, void *data) noexcept;
  };

  /*!
   * \brief The IO class allows to listen on a file descriptor and
   *  perform an action when an event (read|write) happens on it.
   */
  class IO {
  public:
    static const int READ;
    static const int WRITE;

    IO(int fd, Loop &loop = Loop::singleton()) noexcept;
    virtual ~IO() noexcept;

    /*!
     * \brief onEvent Sets the callback
     * \param callback the callback
     */
    void onEvent(std::function<void(int)> callback) noexcept;

    /*!
     * \brief getFd Retrieves the file descriptor
     * \return the file descriptor
     */
    int getFd() const noexcept;

    /*!
     * \brief setRequestedEvents change the event of interest
     *  Possible values are : READ, WRITE or READ | WRITE
     * \param events The event of interest.
     */
    void setRequestedEvents(int events) noexcept;
    int getRequestedEvents() const noexcept;

  private:
    Loop &loop;
    std::function<void(int)> callback{nullptr};
    int requested_events{0};
    struct event *event{nullptr};

    static void trampoline(int fd, short events, void *data) noexcept;
  };

  /*!
   * \brief The Timeout class allows to perform an action when a
   * timeout occurs.
   */
  class Timeout {
  public:
    Timeout(Loop &loop = Loop::singleton()) noexcept;
    virtual ~Timeout() noexcept;

    /*!
     * \brief onTimeout sets the callback
     * \param callback the callback
     */
    void onTimeout(std::function<void()> callback) noexcept;

    /*!
     * \brief set Change the value of the timeout
     * \param after_ms the new timeout value (in milliseconds)
     */
    void set(int after_ms) noexcept;

    /*!
     * \brief cancel Cancels the timeout
     */
    void cancel() noexcept;

  private:
    Loop &loop;
    std::function<void()> callback{nullptr};
    struct event *event{nullptr};
    static void trampoline(int fd, short events, void *data) noexcept;
  };

  /*!
   * \brief The RecurrentTimeout class allows to perform an action
   * on a cyclic timeout.
   */
  class RecurrentTimeout {
  public:
    RecurrentTimeout(int period_ms, Loop &loop = Loop::singleton()) noexcept;
    virtual ~RecurrentTimeout() noexcept;

    /*!
     * \brief onTimeout set the action to perform when timeout occurs
     * \param callback the callback
     */
    void onTimeout(std::function<void()> callback) noexcept;

  private:
    int period_ms;
    Timeout underlyingTimeout;
    std::function<void()> callback{nullptr};
  };

  /*!
   * \brief The AlarmClock class allows to watch for a specific date
   * in a week.
   */
  class AlarmClock {
  public:
    AlarmClock() noexcept;
    virtual ~AlarmClock() noexcept = default;

    virtual void set(int days, int minuteOfTheDay) noexcept;
    virtual void onAlarm(std::function<void()> callback) noexcept;
    virtual void reset() noexcept;

    class Alarm {
    public:
      enum Days {
        DAY_MONDAY = 1 << 0,
        DAY_TUESDAY = 1 << 1,
        DAY_WEDNESDAY = 1 << 2,
        DAY_THURSDAY = 1 << 3,
        DAY_FRIDAY = 1 << 4,
        DAY_SATURDAY = 1 << 5,
        DAY_SUNDAY = 1 << 6,
      };
      static const int DAY_EVERY = 0xFF;

      /*!
       * \brief Alarm Creates an alarm to a specific time of the week
       * \param days one or more days
       * For example, the week-end is `DAY_SATURDAY | DAY_SUNDAY`
       * \param time the number of minutes on the day, starting from
       * midnight For example, 12h34min is `754`
       *
       * \note Default arguments will make an alarm that will never trigger
       */
      Alarm(int days = DAY_EVERY, int time = 0) noexcept;
      ~Alarm() noexcept = default;

      bool matches(int time, int day) const noexcept;

    private:
      int days;
      int time;
    };

  protected:
    std::function<void()> callback;
    bool triggered{false};
    Alarm alarm;
    loop::Loop::RecurrentTimeout timeout{30000};
  };

private:
  struct event_base *event_base{nullptr};
};

} // namespace loop

#endif /* SRC_MINILOOP_LOOP_H_ */
