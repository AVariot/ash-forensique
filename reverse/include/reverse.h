
#ifndef REVERSE_H
#define REVERSE_H

#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <capstone/capstone.h>
#include <unistd.h>
#include <fcntl.h>

int reverse(const char *executable);

#endif // REVERSE_H
