//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef CONDITION_H_
#define CONDITION_H_

#include "noncopyable.h"
#include "MutexLock.h"

#include <pthread.h> 

class Condition : noncopyable
{   
    public:
        //只能直接初始化，抑制构造函数的隐式转换
        explicit Condition(MutexLock &mutex) 
            :mutex_(mutex)
        {
            pthread_cond_init(&cond, NULL);
        }
        ~Condition() {
            pthread_cond_destroy(&cond);
        }

        void wait() {
            pthread_cond_wait(&cond, mutex_.getPthreadMutex());
        }
        //至少唤醒一个
        void notify() {
            pthread_cond_signal(&cond);
        }
        //唤醒所有
        void notifyAll() {
            pthread_cond_broadcast(&cond);
        }

    private:
        MutexLock& mutex_;
        pthread_cond_t cond;
};

#endif