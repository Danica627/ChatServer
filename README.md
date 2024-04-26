# ChatServer
可以工作在nginx，tcp负载均衡环境中的集群聊天服务器和客户端源码，基于muduo实现，利用redis实现跨服务器通信，利用MySQL数据库存储信息。

代码编译方式
cd build
rm -rf *
cmake ..
make

nginx配置tcp负载均衡
nginx编译加入--with-stream参数激活tcp负载均衡模块
nginx编译安装需要先安装pcre、openssl、zlib等库，也可以直接编译执行configure命令，根据错误提示信息，安装相应缺少的库。

下面的make命令会向系统路径拷贝文件，需要在root用户下执行

tony@tony-virtual-machine:~/package/nginx-1.12.2# ./configure --with-stream
tony@tony-virtual-machine:~/package/nginx-1.12.2# make && make install
编译完成后，默认安装在了/usr/local/nginx目录。
tony@tony-virtual-machine:~/package/nginx-1.12.2$ cd /usr/local/nginx/
tony@tony-virtual-machine:/usr/local/nginx$ ls
conf html logs sbin
可执行文件在sbin目录里面，配置文件在conf目录里面。

nginx -s reload 重新加载配置文件启动
nginx -s stop 停止nginx服务

nginx配置tcp负载均衡
主要在conf目录里面配置nginx.conf文件，配置如下：
events {
    worker_connections  1024;
}

# nginx tcp loadbalance config
stream{
	upstream MyServer{
		server 127.0.0.1:6000 weight=1 max_fails=3 fail_timeout=30s;
		server 127.0.0.1:6002 weight=1 max_fails=3 fail_timeout=30s;
	}

	server{
		proxy_connect_timeout 1s;
		listen 8000;
		proxy_pass MyServer;
		tcp_nodelay on;
	}
}
配置完成后，./nginx -s reload平滑重启。

服务器中间件-基于发布-订阅的Redis
redis环境安装和配置
tony@tony-virtual-machine:~$ sudo apt-get install redis-server # ubuntu命令安装redis服务
ubuntu通过上面命令安装完redis，会自动启动redis服务，通过ps命令确认：
tony@tony-virtual-machine:~$ ps -ef | grep redis
redis 2717 1 0 13:24 ? 00:00:00 /usr/bin/redis-server  127.0.0.1:6379
可以看到redis默认工作在本地主机的6379端口上。
redis发布-订阅的客户端编程
redis支持多种不同的客户端编程语言，例如Java对应jedis、php对应phpredis、C++对应的则是hiredis。下面是安装hiredis的步骤：
1. git clone https://github.com/redis/hiredis #从github上下载hiredis客户端，进行源码编译安装
2. cd hiredis
3. make
4. sudo make install
5. sudo ldconfig /usr/local/lib  #拷贝生成的动态库到/usr/local/lib目录下！

