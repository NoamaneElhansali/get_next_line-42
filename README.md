# Get Next Line (GNL)

This project implements the ```get_next_line``` function, which reads a line from a file descriptor.

## Description

The ```get_next_line``` function returns a line read from a file descriptor. It handles reading from files, stdin, or other file descriptors, managing buffers efficiently to avoid memory leaks and handle large files.

## Features

- Reads one line at a time from a file descriptor.
- Handles multiple file descriptors (bonus version).
- Uses static variables to maintain state between calls.
- Includes utility functions for string manipulation.

## Usage

Include the appropriate header file and call the function in a loop:

```c
#include "get_next_line.h"

int main() {
    int fd = open("file.txt", O_RDONLY);
    char *line;
    while ((line = get_next_line(fd)) != NULL) {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return 0;
}
```

For the bonus version (multiple file descriptors):

```c
#include "get_next_line_bonus.h"
// Same usage, but can handle multiple fds simultaneously
```

## Compilation

Compile with a defined ```BUFFER_SIZE``` (e.g., 42):

```bash
gcc -D BUFFER_SIZE=42 -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c -o gnl
```

For bonus:

```bash
gcc -D BUFFER_SIZE=42 -Wall -Wextra -Werror get_next_line_bonus.c get_next_line_utils_bonus.c -o gnl_bonus
```

## Requirements

- C compiler (e.g., gcc)
- Standard C libraries (```<stdlib.h>```, ```<unistd.h>```)

## Files

- ```get_next_line.c```: Main implementation.
- ```get_next_line_bonus.c```: Bonus implementation for multiple FDs.
- ```get_next_line_utils.c```: Utility functions.
- ```get_next_line_utils_bonus.c```: Bonus utilities (if different).
- ```get_next_line.h```: Header for main.
- ```get_next_line_bonus.h```: Header for bonus.

## Author

nelhansa
