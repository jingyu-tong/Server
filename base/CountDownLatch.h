//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef COUNTDOWNLATCH_H_
#define COUNTDOWNLATCH_H_

#include "noncopyable.h"
#include "MutexLock.h"
#include "Condition.h"

//用于确保子线程的启动
 class CountDownLatch : noncopyable
 {
     public:
        //只能直接初始化，抑制构造函数的隐式转换
        explicit CountDownLatch(int count);
        void wait();
        void countdown();
    
    private:
        mutable MutexLock mutex_; //状态可变
        Condition condition_;
        int count_;
 };



#endif