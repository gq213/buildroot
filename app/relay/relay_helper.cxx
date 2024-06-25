#include <stdio.h>
#include "system/hal_linux.hxx"
#include "gpio/gpio_helper.hxx"
#include "relay_helper.hxx"

static void *_relay_timer = NULL;
static void *mutex = NULL;

static void relay_timeout(void *user_data)
{
	if (hal_mutex_trylock(mutex) < 0) {
		printf("%s: ignore\n", __func__);
		return;
	}
	
	set_gpio(RSGPIO_OUT_RELAY, 0);
	
	hal_mutex_unlock(mutex);
}

int init_relay(void)
{
	_relay_timer = HAL_Timer_Create(relay_timeout, NULL);
	if (_relay_timer == NULL) {
		printf("%s: HAL_Timer_Create _relay_timer fail\n", __func__);
		return -1;
	}
	mutex = hal_mutex_init();
	
	return 0;
}

void deinit_relay(void)
{
	HAL_Timer_Stop(_relay_timer);
	HAL_Timer_Delete(_relay_timer);
	hal_mutex_destroy(mutex);
}

void open_relay(void)
{
	hal_mutex_lock(mutex);
	
	HAL_Timer_Start(_relay_timer, 200);
	set_gpio(RSGPIO_OUT_RELAY, 1);
	
	hal_mutex_unlock(mutex);
}

void close_relay(void)
{
	hal_mutex_lock(mutex);
	
	HAL_Timer_Stop(_relay_timer);
	set_gpio(RSGPIO_OUT_RELAY, 0);
	
	hal_mutex_unlock(mutex);
}
