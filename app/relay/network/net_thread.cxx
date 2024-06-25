#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "../system/file.hxx"
#include "../time/ntp_thread.hxx"
#include "net_thread.hxx"

#define SKIP_NET "/userdata/net"
#define IP_READY "/tmp/ip_flag"

void net_init(void)
{
	if (access(SKIP_NET, F_OK) == 0) {
		printf("%s: skip network setting!!!\n", __func__);
		return;
	}
	
	system("killall -9 udhcpc");
	
	system("udhcpc -b -i eth0 -x hostname:eth0 -R -s /etc/simple.script &");
}

static void net_deinit(void)
{
	if (access(SKIP_NET, F_OK) == 0) {
		printf("%s: skip network setting!!!\n", __func__);
		return;
	}
	
	system("killall -9 udhcpc");
	system("ifconfig eth0 0.0.0.0");
	system("route del default gw 0.0.0.0 dev eth0");
	system("echo -n > /etc/resolv.conf");
}

static int reinit_value = 0;

void net_reinit(void)
{
	reinit_value = 1;
}

static char last_value = 0;

int net_getConnectStatus(void)
{
	return ((last_value == '1') ? 1 : 0);
}

/*
cat /sys/class/net/eth0/carrier
1
0
*/
static void *run_net_thread(void *arg)
{
	int *stop = (int *)arg;
	int fd;
	char value;
	
	printf("%s enter ...\n", __func__);
	
	//system("ifconfig eth0 hw ether 00:26:22:d9:fa:25");
	system("ifconfig eth0 up");
	
	fd = open("/sys/class/net/eth0/carrier", O_RDONLY);
	if (fd < 0) {
		printf("%s: error open...\n", __func__);
		goto exit;
	}

	while (1) {
		if (*stop) {
			printf("%s don't continue ...\n", __func__);
			break;
		}
		
		lseek(fd, 0, SEEK_SET);
		read(fd, &value, 1);
		
		if (last_value != value) {
			last_value = value;
			printf("last_value=%c\n", last_value);
			
			file_delete(IP_READY);
			
			if (last_value == '1') {
				printf("network up...\n");
				
				net_init();
			} else if (last_value == '0') {
				printf("network down...\n");
				
				wake_up_ntp_thread(0);
				net_deinit();
			}
			
			sleep(3);
		}
		
		if (access(IP_READY, F_OK) == 0) {
			file_delete(IP_READY);
			printf("got ip...\n");
			
			wake_up_ntp_thread(1);
		}
		
		if (reinit_value) {
			printf("%s: net_reinit start\n", __func__);
			
			file_delete(IP_READY);
			
			wake_up_ntp_thread(0);
			net_deinit();
			
			net_init();
			
			reinit_value = 0;
			
			printf("%s: net_reinit end\n", __func__);
		}
		
		sleep(1);
	}
	
	close(fd);
	
exit:
	printf("%s leave ...\n", __func__);
	return NULL;
}

static int m_stop = 1;
static pthread_t net_thread;

int init_net_thread(void)
{
	int ret;

	if (m_stop == 0) {
		printf("%s: already init!\n", __func__);
		return 0;
	}

	m_stop = 0;
	ret = pthread_create(&net_thread, NULL, run_net_thread, (void *)&m_stop);
	if (ret < 0) {
		m_stop = 1;
		printf("%s: create net_thread fail, errno=%d(%s)\n", __func__, 
			errno, strerror(errno));
		return -1;
	}

	return 0;
}

void deinit_net_thread(void)
{
	if (m_stop) {
		printf("%s: no need deinit!\n", __func__);
		return;
	}
	
	m_stop = 1;
	printf("wait net_thread...\n");
	pthread_join(net_thread, NULL);
	printf("net_thread exit...\n"); 
}
