
#include "file.h"

static char *create_file_name(int index, enum CURRENT_TYPE_E type, const char *folder_dest) {
    char *name = malloc(100);
    if (!name) return NULL;
    
    const char *ext = (type == PNG) ? "png" : (type == JPEG) ? "jpeg" : (type == PDF) ? "pdf" : "webp";
    snprintf(name, 100, "%s/fichier_%d.%s", folder_dest, index, ext);
    return name;
}

int read_metal(const char *path, const char *folder_dest) {
    int fd_in = open(path, O_RDONLY);
    if (fd_in == -1) { perror("Erreur ouverture"); return -1; }
    
    unsigned char buffer[SECTOR_SIZE];
    enum CURRENT_TYPE_E current_type = NONE;
    unsigned int webp_size_rem = 0;
    unsigned long current_file_size = 0;
    FILE *fp_out = NULL;
    int file_index = 0;
    
    while (read(fd_in, buffer, SECTOR_SIZE) > 0) {
        if (current_type == NONE) {
            current_type = get_file_type(buffer);
            if (current_type != NONE) {
                char *filename = create_file_name(file_index++, current_type, folder_dest);
                if (filename) {
                    fp_out = fopen(filename, "wb");
                    free(filename);
                    current_file_size = 0;
                    if (current_type == WEBP) webp_size_rem = get_webp_size(buffer);
                }
            }
        }

        if (current_type != NONE && fp_out) {
            if (current_type == WEBP) {
                size_t to_write = (webp_size_rem > SECTOR_SIZE) ? SECTOR_SIZE : webp_size_rem;
                fwrite(buffer, 1, to_write, fp_out);
                webp_size_rem -= to_write;
                if (webp_size_rem == 0) { fclose(fp_out); fp_out = NULL; current_type = NONE; }
            } else {
                int end_off = end_file(buffer, current_type);
                
                if (end_off != -1 && (current_type == JPEG && current_file_size + end_off < MIN_JPEG_SIZE)) {
                    fwrite(buffer, 1, SECTOR_SIZE, fp_out);
                    current_file_size += SECTOR_SIZE;
                } 
                else if (end_off != -1) {
                    fwrite(buffer, 1, end_off, fp_out);
                    fclose(fp_out);
                    fp_out = NULL;
                    current_type = NONE;
                } 
                else {
                    fwrite(buffer, 1, SECTOR_SIZE, fp_out);
                    current_file_size += SECTOR_SIZE;
                }
            }
        }
    }
    close(fd_in);
    return 0;
}
