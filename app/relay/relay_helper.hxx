#ifndef __RELAY_HELPER_H__
#define __RELAY_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

int init_relay(void);
void deinit_relay(void);
void open_relay(void);
void close_relay(void);

#ifdef __cplusplus
}
#endif

#endif
