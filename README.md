# WebServer
## 介绍
- 使用epoll的IO多路复用，基于Reactor模式的并发模型
- 简单实现线程池管理线程，避免线程频繁创建删除线程的开销
- 基于小根堆实现定时器清理失效连接，解决超时连接占有系统资源的问题
- 简单实现HTTP协议的功能，支持GET请求，支持长连接
## 优化
- **信号量**替换**条件变量**
- **string的find函数**替换**regex**
## 编译
```
cd MyServer
make
```
## 运行
```
./bin/myserver
```
## 测试
```
./webbench-1.5/webbench -c num -t time http://ip:port/

num为并发量
time为测试时间
```
