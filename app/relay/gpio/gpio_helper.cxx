#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../system/hal_linux.hxx"
#include "gpio_helper.hxx"

static const char *node[RSGPIO_OUT_MAX] = {
	// 0,255
	"/sys/class/leds/relay_led/brightness",
};
static int fd[RSGPIO_OUT_MAX] = {
	-1,
};

int init_gpio(void)
{
	int i;
	
	for (i=0; i<RSGPIO_OUT_MAX; i++) {
		fd[i] = open(node[i], O_WRONLY);
		if (fd[i] < 0) {
			printf("open %s fail!\n", node[i]);
		}
	}
	
	return 0;
}

void deinit_gpio(void)
{
	int i;
	
	for (i=0; i<RSGPIO_OUT_MAX; i++) {
		if (fd[i] != -1) {
			close(fd[i]);
			fd[i] = -1;
		}
	}
}

void set_gpio(enum rsgpio_out_idx idx, int val)
{
	if ((idx < 0) || (idx >= RSGPIO_OUT_MAX)) {
		printf("%s: invalid gpio(%d)\n", __func__, idx);
		return;
	}
	
	if (fd[idx] < 0) {
		printf("%s: %s can't opened\n", __func__, node[idx]);
		return;
	}
	
	write_value_string(fd[idx], (val == 0) ? 0 : 255);
}
