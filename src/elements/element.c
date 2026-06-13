
#include "element.h"

const unsigned char JPEG_MAGIC[] = {0xFF, 0xD8, 0xFF};
const unsigned char JPEG_MAGIC_FOOTER[]  = {0xFF, 0xD9};

const unsigned char PDF_MAGIC[]  = {0x25, 0x50, 0x44, 0x46};
const unsigned char PDF_MAGIC_FOOTER[]  = {0x25, 0x25, 0x45, 0x4F, 0x46};

const unsigned char PNG_MAGIC[]  = {0x89, 0x50, 0x4E, 0x47};
const unsigned char PNG_MAGIC_FOOTER[]  = {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82}; // IEND + CRC

const unsigned char WEBP_MAGIC_FIRST[] = {0x52, 0x49, 0x46, 0x46};
const unsigned char WEBP_MAGIC_SECOND[] = {0x57, 0x45, 0x42, 0x50};

unsigned int get_webp_size(unsigned char *buffer) {
    unsigned int chunk_size = buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24);
    return chunk_size + 8; // RIFF header = 8 octets
}

int end_file(unsigned char *buffer, enum CURRENT_TYPE_E current_type) {
    if (current_type == NONE) return -1;

    for (int i = 0; i < SECTOR_SIZE; i++) {
        if (current_type == JPEG && i <= SECTOR_SIZE - JPEG_MAGIC_END_SIZE) {
            if (memcmp(&buffer[i], JPEG_MAGIC_FOOTER, JPEG_MAGIC_END_SIZE) == 0) return i + JPEG_MAGIC_END_SIZE;
        } 
        else if (current_type == PNG && i <= SECTOR_SIZE - PNG_MAGIC_END_SIZE) {
            if (memcmp(&buffer[i], PNG_MAGIC_FOOTER, PNG_MAGIC_END_SIZE) == 0) return i + PNG_MAGIC_END_SIZE;
        } 
        else if (current_type == PDF && i <= SECTOR_SIZE - PDF_MAGIC_END_SIZE) {
            if (memcmp(&buffer[i], PDF_MAGIC_FOOTER, PDF_MAGIC_END_SIZE) == 0) return i + PDF_MAGIC_END_SIZE;
        }
    }
    return -1; 
}

enum CURRENT_TYPE_E get_file_type(unsigned char *buffer) {
    if (memcmp(buffer, JPEG_MAGIC, JPEG_MAGIC_SIZE) == 0) return JPEG;
    if (memcmp(buffer, PDF_MAGIC, PDF_MAGIC_SIZE) == 0) return PDF;
    if (memcmp(buffer, PNG_MAGIC, PNG_MAGIC_SIZE) == 0) return PNG;
    if (memcmp(buffer, WEBP_MAGIC_FIRST, WEBP_MAGIC_FIRST_SIZE) == 0 &&
        memcmp(&buffer[WEBP_MAGIC_START_SECOND_INDEX], WEBP_MAGIC_SECOND, WEBP_MAGIC_SECOND_SIZE) == 0) return WEBP;
    return NONE;
}
