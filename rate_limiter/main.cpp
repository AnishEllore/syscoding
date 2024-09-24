// Request rate limiter

#include <bits/stdc++.h>
using namespace std;

#define TOO_MANY_REQUESTS 429
#define SERVICE_UNAVAILABLE 503
#define SUCCESSS 200

class RateLimiter{
public:
    int sendRequest(uint64_t id, const string& msg) {
        unique_lock<mutex> lock(mtx);
        if(mp.find(id) == mp.end()) {

        } else if(mp[id] == maxRequests) {
            return TOO_MANY_REQUESTS;
        }

        mp[id]++;
        return SUCCESSS;
    }

private:
    mutex mtx;
    unordered_map<uint64_t, uint64_t> mp;
    const uint64_t maxRequests = 10;
};