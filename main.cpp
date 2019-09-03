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

void onMessage(int connfd, Buffer& buf) {
	if(buf == "quit")
		g_loop->quit();
	printf("receive data is: %s\n",  buf.c_str());
}

int main(int, char**) {
	EventLoop loop;
	g_loop = &loop;
	Server server(&loop, 4000);
	server.setMessageCallback(onMessage);
	server.start();

	loop.loop();
}
