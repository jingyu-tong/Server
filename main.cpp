//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Channel.h"
#include "Server.h"
#include "Connection.h"

#include <iostream>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop;

void onMessage(const ConnectionPointer& conn, Buffer& msg) {
	if(msg == "quit\n")
		g_loop->quit();
	conn->send(msg);
	printf("receive data: %s\n", msg.data());
	msg = "";
}
void onConnection(const ConnectionPointer& conn) {
	printf("hello\n");
}

int main(int, char**) {
	EventLoop loop;
	g_loop = &loop;
	Server server(&loop, 3500);
	server.setMessageCallback(onMessage);
	server.setConnectionCallback(onConnection);
	server.start();

	loop.loop();
}
