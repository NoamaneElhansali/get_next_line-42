/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelhansa <nelhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 19:01:45 by nelhansa          #+#    #+#             */
/*   Updated: 2025/11/26 22:59:38 by nelhansa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_free_buffer(char *buffer)
{
	char	*buff;
	char	*new_buff;

	buff = ft_strchr(buffer, '\n');
	if (!buff)
		return (free(buffer), (NULL));
	if (*buff && *buff == '\n')
		buff++;
	new_buff = ft_strdup(buff);
	free(buffer);
	return (new_buff);
}

static char	*ft_read_until_newline(int fd, char *buffer)
{
	char	temp[BUFFER_SIZE + 1];
	int		n;

	n = 1;
	while (!ft_strchr(buffer, '\n') && n > 0)
	{
		n = read(fd, temp, BUFFER_SIZE);
		if (n <= 0)
			break ;
		temp[n] = '\0';
		buffer = ft_strjoin(buffer, temp);
	}
	return (buffer);
}

static char	*ft_extra_line(char *buffer)
{
	char	*line;
	int		len;

	if (!buffer || !buffer[0])
		return (NULL);
	while (buffer[len] && buffer[len] != '\n')
		len++;
	if (buffer[len] == '\n')
		len++;
	line = ft_substr(buffer, 0, len);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*buffer[1024];
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || fd <= 1024)
		return (NULL);
	buffer[fd] = ft_read_until_newline(fd, buffer[fd]);
	if (!buffer[fd])
		return (NULL);
	line = ft_extra_line(buffer[fd]);
	buffer[fd] = ft_free_buffer(buffer[fd]);
	return (line);
}
