tutorial_proxy.c是从网上找的代码，my_proxy.c是自己模仿着实现的。

有几个要点：
1.要用多线程，如果一个连接之后客户端申请了更多的连接，自然需要有多线程来处理
2.代理服务器的accept负责建立和client的连接，所以要用while(1)一直等待accept（accept让程序进入阻塞状态），accept成功的话就要新开线程去accept客户端的socket
3.要点：只支持get类，不支持连接类的http(头部是CONNECT，如baidu.com)，所以这个模型是代理服务器1次recv client，根据这个recv解析出真实服务器信息，然后send到真实服务器，真实服务器会分若干次返回消息，代理服务器要用while循环将这些消息一一发送回client。注意到：client只需要send一次！（我一开始写的一直不能通过，就是因为我写成了client每次recv之后都要send，大错特错！！！）
4.关于真实服务器的ip和端口，借鉴了tutorial_proxy.c的代码。但是需要注意的是：只需要从那里头获取ip和端口就行了。tutorial_proxy.c是直接自己构建了那个socket(它设置了超时时间，还对(192.168.0.1)这类ip地址作了解析，我的方法里头都没管这些)。