# tinys
一个swoole的简单实现，学习swoole源码的练习之作


## 编译扩展(需要php7)

    ```
    git clone https://github.com/wosiwo/tinys.git
    
    cd tinys
    phpize
    需要指定php-config路径，本地机器为/usr/bin/php-config
    ./configure -with-php-config=/usr/bin/php-config
    make && make install
    ```
