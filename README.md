# Get Next Line (GNL)

This project implements the `get_next_line` function, which reads a line from a file descriptor.

## Table of Contents
- [Description](#description)
- [Features](#features)
- [Core Concepts](#core-concepts)
  - [The read() System Call](#1-the-read-system-call)
  - [Static Variables](#2-static-variables)
  - [Buffer Management](#3-buffer-management)
  - [Memory Management](#4-memory-management)
  - [Algorithm Flow](#5-algorithm-flow)
  - [Key Functions (Utils)](#6-key-functions-utils)
  - [Edge Cases](#7-edge-cases-to-handle)
  - [Bonus: Multiple FDs](#8-bonus-multiple-file-descriptors)
  - [Testing](#9-testing-considerations)
- [Usage](#usage)
- [Compilation](#compilation)
- [Requirements](#requirements)
- [Files](#files)
- [Return Value](#return-value)
- [Common Pitfalls](#common-pitfalls)
- [Author](#author)

---

## Description

The `get_next_line` function returns a line read from a file descriptor. It handles reading from files, stdin, or other file descriptors, managing buffers efficiently to avoid memory leaks and handle large files.

---

## Features

- Reads one line at a time from a file descriptor
- Handles multiple file descriptors (bonus version)
- Uses static variables to maintain state between calls
- Includes utility functions for string manipulation
- Memory-efficient buffer management
- Handles edge cases (empty lines, no newline at EOF, etc.)

---

## Core Concepts

### 1. The `read()` System Call

```c
ssize_t read(int fd, void *buf, size_t count);
```

**Parameters:**
- **fd**: File descriptor to read from (0 for stdin, or returned by `open()`)
- **buf**: Buffer to store the read data
- **count**: Maximum number of bytes to read
- **Returns**: Number of bytes read, 0 at EOF, -1 on error

**How it works in GNL:**
```c
int bytes_read = read(fd, buffer, BUFFER_SIZE);
```
- Reads `BUFFER_SIZE` bytes from the file descriptor into buffer
- May read less than `BUFFER_SIZE` if less data is available
- Returns 0 when reaching end of file
- Must be called in a loop until '\n' is found or EOF is reached

**Example:**
```c
char buffer[BUFFER_SIZE + 1];
ssize_t bytes_read;

bytes_read = read(fd, buffer, BUFFER_SIZE);
if (bytes_read == -1)
    return (NULL); // Error
if (bytes_read == 0)
    return (NULL); // EOF
buffer[bytes_read] = '\0'; // Null terminate
```

---

### 2. Static Variables

```c
static char *backup;
```

**Why static?**
- Persists between function calls (not destroyed when function returns)
- Maintains leftover data from previous reads
- Initialized to NULL automatically
- Each file descriptor needs its own backup in bonus: `static char *backup[MAX_FD]`

**How it works:**

1. **First call**: `backup` is NULL, start reading from fd
2. **Read more than one line**: Store remainder in `backup`
3. **Next call**: Check `backup` first before reading more from fd
4. **Last call**: Free `backup` when done (EOF or error)

**Example flow:**
```c
// Call 1: backup = NULL
get_next_line(fd) -> reads "Hello\nWorld\n" -> returns "Hello\n", backup = "World\n"

// Call 2: backup = "World\n"
get_next_line(fd) -> uses backup -> returns "World\n", backup = ""

// Call 3: backup = ""
get_next_line(fd) -> reads more or returns NULL if EOF
```

**Memory diagram:**
```
Initial:    backup = NULL
            
After read: backup = "data read but not returned yet"
            
Next call:  backup = "remaining data after line extraction"
            
End:        backup = NULL (freed)
```

---

### 3. Buffer Management

**BUFFER_SIZE:**
- Defined at compilation: `-D BUFFER_SIZE=42`
- Determines how many bytes to read at once with `read()`
- Smaller = more `read()` calls, less memory
- Larger = fewer `read()` calls, more memory usage
- **Must work with any value** (1, 42, 1000000, etc.)

**Process example:**
```
File content: "Hello\nWorld\nTest\n"

Call 1 with BUFFER_SIZE=5:
┌─────────────────────────────────┐
│ Read: "Hello"                   │
│ No '\n' found, need more...     │
│ Read: "\nWorl"                  │
│ Found '\n' at position 5!       │
│ Return: "Hello\n"               │
│ Backup: "Worl"                  │
└─────────────────────────────────┘

Call 2:
┌─────────────────────────────────┐
│ Backup exists: "Worl"           │
│ Read: "d\nTes"                  │
│ Combined: "World\nTes"          │
│ Found '\n' at position 5!       │
│ Return: "World\n"               │
│ Backup: "Tes"                   │
└─────────────────────────────────┘

Call 3:
┌─────────────────────────────────┐
│ Backup exists: "Tes"            │
│ Read: "t\n"                     │
│ Combined: "Test\n"              │
│ Found '\n' at position 4!       │
│ Return: "Test\n"                │
│ Backup: ""                      │
└─────────────────────────────────┘

Call 4:
┌─────────────────────────────────┐
│ Backup: ""                      │
│ Read: 0 bytes (EOF)             │
│ Return: NULL                    │
│ Free backup                     │
└─────────────────────────────────┘
```

**Key operations:**
```c
// 1. Read from fd
char buffer[BUFFER_SIZE + 1];
bytes_read = read(fd, buffer, BUFFER_SIZE);

// 2. Append to backup
backup = ft_strjoin(backup, buffer);

// 3. Check for newline
char *newline_pos = ft_strchr(backup, '\n');

// 4. Extract line
line = ft_substr(backup, 0, newline_pos - backup + 1);

// 5. Update backup
temp = backup;
backup = ft_substr(backup, newline_pos - backup + 1, ft_strlen(newline_pos + 1));
free(temp);
```

---

### 4. Memory Management

**Critical operations:**

1. **Allocate memory for each line returned**
```c
char *line = malloc(sizeof(char) * (line_length + 1));
```

2. **Caller must free the returned line**
```c
char *line = get_next_line(fd);
if (line)
{
    printf("%s", line);
    free(line); // MANDATORY
}
```

3. **Free static backup when:**
   - End of file reached
   - Error occurs
   - No more data to return

```c
if (bytes_read <= 0 && (!backup || !*backup))
{
    free(backup);
    backup = NULL;
    return (NULL);
}
```

4. **Avoid memory leaks in all scenarios**
   - Always free before returning NULL
   - Free temporary variables
   - Check all malloc() calls

**Memory leak example (WRONG):**
```c
// BAD: Memory leak
char *get_next_line(int fd)
{
    static char *backup;
    
    if (error)
        return (NULL); // LEAK! backup not freed
}
```

**Correct version:**
```c
// GOOD: No leak
char *get_next_line(int fd)
{
    static char *backup;
    
    if (error)
    {
        free(backup);
        backup = NULL;
        return (NULL);
    }
}
```

---

### 5. Algorithm Flow

```
┌─────────────────────────────────────────┐
│     START get_next_line(fd)             │
└────────────────┬────────────────────────┘
                 │
                 ▼
        ┌────────────────┐
        │  Validate fd   │
        │ and BUFFER_SIZE│
        └────────┬───────┘
                 │
                 ▼
     ┌───────────────────────┐
     │ Backup exists with \n?│
     └───────┬───────────────┘
             │
        YES  │  NO
     ┌───────▼────────┐
     │ Extract line   │
     │ Update backup  │
     │ RETURN line    │
     └────────────────┘
             │
             ▼
    ┌────────────────────────┐
    │ LOOP: Read BUFFER_SIZE │
    │        bytes           │
    └────────┬───────────────┘
             │
             ▼
    ┌────────────────────┐
    │ Read returns -1?   │
    │ (Error)            │
    └────┬───────────────┘
         │ YES
         ▼
    ┌────────────────┐
    │ Free backup    │
    │ RETURN NULL    │
    └────────────────┘
         │
         │ NO
         ▼
    ┌────────────────────┐
    │ Read returns 0?    │
    │ (EOF)              │
    └────┬───────────────┘
         │ YES
         ▼
    ┌──────────────────────┐
    │ Backup has data?     │
    └────┬─────────────────┘
         │
    YES  │  NO
         │  │
         │  ▼
         │ ┌──────────────┐
         │ │ Free backup  │
         │ │ RETURN NULL  │
         │ └──────────────┘
         │
         ▼
    ┌────────────────────┐
    │ Return backup data │
    │ Free backup        │
    │ RETURN line        │
    └────────────────────┘
         │
         │ NO (bytes read > 0)
         ▼
    ┌────────────────────────┐
    │ Append buffer to backup│
    └────────┬───────────────┘
             │
             ▼
    ┌────────────────────┐
    │ '\n' in backup?    │
    └────┬───────────────┘
         │ YES
         ▼
    ┌────────────────────┐
    │ Extract line       │
    │ Update backup      │
    │ RETURN line        │
    └────────────────────┘
         │
         │ NO
         └──────┐
                │
                ▼
         Continue loop
         (read more)
```

**Pseudo-code:**
```c
char *get_next_line(int fd)
{
    static char *backup;
    char buffer[BUFFER_SIZE + 1];
    ssize_t bytes_read;
    
    // Validation
    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);
    
    // Read until '\n' or EOF
    while (1)
    {
        // Check if backup already has a line
        if (backup && ft_strchr(backup, '\n'))
            return (extract_line(&backup));
        
        // Read more data
        bytes_read = read(fd, buffer, BUFFER_SIZE);
        
        // Handle errors
        if (bytes_read == -1)
            return (free_and_return(&backup, NULL));
        
        // Handle EOF
        if (bytes_read == 0)
        {
            if (backup && *backup)
                return (extract_remaining(&backup));
            return (free_and_return(&backup, NULL));
        }
        
        // Append read data to backup
        buffer[bytes_read] = '\0';
        backup = ft_strjoin(backup, buffer);
    }
}
```

---

### 6. Key Functions (Utils)

**ft_strlen** - Get string length
```c
size_t ft_strlen(const char *s)
{
    size_t i = 0;
    while (s[i])
        i++;
    return (i);
}
```

**ft_strchr** - Find character in string
```c
char *ft_strchr(const char *s, int c)
{
    while (*s)
    {
        if (*s == (char)c)
            return ((char *)s);
        s++;
    }
    if (c == '\0')
        return ((char *)s);
    return (NULL);
}
```
- **Critical for finding '\n'** in backup
- Returns pointer to first occurrence or NULL

**ft_strjoin** - Join two strings
```c
char *ft_strjoin(char const *s1, char const *s2)
{
    char *result;
    size_t i, j;
    
    if (!s1 && !s2)
        return (NULL);
    if (!s1)
        return (ft_strdup(s2));
    if (!s2)
        return (ft_strdup(s1));
    
    result = malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
    if (!result)
        return (NULL);
    
    i = 0;
    while (s1[i])
    {
        result[i] = s1[i];
        i++;
    }
    j = 0;
    while (s2[j])
        result[i++] = s2[j++];
    result[i] = '\0';
    
    return (result);
}
```
- Used to **append read data to backup**
- Returns new allocated string
- Remember to free old backup after joining

**ft_substr** - Extract substring
```c
char *ft_substr(char const *s, unsigned int start, size_t len)
{
    char *substr;
    size_t i;
    size_t s_len;
    
    if (!s)
        return (NULL);
    
    s_len = ft_strlen(s);
    if (start >= s_len)
        return (ft_strdup(""));
    
    if (len > s_len - start)
        len = s_len - start;
    
    substr = malloc(sizeof(char) * (len + 1));
    if (!substr)
        return (NULL);
    
    i = 0;
    while (i < len && s[start + i])
    {
        substr[i] = s[start + i];
        i++;
    }
    substr[i] = '\0';
    
    return (substr);
}
```
- Used to **extract line** from backup
- Used to **update backup** (remainder after '\n')

**ft_strdup** - Duplicate string
```c
char *ft_strdup(const char *s)
{
    char *dup;
    size_t len;
    size_t i;
    
    len = ft_strlen(s);
    dup = malloc(sizeof(char) * (len + 1));
    if (!dup)
        return (NULL);
    
    i = 0;
    while (s[i])
    {
        dup[i] = s[i];
        i++;
    }
    dup[i] = '\0';
    
    return (dup);
}
```

---

### 7. Edge Cases to Handle

| Case | Input | Expected Output | Notes |
|------|-------|-----------------|-------|
| **Empty file** | `""` | `NULL` | Return NULL immediately |
| **No newline at EOF** | `"Hello"` | `"Hello"` | Return last line without '\n' |
| **Only newline** | `"\n"` | `"\n"` | Return newline |
| **Empty line** | `"Hello\n\nWorld"` | `"Hello\n"`, `"\n"`, `"World"` | Each call returns one line |
| **Multiple newlines** | `"\n\n\n"` | `"\n"` (3 times) | Each newline is a line |
| **Very long line** | 1,000,000 chars + `\n` | Full line | Continue reading until '\n' |
| **BUFFER_SIZE = 1** | `"Hi\n"` | `"Hi\n"` | Read one character at a time |
| **BUFFER_SIZE = 1000000** | `"Hi\n"` | `"Hi\n"` | Works with large buffer |
| **Invalid fd** | `fd = -1` | `NULL` | Check fd validity |
| **Closed fd** | `close(fd); gnl(fd)` | `NULL` | read() returns -1 |
| **Binary file** | Binary data | Lines split at `\n` | Treat as text |
| **No read permission** | File without read perm | `NULL` | read() returns -1 |

**Examples:**

```c
// Empty file
fd = open("empty.txt", O_RDONLY);
line = get_next_line(fd); // Returns NULL

// No newline at EOF
// File: "Hello"
line = get_next_line(fd); // Returns "Hello"
line = get_next_line(fd); // Returns NULL

// Empty lines
// File: "A\n\nB\n"
line = get_next_line(fd); // Returns "A\n"
line = get_next_line(fd); // Returns "\n"
line = get_next_line(fd); // Returns "B\n"
line = get_next_line(fd); // Returns NULL

// BUFFER_SIZE = 1
// File: "OK\n"
// Calls read() 3 times: 'O', 'K', '\n'
line = get_next_line(fd); // Returns "OK\n"
```

---

### 8. Bonus: Multiple File Descriptors

**Concept:**
Handle multiple file descriptors simultaneously without mixing their data.

**Implementation:**
```c
#define MAX_FD 1024  // or OPEN_MAX from <limits.h>

static char *backup[MAX_FD];
```

- **Array of static strings**, one per possible fd
- Each fd maintains its **own backup independently**
- Can interleave reads between different fds

**Example:**
```c
int fd1 = open("file1.txt", O_RDONLY);
int fd2 = open("file2.txt", O_RDONLY);
int fd3 = open("file3.txt", O_RDONLY);

// Read from different files in any order
line = get_next_line(fd1); // backup[fd1] stores remainder
line = get_next_line(fd2); // backup[fd2] stores remainder
line = get_next_line(fd1); // backup[fd1] continues from where it left
line = get_next_line(fd3); // backup[fd3] stores remainder
line = get_next_line(fd2); // backup[fd2] continues from where it left
```

**Key differences from mandatory:**
```c
// Mandatory
static char *backup;  // Single backup for one fd

// Bonus
static char *backup[MAX_FD];  // Array of backups

// Usage in code
backup = ft_strjoin(backup, buffer);        // Mandatory
backup[fd] = ft_strjoin(backup[fd], buffer); // Bonus
```

**Memory management:**
- Free `backup[fd]` when done with that specific fd
- Don't free other backups
- Handle fd out of range (fd >= MAX_FD)

---

### 9. Testing Considerations

**Test with different BUFFER_SIZE values:**
```bash
# Very small
gcc -D BUFFER_SIZE=1 ...
gcc -D BUFFER_SIZE=5 ...

# Medium
gcc -D BUFFER_SIZE=32 ...
gcc -D BUFFER_SIZE=42 ...
gcc -D BUFFER_SIZE=128 ...

# Large
gcc -D BUFFER_SIZE=1000 ...
gcc -D BUFFER_SIZE=10000000 ...
```

**Test files:**
```bash
# Empty file
touch empty.txt

# Single character
echo -n "A" > single.txt

# No newline at end
echo -n "Hello" > no_newline.txt

# Multiple empty lines
printf "\n\n\n" > empty_lines.txt

# Very long line
python3 -c "print('A' * 1000000)" > long_line.txt

# Mixed content
cat > mixed.txt << EOF
Short line
A very very very long line with lots of text...
Another short line

Empty line above
EOF
```

**Test with stdin:**
```bash
./gnl < input.txt
echo "test" | ./gnl
cat file.txt | ./gnl
```

**Memory leak testing:**
```bash
# Compile with debug symbols
gcc -g -D BUFFER_SIZE=42 ...

# Run with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./gnl

# Expected output:
# All heap blocks were freed -- no leaks are possible
```

**Bonus testing:**
```c
// Test multiple fds
int fd1 = open("file1.txt", O_RDONLY);
int fd2 = open("file2.txt", O_RDONLY);
int fd3 = 0; // stdin

// Interleave reads
get_next_line(fd1);
get_next_line(fd2);
get_next_line(fd3);
get_next_line(fd1);
get_next_line(fd2);
```

**Edge cases testing:**
```c
// Invalid fd
get_next_line(-1);    // Should return NULL
get_next_line(10000); // Should return NULL (if not open)

// Closed fd
int fd = open("file.txt", O_RDONLY);
close(fd);
get_next_line(fd);    // Should return NULL

// Read after EOF
int fd = open("file.txt", O_RDONLY);
while (get_next_line(fd)); // Read all lines
get_next_line(fd);         // Should return NULL
```

**Popular testers:**
- [Tripouille/gnlTester](https://github.com/Tripouille/gnlTester)
- [xicodomingues/francinette](https://github.com/xicodomingues/francinette)
- [kodpe/gnl-station-tester](https://github.com/kodpe/gnl-station-tester)

---

## Usage

Include the appropriate header file and call the function in a loop:

**Basic usage (mandatory):**
```c
#include "get_next_line.h"
#include <fcntl.h>  // for open()
#include <stdio.h>  // for printf()

int main(void)
{
    int fd;
    char *line;
    
    // Open file
    fd = open("file.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("Error opening file\n");
        return (1);
    }
    
    // Read line by line
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);  // Don't forget to free!
    }
    
    // Close file
    close(fd);
    return (0);
}
```

**Reading from stdin:**
```c
#include "get_next_line.h"
#include <stdio.h>

int main(void)
{
    char *line;
    
    printf("Enter text (Ctrl+D to end):\n");
    
    // fd = 0 is stdin
    while ((line = get_next_line(0)) != NULL)
    {
        printf("You wrote: %s", line);
        free(line);
    }
    
    return (0);
}
```

**Bonus version (multiple file descriptors):**
```c
#include "get_next_line_bonus.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int fd1, fd2, fd3;
    char *line;
    
    // Open multiple files
    fd1 = open("file1.txt", O_RDONLY);
    fd2 = open("file2.txt", O_RDONLY);
    fd3 = 0; // stdin
    
    // Read from different fds in any order
    printf("=== File 1, Line 1 ===\n");
    line = get_next_line(fd1);
    printf("%s", line);
    free(line);
    
    printf("=== File 2, Line 1 ===\n");
    line = get_next_line(fd2);
    printf("%s", line);
    free(line);
    
    printf("=== Stdin, Line 1 ===\n");
    line = get_next_line(fd3);
    printf("%s", line);
    free(line);
    
    printf("=== File 1, Line 2 ===\n");
    line = get_next_line(fd1);
    printf("%s", line);
    free(line);
    
    // Continue with other fds...
    
    // Close files
    close(fd1);
    close(fd2);
    // Don't close stdin (fd 0)
    
    return (0);
}
```

**Reading entire file:**
```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    int fd;
    char *line;
    int line_count = 0;
    
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return (1);
    }
    
    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        printf("Error: Cannot open file %s\n", argv[1]);
        return (1);
    }
    
    while ((line = get_next_line(fd)) != NULL)
    {
        line_count++;
        printf("%d: %s", line_count, line);
        free(line);
    }
    
    close(fd);
    printf("\nTotal lines: %d\n", line_count);
    
    return (0);
}
```

---

## Compilation

**Mandatory version:**
```bash
# Basic compilation with BUFFER_SIZE=42
gcc -D BUFFER_SIZE=42 -Wall -Wextra -Werror \
    get_next_line.c get_next_line_utils.c main.c -o gnl

# With debug symbols (for valgrind/gdb)
gcc -g -D BUFFER_SIZE=42 -Wall -Wextra -Werror \
    get_next_line.c get_next_line_utils.c main.c -o gnl

# Test with different BUFFER_SIZE
gcc -D BUFFER_SIZE=1 -Wall -Wextra -Werror \
    get_next_line.c get_next_line_utils.c main.c -o gnl_bs1

gcc -D BUFFER_SIZE=9999 -Wall -Wextra -Werror \
    get_next_line.c get_next_line_utils.c main.c -o gnl_bs9999
```

**Bonus version:**
```bash
# Bonus compilation
gcc -D BUFFER_SIZE=42 -Wall -Wextra -Werror \
    get_next_line_bonus.c get_next_line_utils_bonus.c main_bonus.c -o gnl_bonus

# With sanitizer (detect memory errors)
gcc -g -D BUFFER_SIZE=42 -Wall -Wextra -Werror \
    -fsanitize=address -fsanitize=undefined \
    get_next_line_bonus.c get_next_line_utils_bonus.c main_bonus.c -o gnl_bonus
```

**Makefile example:**
```makefile
NAME = gnl
BONUS_NAME = gnl_bonus

CC = gcc
CFLAGS = -Wall -Wextra -Werror
BUFFER_SIZE = 42

# Mandatory
SRCS = get_next_line.c get_next_line_utils.c
OBJS = $(SRCS:.c=.o)

# Bonus
BONUS_SRCS = get_next_line_bonus.c get_next_line_utils_bonus.c
BONUS_OBJS = $(BONUS_SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -D BUFFER_SIZE=$(BUFFER_SIZE) $(OBJS) main.c -o $(NAME)

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CC) $(CFLAGS) -D BUFFER_SIZE=$(BUFFER_SIZE) $(BONUS_OBJS) main_bonus.c -o $(BONUS_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -D BUFFER_SIZE=$(BUFFER_SIZE) -c $< -o $@

clean:
	rm -f $(OBJS) $(BONUS_OBJS)

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)

re: fclean all

test: $(NAME)
	./$(NAME) test_files/test1.txt
	./$(NAME) test_files/test2.txt

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) test_files/test1.txt

.PHONY: all bonus clean fclean re test valgrind
```

---

## Requirements

**Mandatory:**
- C compiler (gcc, clang)
- Standard C libraries:
  - `<stdlib.h>` - for malloc(), free()
  - `<unistd.h>` - for read()
- No forbidden functions:
  - ❌ No `lseek()`
  - ❌ No `printf()` in gnl files
  - ❌ No `libft` (unless you implement needed functions in utils)
  - ❌ No global variables
- No memory leaks
- Must compile with `-Wall -Wextra -Werror`
- Must work with any valid BUFFER_SIZE value

**Norm compliance:**
- Follow 42 Norm rules
- Max 25 lines per function
- Max 5 functions per file
- No more than 4 parameters per function

---

## Files

**Mandatory:**
```
get_next_line/
├── get_next_line.c          # Main function implementation
├── get_next_line.h          # Header file (prototypes, includes)
├── get_next_line_utils.c    # Utility functions (strlen, strchr, etc.)
└── main.c                   # Your test main (not submitted)
```

**Bonus:**
```
get_next_line/
├── get_next_line_bonus.c         # Bonus implementation (multiple fds)
├── get_next_line_bonus.h         # Bonus header
├── get_next_line_utils_bonus.c   # Bonus utils
└── main_bonus.c                  # Bonus test main (not submitted)
```

**Header file example (get_next_line.h):**
```c
#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

// Main function
char    *get_next_line(int fd);

// Utils
size_t  ft_strlen(const char *s);
char    *ft_strchr(const char *s, int c);
char    *ft_strjoin(char const *s1, char const *s2);
char    *ft_substr(char const *s, unsigned int start, size_t len);
char    *ft_strdup(const char *s);

#endif
```

**File submission:**
- Mandatory: `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h`
- Bonus: Add `get_next_line_bonus.c`, `get_next_line_utils_bonus.c`, `get_next_line_bonus.h`
- **Do NOT** submit main.c files

---

## Return Value

**Function prototype:**
```c
char *get_next_line(int fd);
```

**Returns:**
- **Success**: Pointer to the line read (including '\n' if present)
- **EOF**: NULL (when no more data to read)
- **Error**: NULL (invalid fd, read error, malloc failure)

**Important notes:**
- The returned string is allocated with `malloc()`
- **The caller is responsible for freeing** the returned line
- Each line includes the terminating '\n' (if present in file)
- Last line might not have '\n' (if file doesn't end with newline)

**Examples:**
```c
// File: "Hello\nWorld\n"
line = get_next_line(fd);  // Returns "Hello\n" (must free)
line = get_next_line(fd);  // Returns "World\n" (must free)
line = get_next_line(fd);  // Returns NULL (EOF)

// File: "Test"
line = get_next_line(fd);  // Returns "Test" (no \n, must free)
line = get_next_line(fd);  // Returns NULL (EOF)

// Invalid fd
line = get_next_line(-1);  // Returns NULL
```

---

## Common Pitfalls

### 1. **Memory Leaks**
```c
// ❌ WRONG: Not freeing line
char *line = get_next_line(fd);
printf("%s", line);
// Memory leak!

// ✅ CORRECT: Always free
char *line = get_next_line(fd);
printf("%s", line);
free(line);

// ❌ WRONG: Not freeing backup on error
if (error)
    return (NULL);  // backup not freed!

// ✅ CORRECT: Free before returning NULL
if (error)
{
    free(backup);
    backup = NULL;
    return (NULL);
}
```

### 2. **Buffer Overflow**
```c
// ❌ WRONG: Not null-terminating
char buffer[BUFFER_SIZE];
read(fd, buffer, BUFFER_SIZE);
// buffer not null-terminated!

// ✅ CORRECT: Always null-terminate
char buffer[BUFFER_SIZE + 1];
bytes_read = read(fd, buffer, BUFFER_SIZE);
buffer[bytes_read] = '\0';
```

### 3. **Invalid File Descriptor**
```c
// ❌ WRONG: Not checking fd
char *get_next_line(int fd)
{
    char buffer[BUFFER_SIZE + 1];
    read(fd, buffer, BUFFER_SIZE);  // fd might be invalid!
}

// ✅ CORRECT: Validate fd
char *get_next_line(int fd)
{
    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);
    // Continue...
}
```

### 4. **Null Pointer Dereference**
```c
// ❌ WRONG: Not checking malloc
char *line = malloc(size);
line[0] = 'A';  // Crash if malloc failed!

// ✅ CORRECT: Always check malloc
char *line = malloc(size);
if (!line)
    return (NULL);
line[0] = 'A';
```

### 5. **Infinite Loops**
```c
// ❌ WRONG: Not checking read return value
while (1)
{
    read(fd, buffer, BUFFER_SIZE);
    // What if read returns -1 or 0?
}

// ✅ CORRECT: Check return value
while (1)
{
    bytes_read = read(fd, buffer, BUFFER_SIZE);
    if (bytes_read <= 0)
        break;
}
```

### 6. **Not Handling EOF**
```c
// ❌ WRONG: Returning NULL when backup has data
if (bytes_read == 0)
    return (NULL);  // Lost data in backup!

// ✅ CORRECT: Return remaining data
if (bytes_read == 0)
{
    if (backup && *backup)
        return (extract_and_free(&backup));
    return (free_and_return(&backup, NULL));
}
```

### 7. **Forgetting to Update Backup**
```c
// ❌ WRONG: Not updating backup after extraction
line = extract_line(backup);
return (line);  // backup still has old data!

// ✅ CORRECT: Update backup
line = extract_line(backup);
temp = backup;
backup = ft_substr(backup, ...);  // New backup
free(temp);
return (line);
```

### 8. **Mixing Static and Dynamic Memory**
```c
// ❌ WRONG: Losing reference to malloced memory
static char *backup = "initial";  // Static string!
backup = malloc(100);  // Lost reference to static!

// ✅ CORRECT: Initialize properly
static char *backup = NULL;  // Initialize to NULL
backup = malloc(100);  // Now it's dynamic
```

### 9. **Not Handling BUFFER_SIZE = 1**
```c
// ❌ WRONG: Assuming BUFFER_SIZE is large
// Code only works with BUFFER_SIZE >= 10

// ✅ CORRECT: Must work with ANY BUFFER_SIZE
// Test with BUFFER_SIZE=1, 5, 42, 1000000
```

### 10. **Freeing Wrong Pointer**
```c
// ❌ WRONG: Freeing modified pointer
char *ptr = malloc(100);
ptr++;  // Moved pointer!
free(ptr);  // Crash! Not the original address

// ✅ CORRECT: Keep original pointer
char *ptr = malloc(100);
char *temp = ptr;
temp++;  // Use temp
free(ptr);  // Free original
```

---

## Debugging Tips

**1. Use debug prints (remove before submission):**
```c
#ifdef DEBUG
printf("DEBUG: backup = [%s]\n", backup);
printf("DEBUG: bytes_read = %zd\n", bytes_read);
#endif

// Compile with: gcc -DDEBUG ...
```

**2. Valgrind for memory leaks:**
```bash
valgrind --leak-check=full --show-leak-kinds=all ./gnl test.txt
```

**3. Test edge cases systematically:**
```bash
# Create test files
echo -n "" > empty.txt
echo -n "A" > one_char.txt
printf "\n\n\n" > only_newlines.txt
python3 -c "print('A'*1000000)" > huge_line.txt
```

**4. Test with different BUFFER_SIZE:**
```bash
for size in 1 5 10 32 42 100 1000 10000000; do
    gcc -D BUFFER_SIZE=$size ... -o gnl_$size
    ./gnl_$size test.txt
done
```

**5. Use assertions (debug only):**
```c
#include <assert.h>

assert(fd >= 0);
assert(BUFFER_SIZE > 0);
assert(backup != NULL);
```

---

## Resources

**42 Documentation:**
- [Subject PDF](https://cdn.intra.42.fr/pdf/pdf/xxxxx/get_next_line.en.pdf)
- [42 Norm](https://github.com/42School/norminette)

**System Calls:**
- `man 2 read` - Read system call documentation
- `man 2 open` - Open file descriptor
- `man 2 close` - Close file descriptor

**Useful Links:**
- [Understanding static variables in C](https://www.geeksforgeeks.org/static-variables-in-c/)
- [File descriptors explained](https://www.tutorialspoint.com/what-are-file-descriptors-in-linux)
- [Buffer management concepts](https://www.gnu.org/software/libc/manual/html_node/Stream-Buffering.html)

**Testing:**
- [Tripouille/gnlTester](https://github.com/Tripouille/gnlTester)
- [xicodomingues/francinette](https://github.com/xicodomingues/francinette)
- [kodpe/gnl-station-tester](https://github.com/kodpe/gnl-station-tester)

---

## Frequently Asked Questions

**Q: Why use static variables?**  
A: To persist data between function calls without using global variables.

**Q: What if BUFFER_SIZE is 1?**  
A: Your code must work! It will just read one character at a time.

**Q: Should I handle negative BUFFER_SIZE?**  
A: Yes, return NULL if BUFFER_SIZE <= 0.

**Q: Can I use libft functions?**  
A: Only if you reimplement them in get_next_line_utils.c.

**Q: What about binary files?**  
A: Treat them as text - split at '\n' characters.

**Q: Should I close the fd?**  
A: No, the caller is responsible for closing the fd.

**Q: What happens if read() fails?**  
A: Free any allocated memory and return NULL.

**Q: Can backup be global?**  
A: No, must be static inside the function.

**Q: How many lines per function?**  
A: Maximum 25 lines (42 Norm requirement).

**Q: Should I use printf in get_next_line.c?**  
A: No, only for debugging (remove before submission).

---

## Project Evaluation Checklist

Before submitting:

- [ ] Code compiles with `-Wall -Wextra -Werror`
- [ ] No compilation warnings
- [ ] Passes norminette (42 Norm)
- [ ] No memory leaks (valgrind clean)
- [ ] Works with BUFFER_SIZE = 1
- [ ] Works with BUFFER_SIZE = 1000000
- [ ] Handles empty files
- [ ] Handles files without final newline
- [ ] Handles multiple consecutive newlines
- [ ] Returns NULL on error
- [ ] Returns NULL at EOF
- [ ] No global variables
- [ ] No forbidden functions (lseek, etc.)
- [ ] Proper use of static variables
- [ ] All malloc() calls are checked
- [ ] Memory is freed before returning NULL
- [ ] Bonus: Works with multiple fds simultaneously

---

## Author

**nelhansa**

*This project is part of the 42 curriculum*

---

## License

This project is part of 42 School curriculum. Feel free to use for learning purposes.

---

**Good luck with your Get Next Line! 🚀**