//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef MUTEXLOCK_H_
#define MUTEXLOCK_H_

#include "noncopyable.h"
#include "CurrentThread.h"

#include <pthread.h>
#include <cstdio>
#include <assert.h>

//封装pthread_mutex相关操作
class MutexLock
{
    public:
        MutexLock() 
        :   holder_(0)
        {
            pthread_mutex_init(&mutex_, NULL);
        }
        ~MutexLock() {
            assert(!holder_);
            pthread_mutex_destroy(&mutex_);
        }

        void lock() {
            pthread_mutex_lock(&mutex_);
            holder_ = CurrentThread::tid(); //持有锁之后读取，否则可能被抢占
        }
        void unlock() {
            holder_ = 0; //同样，归0后在推出临界区
            pthread_mutex_unlock(&mutex_);
        }
        pthread_mutex_t* get() {
            return &mutex_;
        }

    private:
        pthread_mutex_t mutex_;
        pid_t holder_;

        //给Condition提供mutex的地址
        friend class Condition;
        pthread_mutex_t * getPthreadMutex() {
            return &mutex_;
        }
};

//对MutexLock封装
//通过栈上变量方便的进行加锁/解锁
class MutexLockGuard : noncopyable
{
    public:
         //只能直接初始化，抑制构造函数的隐式转换
        explicit MutexLockGuard(MutexLock& mutex) 
            : mutex_(mutex)
        {
            mutex_.lock();
        }
        ~MutexLockGuard() {
            mutex_.unlock();
        }
    private:
        MutexLock& mutex_;
};




#endif