// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <unistd.h>

int main(void) {
    unsigned char buffer[64] = {0};
    long page_size = sysconf(_SC_PAGESIZE);

    printf("DMA lab userspace program\n");
    printf("CPU virtual address: %p\n", (void *)buffer);
    printf("Buffer size: %zu bytes\n", sizeof(buffer));
    printf("System page size: %ld bytes\n", page_size);

    return 0;
}
