//
// Created by 60458 on 2021/11/12.
//

#include "destor.h"
#include "jcr.h"
#include "utils/sync_queue.h"
#include "net/network.h"
#include "backup.h"

int notify_backup_begin(){
    char buf[512];
    buf[0] = BACKUP_START_REQ;
    int path_len = sdslen(jcr.path);
    if(path_len > 500){
        printf("backup path to long!\n");
        return 0;
    }
    memcpy(buf+1,jcr.path,path_len+1);

    if(send_msg(buf,path_len+2) < 0){
        printf("send msg error!\n");
        return -1;
    };

    int recv_len = 0;
    recv_msg(buf,&recv_len);

    if(buf[0] != BACKUP_START_REP || buf[1] != 1){
        printf("sever not ready to backup\n");
        return -2;
    }
    return 1;

}

void do_backup(char *path){
    // back begin
    /*
     * 1. init_backup_jcr()
     * 路径
     * 网络
     * jcr任务
     */
    init_network(destor.server,destor.port);
    init_backup_jcr(path);
    puts("==== backup begin ====");
    TIMER_DECLARE(1);
    TIMER_BEGIN(1);
    int ret = connect_server();
    assert(ret>0);
    notify_backup_begin();
    start_read_phase();
    start_chunk_phase();
    start_hash_phase();
    start_net_phase();
    do {
        usleep(100);
//        fprintf(stderr,"job %" PRId32 ", %" PRId32 " chunks, %d files processed\r",
//                jcr.id, jcr.data_size,jcr.chunk_num,jcr.file_num);
    }while(jcr.status == JCR_STATUS_RUNNING || jcr.status != JCR_STATUS_DONE);

    stop_read_phase();
    stop_chunk_phase();
    stop_hash_phase();
    stop_net_phase();
    puts("==== backup end ====");
    TIMER_END(1, jcr.total_time);
}