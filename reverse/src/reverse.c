
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
        fclose(fp);
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

    // Offset (where start the header section table)
    Elf64_Off offset = header.e_shoff;
    printf("Offset: %ld\n", offset);

    // Number of section in the exe
    uint16_t section_max_number = header.e_shnum;
    printf("Max number section: %d\n", section_max_number);
    
    // Section Header String Table
    uint16_t shstrndx_index = header.e_shstrndx;
    printf("shstrndx index: %d\n", shstrndx_index);

    Elf64_Shdr section_header;

    fseek(fp, header.e_shoff + (header.e_shstrndx * sizeof(Elf64_Shdr)), SEEK_SET);
    fread(&section_header, sizeof(Elf64_Shdr), 1, fp);
    char *shstrtab = malloc(section_header.sh_size);
    fseek(fp, section_header.sh_offset, SEEK_SET);
    fread(shstrtab, section_header.sh_size, 1, fp);


    // Positioning to the start of the file
    fseek(fp, header.e_shoff, SEEK_SET);
    for (int i = 0; i < header.e_shnum; i++) {
        fread(&section_header, sizeof(Elf64_Shdr), 1, fp);

        char *nom = shstrtab + section_header.sh_name;
    
        printf("Section [%d] : %s\n", i, nom);

        // Check the section nature
        // SHT_PROGBITS is the program generated section
        if (section_header.sh_type == SHT_PROGBITS) {
            printf("SHT_PROGBITS found\n");
        // This section take no place but reserved place in RAM during the execution
        // like .bss that contains all global variable
        } else if (section_header.sh_type == SHT_NOBITS) {
            printf("SHT_NOBITS found\n");
        // Symbol table
        } else if (section_header.sh_type == SHT_SYMTAB) {
            printf("SHT_SYMTAB found\n");
        // String table
        } else if (section_header.sh_type == SHT_STRTAB) {
            printf("SHT_STRTAB found\n");
        }

        // This section will have place allocated in the RAM
        // Here not else if because flags or bits mask using & operation
        if (section_header.sh_flags == SHF_ALLOC) {
            printf("SHF_ALLOC found\n");
        /*
            If the bit = 1 the os knows tthat he can execute the code section
            like a machine code.
            In securitised os only .text have this bit
        */
        }
        if (section_header.sh_flags == SHF_EXECINSTR) {
            printf("SHF_EXECINSTR found\n");
        // Bloc that can be modified during the excution
        // Mostly in .data and not in .text
        }
        if (section_header.sh_flags == SHF_WRITE) {
            printf("SHF_WRITE found\n");
        }
    }

    fclose(fp);
    return 0;
}
