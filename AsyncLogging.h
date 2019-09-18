//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef ASYNCLOGGING_H_
#define ASYNCLOGGING_H_

#include "base/CountDownLatch.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/noncopyable.h"
#include "LogStream.h"

#include <functional>
#include <string>
#include <vector>

class AsyncLogging : noncopyable {
    public:
        AsyncLogging(const std::string base_name, int flush_interval = 2);
        ~AsyncLogging() {
            if(!running_)
                stop();
        }
        void append(const char* logline, int len);

        void start() {
            running_ = true;
            thread_.start();
            latch_.wait();
        }
        void stop() {
            running_ = false;
            cond_.notify();
            thread_.join();
        }

    private:
        void threadFunc(); //线程函数

        typedef FixedBuffer<kLargeBuffer> Buffer;
        typedef std::unique_ptr<Buffer> BufferPtr;
        typedef std::vector<BufferPtr> BufferVector;

        int flush_interval_;
        bool running_;
        std::string base_name_;
        Thread thread_;
        MutexLock mutex_;
        Condition cond_;
        BufferPtr cur_buffer_;
        BufferPtr next_buffer_;
        BufferVector buffers_;
        CountDownLatch latch_;
        
};











#endif