//
// Created by 60458 on 2021/10/31.
//

#ifndef DDBSC_DESTOR_H
#define DDBSC_DESTOR_H
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <openssl/sha.h>
#include <glib.h>
#include <getopt.h>

#include "utils/sds.h"

#define TIMER_DECLARE(n) struct timeval b##n,e##n
#define TIMER_BEGIN(n) gettimeofday(&b##n, NULL)
#define TIMER_END(n,t) gettimeofday(&e##n, NULL); \
    (t)+=e##n.tv_usec-b##n.tv_usec+1000000*(e##n.tv_sec-b##n.tv_sec)


#define DESTOR_BACKUP 1
#define DESTOR_RESTORE 2
#define DESTOR_MAKE_TRACE 3
#define DESTOR_DELETE 4

/* Log levels */
#define DESTOR_DEBUG 0
#define DESTOR_VERBOSE 1
#define DESTOR_NOTICE 2
#define DESTOR_WARNING 3
#define DESTOR_DEFAULT_VERBOSITY DESTOR_NOTICE
#define DESTOR_MAX_LOGMSG_LEN 1024

#define CHUNK_FIXED 0
#define CHUNK_RABIN 1
#define CHUNK_NORMALIZED_RABIN 2
#define CHUNK_FILE 3 /* approximate file-level */
#define CHUNK_AE 4 /* Asymmetric Extremum CDC */
#define CHUNK_TTTD 5
#define CHUNK_FASTCDC 6
#define CHUNK_RBMC 7

#define TEMPORARY_ID (-1L)

/* the buffer size for read phase */
#define DEFAULT_BLOCK_SIZE 1048576 //1MB

#define DEFAULT_NETBUF_SIZE 1048576

/* states of normal chunks. */
#define CHUNK_UNIQUE (0x0000)
#define CHUNK_DUPLICATE (0x0100)
#define CHUNK_SPARSE (0x0200)
#define CHUNK_OUT_OF_ORDER (0x0400)
/* IN_CACHE will deny rewriting an out-of-order chunk */
#define CHUNK_IN_CACHE (0x0800)
/* This flag will deny all rewriting, including a sparse chunk */
#define CHUNK_REWRITE_DENIED (0x1000)

/* signal chunk */
#define CHUNK_FILE_START (0x0001)
#define CHUNK_FILE_END (0x0002)
#define CHUNK_SEGMENT_START (0x0004)
#define CHUNK_SEGMENT_END (0x0008)
#define CHUNK_SEGMENT_FLAG (0x0010)

#define SET_CHUNK(c, f) (c->flag |= f)
#define UNSET_CHUNK(c, f) (c->flag &= ~f)
#define CHECK_CHUNK(c, f) (c->flag & f)



#define DESTOR_CONFIGLINE_MAX 1024

struct destor {
    sds server;
    sds port;

    int verbosity;

    int chunk_algorithm;
    int chunk_max_size;
    int chunk_min_size;
    int chunk_avg_size;

} destor;

typedef unsigned char fingerprint[20];
typedef int64_t containerid; //container id
typedef int64_t segmentid;

struct chunk {
    int32_t size;//4
    int flag;//4
    containerid id;//8
    fingerprint fp;//20
    unsigned char *data;
};

/* struct segment only makes sense for index. */
struct segment {
    segmentid id;
    /* The actual number because there are signal chunks. */
    int32_t chunk_num;
    GSequence *chunks;
    GHashTable* features;
};

enum MsgType{
    BACKUP_START_REQ,
    BACKUP_START_REP,
    BACKUP_END_REQ,
    BACKUP_END_REP,
    RESTORE_START_REQ,
    RESTORE_START_REP,
    RESTORE_STOP_REQ
};

#define DEBUG(fmt, arg...) destor_log(DESTOR_DEBUG, fmt, ##arg);
#define VERBOSE(fmt, arg...) destor_log(DESTOR_VERBOSE, fmt, ##arg);
#define NOTICE(fmt, arg...) destor_log(DESTOR_NOTICE, fmt, ##arg);
#define WARNING(fmt, arg...) destor_log(DESTOR_WARNING, fmt, ##arg);

struct chunk* new_chunk(int32_t);
void free_chunk(struct chunk*);


void destor_log(int level,const char *fmt,...);

#endif //DDBSC_DESTOR_H

