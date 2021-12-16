//
// Created by 60458 on 2021/11/18.
//
#include "destor.h"
#include "jcr.h"
#include "net/network.h"
#include "utils/serial.h"

int notify_restore_begin(int revision){
    char buf[512];
    buf[0] = RESTORE_START_REQ;
    ser_declare;
    ser_begin(buf+1,500);
    ser_int32(revision);
    ser_end(buf+1,500);
    int length = ser_length(buf);
    if(send_msg(buf,length) < 0){
        printf("send msg error!\n");
        return -1;
    };

    int recv_len = 0;
    recv_msg(buf,&recv_len);
    if(buf[0] != RESTORE_START_REP || buf[1] != 1){
        printf("sever not ready to backup\n");
        return -2;
    }
    return 1;

}


void do_restore(int revision,char *path){
    init_network(destor.server,destor.port);
    connect_server();
    notify_restore_begin(revision);
    printf("====restore begin====");
    //通知服务器启动恢复任务
    //接收数据
    //解压缩数据
}