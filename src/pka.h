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
