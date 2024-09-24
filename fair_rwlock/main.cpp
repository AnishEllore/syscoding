// Design and Implement a Fair Read-Write Lock with Starvation-Free 

#include <bits/stdc++.h>
using namespace std;

class RWLock {
public:
    void read_lock() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]{return this->writeLock==false && this->waiting_writers == 0;});
        readers++;
    }


    void read_unlock() {
        unique_lock<mutex> lock(mtx);
        readers--;
        cv.notify_all();
    }

    void write_lock() {
        unique_lock<mutex> lock(mtx);
        waiting_writers++;
        cv.wait(lock, [this]{return this->readers == 0 && this->writeLock == false;});
        writeLock = true;
    }

    void write_unlock() {
        unique_lock<mutex> lock(mtx);
        waiting_writers--;
        writeLock = false;
        cv.notify_all();
    }

private:
    mutex mtx;
    condition_variable cv;
    uint64_t readers = 0;
    uint64_t waiting_writers = 0;
    bool writeLock = false;
};

void doRead(RWLock& rwlock) {
    while(1) {
        rwlock.read_lock();
        cout<<"acquired_read lock"<<endl;
        this_thread::sleep_for(2s);
        rwlock.read_unlock();
    }
}

void doWrite(RWLock& rwlock) {
    while(1) {
        rwlock.write_lock();
        cout<<"write lock acquired"<<endl;
        this_thread::sleep_for(2s);
        rwlock.write_unlock();
        this_thread::sleep_for(5s);
    }
}

int main() {
    vector<thread> threads;
    int rthreads = 10;
    int wthreads = 2;
    RWLock rwlock;
    for(int i = 0; i < rthreads; i++) {
        threads.emplace_back(doRead, ref(rwlock));
    }

    for(int i = 0; i < wthreads; i++) {
        threads.emplace_back(doWrite, ref(rwlock));
    }

    for(int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}