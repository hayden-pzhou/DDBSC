//
// Created by 60458 on 2021/11/02.
//
#include "destor.h"
#include "jcr.h"
#include "backup.h"
#include "isa-l_crypto/mh_sha1.h"

static pthread_t hash_t;
static int64_t chunk_num;

static void* sha1_thread(void* arg){
//    char code[41];
    printf("Hash phase: start hash pipeline ...\n");
    struct mh_sha1_ctx *ctx;
    ctx = malloc(sizeof(struct mh_sha1_ctx));
    while (1){
        struct chunk* c = sync_queue_pop(chunk_queue);

        if(c == NULL){
            sync_queue_term(hash_queue);
            break;
        }

        if (CHECK_CHUNK(c, CHUNK_FILE_START) || CHECK_CHUNK(c, CHUNK_FILE_END)) {
            sync_queue_push(hash_queue, c);
            continue;
        }
        TIMER_DECLARE(1);
        TIMER_BEGIN(1);

        mh_sha1_init(ctx);
        mh_sha1_update_avx2(ctx, c->data, c->size);
        mh_sha1_finalize_avx2(ctx, c->fp);
        TIMER_END(1, jcr.hash_time);

//        hash2code(c->fp,code);
        sync_queue_push(hash_queue, c);
    }
    return NULL;
}

void start_hash_phase(){
    hash_queue = sync_queue_new(100);
    pthread_create(&hash_t,NULL,sha1_thread,NULL);
}
void stop_hash_phase(){
    pthread_join(hash_t,NULL);
//    NOTICE("hash phase stop successfully!");
    printf("Hash phase: stop hash pipeline successfully!\n");
}
