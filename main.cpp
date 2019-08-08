//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"

#include <iostream>

void threadFunc() {
    EventLoop loop;
    loop.loop();
    printf("threadFunc(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
}

int main(int, char**) {
    printf("main(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    
    EventLoop loop;

    Thread thread(threadFunc);
    thread.start();//启动子线程

    loop.loop();
    pthread_exit(NULL);
}
