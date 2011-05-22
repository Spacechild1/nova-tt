#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "semaphore.hpp"

#include <boost/thread/thread.hpp>

using namespace nova;
using namespace boost;

inline timespec ptime_to_timespec (const boost::posix_time::ptime &tm)
{
   const boost::posix_time::ptime epoch(boost::gregorian::date(1970,1,1));
   boost::posix_time::time_duration duration (tm - epoch);
   timespec ts;
   ts.tv_sec  = duration.total_seconds();
   ts.tv_nsec = duration.total_nanoseconds() % 1000000000;
   return ts;
}


BOOST_AUTO_TEST_CASE( sem_timed_wait )
{
    semaphore<true> sem;

    system_time const timeout = get_system_time() + posix_time::milliseconds(500);

    struct timespec timeoutspec = ptime_to_timespec(timeout);
    int status = sem.timed_wait(timeoutspec);
    BOOST_REQUIRE(!status);
}


namespace
{
const int thread_count = 8;

int count = 0;

semaphore<false> s(1);

void test_fn(void)
{
    for (int i = 0; i != 1000000; ++i)
    {
        s.wait();
        ++count;
        s.post();
    }
}

}

BOOST_AUTO_TEST_CASE( sem_test )
{
    thread_group g;

    for (int i = 0; i != thread_count; ++i)
        g.create_thread(test_fn);
    g.join_all();

    BOOST_REQUIRE_EQUAL(count, 8000000);
}

BOOST_AUTO_TEST_CASE( sem_sync_test )
{
    semaphore<false> sem(0);
    sem.post();
    semaphore_sync<semaphore<false> > sync(sem);
}
