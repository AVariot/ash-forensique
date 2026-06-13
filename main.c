
#include "./main.h"

/*
    Algo complex = linear O(N)
    Fragmentation is not handle
*/

// - argv[1] == file to read
// - argv[2] == folder to put get back files
// -- /dev/sdb

int main(int argc, char **argv) {
    if (argc != 3)
        return -1;

    read_metal(argv[1], argv[2]);

    return 0;
}
