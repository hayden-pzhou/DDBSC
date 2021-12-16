cmake_minimum_required(VERSION 3.16)
project(DDBSC C)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(src/chunking)
include_directories(src/utils)
include_directories(src/net)
include_directories(/usr/local/include/glib-2.0/)
include_directories(/usr/local/lib/glib-2.0/include/)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -Wall")

add_executable(DDBSC_cli
        src/chunking/chunking.h
        src/chunking/ae_chunking.c
        src/chunking/fascdc_chunking.c
        src/chunking/md5.h
        src/chunking/md5.c
        src/chunking/rabin_chunking.c
        src/config.c
        src/utils/bloom_filter.c
        src/utils/bloom_filter.h
        src/utils/lru_cache.h
        src/utils/lru_cache.c
        src/utils/queue.h
        src/utils/queue.c
        src/utils/sds.h
        src/utils/sds.c
        src/utils/serial.h
        src/utils/serial.c
        src/utils/sync_queue.h
        src/utils/sync_queue.c
        src/destor.h
        src/destor.c
        src/backup.h
        src/read_phase.c
        src/jcr.h src/jcr.c
        src/recipe/recipestore.h
        src/recipe/recipestore.c
        src/chunk_phase.c
        src/hash_phase.c
        src/net_phase.c
        src/net/network.h
        src/net/network.c
        src/do_backup.c
        src/do_restore.c)

target_link_libraries(DDBSC_cli glib-2.0 pthread ssl crypto isal_crypto zmq)

