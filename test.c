#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int main(int ac, char **av)
{
    int   fd;
    char *line;

    if (ac != 2)
    {
        printf("Usage: %s <file>\n", av[0]);
        return 1;
    }

    fd = open(av[1], O_RDONLY);
    // if (fd < 0)
    // {
    //     perror("open");
    //     return 1;
    // }

    while ((line = get_next_line(fd)) != NULL)
    {
        printf("LINE: %s\n", line);
        free(line);
    }

    close(fd);
    return 0;
}

