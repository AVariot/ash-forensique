
#include "reverse.h"

int reverse(const char *executable) {
    FILE *fp = fopen(executable, "rb");
    Elf64_Ehdr header;

    fread(&header, sizeof(Elf64_Ehdr), 1, fp);

    // Check header if .ELF
    // bit 1, 2, 3, 4 must be .ELF
    if (header.e_ident[EI_MAG0] == ELFMAG0 &&
    header.e_ident[EI_MAG1] == ELFMAG1 &&
    header.e_ident[EI_MAG2] == ELFMAG2 &&
    header.e_ident[EI_MAG3] == ELFMAG3) {
        printf("ELF\n");
    } else {
        printf("NOT ELF\n");
        fclose(fd);
        return -1;
    }
    // bit 5 - architecture in binary
    printf("Architecture: %d\n", header.e_ident[EI_CLASS] == ELFCLASS64 ? 64 : (header.e_ident[EI_CLASS] == ELFCLASS32 ? 32 : 0 ));
    // bit 6 - architecture in binary
    printf("Data encoding of the processor-specific: %s\n", header.e_ident[EI_DATA] == ELFDATA2LSB ? "Little Endian" : (header.e_ident[EI_DATA] == ELFDATA2MSB ? "Big Endian" : "None" ));
    // bit 8 - OS/ABI identification
    const char *osabi;
    switch (header.e_ident[EI_OSABI]) {
        case ELFOSABI_NONE:       osabi = "UNIX System V ABI"; break;
        case ELFOSABI_HPUX:       osabi = "HP-UX ABI"; break;
        case ELFOSABI_NETBSD:     osabi = "NetBSD ABI"; break;
        case ELFOSABI_LINUX:      osabi = "Linux ABI"; break;
        case ELFOSABI_SOLARIS:    osabi = "Solaris ABI"; break;
        case ELFOSABI_IRIX:       osabi = "IRIX ABI"; break;
        case ELFOSABI_FREEBSD:    osabi = "FreeBSD ABI"; break;
        case ELFOSABI_TRU64:      osabi = "TRU64 UNIX ABI"; break;
        case ELFOSABI_ARM:        osabi = "ARM architecture ABI"; break;
        case ELFOSABI_STANDALONE: osabi = "Stand-alone (embedded) ABI"; break;
        default:                  osabi = "Unknown"; break;
    }
    printf("OS/ABI: %s\n", osabi);

    fclose(fd);

    return 0;
}
