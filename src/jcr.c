//
// Created by 60458 on 2021/11/01.
//

#include "jcr.h"

struct jcr jcr;

void init_jcr(char *path){
    jcr.path = sdsnew(path);

    struct stat s;
    if (stat(path, &s) != 0) {
        fprintf(stderr, "backup path does not exist!");
        exit(1);
    }

    /* 目录*/
    if (S_ISDIR(s.st_mode) && jcr.path[sdslen(jcr.path) - 1] != '/')
        jcr.path = sdscat(jcr.path, "/");

    jcr.bv = NULL;

    jcr.id = TEMPORARY_ID;

    jcr.status = JCR_STATUS_INIT;
}

/**
 *
 * @param path
 */
void init_backup_jcr(char *path){
    init_jcr(path);
}
