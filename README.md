# Server
高性能c++web服务器，服务器基于Reactor模式，采用one loop per thread结构，解析了get/head等请求，支持http长短链接，基于红黑树实现了定时器的有效管理，并且实现了基于双缓冲区的异步日志，时刻记录服务器的运行状态。
![0.1框架](./assets/0.1框架.png)
整体框架如图，主reactor负责接收新的连接，建立连接后分配给某个SubReactor。跨线程调用利用回调函数队列实现，需要加锁，锁的竞争只在MainReactor和某一SubReactor之间，不同SubReactor之间没有联系。

### Key Points

- 使用epoll水平触发+非阻塞IO，使用Reactor模式
- 采用one loop per thread结构，使用多线程充分利用多核CPU，并使用线程池避免创建销毁的开销
- 使用eventfd实现了线程的异步唤醒
- 使用基于multimap的定时器，主要用于HTTP超时关闭，使用timerfd唤醒等待线程
- 使用双缓冲技术，实现了简单的异步日志
- 使用了RAII技术设计了多个类
- 解析了HTTP请求(GET HEAD)，支持长短连接

### Build & Run

```shell
./build
./Server
```

### Module

各部分介绍见[模块介绍](./模块介绍.md)

### Test

关于项目的一些问题，改进，以及测试结果见[问题和测试](./问题和测试.md)

