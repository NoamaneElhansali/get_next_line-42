/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelhansa <nelhansa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 22:40:55 by nelhansa          #+#    #+#             */
/*   Updated: 2025/11/27 01:21:16 by nelhansa         ###   ########.fr       */
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
	char	*temp;
	int		n;

	n = 1;
	temp = malloc(BUFFER_SIZE + 1);
	while (!ft_strchr(buffer, '\n') && n > 0)
	{
		n = read(fd, temp, BUFFER_SIZE);
		if (n <= 0)
			break ;
		temp[n] = '\0';
		buffer = ft_strjoin(buffer, temp);
	}
	free(temp);
	return (buffer);
}

static char	*ft_extra_line(char *buffer)
{
	char	*line;
	int		len;

	if (!buffer || !buffer[0])
		return (NULL);
	len = 0;
	while (buffer[len] && buffer[len] != '\n')
		len++;
	if (buffer[len] == '\n')
		len++;
	line = ft_substr(buffer, 0, len);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*buffer;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || BUFFER_SIZE >= INT_MAX)
		return (NULL);
	buffer = ft_read_until_newline(fd, buffer);
	if (!buffer)
		return (NULL);
	line = ft_extra_line(buffer);
	buffer = ft_free_buffer(buffer);
	return (line);
}
