#pragma once
/*******************  PKA procedure to perform an operation  *************/
/*
   Enabling/disabling PKA
   **********************
   Setting the EN bit to 1 in PKA_CR register enables the PKA peripheral. When EN = 0, the
   PKA peripheral is kept under reset, with PKA memory still accessible.

   Executing a PKA operation
   *************************
   1. Load initial data into the PKA RAM.
   2. Write in the MODE field of PKA_CR register, specifying the operation which is to be
      executed and then assert the START bit, also in PKA_CR register.
   3. Wait until the PROCENDF bit in the PKA_SR register is set to “1”, indicating that the
      computation is complete.
   4. Read the result data from the PKA internal RAM, then clear PROCENDF bit by setting
      PROCENDFC bit in PKA_CLRFR.
   Note: When PKA is busy (BUSY = 1) any access by the application to PKA RAM is ignored, and
   the flag RAMERRF is set in PKA_SR.
*/

/*******************  PKA 32-bit registers  *****************************/
enum {
    CR,                 /*!< PKA control register,                 Address offset: 0x00 */
    SR,                 /*!< PKA status register,                  Address offset: 0x04 */
    CLRFR,              /*!< PKA clear flag register,              Address offset: 0x08 */
    ID,
    RAM_ADDR_OFFSET,    /*!< RAM address to read from using register */
    RAM_DATA,           /*!< Data read from RAM_ADDR_OFFSET */
    REGS_NUM,
};

/*******************  Bits definition for PKA_CR register  **************/
#define PKA_CR_EN_Pos              (0U)
#define PKA_CR_EN_Msk              (0x1UL << PKA_CR_EN_Pos)                /*!< 0x00000001 */
#define PKA_CR_EN                  PKA_CR_EN_Msk                           /*!< PKA enable */
#define PKA_CR_START_Pos           (1U)
#define PKA_CR_START_Msk           (0x1UL << PKA_CR_START_Pos)             /*!< 0x00000002 */
#define PKA_CR_START               PKA_CR_START_Msk                        /*!< Start operation */
#define PKA_CR_MODE_Pos            (8U)
#define PKA_CR_MODE_Msk            (0x3FUL << PKA_CR_MODE_Pos)             /*!< 0x00003F00 */
#define PKA_CR_MODE                PKA_CR_MODE_Msk                         /*!< MODE[5:0] PKA operation code */
#define PKA_CR_MODE_0              (0x01U << PKA_CR_MODE_Pos)              /*!< 0x00000100 */
#define PKA_CR_MODE_1              (0x02U << PKA_CR_MODE_Pos)              /*!< 0x00000200 */
#define PKA_CR_MODE_2              (0x04U << PKA_CR_MODE_Pos)              /*!< 0x00000400 */
#define PKA_CR_MODE_3              (0x08U << PKA_CR_MODE_Pos)              /*!< 0x00000800 */
#define PKA_CR_MODE_4              (0x10U << PKA_CR_MODE_Pos)              /*!< 0x00001000 */
#define PKA_CR_MODE_5              (0x20U << PKA_CR_MODE_Pos)              /*!< 0x00002000 */
#define PKA_CR_PROCENDIE_Pos       (17U)
#define PKA_CR_PROCENDIE_Msk       (0x1UL << PKA_CR_PROCENDIE_Pos)         /*!< 0x00020000 */
#define PKA_CR_PROCENDIE           PKA_CR_PROCENDIE_Msk                    /*!< End of operation interrupt enable */
#define PKA_CR_RAMERRIE_Pos        (19U)
#define PKA_CR_RAMERRIE_Msk        (0x1UL << PKA_CR_RAMERRIE_Pos)          /*!< 0x00080000 */
#define PKA_CR_RAMERRIE            PKA_CR_RAMERRIE_Msk                     /*!< RAM error interrupt enable */
#define PKA_CR_ADDRERRIE_Pos       (20U)
#define PKA_CR_ADDRERRIE_Msk       (0x1UL << PKA_CR_ADDRERRIE_Pos)         /*!< 0x00100000 */
#define PKA_CR_ADDRERRIE           PKA_CR_ADDRERRIE_Msk                    /*!< RAM error interrupt enable */

/*******************  Bits definition for PKA_SR register  **************/
#define PKA_SR_BUSY_Pos            (16U)
#define PKA_SR_BUSY_Msk            (0x1UL << PKA_SR_BUSY_Pos)              /*!< 0x00010000 */
#define PKA_SR_BUSY                PKA_SR_BUSY_Msk                         /*!< PKA operation is in progress */
#define PKA_SR_PROCENDF_Pos        (17U)
#define PKA_SR_PROCENDF_Msk        (0x1UL << PKA_SR_PROCENDF_Pos)          /*!< 0x00020000 */
#define PKA_SR_PROCENDF            PKA_SR_PROCENDF_Msk                     /*!< PKA end of operation flag */
#define PKA_SR_RAMERRF_Pos         (19U)
#define PKA_SR_RAMERRF_Msk         (0x1UL << PKA_SR_RAMERRF_Pos)           /*!< 0x00080000 */
#define PKA_SR_RAMERRF             PKA_SR_RAMERRF_Msk                      /*!< PKA RAM error flag */
#define PKA_SR_ADDRERRF_Pos        (20U)
#define PKA_SR_ADDRERRF_Msk        (0x1UL << PKA_SR_ADDRERRF_Pos)          /*!< 0x00100000 */
#define PKA_SR_ADDRERRF            PKA_SR_ADDRERRF_Msk                     /*!< Address error flag */

/*******************  Bits definition for PKA_CLRFR register  **************/
#define PKA_CLRFR_PROCENDFC_Pos    (17U)
#define PKA_CLRFR_PROCENDFC_Msk    (0x1UL << PKA_CLRFR_PROCENDFC_Pos)      /*!< 0x00020000 */
#define PKA_CLRFR_PROCENDFC        PKA_CLRFR_PROCENDFC_Msk                 /*!< Clear PKA end of operation flag */
#define PKA_CLRFR_RAMERRFC_Pos     (19U)
#define PKA_CLRFR_RAMERRFC_Msk     (0x1UL << PKA_CLRFR_RAMERRFC_Pos)       /*!< 0x00080000 */
#define PKA_CLRFR_RAMERRFC         PKA_CLRFR_RAMERRFC_Msk                  /*!< Clear PKA RAM error flag */
#define PKA_CLRFR_ADDRERRFC_Pos    (20U)
#define PKA_CLRFR_ADDRERRFC_Msk    (0x1UL << PKA_CLRFR_ADDRERRFC_Pos)      /*!< 0x00100000 */
#define PKA_CLRFR_ADDRERRFC        PKA_CLRFR_ADDRERRFC_Msk                 /*!< Clear address error flag */

/*******************  Bits definition for PKA RAM  *************************/
#define PKA_RAM_OFFSET                            0x400U                           /*!< PKA RAM address offset */

/* Compute Montgomery parameter input data */
#define PKA_MONTGOMERY_PARAM_IN_MOD_NB_BITS       ((0x404U - PKA_RAM_OFFSET)>>2)   /*!< Input modulus number of bits */
#define PKA_MONTGOMERY_PARAM_IN_MODULUS           ((0xD5CU - PKA_RAM_OFFSET)>>2)   /*!< Input modulus */

/* Compute Montgomery parameter output data */
#define PKA_MONTGOMERY_PARAM_OUT_PARAMETER        ((0x594U - PKA_RAM_OFFSET)>>2)   /*!< Output Montgomery parameter */

/* Arithmetic addition input data */
#define PKA_ARITHMETIC_ADD_NB_BITS                ((0x404U - PKA_RAM_OFFSET)>>2)   /*!< Input operand number of bits */
#define PKA_ARITHMETIC_ADD_IN_OP1                 ((0x8B4U - PKA_RAM_OFFSET)>>2)   /*!< Input operand op1 */
#define PKA_ARITHMETIC_ADD_IN_OP2                 ((0xA44U - PKA_RAM_OFFSET)>>2)   /*!< Input operand op2 */

/* Arithmetic addition output data */
#define PKA_ARITHMETIC_ADD_OUT_RESULT             ((0xBD0U - PKA_RAM_OFFSET)>>2)   /*!< Output result */

/* Arithmetic subtraction input data */
#define PKA_ARITHMETIC_SUB_NB_BITS                ((0x404U - PKA_RAM_OFFSET)>>2)   /*!< Input operand number of bits */
#define PKA_ARITHMETIC_SUB_IN_OP1                 ((0x8B4U - PKA_RAM_OFFSET)>>2)   /*!< Input operand op1 */
#define PKA_ARITHMETIC_SUB_IN_OP2                 ((0xA44U - PKA_RAM_OFFSET)>>2)   /*!< Input operand op2 */

/* Arithmetic subtraction output data */
#define PKA_ARITHMETIC_SUB_OUT_RESULT             ((0xBD0U - PKA_RAM_OFFSET)>>2)   /*!< Output result */

/*******************  Bits definition for PKA Modes  *************************/
#define PKA_MODE_MONTGOMERY_PARAM                 (0x00000001U)
#define PKA_MODE_MODULAR_EXP                      (0x00000000U)
#define PKA_MODE_MODULAR_EXP_FAST_MODE            (0x00000002U)
#define PKA_MODE_ECC_MUL                          (0x00000020U)
#define PKA_MODE_ECC_MUL_FAST_MODE                (0x00000022U)
#define PKA_MODE_ECDSA_SIGNATURE                  (0x00000024U)
#define PKA_MODE_ECDSA_VERIFICATION               (0x00000026U)
#define PKA_MODE_POINT_CHECK                      (0x00000028U)
#define PKA_MODE_RSA_CRT_EXP                      (0x00000007U)
#define PKA_MODE_MODULAR_INV                      (0x00000008U)
#define PKA_MODE_ARITHMETIC_ADD                   (0x00000009U)
#define PKA_MODE_ARITHMETIC_SUB                   (0x0000000AU)
#define PKA_MODE_ARITHMETIC_MUL                   (0x0000000BU)
#define PKA_MODE_COMPARISON                       (0x0000000CU)
#define PKA_MODE_MODULAR_RED                      (0x0000000DU)
#define PKA_MODE_MODULAR_ADD                      (0x0000000EU)
#define PKA_MODE_MODULAR_SUB                      (0x0000000FU)
#define PKA_MODE_MONTGOMERY_MUL                   (0x00000010U)
