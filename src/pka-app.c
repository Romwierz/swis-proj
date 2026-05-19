#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -s <device_bus>:<device_nr>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int i, fd, opt;
    uint32_t *bar0, *bar1;
    uint32_t pcidev_bus, pcidev_nr, pkadev_id;
    char *token;
    char sysfs_dev_path[100] = {0};
    size_t sysfs_dev_path_len = 0;
    char resource0_path[100] = {0};

    // Parse options and arguments
    while((opt = getopt(argc, argv, "s:")) != -1) {
        switch(opt) {
        case 's':
            token = strtok(optarg, ":");
            if(token) pcidev_bus = atoi(token);
            token = strtok(NULL, ":");
            if(token) pcidev_nr = atoi(token);
            sysfs_dev_path_len = snprintf(sysfs_dev_path, sizeof(sysfs_dev_path),
                    "/sys/bus/pci/devices/0000:%02d:%02d.0/", pcidev_bus, pcidev_nr);
        }
    }

    // Open and map resource0 file
    snprintf(resource0_path, sizeof(resource0_path), "%s%s", sysfs_dev_path, "resource0");
    printf("%s\n", resource0_path);
    fd = open(resource0_path, O_RDWR | O_SYNC);
    if(fd < 0) {
        perror("Error opening BAR0 resource file");
        return -1;
    }
    bar0 = mmap(NULL, 256, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if(bar0 == MAP_FAILED) {
        perror("Memory mapping of BAR0 failed");
        return -1;
    }

    pkadev_id = bar0[3];
    printf("Device ID: 0x%08x\n", pkadev_id);

    munmap(bar0, 256);

    return 0;
}
