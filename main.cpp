//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Channel.h"
#include "Server.h"
#include "Connection.h"
#include "HttpServer.h"

#include <iostream>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop;


int main(int, char**) {
	EventLoop loop;
	g_loop = &loop;

	HttpServer server(&loop, 4000, 3);

	loop.loop();
}
