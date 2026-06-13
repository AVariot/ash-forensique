
#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "element.h"

int read_metal(const char *path, const char *folder_dest);

#endif // FILE_H
