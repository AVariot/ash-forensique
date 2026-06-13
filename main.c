
#include "./main.h"

/*
    Algo complex = linear O(N)
    Fragmentation is not handle
*/
/*
    TODO:
        - Create a struct array of function if more command
        - Add argument type as -e ...
*/

// - argv[1] == file to read
// - argv[2] == folder to put get back files
// -- /dev/sdb

static int help() {
    printf("Usage : ./getLostData [OPTION]... [FILES]\n");
    printf("Exemple : ./getLostData getLostData\n\n");
    printf("Files recovery:\n");
    printf("  disk_to_read destination_to_put_files\n");
    printf("\nReverse application:\n");
    printf("  executable to analyse\n");

    return 0;
}

static int parse_args(char **argv) {
    // argv[x]
    // [1]: mode (--img || --rev || --help)
    // [2]: mode == img: path_to_read ? executable
    // [3]: mode == img: dest_path ? null

    if (argv[1] && strcmp(argv[1], "--img") == 0) {
        if (argv[2] && argv[3]) {
            return read_metal(argv[2], argv[3]);
        } else {
            printf("Args error: Need 2 args --img path_to_read dest_path\n");
            return -1;
        }
    } else if (argv[1] && strcmp(argv[1], "--rev") == 0) {
        if (argv[2]) {
            return reverse(argv[2]);
        } else {
            printf("Args error: Need 1 args --rev executable\n");
            return -1;
        }
    } else if (argv[1] && strcmp(argv[1], "--help") == 0) {
        return help();
    } else {
        return help();
    }

    return 0;
}

int main(int argc, char **argv) {

    if (argc == 1) {
        help();
    } else {
        parse_args(argv);
    }
    

    return 0;
}
