//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Channel.h"

#include <iostream>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop;

void print() {
	printf("ready to quit\n");
	g_loop->quit();
}
void threadFunc() {
	printf("thread\n");
	g_loop->runAfter(print, 10000);
}

int main(int, char**) {
	EventLoopThread reactor;
	g_loop = reactor.startLoop();
	g_loop->runAfter(print, 10000);

	sleep(20);	

}
