//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "Channel.h"

#include <iostream>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop;

void timeout2() {
	printf("Timeout 10s\n");
	g_loop->quit();
}
void timeout() { 
	printf("Timeout 5s\n");
	g_loop->runAfter(timeout2, 5000);
}



int main(int, char**) {
	EventLoop loop;
	g_loop = &loop;

	loop.runAfter(timeout, 5000);

	loop.loop();

}
