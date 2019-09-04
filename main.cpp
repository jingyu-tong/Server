//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Channel.h"
#include "Server.h"

#include <iostream>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop;

void onMessage(const ConnectionPointer& conn) {
	printf("receive data\n");
}
void onConnection(const ConnectionPointer& conn) {
	printf("hello\n");
}

int main(int, char**) {
	EventLoop loop;
	g_loop = &loop;
	Server server(&loop, 4000);
	server.setMessageCallback(onMessage);
	server.setConnectionCallback(onConnection);
	server.start();

	loop.loop();
}
