#ifndef __HAL_LINUX_H__
#define __HAL_LINUX_H__

#ifdef __cplusplus
extern "C" {
#endif

int rs_flash_write(const char *dev_path, unsigned int offset, 
	char *data, int size, int erase);
int rs_flash_read(const char *dev_path, unsigned int offset, char *data, int size);
void delay_ms(int n_ms);
void delay_us(int n_us);
void dump_hex(const char *func, unsigned char *buf, int size);
void hex_2_char(unsigned char *buf, int size, char *out);
void print_system_time(const char *head);
void print_system_date_time(const char *head);
int rs_diff_time(void *_new, void *_old);
int rs_diff_time_1(void *_new, void *_old);
unsigned long long get_timestamp_us(void);
unsigned long long get_timestamp_us_1(void);
void hal_reboot(void);
void hal_shutdown(void);
void get_random_name(char *name_buf, int name_size);
void *HAL_Timer_Create(void (*func)(void *), void *user_data);
int HAL_Timer_Start(void *timer, int ms);
int HAL_Timer_Stop(void *timer);
int HAL_Timer_Delete(void *timer);
void *hal_rwlock_init(void);
void hal_rwlock_destroy(void *handle);
void hal_rwlock_rdlock(void *handle);
void hal_rwlock_wrlock(void *handle);
void hal_rwlock_unlock(void *handle);
void *hal_mutex_init(void);
void hal_mutex_destroy(void *handle);
void hal_mutex_lock(void *handle);
int hal_mutex_trylock(void *handle);
void hal_mutex_unlock(void *handle);
void *hal_cond_init(void);
void hal_cond_destroy(void *handle);
void hal_cond_signal(void *handle);
int hal_cond_timedwait(void *mutex, void *handle, int n_ms);
void write_value_string(int fd, int val);
void del_r_n(char *data, int size);

#ifdef __cplusplus
}
#endif

#endif
