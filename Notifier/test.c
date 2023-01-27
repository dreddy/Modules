#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
	u_int32_t cmd=0x11;
	int rv, fd;

	fd = open("/dev/saras", O_RDWR);

	if (fd < 0) {
		fprintf(stderr, "open failed \n");
		return -1;
	}
	rv = write(fd, &cmd, sizeof(u_int32_t));
	fprintf(stderr, "after write rv = %d \n", rv);

	close(fd);
}
