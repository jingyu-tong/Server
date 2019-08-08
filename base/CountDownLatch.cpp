//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "CountDownLatch.h"

//注意顺序，condition要用mutex初始化
CountDownLatch :: CountDownLatch(int count) 
 :   mutex_(),
    condition_(mutex_),
    count_(count) //初始化mutex，给condition
{

}

void CountDownLatch :: wait() {
    MutexLockGuard lock(mutex_);
    while(count_ > 0) //这里要用while，防止虚假唤醒
        condition_.wait();
}

void CountDownLatch :: countdown() {
    MutexLockGuard lock(mutex_);
    --count_;
    if(count_ == 0)
        condition_.notifyAll();
}