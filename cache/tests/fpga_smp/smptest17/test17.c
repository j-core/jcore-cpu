#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* smptest test17.c */
/*  2015-10-22 O. Nishii */
/*     cpu1 pit test, running prime number with/without PIT  */
/*     when PIT is on, int handler makes cpu1 main process */
/*     running 50% cpu load (the execution real time will be twice) */

/* fpga board */
#define RTC_SEC         0xabcd0224
#define RTC_NS          0xabcd0228

/* smp register */
#define CPU1_ENABLE_ADR   0xabcd0640
#define CPU1_INSTR_HEAD   0x14001000
#define CPU1_INTHDL_HEAD  0x14002000
#define CPU1_SP_INIT      0x14004ffc
#define VBR_CPU1          0x8180

/* cpu0 cpu1 communication */
#define COMM_SRAM_HEAD    0x8000

/* AIC parameter */
#define AICP_PITVECTOR    0x1B

/* cpu0 <-> cpu1 parameters */
/* base             offset(adrs) */
/* COMM_SRAM_HEAD + 0x100 : cpu1 processing complete */
/*                + 0x104 : address of sosu_prime[20000] = prt_sosu_prime */
/*                + 0x108 : limit (cpu0->cpu1-main) */
/*                + 0x10c : sosu(prime) count (cpu1-main->cpu0) */
/*                + 0x110 : PIT enable (cpu0->cpu1-main) */
/*                + 0x114 : int counter (init0 by cpu0, count up by cpu1-int) */

int sosu_prime [20000];
int main( )
{
  int i;
  long time_pr_1, time_pr_2;
  int time_r_sec1, time_r_sec2;
  int time_r_ns1,  time_r_ns2;
  volatile int *ptr_sec, *ptr_ns;

  /* cpu1 parameter */
  int pit_on, prime_limit;

  /* smp addition */
  int instbuf[2], poll_count = 0;
  int fd = 0; /* file handle */
  void *ptr_void;
  volatile int *ptr_inst, *ptr_data, *ptr_data_vbrt_jumppc;

  printf("input PIT-en(1 or 0) & sosu(prime) limit\n");
  scanf("%d %d", &pit_on, &prime_limit);

  ptr_void = malloc(1024 * 1024 * 10);

  /* step 2 load cpu1 main cpu1 int instructions */
  ptr_inst = (int *)CPU1_INSTR_HEAD;

  /* punch in cpu1, SR init, VBR init */
  /* -------------------------------------------------------------- */
  /* manual asm (hex punch-in) processing VBR = 0x8180 */
  *(ptr_inst) = 0xe17f7102; /* manual asm.  mov   #127,r1 */
                            /* manual asm.  add   #2,r1 */
    ptr_inst++;
  *(ptr_inst) = 0x4118e27f; /* manual asm.  shll8 r1 */
                            /* manual asm.  mov   #127,r2 */
    ptr_inst++;
  *(ptr_inst) = 0x7201212b; /* manual asm.  add   #1,r2 */
                            /* manual asm.  or    r2,r1 --(r1=r2|r1) */
    ptr_inst++;
  *(ptr_inst) = 0x412e0009; /* manual asm.  ldc   r1,vbr */
                            /* manual asm.  nop */
    ptr_inst++;
  /* -------------------------------------------------------------- */
  *(ptr_inst) = 0xe100410e; /* manual asm.  mov   #0,r1  */
                            /* manual asm.  ldc   r1,sr  */
    ptr_inst++;
  for(i = 0; i < ((0x80 >> 2) - 5); i ++) {
    *(ptr_inst) = 0x00090009; /* manual asm.  nop */
                              /* manual asm.  nop */
      ptr_inst++;
  }
  /* -------------------------------------------------------------- */

  fd = open("te17c1.bin", O_RDONLY );
  if(fd == -1) {
    printf("file (te17c1.bin) open error\n");
    return(1);
  }
  for(i = 0; i < ((21 << 2) + (26 << 2)) ; i++) { /* number- xxd lines-based */
                                                  /* i count by byte */
    read (fd, instbuf, 4);
    if(i >= (21 << 2)) {
      if(i == (33 << 2) + 0) {

        if(instbuf[0] == 0x0000026c) { /* sdiv jsr patch */
          instbuf[0] = CPU1_INSTR_HEAD + 0x1dc; /* by cpu1.map information */
        }
        else {
          printf("inst loader sdiv routine replace error\n");
        }

      }
      *(ptr_inst) = instbuf[0];
        ptr_inst++;
    }
  }
  close(fd);

  ptr_inst = (int *)CPU1_INTHDL_HEAD;
  fd = open("te17ih.bin", O_RDONLY );
  if(fd == -1) {
    printf("file (te17ih.bin) open error\n");
    return(1);
  }
  for(i = 0; i < ((21 << 2) + (5 << 2)) ; i++) { /* number- xxd lines-based */
    read (fd, instbuf, 4);
    if(i >= (21 << 2)) {
      *(ptr_inst) = instbuf[0];
        ptr_inst++;
    }
  }
  close(fd);

  /* step 3: setup CPU1 boot */
   ptr_data = (int *)0x8000;
  *ptr_data = CPU1_INSTR_HEAD;
   ptr_data = (int *)0x8004;
  *ptr_data = CPU1_SP_INIT;
   ptr_data_vbrt_jumppc = (int *)VBR_CPU1 + AICP_PITVECTOR;
  *ptr_data_vbrt_jumppc = CPU1_INTHDL_HEAD;

  /*         setup CPU1 (main) parameters */
   ptr_data = ((int *)COMM_SRAM_HEAD + (0x100 >> 2));
  *ptr_data = 0;                      /* cpu1 compu. complete */
   ptr_data++;
  *ptr_data = (int) &sosu_prime[0];

   ptr_data++;
  *ptr_data = prime_limit;
   ptr_data += 2;
  *ptr_data = pit_on;

   ptr_data++;
  *ptr_data = 0;                       /* int counter */

  /* step 4: time beginning and release cpu1 */
  ptr_sec = (int *)RTC_SEC;
  ptr_ns  = (int *)RTC_NS;
  time_pr_1 = clock( ); /* timer on */
  time_r_sec1 = *ptr_sec;
  time_r_ns1  = *ptr_ns;

   ptr_data = (int *)CPU1_ENABLE_ADR;
  *ptr_data = 1;

   ptr_data = ((int *)COMM_SRAM_HEAD + (0x100 >> 2));
  while(*ptr_data == 0) {
    poll_count ++;
  }  /* waiting cpu1 finishes */

  /* time ending */
  time_pr_2 = clock( );
  time_r_sec2 = *ptr_sec;
  time_r_ns2  = *ptr_ns; 
  printf("test17 (cpu1-pit) results\n");

  printf("time = (process) %.2f sec, (real-time) %.2f sec\n",
    ((float) (time_pr_2 - time_pr_1) / 1.0e6),
    (time_r_sec2 - time_r_sec1) +
    (time_r_ns2  - time_r_ns1)  / 1.0e9);
  printf("prime sum %d poll_count %d\n",
    *((int *)COMM_SRAM_HEAD + (0x10c >> 2)), poll_count);
  printf("PTI routine counter %d\n",
    *((int *)COMM_SRAM_HEAD + (0x114 >> 2)));

  free(ptr_void);
  return(0);
}

