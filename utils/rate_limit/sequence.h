#ifndef __akcs_rate_limiterseq_uence_h__
#define __akcs_rate_limiterseq_uence_h__
#include <atomic>

#define CACHELINE_SIZE_BYTES 64
#define CACHELINE_PADDING_FOR_ATOMIC_INT64_SIZE (CACHELINE_SIZE_BYTES - sizeof(std::atomic<int64_t>))

#ifndef DISALLOW_COPY_MOVE_AND_ASSIGN
#define DISALLOW_COPY_MOVE_AND_ASSIGN(TypeName) TypeName(const TypeName&) = delete; TypeName(const TypeName&&) = delete;  TypeName& operator=(const TypeName&) = delete
#endif

class AtomicSequence
{
public:
	AtomicSequence(int64_t num = 0L) : seq_(num) {};
	~AtomicSequence() {};
    DISALLOW_COPY_MOVE_AND_ASSIGN(AtomicSequence);
    
	void store(const int64_t val)//std::memory_order _order = std::memory_orderseq__cst
	{
		seq_.store(val);
	}

	int64_t load()
	{
		return seq_.load();
	}

	int64_t fetch_add(const int64_t increment)
	{
		return seq_.fetch_add(increment);
	}

private:
	char front_padding_[CACHELINE_PADDING_FOR_ATOMIC_INT64_SIZE];//解决伪共享的问题
	std::atomic<int64_t> seq_;
	char back_padding_[CACHELINE_PADDING_FOR_ATOMIC_INT64_SIZE];
};

#endif //__akcs_rate_limiterseq_uence_h__