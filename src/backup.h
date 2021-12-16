//
// Created by 60458 on 2021/11/01.
//

#ifndef DDBSC_BACKUP_H
#define DDBSC_BACKUP_H

#include "destor.h"
#include "utils/sync_queue.h"

/*
 * CHUNK_FILE_START NORMAL_CHUNK... CHUNK_FILE_END
 */
void start_read_phase();
void stop_read_phase();

/*
 * Input: Raw data blocks
 * Output: Chunks
 */
void start_chunk_phase();
void stop_chunk_phase();

/* Input: Chunks
 * Output: Hashed Chunks.
 */
void start_hash_phase();
void stop_hash_phase();

/*Input: chunk and hash
 * output
 */
void start_net_phase();
void stop_net_phase();

/* Output of read phase. */
SyncQueue* read_queue;
/* Output of chunk phase. */
SyncQueue* chunk_queue;
/* Output of hash phase. */
SyncQueue* hash_queue;
/* Output of net phase */
SyncQueue* net_queue;


#endif //DDBSC_BACKUP_H
