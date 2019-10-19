#include <common.h>
#include <watchdog.h>
#include <command.h>

#define dump_buf(buf, len) do{\
	int i;\
	char *p = (void*)(buf);\
	printf("%s->%d, buf=0x%.8x, len=%d\n", \
			__FUNCTION__, __LINE__, \
			(int)(buf), (int)(len)); \
	for(i=0;i<(len);i++){\
		printf("0x%.2x ", *(p+i));\
		if( !((i+1) & 0x07) )\
		printf("\n");\
	}\
	printf("\n");\
}while(0)

#define	XHEAD	0xAB
#define	XCMD	0xCD
#define	ACK     0xAA            /* ACK VALUE */
#define	NAK     0x55            /* NAK VALUE */

#define START_FRAME_LEN	5
#define MAX_BUFF_SIZE		1024

static char recv_buf[MAX_BUFF_SIZE];

static unsigned short crc16_table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78, 
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067, 
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634, 
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3, 
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0, 
};

static unsigned short calc_crc16(unsigned char *packet, unsigned long length)
{
	unsigned short crc16 = 0;
	unsigned long i;

	for (i = 0; i < length; i++){        
		crc16 = ((crc16<<8)|packet[i])^crc16_table[(crc16>>8)&0xFF];
	}

	for (i = 0; i < 2; i++){
		crc16 = ((crc16<<8)|0)^crc16_table[(crc16>>8)&0xFF];
	}

	return crc16;
}

static int recv_byte(void)
{
	if(serial_tstc()){
		return serial_getc();
	}

	return -1;
}

static char recv_data(void)
{
	int ret = -1;

	while(ret == -1)
		ret = recv_byte();

	return (char)ret;
}

void download_process(void)
{
	int i = 0, cr = 0, ret = -1;
	unsigned int head_frame_len = 0;
	unsigned int cmd_len = 0;
	unsigned char send_buf[20] = {0};
	unsigned short cksum;

	while(1){
retry:
		cr = recv_byte();
		if(cr == -1)
			goto retry;

		if(XHEAD == cr){
			head_frame_len = START_FRAME_LEN;
			recv_buf[0] = (char)cr;

			/* RECV: head frame */
			for(i=0;i<(head_frame_len-1);i++){
				recv_buf[i+1] = recv_data();
			}

			/* crc check */
			cksum = calc_crc16((unsigned char*)recv_buf,3);
			if(cksum == ((recv_buf[3] << 8) | recv_buf[4])){ 

				/* init */
				cmd_len = ((recv_buf[1] << 8) | recv_buf[2]) + 3;

				/* SEND: ack */
				send_buf[0] = ACK;
			} else {
				/* init */
				cmd_len = 0; 

				/* SEND: nak */
				send_buf[0] = NAK;			
			}
			serial_putc(send_buf[0]);
		} else if(XCMD == cr){
			recv_buf[0] = (char)cr;

			/* RECV: cmd data */
			for(i=0;i<(cmd_len-1);i++){
				recv_buf[i+1] = recv_data();
			}

			/* crc check */
			cksum = calc_crc16((unsigned char*)recv_buf, cmd_len - 2);
			if(cksum == ((recv_buf[cmd_len-2] << 8) | recv_buf[cmd_len-1])){
				/* SEND: ack wait result */
				send_buf[0] = ACK;
				serial_putc(send_buf[0]);
			} else {
				memset(recv_buf, 0, sizeof(recv_buf));
				/* SEND: nak */
				send_buf[0] = NAK;
				serial_putc(send_buf[0]);
				//dump_buf(recv_buf, cmd_len);

				goto retry;
			}

			/* clean crc */
			recv_buf[cmd_len-1] = 0;
			recv_buf[cmd_len-2] = 0;

			/* cmd process */
			ret = run_command((recv_buf+1),0);
			if (ret) {
				/* SEND: end flag */
 				serial_puts("[EOT](ERROR)\n");
 			}
			else{
				/* SEND: end flag */
				serial_puts("[EOT](OK)\n");
			}
		} else {
			/* flush fifo */
		}
	}
}

__attribute__((weak)) void download_boot(int (*handle)(void))
{
	if (START_MAGIC == (*(volatile unsigned int *)(REG_START_FLAG))){
		/* clear flag */
		*(volatile unsigned int *)(REG_START_FLAG) = 0;

		serial_puts("start download process.\n");

		/* wait cmd from pc */
		for (;;) {
            if(SELF_BOOT_TYPE_USBDEV == (*(volatile unsigned int *)(SYS_CTRL_REG_BASE + REG_SC_GEN9))) {
			#ifndef CONFIG_MINI_BOOT
			    udc_connect();
			#endif
            }
            else {
			    download_process();
            }
        }
	}

	if (handle)
		handle();
}
