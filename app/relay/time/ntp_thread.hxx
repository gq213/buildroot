#ifndef NTP_THREAD_H
#define NTP_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

void wake_up_ntp_thread(int flag);
int init_ntp_thread(void);
void deinit_ntp_thread(void);

#ifdef __cplusplus
}
#endif

#endif
