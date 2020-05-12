#ifndef DMA_DMAC_H
#define DMA_DMAC_H

#define uint32_t   unsigned int
/* #include <inttypes.h> */

struct dmac_ch_regs {
  uint32_t sar; // Source Address Reg
  uint32_t dar; // Destination Address Reg
  uint32_t tcr; // Transfer Count Reg
  uint32_t chcr; // Channel Control Reg
  // CHCR: MID[31-26] CHAIN[23] RLD[21-20] IEC[19-18] CIF[17] TB[16]
  //       DM[15-14] SM[13-12] RS[9-8] TS[5-3] IE[2] TE[1] DE[0]
};

// MID: Request Module ID
#define DMAC_CHCR_MID_SHIFT          26
#define DMAC_CHCR_MID_MASK           (0x3F << DMAC_CHCR_MID_SHIFT)

// CHAIN: Chain Control
#define DMAC_CHCR_CHAIN_SHIFT        23
#define DMAC_CHCR_CHAIN_MASK         (1 << DMAC_CHCR_CHAIN_SHIFT)
#define DMAC_CHCR_CHAIN_EN           (1 << DMAC_CHCR_CHAIN_SHIFT)
#define DMAC_CHCR_CHAIN_DIS          (0 << DMAC_CHCR_CHAIN_SHIFT)

// RLD: Reload Control
#define DMAC_CHCR_RLD_SHIFT          20
#define DMAC_CHCR_RLD_MASK           (3 << DMAC_CHCR_RLD_SHIFT)
#define DMAC_CHCR_RLD_NO_RELOAD      (0 << DMAC_CHCR_RLD_SHIFT)
#define DMAC_CHCR_RLD_RELOAD_ON_END  (1 << DMAC_CHCR_RLD_SHIFT)
#define DMAC_CHCR_RLD_RELOAD_ON_HALF (2 << DMAC_CHCR_RLD_SHIFT)

// IEC: Cyclic Interrupt Enable
#define DMAC_CHCR_IEC_SHIFT          18
#define DMAC_CHCR_IEC_MASK           (3 << DMAC_CHCR_IEC_SHIFT)
#define DMAC_CHCR_IEC_INT_DISABLED   (0 << DMAC_CHCR_IEC_SHIFT)
#define DMAC_CHCR_IEC_INT_ON_END     (1 << DMAC_CHCR_IEC_SHIFT)
#define DMAC_CHCR_IEC_INT_ON_HALF    (2 << DMAC_CHCR_IEC_SHIFT)
#define DMAC_CHCR_IEC_INT_ON_QUAD    (3 << DMAC_CHCR_IEC_SHIFT)

// CIF: Cyclic Interrupt End Flag
#define DMAC_CHCR_CIF              0x20000
// TB: Transfer Bus Mode
#define DMAC_CHCR_BURST_MODE       0x10000

// DM: Destination Address Mode
#define DMAC_CHCR_DEST_MODE_SHIFT       14
#define DMAC_CHCR_DEST_MODE_MASK        (3 << DMAC_CHCR_DEST_MODE_SHIFT)
#define DMAC_CHCR_DEST_MODE_FIXED       (0 << DMAC_CHCR_DEST_MODE_SHIFT)
#define DMAC_CHCR_DEST_MODE_INC         (1 << DMAC_CHCR_DEST_MODE_SHIFT)
#define DMAC_CHCR_DEST_MODE_DEC         (2 << DMAC_CHCR_DEST_MODE_SHIFT)

// SM: Source Address Mode
#define DMAC_CHCR_SRC_MODE_SHIFT        12
#define DMAC_CHCR_SRC_MODE_MASK         (3 << DMAC_CHCR_SRC_MODE_SHIFT)
#define DMAC_CHCR_SRC_MODE_FIXED        (0 << DMAC_CHCR_SRC_MODE_SHIFT)
#define DMAC_CHCR_SRC_MODE_INC          (1 << DMAC_CHCR_SRC_MODE_SHIFT)
#define DMAC_CHCR_SRC_MODE_DEC          (2 << DMAC_CHCR_SRC_MODE_SHIFT)

// RS: Request Source
#define DMAC_CHCR_REQ_SRC_SHIFT         8
#define DMAC_CHCR_REQ_SRC_MASK          (3 << DMAC_CHCR_REQ_SRC_SHIFT)
#define DMAC_CHCR_REQ_SRC_PROG_REQ      (0 << DMAC_CHCR_REQ_SRC_SHIFT)
#define DMAC_CHCR_REQ_SRC_PERIF_REQ     (1 << DMAC_CHCR_REQ_SRC_SHIFT)

// TS: Transfer Size
#define DMAC_CHCR_TRANSFER_SIZE_SHIFT   3
#define DMAC_CHCR_TRANSFER_SIZE_MASK    (7 << DMAC_CHCR_TRANSFER_SIZE_SHIFT)
#define DMAC_CHCR_TRANSFER_SIZE_1_BYTE  (0 << DMAC_CHCR_TRANSFER_SIZE_SHIFT)
#define DMAC_CHCR_TRANSFER_SIZE_2_BYTE  (1 << DMAC_CHCR_TRANSFER_SIZE_SHIFT)
#define DMAC_CHCR_TRANSFER_SIZE_4_BYTE  (2 << DMAC_CHCR_TRANSFER_SIZE_SHIFT)
#define DMAC_CHCR_TRANSFER_SIZE_16_BYTE (3 << DMAC_CHCR_TRANSFER_SIZE_SHIFT)
#define DMAC_CHCR_TRANSFER_SIZE_32_BYTE (4 << DMAC_CHCR_TRANSFER_SIZE_SHIFT)

// IE: Interrupt Enable
#define DMAC_CHCR_INT_EN       0x4
// TE: Transfer End Status
#define DMAC_CHCR_TRANSFER_END 0x2
// DE: DME Enable
#define DMAC_CHCR_DME_EN       0x1

#define DMAC_NUM_CHANNELS 64

struct dmac_regs {

  uint32_t dmaor; // DMA Operation Reg
  uint32_t reserved[3];
  uint32_t tesr0; // Transfer End Status Reg. Contains TE of all channels
  uint32_t tesr1;
  uint32_t cifsr0; // Cyclic Interrupt Status Reg. Contains CIF of all channels
  uint32_t cifsr1;

  struct dmac_ch_regs channels[DMAC_NUM_CHANNELS];
  uint32_t rcr[DMAC_NUM_CHANNELS]; // Reload Count Regs. Optionally reloaded into
  // the TCR regs
  uint32_t lacp[DMAC_NUM_CHANNELS]; // Linked Array Control Pointer
};

#define DMAC_REGS ((volatile struct dmac_regs *) 0xabcf0000)

#define DMAC_DMAOR_PRIORITY_MODE_SHIFT  8
#define DMAC_DMAOR_PRIORITY_MODE_MASK   (3 << DMAC_DMAOR_PRIORITY_MODE_SHIFT)
#define DMAC_DMAOR_PRIORITY_MODE_FIXED1 (0 << DMAC_DMAOR_PRIORITY_MODE_SHIFT)
#define DMAC_DMAOR_PRIORITY_MODE_FIXED2 (1 << DMAC_DMAOR_PRIORITY_MODE_SHIFT)
#define DMAC_DMAOR_PRIORITY_MODE_RR     (2 << DMAC_DMAOR_PRIORITY_MODE_SHIFT)

#define DMAC_DMAOR_POSTED_WRITE 0x8
#define DMAC_DMAOR_ADDR_ERROR   0x4
#define DMAC_DMAOR_ENABLE       0x1

#endif

