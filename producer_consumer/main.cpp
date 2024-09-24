// Producer consumer problem with bounded buffer
// TODO: implement you own lock, cv, thread, etc;

#include <bits/stdc++.h>
using namespace std;

class Logger {
public:
    void Log(const string& msg) {
        unique_lock<mutex> lock(mtx);
        wcv.wait(lock, [this]{return writerQ.size() < maxSize;});
        writerQ.push_back(msg);
        if(writerQ.size() == maxSize)rcv.notify_one();
        else wcv.notify_one();
    }

    Logger() {
        readerQ.reserve(maxSize);
        writerQ.reserve(maxSize);
        reader = thread(&Logger::readLogs, this);
    }

    ~Logger() {
        stopLogger = true;
        rcv.notify_one();
        reader.join();
        std::cout<<"Logger  destructor called exiting safely"<<std::endl;
    }

private:
    // synchronization
    mutex mtx;
    condition_variable wcv;
    condition_variable rcv;

    // logger infra
    const int maxSize = 10;
    string logFile = "debug.log";
    vector<string> readerQ;
    vector<string> writerQ;
    atomic_bool stopLogger = false;

    // reader
    thread reader;
    void readLogs() {
        while(!stopLogger) {
            unique_lock<mutex> lock(mtx);
            rcv.wait(lock, [this]{return stopLogger || !writerQ.empty();});
            readerQ.swap(writerQ);
            lock.unlock();
            wcv.notify_one();
            for(const string& msg: readerQ) {
                cout<<msg<<endl;
            }
            readerQ.clear();
        }
        this_thread::sleep_for(3s);
        cout<<"exiting reader"<<endl;
    }

};

void doWork(Logger& logger, int id) {
    for(int i = 0; i < 10; i++) {
        string str ="ID: " + to_string(id) + " and msgid: " + to_string(i);
        logger.Log(str);
        //this_thread::sleep_for(1s);
    }
}

int main() {
    Logger logger;
    vector<thread> threads;
    int writers = 5;
    for(int i = 0; i < writers; i++) {
        threads.emplace_back(doWork, ref(logger), i);
    }

    for(int i = 0; i < writers; i++) {
        threads[i].join();
    }
    cout<<"here"<<endl;
}