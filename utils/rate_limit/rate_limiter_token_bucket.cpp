#include "rate_limiter_token_bucket.h"
#include "spinlock_guard.h"
#include <sys/time.h>
#include <float.h>
#include <unistd.h>
#include <thread>

#define RETRY_IMMEDIATELY_TIMES 100//不让出cpu的最大重试获得令牌的次数
#define NS_PER_SECOND 1000000000
namespace evpp {
namespace rate_limiter {
    
RateLimiterTokenBucket::RateLimiterTokenBucket(uint64_t qps, int8_t burst_factor)
:burst_factor_(burst_factor),token_available_(0),supply_unit_time_(NS_PER_SECOND / qps)
{
    assert(qps <= NS_PER_SECOND);
	assert(qps >= 0);
    bucket_size_ = burst_factor_ * qps;
    last_add_token_time_ = Now();
}

int64_t RateLimiterTokenBucket::Now()
{
	struct timeval tv;
	::gettimeofday(&tv, 0);
	int64_t seconds = tv.tv_sec;
	return seconds * NS_PER_SECOND + tv.tv_usec * NS_PER_USECOND;
}

//对外接口，能返回说明流量在限定值内
void RateLimiterTokenBucket::Acquire()
{
	return GetToken();
}

void RateLimiterTokenBucket::GetToken()
{
	bool is_get_token = false;
	for(int i = 0; i < RETRY_IMMEDIATELY_TIMES; ++i)
	{
		is_get_token =  TryGetToken();
		if(is_get_token)
		{
			return;
		}
	}

	while(1)
	{
		is_get_token = TryGetToken();
		if(is_get_token)
		{
			return;
		}
		else
		{
			//主动让出CPU
            std::this_thread::yield();
		}
	}
}

//尝试获得令牌
//成功获得则返回true
//失败则返回false
bool RateLimiterTokenBucket::TryGetToken()
{
    SupplyTokens();

	//获得n个令牌
	auto token = token_available_.fetch_add(-1);
	if(token <= 0)
	{   
        //已经没有足够的令牌了，归还透支的令牌
		token_available_.fetch_add(1);
		return false;
	}
    return true;
}

//added by chenyc,计算距离上一次，已经产生多少token了
void RateLimiterTokenBucket::SupplyTokens()
{
    int64_t cur = Now();
	if (cur - last_add_token_time_ < supply_unit_time_)
	{
		return;
	}

	{
		SpinlockGuard lock_guard(lock_);
		//等待自旋锁期间可能已经补充过令牌了
		int64_t new_tokens = (cur - last_add_token_time_) / supply_unit_time_;
		if (new_tokens <= 0)
		{
			return;
		}
		//更新补充时间,不能直接=cur,否则会导致时间步长丢失
		last_add_token_time_ += (new_tokens * supply_unit_time_);
		//当前桶剩余的空间
		auto free_room = bucket_size_ - token_available_.load();
        new_tokens = new_tokens < free_room ? new_tokens : free_room;		
		token_available_.fetch_add(new_tokens);
	}
}
}// rate_limiter
}// evpp