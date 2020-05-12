#include <stdio.h>
#define CCR 0xabcd00c0
/* ------------------------------------ */
/* 03_14kbyte.c (cache_test_03.c) 	*/
/* 	Osamu Nishii Feb-03-2015	*/
/* ------------------------------------ */

int main( );
#define b(p1,p2,p3) ((((p1 << 2) ^ p2 ^ (p3 << 5) ^ (p3 >> 19)) + 9999) & 0x00ffffff)

int main( )
{
  int i, key, par, l_loop_limit; /* integer for calc */
  int ic_onmode, i_dummy, *ptr_1, retd; /* control */

  par = 0x0000aaaa;

  printf("input repeat count (decimal 1-10000) \n");
  scanf("%d", &l_loop_limit);
  printf("input init number (decimal 0-99999) \n");
  scanf("%d", &key);

  printf("cache_test_03: (14kB code test) select cache on/off (1=on, 0=off) \n");
  scanf("%d", &ic_onmode);

  if(ic_onmode == 1) {
    ptr_1 = (int *) CCR; *ptr_1 = 0x00000001;
    for(i_dummy = 0; i_dummy < 100; i_dummy ++) { }
    retd = *ptr_1; printf("set CCR %08x\n", retd);
    printf("cache_test_01: ic is enabled\n");
  }

  for(i = 0; i < l_loop_limit; i++) {
  par = b(key, 0, par);
  par = b(key, 6314, par);
  par = b(key, 2609, par);
  par = b(key, 8923, par);
  par = b(key, 5218, par);
  par = b(key, 1513, par);
  par = b(key, 7827, par);
  par = b(key, 4122, par);
  par = b(key, 417, par);
  par = b(key, 6731, par);
  par = b(key, 3026, par);
  par = b(key, 9340, par);
  par = b(key, 5635, par);
  par = b(key, 1930, par);
  par = b(key, 8244, par);
  par = b(key, 4539, par);
  par = b(key, 834, par);
  par = b(key, 7148, par);
  par = b(key, 3443, par);
  par = b(key, 9757, par);
  par = b(key, 6052, par);
  par = b(key, 2347, par);
  par = b(key, 8661, par);
  par = b(key, 4956, par);
  par = b(key, 1251, par);
  par = b(key, 7565, par);
  par = b(key, 3860, par);
  par = b(key, 155, par);
  par = b(key, 6469, par);
  par = b(key, 2764, par);
  par = b(key, 9078, par);
  par = b(key, 5373, par);
  par = b(key, 1668, par);
  par = b(key, 7982, par);
  par = b(key, 4277, par);
  par = b(key, 572, par);
  par = b(key, 6886, par);
  par = b(key, 3181, par);
  par = b(key, 9495, par);
  par = b(key, 5790, par);
  par = b(key, 2085, par);
  par = b(key, 8399, par);
  par = b(key, 4694, par);
  par = b(key, 989, par);
  par = b(key, 7303, par);
  par = b(key, 3598, par);
  par = b(key, 9912, par);
  par = b(key, 6207, par);
  par = b(key, 2502, par);
  par = b(key, 8816, par);
  par = b(key, 5111, par);
  par = b(key, 1406, par);
  par = b(key, 7720, par);
  par = b(key, 4015, par);
  par = b(key, 310, par);
  par = b(key, 6624, par);
  par = b(key, 2919, par);
  par = b(key, 9233, par);
  par = b(key, 5528, par);
  par = b(key, 1823, par);
  par = b(key, 8137, par);
  par = b(key, 4432, par);
  par = b(key, 727, par);
  par = b(key, 7041, par);
  par = b(key, 3336, par);
  par = b(key, 9650, par);
  par = b(key, 5945, par);
  par = b(key, 2240, par);
  par = b(key, 8554, par);
  par = b(key, 4849, par);
  par = b(key, 1144, par);
  par = b(key, 7458, par);
  par = b(key, 3753, par);
  par = b(key, 48, par);
  par = b(key, 6362, par);
  par = b(key, 2657, par);
  par = b(key, 8971, par);
  par = b(key, 5266, par);
  par = b(key, 1561, par);
  par = b(key, 7875, par);
  par = b(key, 4170, par);
  par = b(key, 465, par);
  par = b(key, 6779, par);
  par = b(key, 3074, par);
  par = b(key, 9388, par);
  par = b(key, 5683, par);
  par = b(key, 1978, par);
  par = b(key, 8292, par);
  par = b(key, 4587, par);
  par = b(key, 882, par);
  par = b(key, 7196, par);
  par = b(key, 3491, par);
  par = b(key, 9805, par);
  par = b(key, 6100, par);
  par = b(key, 2395, par);
  par = b(key, 8709, par);
  par = b(key, 5004, par);
  par = b(key, 1299, par);
  par = b(key, 7613, par);
  par = b(key, 3908, par);
  par = b(key, 203, par);
  par = b(key, 6517, par);
  par = b(key, 2812, par);
  par = b(key, 9126, par);
  par = b(key, 5421, par);
  par = b(key, 1716, par);
  par = b(key, 8030, par);
  par = b(key, 4325, par);
  par = b(key, 620, par);
  par = b(key, 6934, par);
  par = b(key, 3229, par);
  par = b(key, 9543, par);
  par = b(key, 5838, par);
  par = b(key, 2133, par);
  par = b(key, 8447, par);
  par = b(key, 4742, par);
  par = b(key, 1037, par);
  par = b(key, 7351, par);
  par = b(key, 3646, par);
  par = b(key, 9960, par);
  par = b(key, 6255, par);
  par = b(key, 2550, par);
  par = b(key, 8864, par);
  par = b(key, 5159, par);
  par = b(key, 1454, par);
  par = b(key, 7768, par);
  par = b(key, 4063, par);
  par = b(key, 358, par);
  par = b(key, 6672, par);
  par = b(key, 2967, par);
  par = b(key, 9281, par);
  par = b(key, 5576, par);
  par = b(key, 1871, par);
  par = b(key, 8185, par);
  par = b(key, 4480, par);
  par = b(key, 775, par);
  par = b(key, 7089, par);
  par = b(key, 3384, par);
  par = b(key, 9698, par);
  par = b(key, 5993, par);
  par = b(key, 2288, par);
  par = b(key, 8602, par);
  par = b(key, 4897, par);
  par = b(key, 1192, par);
  par = b(key, 7506, par);
  par = b(key, 3801, par);
  par = b(key, 96, par);
  par = b(key, 6410, par);
  par = b(key, 2705, par);
  par = b(key, 9019, par);
  par = b(key, 5314, par);
  par = b(key, 1609, par);
  par = b(key, 7923, par);
  par = b(key, 4218, par);
  par = b(key, 513, par);
  par = b(key, 6827, par);
  par = b(key, 3122, par);
  par = b(key, 9436, par);
  par = b(key, 5731, par);
  par = b(key, 2026, par);
  par = b(key, 8340, par);
  par = b(key, 4635, par);
  par = b(key, 930, par);
  par = b(key, 7244, par);
  par = b(key, 3539, par);
  par = b(key, 9853, par);
  par = b(key, 6148, par);
  par = b(key, 2443, par);
  par = b(key, 8757, par);
  par = b(key, 5052, par);
  par = b(key, 1347, par);
  par = b(key, 7661, par);
  par = b(key, 3956, par);
  par = b(key, 251, par);
  par = b(key, 6565, par);
  par = b(key, 2860, par);
  par = b(key, 9174, par);
  par = b(key, 5469, par);
  par = b(key, 1764, par);
  par = b(key, 8078, par);
  par = b(key, 4373, par);
  par = b(key, 668, par);
  par = b(key, 6982, par);
  par = b(key, 3277, par);
  par = b(key, 9591, par);
  par = b(key, 5886, par);
  par = b(key, 2181, par);
  par = b(key, 8495, par);
  par = b(key, 4790, par);
  par = b(key, 1085, par);
  par = b(key, 7399, par);
  par = b(key, 3694, par);
  par = b(key, 10008, par);
  par = b(key, 6303, par);
  par = b(key, 2598, par);
  par = b(key, 8912, par);
  par = b(key, 5207, par);
  par = b(key, 1502, par);
  par = b(key, 7816, par);
  par = b(key, 4111, par);
  par = b(key, 406, par);
  par = b(key, 6720, par);
  par = b(key, 3015, par);
  par = b(key, 9329, par);
  par = b(key, 5624, par);
  }
  printf("key %d par %x\n", key, par);
  if(ic_onmode == 1) {
    for(i_dummy = 0; i_dummy < 100; i_dummy ++) { }
    ptr_1 = (int *) CCR; *ptr_1 = 0x00000000;
    printf("cache_test_02: ic is disabled\n");
  }
  return(0);
}

