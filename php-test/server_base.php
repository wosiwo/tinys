<?php
set_time_limit(0);
//ini_set();
//sleep(8);   //暂停几秒，使用 strace 捕捉进程
//使用 tinys扩展
//调用扩展类，发送数据
class callBack{
     function  onReceive( $fd, $data){
        echo "php onReceive fd $fd data $data  \n";
        //调用扩展类，发送数据
         $ret = [
             'code' => 0,
             'data' => 'send back',
         ];
        $this->tinys->send($fd,json_encode($ret));
         echo "test 2 \n";
    }
}

$callback = new callBack();

$tinys = new tinys();

//
$callback->tinys = $tinys;

//$tinys->send(1,'send back\n');

$tinys->on('Receive',[$callback,'onReceive']);

$tinys->run("127.0.0.1",4989);

echo "start \n";