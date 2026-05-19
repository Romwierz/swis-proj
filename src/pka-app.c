#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "pka.h"

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PCI_VENDOR_ID_OFFSET 0x00
#define PCI_DEVICE_ID_OFFSET 0x02
#define PCI_COMMAND_OFFSET   0x04
#define PCI_STATUS_OFFSET    0x06

#define PCI_COMMAND_MEMSPACE 1 << 1

void dump_pcidev_config(int fd)
{
    uint16_t val;
    for(int i = 0; i < 4; ++i) {
        if(pread(fd, &val, sizeof(val), i * 2) < 0) {
            handle_error("Reading from configuration space failed");
        }
        printf("Read from config space at offset 0x%02x: 0x%04x\n", i * 2, val);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -s <device_bus>:<device_nr>\n", argv[0]);
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
    uint16_t pcidev_status;
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
    fd = open(config_path, O_RDWR | O_SYNC);
    if(fd < 0) {
        handle_error("Error opening configuration space file");
    }
    if(dump_conf_regs) {
        dump_pcidev_config(fd);
        close(fd);
        exit(EXIT_SUCCESS);
    }
    pcidev_status = PCI_COMMAND_MEMSPACE;
    if(pwrite(fd, &pcidev_status, sizeof(pcidev_status), PCI_COMMAND_OFFSET) < 0) {
        handle_error("Error writing configuration space file");
    }
    close(fd);

    // Open and map resource0 file
    snprintf(resource0_path, sizeof(resource0_path), "%s%s", sysfs_dev_path, "resource0");
    fd = open(resource0_path, O_RDWR | O_SYNC);
    if(fd < 0) {
        handle_error("Error opening BAR0 resource file");
    }
    pka_regs = mmap(NULL, REGS_NUM * sizeof(uint32_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if(pka_regs == MAP_FAILED) {
        perror("Memory mapping of BAR0 failed");
        return -1;
    }

    pkadev_id = pka_regs[ID];
    printf("Device ID: 0x%08x\n", pkadev_id);

    munmap(pka_regs, REGS_NUM * sizeof(uint32_t));

    return 0;
}
