# cpp_concurrent


很长时间感觉自己写的东西都不成体系，这里建了一个仓库把自己的代码放进来，希望自己平时写的代码也能够严谨一点，而不仅仅只是小demo。
这里不仅仅是兵法相关的东西，还有很多网络以及各种各样的组件，之所以叫cpp_concurrent是因为当时总结并发相关的知识了。
很多东西都是看了陈硕大佬的东西写下的，算是一种copy吧，希望以这种方式让自己进步一点。

# concurrent
- JMutex、JCond、JBrarrier、JCountLatch：实现了Mutex、Cond、Barrrier、CountLatch等基础同步组件。
- JThread：封装了线程相关的操作，可以很方便的启动一个线程，代码里面很多地方都是通过JThread类结合一个lambda函数启动一个线程。
- JSyncQueue：一个简单的同步队列，使用互斥量和条件变量来保证安全。

# network
封装了基本的网络操作，主要有以下基础组件：
- JConn：表示一个Tcp连接；
- JEventLoop：事件循环；
- JEpoller：封装epoll操作；
- JServer：Tcp的Server；
- JNetBuffer：用于网络操作的Buffer，可以自动扩充空间，可以直接从socket中读写数据，减少拷贝；

# util
- JString：字符串操作相关组件；
- JLog：一个多线程日志组件；
- JBuffer：字符缓冲组件；
- JTime：时间处理操作组件；
