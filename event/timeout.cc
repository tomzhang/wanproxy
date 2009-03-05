#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#include <event/action.h>
#include <event/callback.h>
#include <event/timeout.h>

TimeoutQueue::NanoTime
TimeoutQueue::NanoTime::current_time(void)
{
	struct timespec ts;
	NanoTime nt;
	int rv;

	rv = ::clock_gettime(CLOCK_MONOTONIC, &ts);
	ASSERT(rv != -1);

	nt.seconds_ = ts.tv_sec;
	nt.nanoseconds_ = ts.tv_nsec;

	return (nt);
}

Action *
TimeoutQueue::append(uintmax_t ms, Callback *cb)
{
	TimeoutQueue::NanoTime now = TimeoutQueue::NanoTime::current_time();

	now.seconds_ += ms / 1000;
	now.nanoseconds_ += (ms % 1000) * 1000000;

	CallbackQueue& queue = timeout_queue_[now];
	Action *a = queue.append(cb);
	return (a);
}

uintmax_t
TimeoutQueue::interval(void) const
{
	TimeoutQueue::NanoTime now = TimeoutQueue::NanoTime::current_time();
	timeout_map_t::const_iterator it = timeout_queue_.begin();
	if (it == timeout_queue_.end())
		return (0);
	if (it->first < now)
		return (0);
	now -= it->first;
	return ((now.seconds_ * 1000) + (now.nanoseconds_ / 1000000));
}

void
TimeoutQueue::perform(void)
{
	timeout_map_t::iterator it = timeout_queue_.begin();
	if (it == timeout_queue_.end())
		return;
	TimeoutQueue::NanoTime now = TimeoutQueue::NanoTime::current_time();
	if (it->first > now)
		return;
	CallbackQueue& queue = it->second;
	if (queue.empty())
		timeout_queue_.erase(it);
	else
		queue.perform();
}

bool
TimeoutQueue::ready(void) const
{
	timeout_map_t::const_iterator it = timeout_queue_.begin();
	if (it == timeout_queue_.end())
		return (false);
	TimeoutQueue::NanoTime now = TimeoutQueue::NanoTime::current_time();
	if (it->first <= now)
		return (true);
	return (false);
}
