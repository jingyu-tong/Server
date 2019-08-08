//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef THREAD_H_
#define THREAD_H_

#include "noncopyable.h"
#include "CountDownLatch.h"

#include <pthread.h>
#include <functional>
#include <memory>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>

class Thread : noncopyable
{
    public:
        typedef std::function<void ()> ThreadFunc;
        //只能直接初始化，抑制构造函数的隐式转换
        explicit Thread(const ThreadFunc& func, const std::string& name = std::string());
        ~Thread();
        //创建线程
        void start();
        //封装pthread_join，等待该Thred结束
        int join();
        bool started() const {  return started_; }
        pid_t tid() const { return tid_;    }
        const std::string& name() const {   return name_;   }

    private:
        void setDefaultName();
        bool started_;
        bool joined_;
        pthread_t pthreadID_;
        pid_t tid_;
        ThreadFunc func_;
        std::string name_;
        CountDownLatch latch_;
};


#endif