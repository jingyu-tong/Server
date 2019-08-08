//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"

#include <poll.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

__thread EventLoop* t_loopInThisThread = 0;

EventLoop ::EventLoop() 
:   looping_(false),
    threadID_(CurrentThread::tid())
    {
        //只允许一个EventLoop
        if(t_loopInThisThread) {

        } else {
            t_loopInThisThread = this;
        }
    }

EventLoop :: ~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}

void EventLoop :: loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    :: poll(NULL, 0, 5000);

    looping_ = false;
}