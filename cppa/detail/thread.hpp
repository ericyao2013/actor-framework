#ifndef THREAD_HPP
#define THREAD_HPP

#ifdef __APPLE__

#include <boost/thread.hpp>
#include "cppa/util/duration.hpp"

namespace cppa { namespace detail {

using boost::mutex;
using boost::thread;
using boost::unique_lock;
using boost::condition_variable;

namespace this_thread { using namespace boost::this_thread; }

template<class Lock, class Condition>
inline bool wait_until(Lock& lock, Condition& cond,
                       const boost::system_time& timeout)
{
    return cond.timed_wait(lock, timeout);
}

inline boost::system_time now()
{
    return boost::get_system_time();
}

} } // namespace cppa::detail

inline boost::system_time& operator+=(boost::system_time& lhs,
                                      const cppa::util::duration& rhs)
{
    switch (rhs.unit)
    {
        case cppa::util::time_unit::seconds:
            lhs += boost::posix_time::seconds(rhs.count);
            break;

        case cppa::util::time_unit::milliseconds:
            lhs += boost::posix_time::milliseconds(rhs.count);
            break;

        case cppa::util::time_unit::microseconds:
            lhs += boost::posix_time::microseconds(rhs.count);
            break;

        default: break;
    }
    return lhs;
}

#else

#include <mutex>
#include <thread>
#include <condition_variable>

namespace cppa { namespace detail {

using std::mutex;
using std::thread;
using std::condition_variable;
using std::unique_lock;

namespace this_thread { using namespace std::this_thread; }

// returns false if a timeout occured
template<class Lock, class Condition, typename TimePoint>
inline bool wait_until(Lock& lock, Condition& cond, const TimePoint& timeout)
{
    return cond.wait_until(lock, timeout) != std::cv_status::timeout;
}

inline auto now() -> decltype(std::chrono::high_resolution_clock::now())
{
    return std::chrono::high_resolution_clock::now();
}

} } // namespace cppa::detail

#endif // __APPLE__

#endif // THREAD_HPP
