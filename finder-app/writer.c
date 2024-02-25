#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

int main(int argc, char* argv[])
{
	openlog("writer", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
	if(argc != 3)
	{
		syslog(LOG_ERR, "Incorrect number of arguments");
		closelog();
		return EXIT_FAILURE;
	}
	int fd = open(argv[1], O_WRONLY | O_CREAT, 0666);
	if(fd == -1)
	{
		syslog(LOG_ERR, "File cannot be opened");
		closelog();
		return EXIT_FAILURE;
	}
	int ret = write(fd, argv[2], strlen(argv[2]));
	if(ret < 0)
	{
		syslog(LOG_ERR, "Write failed");
		closelog();
		return EXIT_FAILURE;
	}
	syslog(LOG_DEBUG, "Writing %s to file %s", argv[2], argv[1]);
	close(fd);
	return EXIT_SUCCESS;
}
