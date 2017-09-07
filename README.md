# tinys
一个swoole的简单实现，学习swoole源码的练习之作，目前更新到version2

version2.1 
主进程创建多个reactor线程维持与客户端连接
TODO:代码结束时清理变量

version2更新:
主进程改为负责接收/发送数据给客户端
worker多进程处理逻辑，通过管道与主进程交互，不直接与客户端连接


## 编译扩展(需要php7)

需要指定php-config路径，本地机器为/usr/bin/php-config


    ```
    git clone https://github.com/wosiwo/tinys.git
    cd tinys
    phpize
    ./configure -with-php-config=/usr/bin/php-config
    make && make install
    ```
