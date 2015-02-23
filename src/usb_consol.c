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
#include <string.h>
#include <libusb-1.0/libusb.h>

libusb_device_handle * usb_open();
void usb_close(libusb_device_handle * handle);
void IncreaseCnt();
void DecodeData(uint8_t *ReadBuffer, uint8_t index); /// декодирование принятой информации

libusb_context *ctx;
libusb_device_handle *handle_usb;
libusb_device        *device_usb;
uint32_t Device_ID;
uint8_t  DeviceCnt;
uint16_t rTimeCnt;
uint16_t rDustDegree;

int config;
int in_count;
int out_count;
unsigned char buf[8];

#define VID 0x2156
#define PID 0xc00d
#define DEV_INTF 0

#define READ_USB   192

#define GET_IDINFO 128
#define READ_DATA  129
#define WRITE_DATA 130
#define SET_LED    131

int er;

int main(void)
{
	puts("Тестовая программа для датчика пыли");
	libusb_init(&ctx);
	libusb_set_debug(ctx,0);

	 handle_usb=usb_open();
	 memset(buf,0,sizeof(buf));
	 er=libusb_control_transfer(handle_usb,READ_USB,READ_DATA,0,0,buf,4,1000);
	 printf("USB >> %X  %X  %X  %X \n",buf[0],buf[1],buf[2],buf[3]);
	 DecodeData(buf,0);
	 printf("USB decode >> %X  %X  %X  %X \n",buf[0],buf[1],buf[2],buf[3]);
	 rDustDegree=buf[0]+(buf[1]<<8);
	 rTimeCnt=buf[2]+(buf[3]<<8);
	 printf("Уровень пыли %i \n",rDustDegree);
	 printf("Количество сработок %u \n",rTimeCnt);
	 usb_close(handle_usb);

    libusb_exit(ctx);
	return EXIT_SUCCESS;
}

libusb_device_handle * usb_open()
{
	libusb_device_handle *handle;
	handle=libusb_open_device_with_vid_pid(NULL,VID,PID);
    if (handle == NULL)
		 {
			 printf("Устройство не подключено\n");
			 return NULL;
		 }
    else printf("Устройство подключено\n");
   // if (libusb_kernel_driver_active(handle,0)) libusb_detach_kernel_driver(handle, 0);
	//if (libusb_claim_interface(handle,0) < 0)
	//	 {
		//	 printf("Ошибка интерфейса\n");
	//		 return NULL;
	//	 }
    // считываем ид устройства и внутренний счетчик датчика по которому декодируем информацию.
    er=libusb_control_transfer(handle,READ_USB,GET_IDINFO,0,0,buf,5,1000);
    printf("USB >> %X  %X  %X  %X  %X \n",buf[0],buf[1],buf[2],buf[3],buf[4]);
    Device_ID=buf[0]+(buf[1]<<8)+(buf[2]<<16)+(buf[3]<<24);
    printf("ID datchika %X \n",Device_ID);
    DeviceCnt=buf[4];
    printf("кодовый счетчик %X \n",DeviceCnt);
	return handle;
}

void usb_close(libusb_device_handle * handle)
{
//	libusb_attach_kernel_driver(handle,0);
	libusb_close(handle);
}

// подпрограмма увеличения счетчика
void IncreaseCnt()
{
	DeviceCnt++;
	if(DeviceCnt>65535) DeviceCnt=0;
}

void DecodeData(uint8_t *ReadBuffer, uint8_t index)
{
	int flag2;
	int ui1;
	ulong ul1,ul2,ul3;
	uint i1,i2,i3,i4,i5,i6,i7;
	ui1=ReadBuffer[index]+(ReadBuffer[index+1]<<8)+(ReadBuffer[index+2]<<16)+(ReadBuffer[index+3]<<24);
	i2=(DeviceCnt/16)&0x0f;
	i3=DeviceCnt&0x0f;
    i3^=i2;
    i4=i3-1;
    ul1=1;
    ul2=1;
    ul3=0;
    i7=0;
    i1=0;
    while(i1<=i7)
    {
    	ul1<<=1;
    	i1++;
    	i7=i4;
    }
    i5=31-i3;
    i1=0;
    while(i1<=i7)
    {
    	flag2=(((int)ui1 & (int)(long)ul2) >0);
    	if(flag2) ul3+=ul1;
    	ul2<<=1;
    	ul1<<=1;
    	i1++;
    	i7=i5;
    }
    i6=i3-1;
    i1=0;
    while(i1<=i7)
    {
    	flag2=(((int)ui1 & (int)(long)ul2) >0);
    	if(flag2) ul3+=ul1;
    	ul2<<=1;
    	ul1<<=1;
    	i1++;
    	i7=i6;
    }
    ul3=(ulong)((long)ul3 & (long)-1);
    ul3^=(ulong)Device_ID;
    ReadBuffer[index]=(uint8_t)ul3&0xff;
    ReadBuffer[index+1]=(uint8_t)(ul3>>8)&0xff;
    ReadBuffer[index+2]=(uint8_t)(ul3>>16)&0xff;
    ReadBuffer[index+3]=(uint8_t)(ul3>>24)&0xff;

}
