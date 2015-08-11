#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd = open("/dev/zero", O_RDONLY);
	if (fd == -1) err(EXIT_FAILURE, "open");

	size_t const len = 1024*LINE_MAX;
	char* buf = malloc(len);
	if (!buf) err(EXIT_FAILURE, "malloc");

	printf("reading %g MiB\n", len/(1024.0*1024.0));
	posix_fadvise(fd, 0, len, POSIX_FADV_SEQUENTIAL);
	posix_fadvise(fd, 0, len, POSIX_FADV_WILLNEED);

	char* rdp = buf;
	ssize_t ret, des = len;
	while (des > 0 && (ret = read(fd, rdp, des)) != 0) {
		if (ret == -1) {
			if (errno == EINTR) continue;
			warn("read");
			break;
		}
		des -= ret;
		rdp += ret;
	}

	posix_fadvise(fd, 0, len, POSIX_FADV_DONTNEED);
	if (close(fd) == -1) err(EXIT_FAILURE, "close");
	free(buf);
}

