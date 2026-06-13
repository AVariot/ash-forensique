
#ifndef ELEMENT
#define ELEMENT

#include <string.h>

#define SECTOR_SIZE 512
#define MIN_JPEG_SIZE 50000

#define JPEG_MAGIC_SIZE 3
#define JPEG_MAGIC_END_SIZE 2

#define PDF_MAGIC_SIZE 4
#define PDF_MAGIC_END_SIZE 5 // %%EOF

#define PNG_MAGIC_SIZE 4
#define PNG_MAGIC_SIGN_SIZE 4
#define PNG_MAGIC_END_SIZE 4

#define WEBP_MAGIC_FIRST_SIZE 4
#define WEBP_MAGIC_SECOND_SIZE 4
#define WEBP_MAGIC_START_SECOND_INDEX 8 /// octet 8 start the WEBP in hexa
// No end because we gat size from octet 4 to octet 7

enum CURRENT_TYPE_E {
    PNG,
    JPEG,
    PDF,
    WEBP,
    NONE
};

extern const unsigned char JPEG_MAGIC[];
extern const unsigned char JPEG_MAGIC_FOOTER[];

extern const unsigned char PDF_MAGIC[];
extern const unsigned char PDF_MAGIC_FOOTER[];

extern const unsigned char PNG_MAGIC[];
extern const unsigned char PNG_MAGIC_FOOTER[];

extern const unsigned char WEBP_MAGIC_FIRST[];
extern const unsigned char WEBP_MAGIC_SECOND[];


enum CURRENT_TYPE_E get_file_type(unsigned char *buffer);
int end_file(unsigned char *buffer, enum CURRENT_TYPE_E current_type);
unsigned int get_webp_size(unsigned char *buffer);

#endif // ELEMENT
