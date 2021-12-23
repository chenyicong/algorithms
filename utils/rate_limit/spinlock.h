#ifndef __akcs_spinlock_h__
#define __akcs_spinlock_h__
#include <atomic>

#ifndef DISALLOW_COPY_MOVE_AND_ASSIGN
#define DISALLOW_COPY_MOVE_AND_ASSIGN(TypeName) TypeName(const TypeName&) = delete; TypeName(const TypeName&&) = delete;  TypeName& operator=(const TypeName&) = delete
#endif
class Spinlock
{
public:
	Spinlock()
    :lock_(ATOMIC_FLAG_INIT)
    {
    }

	~Spinlock()
    {
        unlock(); 
    }

	DISALLOW_COPY_MOVE_AND_ASSIGN(Spinlock);
    //以下摘抄自：https://en.cppreference.com/w/cpp/atomic/atomic_flag
	void lock()
	{
        //memory order的操作是关键
        while (lock_.test_and_set(std::memory_order_acquire)) {// acquire lock
            #if defined(__cpp_lib_atomic_flag_test)
                while (lock.test(std::memory_order_relaxed))// test lock
            #endif
            ; // spin
        }
	}

	void unlock()
	{
		lock_.clear(std::memory_order_release);  
	}

private:
    std::atomic_flag lock_;
};
#endif //__akcs_spinlock_h__