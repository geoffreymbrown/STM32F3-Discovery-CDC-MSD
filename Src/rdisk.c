// Disk Image

#include <string.h>
#include "ch.h"
#include "hal.h"

extern const uint32_t DiskImageLen;
extern unsigned char DiskImage[];

typedef struct {
  const struct BaseBlockDeviceVMT *vmt;
  _base_block_device_data
  unsigned char *DiskImage;
  const uint32_t *pDiskImageLen;
} ramBlockDevice;


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
  ramBlockDevice *pDisk = (ramBlockDevice *) instance;
  if (n + startblk > *pDisk->pDiskImageLen/512)
    return false;
  memcpy(buffer,&(pDisk->DiskImage[startblk*512]),n*512);
  return true;
}

static bool write(void *instance, uint32_t startblk, const uint8_t *buffer, uint32_t n) {
  ramBlockDevice *pDisk = (ramBlockDevice *) instance;
  if (n + startblk > *pDisk->pDiskImageLen/512)
    return false;
  memcpy(&(pDisk->DiskImage[startblk*512]),buffer,n*512);
  return true;
}

static bool sync(void *instance){
  (void) instance;
  return HAL_SUCCESS;
}

static bool get_info(void *instance, BlockDeviceInfo *bdip) {
  ramBlockDevice *pDisk = (ramBlockDevice *) instance;
  bdip->blk_num = *pDisk->pDiskImageLen/512;
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

static ramBlockDevice rDisk = {&vmt, BLK_READY, 
			       DiskImage,
			       &DiskImageLen};

BaseBlockDevice *pDisk = (BaseBlockDevice *) &rDisk;



