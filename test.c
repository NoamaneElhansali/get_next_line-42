#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(int ac, char **av)
{
	int		fd;
	char	*line;

	if (ac != 2)
	{
		printf("Usage: %s <file>\n", av[0]);
		return (1);
	}
	fd = open(av[1], O_RDONLY);
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("LINE: %s\n", line);
		free(line);
	}
	close(fd);
	return (0);
}
