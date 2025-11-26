/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelhansa <nelhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 22:40:55 by nelhansa          #+#    #+#             */
/*   Updated: 2025/11/26 12:47:04 by nelhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char *ft_free_buffer(char *buffer)
{
	char *buff;
	buff = ft_strchr(buffer,'\n');
	if (!buff)
		return (free(buffer),NULL);
	buff++;
	buff = ft_strdup(buff);
	free(buffer);
	return buff;
}

static char *ft_read_until_newline(int fd, char *buffer)
{
    char    temp[BUFFER_SIZE + 1];
    int     n = 1;

    if (!ft_strchr(buffer, '\n') && n > 0)
    {
        n = read(fd, temp, BUFFER_SIZE);
        if (n <= 0)
			return buffer;
		temp[n] = '\0';
        buffer = ft_strjoin(buffer, temp);
    }
	
    return buffer;
}

static char *ft_extra_line(char *buffer)
{
	char *line;
	if (!buffer[0])
		return (NULL);
	line = ft_substr(buffer,0,ft_strchr(buffer,'\n') - buffer);
	if (!line)
		return (NULL);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*buffer;
    char *line;

	if (fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE > 1024)
		return (NULL);
	buffer = ft_read_until_newline(fd,buffer);
	if (!buffer)
		return (NULL);
	line = ft_extra_line(buffer);
	buffer = ft_free_buffer(buffer);
	return (line);
}


// #include <stdio.h>
// int	main(void)
// {
// 	int fd = open("cc.txt", O_RDONLY);

// 	char *data = ft_get_next_line(fd);
// 	char *s = ft_get_next_line(fd);
// 	char *b = ft_get_next_line(fd);
// 	printf("%s",b);
// 	free(data);
// 	free(s);
// 	free(b);
// 	return (0);
// }