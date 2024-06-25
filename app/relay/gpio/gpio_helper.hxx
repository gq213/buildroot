#ifndef __GPIO_HELPER_H__
#define __GPIO_HELPER_H__

enum rsgpio_out_idx {
	RSGPIO_OUT_RELAY = 0,
	RSGPIO_OUT_MAX,
};

#ifdef __cplusplus
extern "C" {
#endif

int init_gpio(void);
void deinit_gpio(void);
void set_gpio(enum rsgpio_out_idx idx, int val);

#ifdef __cplusplus
}
#endif

#endif
