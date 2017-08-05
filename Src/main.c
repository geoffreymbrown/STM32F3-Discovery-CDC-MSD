/*
    ChibiOS/HAL - Copyright (C) 2017 Geoffrey Brown

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"

#include "usbcfg.h"
#include "hal_usb_msd.h"

#include "shell.h"
#include "chprintf.h"

extern BaseBlockDevice *pDisk;
#define LED LINE_LED3_RED

static uint8_t blkbuf[512];
static const scsi_inquiry_response_t scsi_inquiry_response = {
    0x00,           /* direct access block device     */
    0x80,           /* removable                      */
    0x04,           /* SPC-2                          */
    0x02,           /* response data format           */
    0x20,           /* response has 0x20 + 4 bytes    */
    0x00,
    0x00,
    0x00,
    "IU CS",
    "Tag Storage",
    {'v',CH_KERNEL_MAJOR+'0','.',CH_KERNEL_MINOR+'0'}
};


static const ShellCommand commands[] = {
  {NULL, NULL}
};


static ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};


static THD_WORKING_AREA(waThread1, 256);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");

  while (true) {
    palSetLine(LED);
    chThdSleepMilliseconds(500);
    palClearLine(LED);
    chThdSleepMilliseconds(500);
  }
}


int main(void) {

  // start the hal

  halInit();

  // Start ChibiOS

  chSysInit();

  sduObjectInit(&SDU1);
  sduStart(&SDU1,&serusbcfg);

  // Stop the USB bus

#ifdef usb_lld_disconnect_bus
  usbDisconnectBus(&USBD1);
  chThdSleepMilliseconds(1500);
#endif

  // Init USB1
  
  usbStart(&USBD1, &usbcfg);

  // Init the mass storage device

  msdObjectInit(&USBMSD1);  
  msdStart(&USBMSD1, &USBD1, pDisk /*&Disk &vdp*/, blkbuf,  &scsi_inquiry_response);

  // Reconnect USB

#ifdef usb_lld_connect_bus
  usbConnectBus(&USBD1);
#endif

  // Create Blinker

  chThdCreateStatic(waThread1, sizeof(waThread1), 
		    NORMALPRIO, Thread1, NULL);
  
  while (true) {
    if (SDU1.config->usbp->state == USB_ACTIVE) {
      // must wait for SDU1 to be active
      shellThread(&shell_cfg1);
    }
    chThdSleepMilliseconds(1000);
  }
}

volatile uint32_t vecactive;
void _unhandled_exception(void) {
  vecactive = (*((uint32_t *) 0xE000ED04U)) & 0xff;
  while (true);
}

// this sets up the stack so gdb can unwind it

void HardFault_Handler(void) {
  __asm( ".syntax unified\n"
	 ".thumb\n"
	 "tst lr, #4\n"
	 "ite eq\n"
	 "mrseq r0, msp\n"
	 "mrsne r0, psp\n"
	 "mov sp, r0\n"
	 "bkpt #1\n");
  while (1);
}
