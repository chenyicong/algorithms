#ifndef __akcs_spinlock_guard_h__
#define __akcs_spinlock_guard_h__
#include "spinlock.h"

class SpinlockGuard
{
public:
	SpinlockGuard(Spinlock& l)
		: lock_(l)
	{
		lock_.lock();
	}

	~SpinlockGuard()
	{
		lock_.unlock();
	}

	DISALLOW_COPY_MOVE_AND_ASSIGN(SpinlockGuard);

private:
	Spinlock& lock_;

};

#endif //__akcs_spinlock_guard_h__