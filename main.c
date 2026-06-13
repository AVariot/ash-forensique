
#include "./main.h"

/*
    Algo complex = linear O(N)
    Fragmentation is not handle
*/

// - argv[1] == file to read
// - argv[2] == folder to put get back files
// -- /dev/sdb

int main(int argc, char **argv) {
    
    // TODO: command handler
    // if (argc != 2)
    //     return -1;
    // -s = stripped
    // -n = not stripped
    // read_metal(argv[1], argv[2]);
    (void)argc;
    reverse(argv[1]);

    return 0;
}
