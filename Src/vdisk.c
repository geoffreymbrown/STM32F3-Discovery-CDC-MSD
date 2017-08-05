// Disk Image

#include <stdint.h>
#include "fat.h"
#include "ch.h"
#include "hal.h"

typedef struct {
  const struct BaseBlockDeviceVMT *vmt;
  _base_block_device_data
  const vfile_t *filesys;
} vBlockDevice;


int vfile_1(uint32_t blknum, uint8_t *buf, const vfile_t *filesys, uint32_t index) {
  (void) filesys;
  (void) index;
   
  int i;
  char f;

  if (blknum == 0) 
    f = '0';
  else
    f = '1';
  if (blknum < 2)
    for (i = 0; i < 512; i++)
      buf[i] = f;
  return 0;
}

extern const vfile_t filesys;

static bool is_inserted(void *instance) {
  (void) instance;
  return true;
}

static bool is_protected(void *instance) {
  (void)instance;
  return false;
}

static bool connect(void *instance) {
  (void) instance;
  return HAL_SUCCESS;
}

static bool disconnect(void *instance) {
  (void) instance;
  return HAL_SUCCESS;
}

static bool read(void *instance, uint32_t startblk, uint8_t *buffer, uint32_t n) {
  vBlockDevice *pvDisk = (vBlockDevice *) instance;
  return read_vdisk(pvDisk->filesys, startblk, buffer, n);
}

static bool write(void *instance, uint32_t startblk, const uint8_t *buffer, uint32_t n) {
  (void) instance;
  (void) startblk;
  (void) buffer;
  (void) n;
  return HAL_SUCCESS;
}

static bool sync(void *instance){
  (void) instance;
  return HAL_SUCCESS;
}

static bool get_info(void *instance, BlockDeviceInfo *bdip) {
  vBlockDevice *pvDisk = (vBlockDevice *) instance;
  bdip->blk_num = pvDisk->filesys->clusterstart + pvDisk->filesys->maxcluster*filesys.blocks_per_cluster;
  bdip->blk_size = 512;
  return HAL_SUCCESS;
}

static struct BaseBlockDeviceVMT vmt = {
  is_inserted, 
  is_protected, 
  connect, 
  disconnect, 
  read,
  write, 
  sync, 
  get_info
};


static vBlockDevice vDisk = { &vmt, BLK_READY, &filesys };

BaseBlockDevice *pDisk = (BaseBlockDevice *) &vDisk;



