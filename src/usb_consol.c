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
uint8_t bus_number;
int config;
int in_count;
int out_count;
unsigned char buf[8];

#define VID 0x2156
#define PID 0xc00d
#define DEV_INTF 0

#define CTRL_IN	(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_DEVICE)
#define CTRL_OUT	(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT | LIBUSB_RECIPIENT_DEVICE)
#define USB_RQ	0x0C

int er;

int main(void)
{
	int i;
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	er=libusb_init(&ctx);
	libusb_set_debug(ctx,4);
	// обработчик USB
	handle_usb=libusb_open_device_with_vid_pid(NULL,VID,PID);
	 if (handle_usb == NULL)
	 {
		 printf("Устройство не подключено\n");
		 return 1;
	 }
	 else printf("Устройство подключено\n");
	 device_usb=libusb_get_device(handle_usb);
	 bus_number=libusb_get_bus_number(device_usb);
	// libusb_reset_device(handle_usb);
	 if (libusb_kernel_driver_active(handle_usb,0)) libusb_detach_kernel_driver(handle_usb, 0);
	 if (libusb_claim_interface(handle_usb,0) < 0)
	 {
		 printf("Ошибка интерфейса\n");
		 return 1;
	 }
	 for(i=0; i<=0xff; i++)
	 {
		 er=libusb_control_transfer(handle_usb,CTRL_IN,i,0x00,0x00,buf,5,500);
		 if(er<0)
		 {
			 libusb_reset_device(handle_usb);
			 printf("Ошибка команды\n");
		 }
	 }

	 if(er)  printf(libusb_error_name(er));
	//******************************
	libusb_attach_kernel_driver(handle_usb,0);
	libusb_close(handle_usb);
	libusb_exit(ctx);
	return EXIT_SUCCESS;
}

