#include <stdio.h>

#define DMA_TRANSFER_BYTES    524288
#define DMA_ROWMISS_ESC_OFFSET_BYTES 1024

int dma_mem[(DMA_TRANSFER_BYTES >> 1) +
            ((DMA_ROWMISS_ESC_OFFSET_BYTES + 32) >> 2)];
int main( )
{

  int i, dma_align32_offset, sum;
  dma_align32_offset = 7;

  for(i = 0; i < ((DMA_TRANSFER_BYTES >> 2) + (DMA_TRANSFER_BYTES >> 4));
      i++) {
    dma_mem[i + dma_align32_offset] = (i & 0x3ff) + (i >> 12);
  }
  for(i = 0; i < (DMA_TRANSFER_BYTES >> 2); i++) {
    dma_mem[ i + dma_align32_offset +
             ((DMA_TRANSFER_BYTES + DMA_ROWMISS_ESC_OFFSET_BYTES) >> 2)]
    = dma_mem[ i + dma_align32_offset];
  }
  sum = 0;
  for(i = 0; i < (DMA_TRANSFER_BYTES >> 2); i++) {
    sum += dma_mem[
      ((DMA_TRANSFER_BYTES + DMA_ROWMISS_ESC_OFFSET_BYTES) >> 2) +
       dma_align32_offset + i];
  }
  printf("sum = %x\n", sum);
  return(0);
}
