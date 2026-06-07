#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "pka.h"

// The maximum ROS (RSA operand size) is 3136-bit which is 98 32-bit words. The additional word is set to 0.
#define ROS_SIZE_MAX 99 - 1
#define PKA_RAM_SIZE 894

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PCI_VENDOR_ID_OFFSET 0x00
#define PCI_DEVICE_ID_OFFSET 0x02
#define PCI_COMMAND_OFFSET   0x04
#define PCI_STATUS_OFFSET    0x06

#define PCI_COMMAND_MEMSPACE 1 << 1

void dump_pcidev_config(char *config_path)
{
    int fd;
    uint16_t val;

    fd = open(config_path, O_RDWR | O_SYNC);
    if(fd < 0) {
        handle_error("Error opening configuration space file");
    }

    for(int i = 0; i < 4; ++i) {
        if(pread(fd, &val, sizeof(val), i * 2) < 0) {
            handle_error("Reading from configuration space failed");
        }
        printf("Read from config space at offset 0x%02x: 0x%04x\n", i * 2, val);
    }
    
    close(fd);
}

void read_pcidev_ram(uint32_t *offset, uint32_t n)
{
    if(n > PKA_RAM_SIZE)
        n = PKA_RAM_SIZE;
    for(int i = n - 1; i >= 0; --i) {
        printf("0x%04x ", offset[i]);
    }
    printf("\n");
}

void write_pcidev_ram(uint32_t *offset, uint32_t *data, uint32_t n)
{
    if(n > ROS_SIZE_MAX) {
        fprintf(stderr, "Error writing to device's RAM: src data is too large: %d\n", n);
        exit(EXIT_FAILURE);
    }

    // Initiliaze operand's memory space with 0's
    for(uint32_t i = 0; i <= n; ++i) {
        memset(&offset[i], 0, 4);
    }

    // 4-byte block access is required. Bus error appears otherwise.
    for(uint32_t i = 0; i < n; ++i) {
        memcpy(&offset[i], &data[i], 4);
    }
}

void configure_pcidev(char *config_path)
{
    int fd;
    uint16_t pcidev_status;

    fd = open(config_path, O_RDWR | O_SYNC);
    if(fd < 0) {
        handle_error("Error opening configuration space file");
    }
    pcidev_status = PCI_COMMAND_MEMSPACE;
    if(pwrite(fd, &pcidev_status, sizeof(pcidev_status), PCI_COMMAND_OFFSET) < 0) {
        handle_error("Error writing configuration space file");
    }
    close(fd);
}

uint32_t *map_pci_resource(char *resource_path, size_t resource_path_size, size_t length)
{
    int fd;
    uint32_t *map_addr;

    fd = open(resource_path, O_RDWR | O_SYNC);
    if(fd < 0) {
        fprintf(stderr, "%s - ", resource_path);
        handle_error("Error opening BAR resource file");
    }
    map_addr = mmap(NULL, length * sizeof(uint32_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if(map_addr == MAP_FAILED) {
        fprintf(stderr, "%s - ", resource_path);
        handle_error("Memory mapping of BAR failed");
    }

    return map_addr;
}

void execute_operatione(uint32_t *pka_regs, uint32_t *pka_ram)
{
    uint32_t op1[] = {2};
    uint32_t op2[] = {3};
    uint32_t tmp;

    write_pcidev_ram(&pka_ram[PKA_ARITHMETIC_ADD_IN_OP1], op1, 1);
    write_pcidev_ram(&pka_ram[PKA_ARITHMETIC_ADD_IN_OP2], op2, 1);

    // Simultaneous read and write using |= operator causes Bus error
    tmp = pka_regs[CR];
    pka_regs[CR] = tmp | (PKA_MODE_ARITHMETIC_ADD << PKA_CR_MODE_Pos) | PKA_CR_START;

    // Wait for PROCENDF flag, then clear it and read the result
    while((pka_regs[SR] & PKA_SR_PROCENDF) == 0)
        printf("Waiting for PKA to finish...\n");
    pka_regs[CLRFR] = PKA_CLRFR_PROCENDFC;
    printf("Result: ");
    read_pcidev_ram(&pka_ram[PKA_ARITHMETIC_ADD_OUT_RESULT], 1);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -s <device_bus>:<device_nr> [-v]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int i, fd, opt;
    uint32_t *pka_regs, *pka_ram; // BAR0 and BAR1
    uint32_t pcidev_bus, pcidev_nr, pkadev_id;
    char *token;
    char sysfs_dev_path[100] = {0};
    size_t sysfs_dev_path_len = 0;
    char config_path[100] = {0};
    char resource0_path[100] = {0};
    char resource1_path[100] = {0};
    int dump_conf_regs = 0;

    // Parse options and arguments
    while((opt = getopt(argc, argv, "s:v")) != -1) {
        switch(opt) {
        case 's':
            token = strtok(optarg, ":");
            if(token) pcidev_bus = atoi(token);
            token = strtok(NULL, ":");
            if(token) pcidev_nr = atoi(token);
            sysfs_dev_path_len = snprintf(sysfs_dev_path, sizeof(sysfs_dev_path),
                    "/sys/bus/pci/devices/0000:%02d:%02d.0/", pcidev_bus, pcidev_nr);
            break;
        case 'v':
            dump_conf_regs = 1;
            break;
        }
    }

    // Open config space file and set command register to enable memspace access
    snprintf(config_path, sizeof(config_path), "%s%s", sysfs_dev_path, "config");
    configure_pcidev(config_path);

    // Dump config and exit
    if(dump_conf_regs) {
        dump_pcidev_config(config_path);
        exit(EXIT_SUCCESS);
    }

    // Open and map resource0 file
    snprintf(resource0_path, sizeof(resource0_path), "%s%s", sysfs_dev_path, "resource0");
    pka_regs = map_pci_resource(resource0_path, sizeof(resource0_path), REGS_NUM);

    pkadev_id = pka_regs[ID];
    printf("Device ID: 0x%08x\n", pkadev_id);

    // Open and map resource1 file
    snprintf(resource1_path, sizeof(resource1_path), "%s%s", sysfs_dev_path, "resource1");
    pka_ram = map_pci_resource(resource1_path, sizeof(resource1_path), PKA_RAM_SIZE);

    execute_operatione(pka_regs, pka_ram);

    munmap(pka_regs, REGS_NUM * sizeof(uint32_t));
    munmap(pka_ram, PKA_RAM_SIZE * sizeof(uint32_t));

    return 0;
}
