#ifndef NET_THREAD_H
#define NET_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

void net_init(void);
void net_reinit(void);
int net_getConnectStatus(void);
int init_net_thread(void);
void deinit_net_thread(void);

#ifdef __cplusplus
}
#endif

#endif
