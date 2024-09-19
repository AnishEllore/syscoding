// Design a resource allocation system for a shared printer that
// is used by multiple departments. The system should ensure fair
// access to the printer, avoid deadlocks, and handle multiple
// concurrent requests from different departments efficiently.
// You can implement deadlock prevention or detection as part
// of your solution. How would you approach this problem?

// Improvement: Make your own condition variables and mutexes in c++.

#include <bits/stdc++.h>

using namespace std;

class Printer {
public:
    bool RequestPrinter(const uint64_t depID) {
        unique_lock<mutex> lock(mtx);
        bool validReq = rankMap.find(depID) == rankMap.end();
        if(!validReq) return false;

        int id = getID();
        if(id == -1) return false;
        
        rankMap[depID] = id;
        cv.wait(lock, [id, this]{return readyID == id;});
        cout<<"Printer acquired by "<<depID<<endl;
        return true;
    }

    bool ReleasePrinter(const uint64_t depID) {
        unique_lock<mutex> lock(mtx);
        if(rankMap.find(depID) == rankMap.end()) return false;
        int id = rankMap[depID];
        rankMap.erase(depID);
        qSize--;
        readyID = (readyID + 1) % maxSize;
        cv.notify_all();
        cout<<"Printer released by "<<depID<<endl;
        return true;
    }

    // give rank in queue.
    // Improvement: give wait time by calculating avg request time.
    int CheckQueueStatus(const uint64_t depID) {
        unique_lock<mutex> lock(mtx);
        bool validReq = rankMap.find(depID) != rankMap.end();
        if(!validReq) return -1;
        return (rankMap[depID] - readyID+ maxSize) % maxSize;
    }

    bool CancelRequest(const uint64_t depID) {
        return true;
    }
private:
    // maintaining the department ranking.
    uint64_t freeID = 0;
    uint64_t qSize = 0;
    const uint64_t maxSize = 10;
    unordered_map<uint64_t, uint64_t> rankMap;

    mutex mtx;
    condition_variable cv;
    int readyID = 0;

    int getID() {
        if(qSize == maxSize) {
            return -1;
        }
        int id = freeID++;
        freeID %= maxSize;
        qSize++;
        return id;
    }
};

void doWork(Printer* p, int id) {
    bool status = p->RequestPrinter(id);
    cout<<"Hey I'm printing my department id: "<<id<<endl;
    this_thread::sleep_for(std::chrono::seconds(10));
    status = p->ReleasePrinter(id);
}

int main() {
    Printer *p = new Printer();
    vector<thread*> threads;
    for(int i = 0; i <= 4; i++) {
        thread *t = new thread(doWork, p, i);
        threads.push_back(t);
    }
    for(int  i =0; i < 10; i++) {
        int id = rand() % 5;
        int status = p->CheckQueueStatus(id);
        cout<<"wait time for " <<id<<" for "<<status<<endl;
        this_thread::sleep_for(std::chrono::seconds(2)); 
    }
    for(int i = 0; i <= 4; i++) {
        threads[i]->join();
    }
}