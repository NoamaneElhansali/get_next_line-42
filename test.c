#include "get_next_line_bonus.h"
#include <fcntl.h>
#include <stdio.h>

int	main()
{
	int		fd;
	char	*line;
	//(void)ac;
	// if (ac != 2)
	// {
	// 	printf("Usage: %s <file>\n", av[0]);
	// 	return (1);
	// }
	fd = open("cc.txt", O_RDONLY);
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("LINE: %s\n", line);
		free(line);
	}
	close(fd);
	return (0);
}
