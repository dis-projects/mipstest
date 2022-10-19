#ifndef MC_ASM_H
#define MC_ASM_H

#define LEAF(a)  .global a;.type a, %function; a:;
#define END(a)    ;

        .set $C0_STATUS, $12
            .set NMI_IRQ, 0x80000            
        .set $C0_CAUSE, $13
            .set IV_ZERO, 0xFF7FFFFF
        .set $C0_CONFIG, $16
            .set CACHE_EN_MASK, 0xFFFFFFF8
            .set HIGHMEM_CACHE_EN_MASK, 0x8FFFFFFF

	/* MC_CSR */
        .set FLUSH_I,             0x1000
        .set TR_CRAM,             0x2
            
        .macro INIT_REGS
        move	$1, $0
        move	$2, $0
        move	$3, $0
        move	$4, $0
        move	$5, $0
        move	$6, $0
        move	$7, $0
        move	$8, $0
        move	$9, $0
        move	$10, $0
        move	$11, $0
        move	$12, $0
        move	$13, $0
        move	$14, $0
        move	$15, $0
        move	$16, $0
        move	$17, $0
        move	$18, $0
        move	$19, $0
        move	$20, $0
        move	$21, $0
        move	$22, $0
        move	$23, $0
        move	$24, $0
        move	$25, $0  
        .endm

#endif
