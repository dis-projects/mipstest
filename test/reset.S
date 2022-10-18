#/* 
# * File:   reset.s
# * Author: oen
# *
# * Created on May 18, 2015, 12:42 PM
# */
        .text
        .set    noreorder
        .set	mips32
        .set    noat
#include "mc_asm.h"
#define MC_CSR_REG              0xa1928000
        
        .extern _start

  #Exception Vector 0x000 RESET
    .org 0x000
LEAF(_reset)
    mfc0        $k1, $C0_STATUS
    li          $k0, NMI_IRQ
    and         $k0, $k1, $k0
    beqz        $k0, _cold_reset
    nop
_nmi_reset:
    li          $k0, NMI_IRQ
    xor         $k1, $k1, $k0
    mtc0        $k1, $C0_STATUS        #reset nmi irq
    nop
    nop
_cold_reset:
    li          $k1, 0x000000            #disable interrupts, IE[0] = 0, ERL[2] = 0, BEV[22] = 0
    mtc0        $k1, $C0_STATUS
    li          $k0, IV_ZERO             # IV = 0
    mfc0        $k1, $C0_CAUSE
    and         $k1, $k1, $k0
    mtc0        $k1, $C0_CAUSE
    li          $k1, MC_CSR_REG              #interrupt vector to CRAM (BEV = 0)
    lw          $k0, ($k1)
    ori         $k0, TR_CRAM
    sw          $k0, ($k1)
    mfc0        $k1, $C0_CONFIG        #enable program cache
    li          $k0, CACHE_EN_MASK
    and         $k1, $k1, $k0
    li          $k0, HIGHMEM_CACHE_EN_MASK
    and         $k1, $k1, $k0
    mtc0        $k1, $C0_CONFIG
_init_regs:
    INIT_REGS
    la          $k0, _start
    jalr        $k0                     #jump to CRT0
    nop
_loop:
    nop
    b           _loop
    nop
END(_reset)

  #Exception Vector 0x180
    .org 0x180
    lui $k1,0xa192
    ori $k1,0x8000
    li  $k0,1
    sw  $k0,4($k1)
    li  $k0,4
    sw  $k0,0x4000($k1)
    sync
    sync
1:
    lw  $k0,4($k1)
    bnez $k0, 1b
    nop
    eret
#    b           1f
    nop
  #Exception Vector 0x200
    .org 0x200
    b           1f
    nop
  #Exception Vector 0x380
    .org 0x380
    b           1f
    nop
    .org 0x400  
1:  la          $k0, MC_CSR_REG
    lw          $k1, ($k0)
    ori         $k1, FLUSH_I /* + FLUSH_D */
    sw          $k1, ($k0)
    
#    la          $k0, _interrupt_entry
#    jr          $k0
2:
    nop
    b	2b
    nop

        .set at
	.set reorder
