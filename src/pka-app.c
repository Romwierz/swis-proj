#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s some_arg\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("Success! The argument is: %s\n", argv[1]);
    return 0;
}
