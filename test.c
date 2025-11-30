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
	// fd = open("ff.txt", O_RDONLY);
	// line = get_next_line(fd);
	// printf("LINE: %s\n", line);
	// free(line);

	fd = open("cc.txt", O_RDONLY);
	// line = get_next_line(fd);
	// printf("LINE: %s\n", line);
	// free(line);
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("LINE: %s\n", line);
		free(line);
	}
	close(fd);



// 	fdopen(1024,"w+");
// 	write(1024,"hello",5);
// 	line = get_next_line(1024);
// 	printf("%s",line);
// 	free(line);
// 	return (0);
}



// #include <stdio.h>
// #include <fcntl.h>
// #include <unistd.h>

// int main(void)
// {
//     int fd;
//     char name[32];

//     for (int i = 0; i < 6000; i++)
//     {
//         snprintf(name, sizeof(name), "file_%d.txt", i);

//         fd = open(name, O_CREAT | O_RDWR | O_TRUNC, 0644);
//         if (fd == -1)
//         {
//             printf("Failed at %d — system FD limit reached\n", i);
//             break;
//         }
        
//         printf("Opened %s with fd = %d\n", name, fd);
//     }

//     return 0;
// }

