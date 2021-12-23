#ifndef __akcs_rate_limiter_token_bucket_interface_h__
#define __akcs_rate_limiter_token_bucket_interface_h__

#define NS_PER_SECOND 1000000000//一秒的纳秒数
#define NS_PER_USECOND 1000//一微秒的纳秒数

//基于令牌桶算法实现的限流器
//最大qps为1,000,000,000,最小为1
//使用：
// RateLimiter r(100,100);
// r.Acquire();
//能通过r.acquire()函数即可保证流速

namespace evpp {
namespace rate_limiter {
class RateLimiterTokenBucketInterface 
{
public:
    virtual ~RateLimiterTokenBucketInterface() {};
    //qps:限制最大为十亿,受限于时间精度,ns级别
    //bucket_size：桶的大小，默认为qps的大小，即允许1s的爆发，最大为2*qps,允许2s的瞬间爆发
    //virtual void SetRateLimit(int64_t qps, int64_t bucket_size) = 0;
    //对外接口，能返回说明流量在限定值内
    virtual void Acquire() = 0;
};
}// rate_limiter
}// evpp
#endif //__akcs_rate_limiter_token_bucket_interface_h__