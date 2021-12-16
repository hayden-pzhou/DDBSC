//
// Created by 60458 on 2021/11/02.
//
#include "destor.h"
#include "backup.h"
#include "jcr.h"
#include "utils/serial.h"
#include "net/network.h"

#define NET_BUFFSIZE  1048576

static pthread_t net_t;
static unsigned char send_buf[NET_BUFFSIZE];

static void* net_thread(void *arg){
    /**
     * 将hash队列中的数据块一个一个的发送到服务器端 进行去重
     */
    struct chunk * c= NULL;
    while (1){
        c = sync_queue_pop(hash_queue);
        if(c==NULL){
//            sync_queue_term(net_queue);
            send_buf[0] = BACKUP_END_REQ;
            send_msg(send_buf,1);
            //结束
            break;
        }
        send_buf[0] = BACKUP_START_REQ;
        ser_declare;
        ser_begin(send_buf+1,NET_BUFFSIZE);
        ser_int32(c->size);
        ser_int32(c->flag);
        ser_int64(c->id);
        ser_bytes(&c->fp,sizeof(fingerprint));
        if(c->data){
            ser_bytes(c->data,c->size);
        }
        int32_t len = ser_length(send_buf);
        ser_end(send_buf+1,NET_BUFFSIZE);
        send_msg(send_buf,len+1);
//        printf("send %d bytes\n",len);
        free_chunk(c);
        c = NULL;
//        sync_queue_push(net_queue,c);
    }
    /*all chunk send */
    jcr.status = JCR_STATUS_DONE;
    return NULL;
}

void start_net_phase(){
    net_queue = sync_queue_new(100);
    pthread_create(&net_t,NULL,net_thread,NULL);
}

void stop_net_phase(){
    pthread_join(net_t,NULL);
    NOTICE("net phase stops successfully!");
}

