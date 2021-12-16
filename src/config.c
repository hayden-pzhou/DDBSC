//
// Created by 60458 on 2021/10/31.
//

#include "destor.h"

int yesnotoi(char *s) {
    if (strcasecmp(s, "yes") == 0)
        return 1;
    else if (strcasecmp(s, "no") == 0)
        return 0;
    else
        return -1;
}

void load_config_from_string(sds config){
    char *err = NULL;
    int linenum = 0,totallines,i;
    sds *lines = sdssplitlen(config, strlen(config), "\n", 1, &totallines);

    for(int i=0;i<totallines;i++){
        sds *argv;
        int argc;

        linenum = i+1;
        lines[i] = sdstrim(lines[i],"\t\r\n");

        if (lines[i][0] == '#' || lines[i][0] == '\0')
            continue;

        argv = sdssplitargs(lines[i], &argc);
        if (argv == NULL) {
            err = "Unbalanced quotes in configuration line";
            goto loaderr;
        }

        if (argc == 0) {
            sdsfreesplitres(argv, argc);
            continue;
        }
        sdstolower(argv[0]);

        if(strcasecmp(argv[0],"server")==0 && argc==2){
            destor.server = sdscpy(destor.server,argv[1]);
        }else if(strcasecmp(argv[0],"port")==0&&argc==2){
            destor.port = sdscpy(destor.port,argv[1]);
        }else if(strcasecmp(argv[0],"log-level")==0&&argc==2){
            if (strcasecmp(argv[1], "debug") == 0) {
                destor.verbosity = DESTOR_DEBUG;
            } else if (strcasecmp(argv[1], "verbose") == 0) {
                destor.verbosity = DESTOR_VERBOSE;
            } else if (strcasecmp(argv[1], "notice") == 0) {
                destor.verbosity = DESTOR_NOTICE;
            } else if (strcasecmp(argv[1], "warning") == 0) {
                destor.verbosity = DESTOR_WARNING;
            } else {
                err = "Invalid log level";
                goto loaderr;
            }
        }else if(strcasecmp(argv[0] ,"chunk-algorithm")==0 && argc ==2) {
            if (strcasecmp(argv[1], "fixed") == 0) {
                destor.chunk_algorithm = CHUNK_FIXED;
            } else if (strcasecmp(argv[1], "rabin") == 0) {
                destor.chunk_algorithm = CHUNK_RABIN;
            } else if (strcasecmp(argv[1], "normalized_rabin") == 0) {
                destor.chunk_algorithm = CHUNK_NORMALIZED_RABIN;
            } else if (strcasecmp(argv[1], "tttd") == 0) {
                destor.chunk_algorithm = CHUNK_TTTD;
            } else if (strcasecmp(argv[1], "file") == 0) {
                destor.chunk_algorithm = CHUNK_FILE;
            } else if (strcasecmp(argv[1], "ae") == 0) {
                destor.chunk_algorithm = CHUNK_AE;
            } else if (strcasecmp(argv[1], "fastcdc") == 0){
                destor.chunk_algorithm = CHUNK_FASTCDC;
            } else if (strcasecmp(argv[1], "rbmc") == 0){
                destor.chunk_algorithm = CHUNK_RBMC;
            }
            else {
                err = "Invalid chunk algorithm";
                goto loaderr;
            }
        }else if (strcasecmp(argv[0], "chunk-avg-size") == 0 && argc == 2) {
            destor.chunk_avg_size = atoi(argv[1]);
        } else if (strcasecmp(argv[0], "chunk-max-size") == 0 && argc == 2) {
            destor.chunk_max_size = atoi(argv[1]);
        } else if (strcasecmp(argv[0], "chunk-min-size") == 0 && argc == 2) {
            destor.chunk_min_size = atoi(argv[1]);
        }
    }

    loaderr: fprintf(stderr, "\n*** FATAL CONFIG FILE ERROR in client ***\n");
    fprintf(stderr, "Reading the configuration file, at line %d\n", linenum);
    fprintf(stderr, ">>> '%s'\n", lines[i]);
    fprintf(stderr, "%s\n", err);
    exit(1);
}

void load_config(){
    sds config = sdsempty();
    char buf[DESTOR_CONFIGLINE_MAX+1];
    FILE *fp;
    if ((fp = fopen("client.config", "r")) == 0) {
        destor_log(DESTOR_WARNING, "No client.config file!");
        return;
    }
    while (fgets(buf, DESTOR_CONFIGLINE_MAX + 1, fp) != NULL)
        config = sdscat(config, buf);
    fclose(fp);
    load_config_from_string(config);
    sdsfree(config);
}