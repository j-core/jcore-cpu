#include "bare.h"

void fflush(int fd) {
}

void exit(int code) {
}

#define READ_MEM(A) (*(volatile unsigned int*)(A))

#define sys_SYS_BASE 0xabcd0200
#define Sys_RTCSecM	0x20	/* RealTime Clock Second MSW */
#define Sys_RTCSecL	0x24	/* RealTime Clock Second LSW */
#define Sys_RTCnsec	0x28	/* Real Time Clock nSec */
#define SEC_HI READ_MEM(sys_SYS_BASE + Sys_RTCSecM)
#define SEC_LO READ_MEM(sys_SYS_BASE + Sys_RTCSecL)

long time(long *t) {
  return SEC_LO;
}
