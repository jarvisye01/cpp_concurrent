# minikv

**backgroud**

在实现了基本的网络通讯组件之后萌生的想法，搞一个最简单的kv存储，主要是验证目前所写代码的可用性，不需要复杂设计越快跑起来越好;

**实现**

1.存储: 一个同步的map，保证并发安全，没有考虑数据落盘问题;

2.server: 使用network中的easyserver，开启8个worker线程;

3.协议: 采用最简单的tlv协议;

**组件**

1.server: kv server，在easyserver的基础上实现相关callback，完成对kv的操作;

2.cli: 一个命令行工具来简单的使用kv server，在命令行使用get和set命令进行操作;