//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "EventLoop.h"
#include "Channel.h"

#include <iostream>
#include <sys/timerfd.h>
#include <string.h>

EventLoop* g_loop;
Channel* g_channel;

void timeout() { 
	printf("Timeout\n");
	g_channel->disableReading();
	g_loop->quit();
}

int main(int, char**) {
	printf("begin\n");
	
	EventLoop loop;
	g_loop = &loop;

	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(&loop, timerfd);
	g_channel = &channel;
	channel.setReadCallback(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 5;
	timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();
	
	close(timerfd);
}
