#ifndef __akcs_rate_limiter_token_bucket_h__
#define __akcs_rate_limiter_token_bucket_h__
#include "sequence.h"
#include "spinlock.h"
#include <assert.h>
#include <unistd.h>
#include <rate_limiter_token_bucket_interface.h>
#define NS_PER_SECOND 1000000000//一秒的纳秒数
#define NS_PER_USECOND 1000//一微秒的纳秒数

//基于令牌桶算法实现的限流器
//最大qps为1,000,000,000,最小为1
//使用：
// RateLimiterTokenBucket r(100,1);
// r.Acquire();
//能通过r.acquire()函数即可保证流速

namespace evpp {
namespace rate_limiter {
class RateLimiterTokenBucket : public RateLimiterTokenBucketInterface 
{
public:
    //qps:限制最大为十亿,受限于时间精度,ns级别
    //burst_factor: 允许瞬间爆发的因子，默认为1，最大为2
    RateLimiterTokenBucket(uint64_t qps, int8_t burst_factor = 1);
    DISALLOW_COPY_MOVE_AND_ASSIGN(RateLimiterTokenBucket);

    //对外接口，能返回说明流量在限定值内
    void Acquire();

private:
    //更新令牌桶中的token
    void SupplyTokens();
    bool TryGetToken();
    void GetToken();
    //获得当前时间，单位ns
    int64_t Now();
    //令牌桶大小
private:
    const int8_t burst_factor_;
    int64_t bucket_size_;
    //当前桶里面可用的token数
    AtomicSequence token_available_;
    //补充token的单位时间
    const int64_t supply_unit_time_;
    //上次触发补充token的时间,精确到纳秒
    int64_t last_add_token_time_;
    //自旋锁
    Spinlock lock_;
};
}// rate_limiter
}// evpp
#endif //__akcs_rate_limiter_token_bucket_h__