#include <stx/Clock.h>
#include <stx/Duration.h>
#include <stx/UnixTime.h>
#include <stx/exception.h>
#include <stx/defines.h>

#include <time.h>

#if defined(STX_OS_DARWIN)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

namespace stx {

#if defined(HAVE_CLOCK_GETTIME)
class SystemClock : public Clock {
public:
  SystemClock(int driver) : id_(driver) {}

  void update() override {
    timespec ts;
    int rv = clock_gettime(id_, &ts);
    if (rv < 0)
      RAISE_ERRNO(kRuntimeError, "Could not retrieve clock.");

    uint64_t secs = Duration::fromSeconds(ts.tv_sec).microseconds();
    uint64_t nsecs = Duration::fromNanoseconds(ts.tv_nsec).microseconds();
    UnixTime theTime = UnixTime(secs + nsecs);

    set(theTime);
  }

private:
  int id_;
};
#endif

#if defined(STX_OS_DARWIN)
// OSX: http://stackoverflow.com/questions/11680461/monotonic-clock-on-osx
class MachMonotonicClock : public Clock {
public:
  MachMonotonicClock() {
    mach_timebase_info(&timebaseInfo_);
  }

  void update() override {
    uint64_t machTimeUnits = mach_absolute_time();

    uint64_t nanos = machTimeUnits * timebaseInfo_.numer / timebaseInfo_.denom;
    UnixTime theTime = UnixTime(Duration::fromNanoseconds(nanos).microseconds());

    set(theTime);
  }

private:
  mach_timebase_info_data_t timebaseInfo_;
};
#endif

Clock::Clock() : theTime_() {
}

Clock* Clock::realtime() {
#if defined(HAVE_CLOCK_GETTIME)
  static SystemClock clk(CLOCK_REALTIME);
  return &clk;
#else
  RAISE(kNotImplementedError);
#endif
}

Clock* Clock::realtimeCoarse() {
#if defined(HAVE_CLOCK_GETTIME)
  static SystemClock clk(CLOCK_REALTIME_COARSE);
  return &clk;
#else
  RAISE(kNotImplementedError);
#endif
}

Clock* Clock::monotonic() {
#if defined(HAVE_CLOCK_GETTIME)
  static SystemClock clk(CLOCK_MONOTONIC);
  return &clk;
#elif defined(STX_OS_DARWIN)
  static MachMonotonicClock clk;
  return &clk;
#else
  RAISE(kNotImplementedError);
#endif
}

Clock* Clock::monotonicCoarse() {
#if defined(HAVE_CLOCK_GETTIME)
  static SystemClock clk(CLOCK_MONOTONIC_COARSE);
  return &clk;
#elif defined(STX_OS_DARWIN)
  static MachMonotonicClock clk;
  return &clk;
#else
  RAISE(kNotImplementedError);
#endif
}

Clock* Clock::monotonicBootTime() {
#if defined(HAVE_CLOCK_GETTIME)
  static SystemClock clk(CLOCK_BOOTTIME);
  return &clk;
#else
  // OSX: http://stackoverflow.com/questions/12488481/getting-ios-system-uptime-that-doesnt-pause-when-asleep
  RAISE(kNotImplementedError);
#endif
}

Clock* Clock::processCpuTime() {
#if defined(HAVE_CLOCK_GETTIME)
  static SystemClock clk(CLOCK_PROCESS_CPUTIME_ID);
  return &clk;
#else
  RAISE(kNotImplementedError);
#endif
}

Clock* Clock::threadCpuTime() {
#if defined(HAVE_CLOCK_GETTIME)
  static SystemClock clk(CLOCK_THREAD_CPUTIME_ID);
  return &clk;
#else
  RAISE(kNotImplementedError);
#endif
}

} // namespace stx
