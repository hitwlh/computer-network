这两个.c代码是为了验证shutdown和close之间的区别



首先建立一个连接，然后client端用同一个连接创建多个线程，去发送相同的数据
在线程里头，发送完有四种断开可能，具体查看client.c的58~69行

结果：
用close(ConnectSocket);不影响后面的线程
iResult = shutdown(ConnectSocket, SHUT_RD);也不影响后面的线程
SHUT_RD代表不能发送但是可以接收，但是后面的线程仍然可以正常读写，主进程的i'm here to fuck world也可以执行。


iResult = shutdown(ConnectSocket, SHUT_WR);
iResult = shutdown(ConnectSocket, SHUT_RDWR);
会影响后面的线程
直接退出，无法再发，连主进程的printf("i'm here to fuck world\n");都不能执行。