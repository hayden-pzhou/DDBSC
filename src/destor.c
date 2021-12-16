//
// Created by 60458 on 2021/10/31.
//
#include "destor.h"

extern void do_backup(char *path);
extern int load_config();


/* : means argument is required.
 * :: means argument is required and no space.
 */
const char * const short_options = "sr::t::p::h";

extern void destroy_network();
extern void do_restore(int revision,char* path);

struct option long_options[] = {
        { "state", 0, NULL, 's' },
        { "help", 0, NULL, 'h' },
        { NULL, 0, NULL, 0 }
};


struct chunk* new_chunk(int32_t size) {
    struct chunk* ck = (struct chunk*) malloc(sizeof(struct chunk));

    ck->flag = CHUNK_UNIQUE;
    ck->id = TEMPORARY_ID;
    memset(&ck->fp, 0x0, sizeof(fingerprint));
    ck->size = size;

    if (size > 0)
        ck->data = malloc(size);
    else
        ck->data = NULL;

    return ck;
}
void free_chunk(struct chunk* ck) {
    if (ck->data) {
        free(ck->data);
        ck->data = NULL;
    }
    free(ck);
}

void destor_log(int level, const char *fmt, ...) {
    va_list ap;
    char msg[DESTOR_MAX_LOGMSG_LEN];

    if ((level & 0xff) < destor.verbosity)
        return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    fprintf(stdout, "%s\n", msg);
}

void destor_start(){
    /* Init */
    destor.server = sdsnew("127.0.0.1");
    destor.port = sdsnew("8899");
    destor.verbosity = DESTOR_WARNING;
    destor.chunk_algorithm = CHUNK_RABIN;
    destor.chunk_max_size = 65536;
    destor.chunk_min_size = 1024;
    destor.chunk_avg_size = 8192;
    load_config();
}

void destor_shutdown(){
    destroy_network();
}

int main(int argc,char **argv){
    destor_start();
    int job = DESTOR_BACKUP;
    int revision = -1;

    int opt = 0;
    while ((opt = getopt_long(argc,argv,short_options,long_options,NULL)) != -1){
        switch (opt) {
            case 'r':
                job = DESTOR_RESTORE;
                revision = atoi(optarg);
                break;
            case 'h':
//                usage();
                break;
            default:
                return 0;
        }

    }
    sds path = NULL;

    switch (job) {
        case DESTOR_BACKUP:
            if(argc > optind) {
                path = sdsnew(argv[optind]);
            }else{
                fprintf(stderr,"backup job needs a protected path!\n");
//                usage();
            }

            struct timeval t0, t1;
            gettimeofday(&t0, NULL);
            gettimeofday(&t1, NULL);
            do_backup(path);
            sdsfree(path);
            break;

        case DESTOR_RESTORE:
            if(revision < 0){
                fprintf(stderr,"A job id is required!\n");
//                usage();
            }
            if (argc > optind) {
                path = sdsnew(argv[optind]); //path 为写入路径
            } else {
                fprintf(stderr, "A target directory is required!\n");
//                usage();
            }
            do_restore(revision, path[0] == 0 ? 0 : path);
            sdsfree(path);
            break;

        default:
            fprintf(stderr, "Invalid job type!\n");
//            usage();
    }
    destor_shutdown();
    return 0;
}
