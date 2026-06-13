#include "reverse.h"

/*
    man elf
    to replace capstone read:
        http://ref.x86asm.net/geek64.html
        or
        https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf
*/

static void disassemble(unsigned char *code, size_t size, uint64_t address) {
    csh handle;
    cs_insn *insn;
    size_t count;

    // Initialisation pour x86_64
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) return;

    // Désassemblage
    count = cs_disasm(handle, code, size, address, 0, &insn);
    if (count > 0) {
        for (size_t i = 0; i < count; i++) {
            printf("0x%"PRIx64":\t%s\t\t%s\n", insn[i].address, insn[i].mnemonic, insn[i].op_str);
        }
        cs_free(insn, count);
    }
    cs_close(&handle);
}

int reverse(const char *executable) {
    FILE *fp = fopen(executable, "rb");
    if (!fp) return -1;
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

    Elf64_Shdr section_header_shstrtab;
    fseek(fp, header.e_shoff + (header.e_shstrndx * sizeof(Elf64_Shdr)), SEEK_SET);
    fread(&section_header_shstrtab, sizeof(Elf64_Shdr), 1, fp);
    
    char *shstrtab = malloc(section_header_shstrtab.sh_size);
    fseek(fp, section_header_shstrtab.sh_offset, SEEK_SET);
    fread(shstrtab, section_header_shstrtab.sh_size, 1, fp);

    // Loop to read all sections
    for (int i = 0; i < header.e_shnum; i++) {
        fseek(fp, header.e_shoff + (i * sizeof(Elf64_Shdr)), SEEK_SET);
        Elf64_Shdr section_header;
        fread(&section_header, sizeof(Elf64_Shdr), 1, fp);

        
        // SECURE ACCESS: vérification des bornes pour éviter le crash
        if (section_header.sh_name < section_header_shstrtab.sh_size) {
            char *nom = shstrtab + section_header.sh_name;
            
            printf("Section [%d] : %s (Adresse Virutal Memory Address: 0x%lx)\n", i, nom, section_header.sh_addr);
            if (strcmp(nom, ".text") == 0) {
                printf("Bingo ! Section .text trouvée.\n");
                unsigned char *code_buffer = malloc(section_header.sh_size);
                if (code_buffer != NULL) {
                    pread(fileno(fp), code_buffer, section_header.sh_size, section_header.sh_offset);
                    // TODO: I commented bcause too long
                    disassemble(code_buffer, section_header.sh_size, section_header.sh_addr);
                    free(code_buffer);
                }
            }
            printf("Section [%d] : %s\n", i, nom);
        } else {
            printf("Section [%d] : Nom corrompu\n", i);
        }

        // Check the section nature
        // SHT_PROGBITS is the program generated section
        // const real data, not metadata
        if (section_header.sh_type == SHT_PROGBITS) {
            printf("SHT_PROGBITS found\n");
            printf("  -> Taille physique : %ld octets\n", section_header.sh_size);
            printf("  -> Position dans le fichier : 0x%lx\n", section_header.sh_offset);
            printf("  -> Adresse en RAM (VMA) : 0x%lx\n", section_header.sh_addr);
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
        } else if (section_header.sh_type == SHT_DYNSYM) {
            printf("SHT_DYNSYM found\n");
            
            // 1. Read the tbale of links (indiquée par sh_link)
            // sh_link index of sector names (.dynstr)
            Elf64_Shdr strtab_header;
            fseek(fp, header.e_shoff + (section_header.sh_link * sizeof(Elf64_Shdr)), SEEK_SET);
            fread(&strtab_header, sizeof(Elf64_Shdr), 1, fp);
            
            char *dynstr = malloc(strtab_header.sh_size);
            fseek(fp, strtab_header.sh_offset, SEEK_SET);
            fread(dynstr, strtab_header.sh_size, 1, fp);

            int num_symbols = section_header.sh_size / section_header.sh_entsize;
            fseek(fp, section_header.sh_offset, SEEK_SET);
            
            for (int j = 0; j < num_symbols; j++) {
                Elf64_Sym sym;
                fread(&sym, sizeof(Elf64_Sym), 1, fp);
                
                char *sym_name = dynstr + sym.st_name;
                
                if (sym.st_name != 0) {
                    printf("  Symbole [%d] : %s\n", j, sym_name);
                }
            }
            free(dynstr);
        //  .rela.plt, known where each symbol is called
        } else if (section_header.sh_type == SHT_RELA) {
            printf("SHT_RELA found (Contient les localisations des appels)\n");
            
            int num_relocs = section_header.sh_size / section_header.sh_entsize;
            fseek(fp, section_header.sh_offset, SEEK_SET);
            
            for (int j = 0; j < num_relocs; j++) {
                Elf64_Rela rela;
                fread(&rela, sizeof(Elf64_Rela), 1, fp);
                
                // rela.r_offset : call address of the linked symbole
                // ELF64_R_SYM(rela.r_info) : Symbole index
                printf("  Appel trouvé à l'offset: 0x%lx (index symbole: %ld)\n", 
                    rela.r_offset, ELF64_R_SYM(rela.r_info));
            }
        }

        // This section will have place allocated in the RAM
        // Here not else if because flags or bits mask using & operation
        if (section_header.sh_flags & SHF_ALLOC) {
            printf("SHF_ALLOC found\n");
        /*
            If the bit = 1 the os knows tthat he can execute the code section
            like a machine code.
            In securitised os only .text have this bit
        */
        }
        if (section_header.sh_flags & SHF_EXECINSTR) {
            printf("SHF_EXECINSTR found\n");
        // Bloc that can be modified during the excution
        // Mostly in .data and not in .text
        }
        if (section_header.sh_flags & SHF_WRITE) {
            printf("SHF_WRITE found\n");
        }
        printf("Permissions: %c%c%c\n", 
            (section_header.sh_flags & SHF_ALLOC) ? 'R' : '-',
            (section_header.sh_flags & SHF_WRITE) ? 'W' : '-',
            (section_header.sh_flags & SHF_EXECINSTR) ? 'X' : '-'
        );
    }

    free(shstrtab);
    fclose(fp);
    return 0;
}