<?php
//客户端使用swoole扩展的客户端

$client = new swoole_client(SWOOLE_SOCK_TCP);

//连接
if (!$client->connect('127.0.0.1', 4989, 0.5))
{
    die("connect failed.");
}

//发送请求
if (!$client->send("hello world"))
{
    die("send failed.");
}

//接收返回
$data = $client->recv();
if (!$data)
{
    die("recv failed.");
}


var_dump($data);

$client->close();