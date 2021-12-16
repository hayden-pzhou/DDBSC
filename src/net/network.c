//
// Created by 60458 on 2021/11/12.
//
#include "network.h"

void init_network(char* ip,char* port){
    printf("init client,server addr %s,prot %s\n",ip,port);
    client = (struct network*)malloc(sizeof(struct network));
    client->server_ip = sdsnew(ip);
    client->server_port = sdsnew(port);
    client->zmq_context = zmq_ctx_new();
    client->zmq_socket = zmq_socket(client->zmq_context,ZMQ_DEALER);
}
void destroy_network(){
    zmq_close(client->zmq_socket);
    zmq_ctx_destroy(client->zmq_context);
    sdsfree(client->server_ip);
    sdsfree(client->server_port);
    free(client);
}

int connect_server(){
    puts("=====connecting to server...======");
    sds remote_addr = sdsnew("tcp://");
    printf("server:%s,port:%s\n",client->server_ip,client->server_port);
    sdscat(remote_addr,client->server_ip);
    sdscat(remote_addr,":");
    sdscat(remote_addr,client->server_port);
    printf("server address: %s\n",remote_addr);
    if(zmq_connect(client->zmq_socket,remote_addr)!=0){
        return -1;
    }
    return 1;
}

int send_msg(char* send_buf,int len){
    zmq_msg_t msg;

    //send_bytes += len;

    if(zmq_msg_init_size(&msg,len) != 0){
        return -1;
    }
    memcpy(zmq_msg_data(&msg),send_buf,len);

    if(zmq_msg_send(&msg,client->zmq_socket,0) < 0) {
        return -2;
    }
    if(zmq_msg_close(&msg) != 0)
        return -3;
    return 1;
}

int recv_msg(char* recv_buf,int* len){
    zmq_msg_t msg;
    if(zmq_msg_init(&msg) != 0)
        return -1;

    if(zmq_msg_recv(&msg,client->zmq_socket,0) < 0){
        return -2;
    }
    char *p = (char*) zmq_msg_data(&msg);

    int msg_len = zmq_msg_size(&msg);
    memcpy(recv_buf,p,msg_len);
    *len = msg_len;

    if(zmq_msg_close(&msg) != 0)
        return -3;

    //recv_byte_ += len;
    return 1;
}


