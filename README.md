# cpp_concurrent


很长时间感觉自己写的东西都不成体系，这里建了一个仓库把自己的代码放进来，希望自己平时写的代码也能够严谨一点，而不仅仅只是小demo。

这里不仅仅是并发相关的东西，还有很多网络以及各种各样的组件，之所以叫cpp_concurrent是因为当时想总结并发相关的知识。

很多东西都是看了陈硕大佬的东西写下的，算是一种copy吧，希望以这种方式让自己进步一点。

个人博客：https://mr-xingjian.github.io/

# concurrent
- JMutex、JCond、JBrarrier、JCountLatch: 实现了Mutex、Cond、Barrrier、CountLatch等基础同步组件;
- JThread: 封装了线程相关的操作，可以很方便的启动一个线程，代码里面很多地方都是通过JThread类结合一个lambda函数启动一个线程;
- JSyncQueue: 一个简单的同步队列，使用互斥量和条件变量来保证安全;
- JSycMap: 一个简单的线程安全map;

# network
封装了基本的网络操作，主要有以下基础组件：
- JConn: 表示一个Tcp连接;
- JEventLoop: 事件循环;
- JEpoller: 封装epoll操作;
- JServer: Tcp的Server;
- JNetBuffer: 用于网络操作的Buffer，可以自动扩充空间，可以直接从socket中读写数据，减少拷贝;

# util
- JString: 字符串操作相关组件;
- JLog: 一个多线程日志组件;
- JBuffer: 字符缓冲组件;
- JTime: 时间处理操作组件;

<br></br>
# Update 2022.02.26
**concurent**
- shm设计: 可以进行共享内存的分配和管理，只做了简单的测试，可能还存在问题;

**network**
- epoller重新设计: 简化了epoller的操作，设计了poller接口，未来可以根据不同的平台使用不同的多路复用io系统调用;
- eventloop重新设计: 精简api，暂时没有加入定时清理不活跃socket的能力;
- socket设计: 封装对socket的操作，主要有server和client两个分支完成client和server的各项操作，二者均能够基于buffer缓存收发数据;
- easyserver设计: 简化了tcpserver的设计;
- basetlv设计: 添加了一个基本的tlv类型，按照Type + Length + Data存储数据，并且可以进行序列化和反序列化;

**minikv**
- minikv: 基于已经实现的组件，实现了一个在线kv，只是一个小玩具，没有考虑很多，目前只是能跑起来;

**util**
- buffer重新设计: 经过使用发现之前设计的buffer有缺陷，重新设计了buffer，新的buffer在继承中主要有两个分支，分别用于文件和网络;
- ref设计: 添加了一个引用计数工具，在设计句柄类时有一点作用;