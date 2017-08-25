<?php
//客户端使用swoole扩展的客户端

$client = new tinys_client();
$client->connect('127.0.0.1', 4989);
$client->send("hello world");
$data = $client->recv();
if (!$data)
{
    die("recv failed.");
}
var_dump($data);