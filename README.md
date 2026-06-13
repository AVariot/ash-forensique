# ash-forensique

Forensic file recovery tool written in C for recovering deleted files from raw storage devices. The program scans disk sectors for binary signatures (magic bytes) and reconstructs erased files.

## Supported Formats

| Format | Detection | End of File |
|--------|-----------|-------------|
| JPEG   | `FF D8 FF` | `FF D9` |
| PNG    | `89 50 4E 47` | `IEND` chunk |
| PDF    | `%PDF` | `%%EOF` |
| WEBP   | `RIFF...WEBP` | Size read from header |

## How It Works

The program reads the device sector by sector (512 bytes) in a single **O(N)** pass:

1. Each sector is scanned for magic bytes
2. When a header is detected, an output file is created
3. Data is written until the footer is found (or the expected size is reached for WEBP)
4. The file is closed and scanning continues

> **Note**: The algorithm does not handle fragmentation. Only files stored contiguously on disk can be recovered.

## Building

```bash
# Build the executable
make

# Build as a shared library
make lib

# Clean
make clean    # remove .o files
make fclean   # remove everything (objects + binary + lib)
make re       # full rebuild
```

## Usage

```bash
./getLostData <device> <output_directory>
```

```bash
# Example: recover files from a USB drive
sudo ./getLostData /dev/sdb ./recovered
```

Recovered files are named `fichier_0.jpeg`, `fichier_1.png`, etc.

## Project Structure

```
.
├── main.c                   # Entry point
├── main.h
├── Makefile
├── include/
│   ├── elements/
│   │   └── element.h        # Binary signatures for supported formats
│   └── sys/
│       └── linux/
│           └── file.h       # Linux I/O interface
└── src/
    ├── elements/
    │   └── element.c        # File type and footer detection
    └── sys/
        └── linux/
            └── file.c       # Carving algorithm (read_metal)
```

## Technical Details

- **Sector-based reading**: 512-byte blocks (standard disk sector size)
- **JPEG validation**: minimum 50 KB size to filter out false positives
- **WEBP**: size is extracted from the RIFF header instead of using a footer
- **Zero external dependencies**: only the standard libc
- **Strict compilation**: `-Wall -Wextra -Werror`
