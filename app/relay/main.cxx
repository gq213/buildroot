#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "network/net_thread.hxx"
#include "time/ntp_thread.hxx"
#include "gpio/gpio_helper.hxx"
#include "gpio/key_thread.hxx"
#include "relay_helper.hxx"

static int quit = 0;

static void signal_handler(int sig)
{
	printf("signal %d\n", sig);
	quit = 1;
}

int main(int argc, char *argv[])
{
	printf("+ build time: " __DATE__ " " __TIME__ "\n");
	
	init_gpio();
	init_relay();
	init_key_thread();
	init_ntp_thread();
	init_net_thread();
	
	open_relay();
	
	signal(SIGINT, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGTERM, signal_handler);
	while (quit == 0) {
		usleep(1*1000*1000);
		
		if (access("/tmp/relay", F_OK) == 0) {
			unlink("/tmp/relay");
			open_relay();
		}
	}
	
	close_relay();
	
	deinit_net_thread();
	deinit_ntp_thread();
	deinit_key_thread();
	deinit_relay();
	deinit_gpio();
	
	return 0;
}
