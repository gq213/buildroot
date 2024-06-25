#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "../system/hal_linux.hxx"
#include "ntp_thread.hxx"

static void start_ntp(void)
{
	print_system_time("before ntpd -qNnd");
	system("ntpd -qNnd");
	print_system_time("after ntpd -qNnd");
	
	// system("hwclock -r --utc");
	// system("hwclock -w --utc");
	// system("hwclock -r --utc");
	system("date -R");
	
	printf("%s: write time\n", __func__);
}

static void stop_ntp(void)
{
	print_system_time("before killall -9 ntpd");
	system("killall -9 ntpd");
	print_system_time("after killall -9 ntpd");
}

static int mflag = -1;

void wake_up_ntp_thread(int flag)
{
	stop_ntp();
	
	mflag = flag;
}

#define NTP_INTERVAL_MS (10 * 60 * 1000)

static void *run_ntp_thread(void *arg)
{
	int *stop = (int *)arg;
	int connected = 0;
	struct timespec last_time = {0, 0};
	struct timespec now_time;
	int diff;
	
	printf("%s enter ...\n", __func__);

	while (1) {
		if (*stop) {
			printf("%s don't continue ...\n", __func__);
			break;
		}
		
		sleep(1);
		
		if (mflag == 1) {
			mflag = -1;
			connected = 1;
			
			start_ntp();
			clock_gettime(CLOCK_MONOTONIC, &last_time);
		} else if (mflag == 0) {
			mflag = -1;
			connected = 0;
		} else if (mflag == 2) {
			mflag = -1;
			
			if (connected) {
				start_ntp();
				clock_gettime(CLOCK_MONOTONIC, &last_time);
			}
		} else {
			if (connected) {
				clock_gettime(CLOCK_MONOTONIC, &now_time);
				diff = rs_diff_time_1(&now_time, &last_time);
				if (diff > NTP_INTERVAL_MS) {
					start_ntp();
					clock_gettime(CLOCK_MONOTONIC, &last_time);
				}
			}
		}
	}
	
	printf("%s leave ...\n", __func__);
	return NULL;
}

static int m_stop = 1;
static pthread_t ntp_thread;

int init_ntp_thread(void)
{
	int ret;

	if (m_stop == 0) {
		printf("%s: already init!\n", __func__);
		return 0;
	}

	m_stop = 0;
	ret = pthread_create(&ntp_thread, NULL, run_ntp_thread, (void *)&m_stop);
	if (ret < 0) {
		m_stop = 1;
		printf("%s: create ntp_thread fail, errno=%d(%s)\n", __func__, 
			errno, strerror(errno));
		return -1;
	}

	return 0;
}

void deinit_ntp_thread(void)
{
	if (m_stop) {
		printf("%s: no need deinit!\n", __func__);
		return;
	}
	
	m_stop = 1;
	printf("wait ntp_thread...\n");
	pthread_join(ntp_thread, NULL);
	printf("ntp_thread exit...\n"); 
}
