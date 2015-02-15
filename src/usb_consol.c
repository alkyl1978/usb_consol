/*
 ============================================================================
 Name        : usb_consol.c
 Author      : aleks
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

libusb_context *ctx;
libusb_device_handle *handle_usb;
libusb_device        *device_usb;
int config;

#define VID 0x2156
#define PID 0xc00d
#define DEV_INTF 0

int er;

int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	er=libusb_init(&ctx);
	// обработчик USB
	handle_usb=libusb_open_device_with_vid_pid(NULL,VID,PID);
	 if (handle_usb == NULL) printf("Устройство не подключено\n");
	 else printf("Устройство подключено\n");
	 if (libusb_kernel_driver_active(handle_usb,DEV_INTF)) libusb_detach_kernel_driver(handle_usb, DEV_INTF);
	 if (libusb_claim_interface(handle_usb,  DEV_INTF) < 0) printf("Ошибка интерфейса\n");
	er=libusb_reset_device(handle_usb);
	er=libusb_get_configuration(handle_usb,&config);
	device_usb=libusb_get_device(handle_usb);
	libusb_fill_control_setup();
	//******************************
	libusb_attach_kernel_driver(handle_usb, DEV_INTF);
	libusb_close(handle_usb);
	libusb_exit(ctx);
	return EXIT_SUCCESS;
}

