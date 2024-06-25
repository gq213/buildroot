#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>
#include "../relay_helper.hxx"
#include "../system/hal_linux.hxx"
#include "key_thread.hxx"

static int key_read_data(int *stop, int *fd, int size)
{
	int ret;
	int i;
	int maxfd;
	struct timeval tv;
	fd_set rfds, efds;
	struct input_event event;
	
	while (1) {
		if (stop && *stop) {
			printf("%s: force stop\n", __func__);
			return 0;
		}
		
		maxfd = -1;
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
		
		FD_ZERO(&rfds);
		
		for (i=0; i<size; i++) {
			FD_SET(fd[i], &rfds);
			if (maxfd < fd[i]) {
				maxfd = fd[i];
			}
		}
		
		memcpy(&efds, &rfds, sizeof(rfds));

		ret = select(maxfd + 1, &rfds, NULL, &efds, &tv);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			}
			
			perror("select");
			return -1;
		} else if (ret == 0) {
			//printf("%s: select timeout!\n", __func__);
			continue;
		}

		/*check efds fd*/
		for (i=0; i<size; i++) {
			if (FD_ISSET(fd[i], &efds)) {
				perror("efds");
				return -1;
			}
		}
		
		/*check rfds fd*/
		for (i=0; i<size; i++) {
			if (FD_ISSET(fd[i], &rfds)) {
				ret = read(fd[i], &event, sizeof(event));
				if (ret < 0) {
					perror("key read");
					continue;
				}
				
				printf("%s: event.code=%d, event.value=%d\n", __func__, event.code, event.value);
				if (event.code == KEY_VENDOR) {
					if (event.value) {
						printf("KEY_VENDOR\n");
						open_relay();
					}
				} else if (event.code == KEY_POWER) {
					if (event.value) {
						printf("KEY_POWER\n");
						hal_shutdown();
					}
				}
			}
		}
	}
}

static void *run_key_thread(void *arg)
{
	int *stop = (int *)arg;
	#define INPUT_SIZE 3
	int fd[INPUT_SIZE] = {-1, -1};
	int idx = 0;
	char dev_node[24];
	int i;
	
	printf("%s enter ...\n", __func__);

	for (i=0; i<INPUT_SIZE; i++) {
		snprintf(dev_node, sizeof(dev_node), "/dev/input/event%d", i);
		fd[idx] = open(dev_node, O_RDONLY | O_NOCTTY);
		if (fd[idx] >= 0) {
			idx++;
		} else {
			printf("%s: open %s failed\n", __func__, dev_node);
		}
	}
	if (idx == 0) {
		goto exit;
	}
	
	key_read_data(stop, fd, idx);

	for (i=0; i<idx; i++) {
		close(fd[i]);
	}
	
exit:
	printf("%s leave ...\n", __func__);
	return NULL;
}

static int m_stop = 1;
static pthread_t key_thread;

int init_key_thread(void)
{
	int ret;
	
	if (m_stop == 0) {
		printf("%s: already init!\n", __func__);
		return 0;
	}

	m_stop = 0;
	ret = pthread_create(&key_thread, NULL, run_key_thread, (void *)&m_stop);
	if (ret < 0) {
		m_stop = 1;
		perror("pthread_create key_thread");
		return -1;
	}
	
	return 0;
}

void deinit_key_thread(void)
{
	if (m_stop) {
		printf("%s: no need deinit!\n", __func__);
		return;
	}
	
	m_stop = 1;
	printf("wait key_thread...\n");
	pthread_join(key_thread, NULL);
	printf("key_thread exit...\n");	
}