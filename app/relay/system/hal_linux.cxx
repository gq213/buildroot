#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "hal_linux.hxx"

int rs_flash_write(const char *dev_path, unsigned int offset, 
	char *data, int size, int erase)
{
	int ret = -1;
	int dev_fd;
	int write_size;
	
	dev_fd = open(dev_path, O_RDWR | O_SYNC);
	if (dev_fd < 0) {
		printf("%s: open %s fail !\n", __func__, dev_path);
		goto err;
	}
	
	if (erase)
		;
	
	lseek(dev_fd, offset, SEEK_SET);
	
	write_size = write(dev_fd, data, size);
	if (size != write_size) {
		printf("%s: write %s fail (%d/%d) !\n", __func__, dev_path, write_size, size);
	} else {
		ret = 0;
	}
	
	fsync(dev_fd);
	sync();
	
	close(dev_fd);
	
err:	
	return ret;
}

int rs_flash_read(const char *dev_path, unsigned int offset, char *data, int size)
{
	int ret = -1;
	int dev_fd;
	int read_size;
	
	dev_fd = open(dev_path, O_RDONLY);
	if (dev_fd < 0) {
		printf("%s: open %s fail !\n", __func__, dev_path);
		goto err;
	}
	
	lseek(dev_fd, offset, SEEK_SET);
	
	read_size = read(dev_fd, data, size);
	if (size != read_size) {
		printf("%s: read %s fail (%d/%d) !\n", __func__, dev_path, read_size, size);
		if (read_size == -1) {
			printf("%s: errno=%d(%s)\n", __func__, errno, strerror(errno));
		}
	} else {
		ret = 0;
	}
	
	close(dev_fd);
	
err:	
	return ret;
}

void delay_ms(int n_ms)
{
	struct timeval tv;
	
	tv.tv_sec = n_ms / 1000;
	tv.tv_usec = (n_ms % 1000) * 1000;
	
	select(0, NULL, NULL, NULL, &tv);
}

void delay_us(int n_us)
{
	struct timeval tv;
	
	tv.tv_sec = n_us / 1000000;
	tv.tv_usec = n_us % 1000000;
	
	select(0, NULL, NULL, NULL, &tv);
}

void dump_hex(const char *func, unsigned char *buf, int size)
{
	int i;
	
	printf("%s: size=%d, [", func, size);
	for (i=0; i<size; i++) {
		printf("%02x", buf[i]);
		if (i < (size - 1)) {
			printf(", ");
		}
	}
	printf("]\n");
}

void hex_2_char(unsigned char *buf, int size, char *out)
{
	int i;
	unsigned char tmp;
	
	for (i=0; i<size; i++) {
		tmp = (buf[i] >> 4) & 0x0f;
		out[2*i] = (tmp <= 0x9) ? (tmp + '0') : (tmp - 0xa + 'A');
		tmp = buf[i] & 0x0f;
		out[2*i+1] = (tmp <= 0x9) ? (tmp + '0') : (tmp - 0xa + 'A');
	}
	out[2*i] = '\0';
}

void print_system_time(const char *head)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	printf("=====================>>>%s time is %ld.%03ld\n", head, tv.tv_sec, tv.tv_usec / 1000);
}

void print_system_date_time(const char *head)
{
	struct timeval tv;
	struct tm *p;
	
	gettimeofday(&tv, NULL);
	p = localtime(&tv.tv_sec);

	printf("=====================>>>%s time is %4d-%02d-%02d_%02d:%02d:%02d.%03ld\n", 
		head, 
		p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, 
		tv.tv_usec / 1000);
}

int rs_diff_time(void *_n, void *_o)
{
	long long t0,t1,diff;
	struct timeval *_new = (struct timeval *)_n;
	struct timeval *_old = (struct timeval *)_o;

	t0 = _new->tv_sec;
	t0 *= 1000;
	t0 += (_new->tv_usec/1000);

	t1 = _old->tv_sec;
	t1 *= 1000;
	t1 += (_old->tv_usec/1000);

	diff = t0 - t1;

	if (diff > INT_MAX) {
		return INT_MAX;
	} else if (diff < INT_MIN) {
		return INT_MIN;
	} else {
		return diff;
	}
}

int rs_diff_time_1(void *_n, void *_o)
{
	long long t0,t1,diff;
	struct timespec *_new = (struct timespec *)_n;
	struct timespec *_old = (struct timespec *)_o;

	t0 = _new->tv_sec;
	t0 *= 1000;
	t0 += (_new->tv_nsec/1000000);

	t1 = _old->tv_sec;
	t1 *= 1000;
	t1 += (_old->tv_nsec/1000000);

	diff = t0 - t1;

	if (diff > INT_MAX) {
		return INT_MAX;
	} else if (diff < INT_MIN) {
		return INT_MIN;
	} else {
		return diff;
	}
}

unsigned long long get_timestamp_us(void)
{
	struct timeval tv;
	unsigned long long pts;

	gettimeofday(&tv, NULL);
	pts = tv.tv_sec * 1000000;
	pts += tv.tv_usec;

	return pts;
}

unsigned long long get_timestamp_us_1(void)
{
	struct timespec ts;
	unsigned long long pts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	pts = ts.tv_sec * 1000000;
	pts += ts.tv_nsec / 1000;

	return pts;
}

void hal_reboot(void)
{
	system("sync");
	sleep(1);
	
	system("busybox reboot");
}

void hal_shutdown(void)
{
	system("sync");
	sleep(1);
	
	system("busybox poweroff");
}

void get_random_name(char *name_buf, int name_size)
{
	int fd;
	unsigned char buf[4] = {0, 1, 2, 3};
	struct timeval tv;
	struct tm *p;

	fd = open("/dev/urandom", O_RDONLY | O_NONBLOCK);
	if (fd != -1) {
		read(fd, buf, sizeof(buf));
		close(fd);
	}

	gettimeofday(&tv, NULL);
	p = localtime(&tv.tv_sec);
	
	snprintf(name_buf, name_size, "%4d%02d%02d_%02d%02d%02d_%03ld_%02X%02X%02X%02X", 
			p->tm_year+1900, p->tm_mon+1, p->tm_mday, 
			p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec / 1000, 
			buf[0], buf[1], buf[2], buf[3]);
}

void *HAL_Timer_Create(void (*func)(void *), void *user_data)
{
    timer_t *timer = NULL;

    struct sigevent ent;

    /* check parameter */
    if (func == NULL) {
        return NULL;
    }

    timer = (timer_t *)malloc(sizeof(time_t));

    /* Init */
    memset(&ent, 0x00, sizeof(struct sigevent));

    /* create a timer */
    ent.sigev_notify = SIGEV_THREAD;
    ent.sigev_notify_function = (void (*)(union sigval))func;
    ent.sigev_value.sival_ptr = user_data;

    if (timer_create(CLOCK_MONOTONIC, &ent, timer) != 0) {
        free(timer);
        return NULL;
    }

    return (void *)timer;
}

int HAL_Timer_Start(void *timer, int ms)
{
    struct itimerspec ts;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000000;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}

int HAL_Timer_Stop(void *timer)
{
    struct itimerspec ts;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    /* it_interval=0: timer run only once */
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;

    /* it_value=0: stop timer */
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 0;

    return timer_settime(*(timer_t *)timer, 0, &ts, NULL);
}

int HAL_Timer_Delete(void *timer)
{
    int ret = 0;

    /* check parameter */
    if (timer == NULL) {
        return -1;
    }

    ret = timer_delete(*(timer_t *)timer);

    free(timer);

    return ret;
}

void *hal_rwlock_init(void)
{
	pthread_rwlock_t *handle = (pthread_rwlock_t *)malloc(sizeof(pthread_rwlock_t));
	if (handle == NULL) {
		printf("%s-%d: malloc fail\n", __func__, __LINE__);
		return NULL;
	}

	if (pthread_rwlock_init(handle, NULL) != 0) {
		perror("pthread_rwlock_init");
	}

	return handle;
}

void hal_rwlock_destroy(void *handle)
{
	pthread_rwlock_destroy((pthread_rwlock_t *)handle);
	free(handle);
}

void hal_rwlock_rdlock(void *handle)
{
	pthread_rwlock_rdlock((pthread_rwlock_t *)handle);
}

void hal_rwlock_wrlock(void *handle)
{
	pthread_rwlock_wrlock((pthread_rwlock_t *)handle);
}

void hal_rwlock_unlock(void *handle)
{
	pthread_rwlock_unlock((pthread_rwlock_t *)handle);
}

void *hal_mutex_init(void)
{
	pthread_mutex_t *handle = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (handle == NULL) {
		printf("%s-%d: malloc fail\n", __func__, __LINE__);
		return NULL;
	}
	
	if (pthread_mutex_init(handle, NULL) != 0) {
		perror("pthread_mutex_init");
	}
	
	return handle;
}

void hal_mutex_destroy(void *handle)
{
	pthread_mutex_destroy((pthread_mutex_t *)handle);
	free(handle);
}

void hal_mutex_lock(void *handle)
{
	pthread_mutex_lock((pthread_mutex_t *)handle);
}

int hal_mutex_trylock(void *handle)
{
	if (pthread_mutex_trylock((pthread_mutex_t *)handle) == 0)
		return 0;
	else
		return -1;
}

void hal_mutex_unlock(void *handle)
{
	pthread_mutex_unlock((pthread_mutex_t *)handle);
}

void *hal_cond_init(void)
{
	pthread_condattr_t attr;
	
	pthread_cond_t *handle = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
	if (handle == NULL) {
		printf("%s-%d: malloc fail\n", __func__, __LINE__);
		return NULL;
	}
	
	if (pthread_condattr_init(&attr) != 0) {
		perror("pthread_condattr_init");
	}
	if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC) != 0) {
		perror("pthread_condattr_setclock");
	}
	if (pthread_cond_init(handle, &attr) != 0) {
		perror("pthread_cond_init");
	}
	pthread_condattr_destroy(&attr);
	
	return handle;
}

void hal_cond_destroy(void *handle)
{
	pthread_cond_destroy((pthread_cond_t *)handle);
	free(handle);
}

void hal_cond_signal(void *handle)
{
	pthread_cond_signal((pthread_cond_t *)handle);
}

int hal_cond_timedwait(void *mutex, void *handle, int n_ms)
{
	pthread_mutex_t *t = (pthread_mutex_t *)mutex;
	pthread_cond_t *c = (pthread_cond_t *)handle;
    struct timespec timeout,start,end;
	int diff;
	
	pthread_mutex_lock(t);
	
	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
		perror("clock_gettime");
	}
	if (n_ms > 0) {
		timeout.tv_sec = start.tv_sec + (n_ms / 1000);
		timeout.tv_nsec = start.tv_nsec + (n_ms % 1000) * 1000 * 1000;
		if (timeout.tv_nsec > 1000000000) {
			timeout.tv_sec += 1;
			timeout.tv_nsec -= 1000000000;
		}
		pthread_cond_timedwait(c, t, &timeout);
	} else {
		pthread_cond_wait(c, t);
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
		perror("clock_gettime");
	}
	diff = rs_diff_time_1(&end, &start);
	//printf("%s: %d ms -> %d ms\n", __func__, n_ms, diff);
	
	pthread_mutex_unlock(t);
	
	return diff;
}

void write_value_string(int fd, int val)
{
	char buf[8];
	int size;
	int ret;
	
	size = snprintf(buf, sizeof(buf), "%d", val);
	ret = write(fd, buf, size);
	if (ret != size) {
		printf("%s: write error(%d != %d)(%d)\n", __func__, ret, size, val);
	}
}

void del_r_n(char *data, int size)
{
	int i, idx;
	
	for (i=0; i<2; i++) {
		idx = size - 1 - i;
		if (idx < 0) {
			break;
		}
		
		if (data[idx] == '\r') {
			data[idx] = '\0';
		} else if (data[idx] == '\n') {
			data[idx] = '\0';
		}
	}
}
