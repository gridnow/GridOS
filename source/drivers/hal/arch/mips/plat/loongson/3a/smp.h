#ifndef PLAT_LOONGSON3_SMP_H
#define PLAT_LOONGSON3_SMP_H

#define  smp_core_group0_base    smp_group0 
#define  smp_core_group1_base    smp_group1 
#define  smp_core_group2_base    smp_group2 
#define  smp_core_group3_base    smp_group3 

#define  smp_core0_offset  0x0
#define  smp_core1_offset  0x100
#define  smp_core2_offset  0x200
#define  smp_core3_offset  0x300

#define  STATUS0  0x0
#define  EN0      0x4
#define  SET0     0x8
#define  CLEAR0   0xc
#define  STATUS1  0x10
#define  MASK1    0x14
#define  SET1     0x18
#define  CLEAR1   0x1c
#define  BUF      0x20

#define  core0_STATUS0    smp_core_group0_base+smp_core0_offset+0x0             // R
#define  core0_EN0        smp_core_group0_base+smp_core0_offset+0x4             // R/W
#define  core0_SET0       smp_core_group0_base+smp_core0_offset+0x8             // W
#define  core0_CLEAR0     smp_core_group0_base+smp_core0_offset+0xc             // W
#define  core0_STATUS1    smp_core_group0_base+smp_core0_offset+0x10            // R
#define  core0_MASK1      smp_core_group0_base+smp_core0_offset+0x14            // R/W
#define  core0_SET1       smp_core_group0_base+smp_core0_offset+0x18            // W
#define  core0_CLEAR1     smp_core_group0_base+smp_core0_offset+0x1c            // w
#define  core0_BUF        smp_core_group0_base+smp_core0_offset+0x20            // R/w

#define  core1_STATUS0    smp_core_group0_base+smp_core1_offset+0x0             // R
#define  core1_EN0        smp_core_group0_base+smp_core1_offset+0x4             // R/W
#define  core1_SET0       smp_core_group0_base+smp_core1_offset+0x8             // W
#define  core1_CLEAR0     smp_core_group0_base+smp_core1_offset+0xc             // W
#define  core1_STATUS1    smp_core_group0_base+smp_core1_offset+0x10            // R
#define  core1_MASK1      smp_core_group0_base+smp_core1_offset+0x14            // R/W
#define  core1_SET1       smp_core_group0_base+smp_core1_offset+0x18            // W
#define  core1_CLEAR1     smp_core_group0_base+smp_core1_offset+0x1c            // w
#define  core1_BUF        smp_core_group0_base+smp_core1_offset+0x20            // R/w

#define  core2_STATUS0    smp_core_group0_base+smp_core2_offset+0x0             // R
#define  core2_EN0        smp_core_group0_base+smp_core2_offset+0x4             // R/W
#define  core2_SET0       smp_core_group0_base+smp_core2_offset+0x8             // W
#define  core2_CLEAR0     smp_core_group0_base+smp_core2_offset+0xc             // W
#define  core2_STATUS1    smp_core_group0_base+smp_core2_offset+0x10            // R
#define  core2_MASK1      smp_core_group0_base+smp_core2_offset+0x14            // R/W
#define  core2_SET1       smp_core_group0_base+smp_core2_offset+0x18            // W
#define  core2_CLEAR1     smp_core_group0_base+smp_core2_offset+0x1c            // w
#define  core2_BUF        smp_core_group0_base+smp_core2_offset+0x20            // R/w

#define  core3_STATUS0    smp_core_group0_base+smp_core3_offset+0x0             // R
#define  core3_EN0        smp_core_group0_base+smp_core3_offset+0x4             // R/W
#define  core3_SET0       smp_core_group0_base+smp_core3_offset+0x8             // W
#define  core3_CLEAR0     smp_core_group0_base+smp_core3_offset+0xc             // W
#define  core3_STATUS1    smp_core_group0_base+smp_core3_offset+0x10            // R
#define  core3_MASK1      smp_core_group0_base+smp_core3_offset+0x14            // R/W
#define  core3_SET1       smp_core_group0_base+smp_core3_offset+0x18            // W
#define  core3_CLEAR1     smp_core_group0_base+smp_core3_offset+0x1c            // w
#define  core3_BUF        smp_core_group0_base+smp_core3_offset+0x20            // R/w

#define  core4_STATUS0    smp_core_group1_base+smp_core0_offset+0x0             // R
#define  core4_EN0        smp_core_group1_base+smp_core0_offset+0x4             // R/W
#define  core4_SET0       smp_core_group1_base+smp_core0_offset+0x8             // W
#define  core4_CLEAR0     smp_core_group1_base+smp_core0_offset+0xc             // W
#define  core4_STATUS1    smp_core_group1_base+smp_core0_offset+0x10            // R
#define  core4_MASK1      smp_core_group1_base+smp_core0_offset+0x14            // R/W
#define  core4_SET1       smp_core_group1_base+smp_core0_offset+0x18            // W
#define  core4_CLEAR1     smp_core_group1_base+smp_core0_offset+0x1c            // w
#define  core4_BUF        smp_core_group1_base+smp_core0_offset+0x20            // R/w

#define  core5_STATUS0    smp_core_group1_base+smp_core1_offset+0x0             // R
#define  core5_EN0        smp_core_group1_base+smp_core1_offset+0x4             // R/W
#define  core5_SET0       smp_core_group1_base+smp_core1_offset+0x8             // W
#define  core5_CLEAR0     smp_core_group1_base+smp_core1_offset+0xc             // W
#define  core5_STATUS1    smp_core_group1_base+smp_core1_offset+0x10            // R
#define  core5_MASK1      smp_core_group1_base+smp_core1_offset+0x14            // R/W
#define  core5_SET1       smp_core_group1_base+smp_core1_offset+0x18            // W
#define  core5_CLEAR1     smp_core_group1_base+smp_core1_offset+0x1c            // w
#define  core5_BUF        smp_core_group1_base+smp_core1_offset+0x20            // R/w

#define  core6_STATUS0    smp_core_group1_base+smp_core2_offset+0x0             // R
#define  core6_EN0        smp_core_group1_base+smp_core2_offset+0x4             // R/W
#define  core6_SET0       smp_core_group1_base+smp_core2_offset+0x8             // W
#define  core6_CLEAR0     smp_core_group1_base+smp_core2_offset+0xc             // W
#define  core6_STATUS1    smp_core_group1_base+smp_core2_offset+0x10            // R
#define  core6_MASK1      smp_core_group1_base+smp_core2_offset+0x14            // R/W
#define  core6_SET1       smp_core_group1_base+smp_core2_offset+0x18            // W
#define  core6_CLEAR1     smp_core_group1_base+smp_core2_offset+0x1c            // w
#define  core6_BUF        smp_core_group1_base+smp_core2_offset+0x20            // R/w

#define  core7_STATUS0    smp_core_group1_base+smp_core3_offset+0x0             // R
#define  core7_EN0        smp_core_group1_base+smp_core3_offset+0x4             // R/W
#define  core7_SET0       smp_core_group1_base+smp_core3_offset+0x8             // W
#define  core7_CLEAR0     smp_core_group1_base+smp_core3_offset+0xc             // W
#define  core7_STATUS1    smp_core_group1_base+smp_core3_offset+0x10            // R
#define  core7_MASK1      smp_core_group1_base+smp_core3_offset+0x14            // R/W
#define  core7_SET1       smp_core_group1_base+smp_core3_offset+0x18            // W
#define  core7_CLEAR1     smp_core_group1_base+smp_core3_offset+0x1c            // w
#define  core7_BUF        smp_core_group1_base+smp_core3_offset+0x20            // R/w

#define  core8_STATUS0    smp_core_group2_base+smp_core0_offset+0x0             // R
#define  core8_EN0        smp_core_group2_base+smp_core0_offset+0x4             // R/W
#define  core8_SET0       smp_core_group2_base+smp_core0_offset+0x8             // W
#define  core8_CLEAR0     smp_core_group2_base+smp_core0_offset+0xc             // W
#define  core8_STATUS1    smp_core_group2_base+smp_core0_offset+0x10            // R
#define  core8_MASK1      smp_core_group2_base+smp_core0_offset+0x14            // R/W
#define  core8_SET1       smp_core_group2_base+smp_core0_offset+0x18            // W
#define  core8_CLEAR1     smp_core_group2_base+smp_core0_offset+0x1c            // w
#define  core8_BUF        smp_core_group2_base+smp_core0_offset+0x20            // R/w

#define  core9_STATUS0    smp_core_group2_base+smp_core1_offset+0x0             // R
#define  core9_EN0        smp_core_group2_base+smp_core1_offset+0x4             // R/W
#define  core9_SET0       smp_core_group2_base+smp_core1_offset+0x8             // W
#define  core9_CLEAR0     smp_core_group2_base+smp_core1_offset+0xc             // W
#define  core9_STATUS1    smp_core_group2_base+smp_core1_offset+0x10            // R
#define  core9_MASK1      smp_core_group2_base+smp_core1_offset+0x14            // R/W
#define  core9_SET1       smp_core_group2_base+smp_core1_offset+0x18            // W
#define  core9_CLEAR1     smp_core_group2_base+smp_core1_offset+0x1c            // w
#define  core9_BUF        smp_core_group2_base+smp_core1_offset+0x20            // R/w

#define  coreA_STATUS0    smp_core_group2_base+smp_core2_offset+0x0             // R
#define  coreA_EN0        smp_core_group2_base+smp_core2_offset+0x4             // R/W
#define  coreA_SET0       smp_core_group2_base+smp_core2_offset+0x8             // W
#define  coreA_CLEAR0     smp_core_group2_base+smp_core2_offset+0xc             // W
#define  coreA_STATUS1    smp_core_group2_base+smp_core2_offset+0x10            // R
#define  coreA_MASK1      smp_core_group2_base+smp_core2_offset+0x14            // R/W
#define  coreA_SET1       smp_core_group2_base+smp_core2_offset+0x18            // W
#define  coreA_CLEAR1     smp_core_group2_base+smp_core2_offset+0x1c            // w
#define  coreA_BUF        smp_core_group2_base+smp_core2_offset+0x20            // R/w

#define  coreB_STATUS0    smp_core_group2_base+smp_core3_offset+0x0             // R
#define  coreB_EN0        smp_core_group2_base+smp_core3_offset+0x4             // R/W
#define  coreB_SET0       smp_core_group2_base+smp_core3_offset+0x8             // W
#define  coreB_CLEAR0     smp_core_group2_base+smp_core3_offset+0xc             // W
#define  coreB_STATUS1    smp_core_group2_base+smp_core3_offset+0x10            // R
#define  coreB_MASK1      smp_core_group2_base+smp_core3_offset+0x14            // R/W
#define  coreB_SET1       smp_core_group2_base+smp_core3_offset+0x18            // W
#define  coreB_CLEAR1     smp_core_group2_base+smp_core3_offset+0x1c            // w
#define  coreB_BUF        smp_core_group2_base+smp_core3_offset+0x20            // R/w

#define  coreC_STATUS0    smp_core_group3_base+smp_core0_offset+0x0             // R
#define  coreC_EN0        smp_core_group3_base+smp_core0_offset+0x4             // R/W
#define  coreC_SET0       smp_core_group3_base+smp_core0_offset+0x8             // W
#define  coreC_CLEAR0     smp_core_group3_base+smp_core0_offset+0xc             // W
#define  coreC_STATUS1    smp_core_group3_base+smp_core0_offset+0x10            // R
#define  coreC_MASK1      smp_core_group3_base+smp_core0_offset+0x14            // R/W
#define  coreC_SET1       smp_core_group3_base+smp_core0_offset+0x18            // W
#define  coreC_CLEAR1     smp_core_group3_base+smp_core0_offset+0x1c            // w
#define  coreC_BUF        smp_core_group3_base+smp_core0_offset+0x20            // R/w

#define  coreD_STATUS0    smp_core_group3_base+smp_core1_offset+0x0             // R
#define  coreD_EN0        smp_core_group3_base+smp_core1_offset+0x4             // R/W
#define  coreD_SET0       smp_core_group3_base+smp_core1_offset+0x8             // W
#define  coreD_CLEAR0     smp_core_group3_base+smp_core1_offset+0xc             // W
#define  coreD_STATUS1    smp_core_group3_base+smp_core1_offset+0x10            // R
#define  coreD_MASK1      smp_core_group3_base+smp_core1_offset+0x14            // R/W
#define  coreD_SET1       smp_core_group3_base+smp_core1_offset+0x18            // W
#define  coreD_CLEAR1     smp_core_group3_base+smp_core1_offset+0x1c            // w
#define  coreD_BUF        smp_core_group3_base+smp_core1_offset+0x20            // R/w

#define  coreE_STATUS0    smp_core_group3_base+smp_core2_offset+0x0             // R
#define  coreE_EN0        smp_core_group3_base+smp_core2_offset+0x4             // R/W
#define  coreE_SET0       smp_core_group3_base+smp_core2_offset+0x8             // W
#define  coreE_CLEAR0     smp_core_group3_base+smp_core2_offset+0xc             // W
#define  coreE_STATUS1    smp_core_group3_base+smp_core2_offset+0x10            // R
#define  coreE_MASK1      smp_core_group3_base+smp_core2_offset+0x14            // R/W
#define  coreE_SET1       smp_core_group3_base+smp_core2_offset+0x18            // W
#define  coreE_CLEAR1     smp_core_group3_base+smp_core2_offset+0x1c            // w
#define  coreE_BUF        smp_core_group3_base+smp_core2_offset+0x20            // R/w

#define  coreF_STATUS0    smp_core_group3_base+smp_core3_offset+0x0             // R
#define  coreF_EN0        smp_core_group3_base+smp_core3_offset+0x4             // R/W
#define  coreF_SET0       smp_core_group3_base+smp_core3_offset+0x8             // W
#define  coreF_CLEAR0     smp_core_group3_base+smp_core3_offset+0xc             // W
#define  coreF_STATUS1    smp_core_group3_base+smp_core3_offset+0x10            // R
#define  coreF_MASK1      smp_core_group3_base+smp_core3_offset+0x14            // R/W
#define  coreF_SET1       smp_core_group3_base+smp_core3_offset+0x18            // W
#define  coreF_CLEAR1     smp_core_group3_base+smp_core3_offset+0x1c            // w
#define  coreF_BUF        smp_core_group3_base+smp_core3_offset+0x20            // R/w

/* for the loongson platform to register the ops to upper layer */
struct plat_smp_op;
extern struct plat_smp_ops loongson3_smp_ops;

#endif
