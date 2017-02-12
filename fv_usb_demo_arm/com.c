/************************Copyright(c)******************************* 
 **                       西安邮电学院 
 **                       graduate school 
 **                                     XNMS项目组 
 **                       WebSite :blog.csdn.net/tigerjb 
 **------------------------------------------FileInfo------------------------------------------------------- 
 ** File name:                 main.c 
 ** Last modified Date:  2011-01-31 
 ** Last Version:              1.0 
 ** Descriptions:             
 **------------------------------------------------------------------------------------------------------ 
 ** Created by:               冀博 
 ** Created date:            2011-01-31 
 ** Version:                            1.0 
 ** Descriptions:             The original version 
 **------------------------------------------------------------------------------------------------------ 
 ** Modified by: 
 ** Modified date: 
 ** Version: 
 ** Descriptions: 
 *******************************************************************/  


//串口相关的头文件  
#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<unistd.h>     /*Unix 标准函数定义*/  
#include<sys/types.h>   
#include<sys/stat.h>     
#include<fcntl.h>      /*文件控制定义*/  
#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#include"com.h"



//宏定义  
#define FALSE  -1  
#define TRUE   0  

/******************************************************************* 
 * 名称：                  UART0_Open 
 * 功能：                打开串口并返回串口设备文件描述 
 * 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
 * 出口参数：        正确返回为1，错误返回为0 
 *******************************************************************/  
int UART0_Open(int fd,char* port)  
{  

	fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);  
	if (FALSE == fd)  
	{  
		perror("Can't Open Serial Port");  
		return(FALSE);  
	}  
	//恢复串口为阻塞状态                                 
	if(fcntl(fd, F_SETFL, 0) < 0)  
	{  
		printf("fcntl failed!\n");  
		return(FALSE);  
	}       
	else  
	{  
		printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));  
	}  
	//测试是否为终端设备      
	if(0 == isatty(STDIN_FILENO))  
	{  
		printf("standard input is not a terminal device\n");  
		return(FALSE);  
	}  
	else  
	{  
		printf("isatty success!\n");  
	}                
	printf("fd->open=%d\n",fd);  
	return fd;  
}  
/******************************************************************* 
 * 名称：                UART0_Close 
 * 功能：                关闭串口并返回串口设备文件描述 
 * 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
 * 出口参数：        void 
 *******************************************************************/  

void UART0_Close(int fd)  
{  
	close(fd);  
}  

/******************************************************************* 
 * 名称：                UART0_Set 
 * 功能：                设置串口数据位，停止位和效验位 
 * 入口参数：        fd        串口文件描述符 
 *                              speed     串口速度 
 *                              flow_ctrl   数据流控制 
 *                           databits   数据位   取值为 7 或者8 
 *                           stopbits   停止位   取值为 1 或者2 
 *                           parity     效验类型 取值为N,E,O,,S 
 *出口参数：          正确返回为1，错误返回为0 
 *******************************************************************/  
int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  

	int   i;  
	int   status;  
	int   speed_arr[] = { B115200, B57600, B19200, B9600, B4800, B2400, B1200, B300};  
	int   name_arr[] = {115200,  57600, 19200,  9600,  4800,  2400,  1200,  300};  

	struct termios options;  

	/*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1. 
	*/  
	if  ( tcgetattr( fd,&options)  !=  0)  
	{  
		perror("SetupSerial 1");      
		return(FALSE);   
	}  

	//设置串口输入波特率和输出波特率  
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if  (speed == name_arr[i])  
		{               
			cfsetispeed(&options, speed_arr[i]);   
			cfsetospeed(&options, speed_arr[i]);    
		}  
	}       

	//修改控制模式，保证程序不会占用串口  
	options.c_cflag |= CLOCAL;  
	//修改控制模式，使得能够从串口中读取输入数据  
	options.c_cflag |= CREAD;  

	//设置数据流控制  
	switch(flow_ctrl)  
	{  

		case 0 ://不使用流控制  
			options.c_cflag &= ~CRTSCTS;  
			break;     

		case 1 ://使用硬件流控制  
			options.c_cflag |= CRTSCTS;  
			break;  
		case 2 ://使用软件流控制  
			options.c_cflag |= IXON | IXOFF | IXANY;  
			break;  
	}  
	//设置数据位  
	//屏蔽其他标志位  
	options.c_cflag &= ~CSIZE;  
	switch (databits)  
	{    
		case 5    :  
			options.c_cflag |= CS5;  
			break;  
		case 6    :  
			options.c_cflag |= CS6;  
			break;  
		case 7    :      
			options.c_cflag |= CS7;  
			break;  
		case 8:      
			options.c_cflag |= CS8;  
			break;    
		default:     
			fprintf(stderr,"Unsupported data size\n");  
			return (FALSE);   
	}  
	//设置校验位  
	switch (parity)  
	{    
		case 'n':  
		case 'N': //无奇偶校验位。  
			options.c_cflag &= ~PARENB;   
			options.c_iflag &= ~INPCK;      
			break;   
		case 'o':    
		case 'O'://设置为奇校验      
			options.c_cflag |= (PARODD | PARENB);   
			options.c_iflag |= INPCK;               
			break;   
		case 'e':   
		case 'E'://设置为偶校验    
			options.c_cflag |= PARENB;         
			options.c_cflag &= ~PARODD;         
			options.c_iflag |= INPCK;        
			break;  
		case 's':  
		case 'S': //设置为空格   
			options.c_cflag &= ~PARENB;  
			options.c_cflag &= ~CSTOPB;  
			break;   
		default:    
			fprintf(stderr,"Unsupported parity\n");      
			return (FALSE);   
	}   
	// 设置停止位   
	switch (stopbits)  
	{    
		case 1:     
			options.c_cflag &= ~CSTOPB; break;   
		case 2:     
			options.c_cflag |= CSTOPB; break;  
		default:     
			fprintf(stderr,"Unsupported stop bits\n");   
			return (FALSE);  
	}  

	//修改输出模式，原始数据输出  
	options.c_oflag &= ~OPOST;  

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);//我加的  
	//options.c_lflag &= ~(ISIG | ICANON);  

	//设置等待时间和最小接收字符  
	options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */    
	options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */  

	//如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
	tcflush(fd,TCIFLUSH);  

	//激活配置 (将修改后的termios数据设置到串口中）  
	if (tcsetattr(fd,TCSANOW,&options) != 0)    
	{  
		perror("com set error!\n");    
		return (FALSE);   
	}  
	return (TRUE);   
}  
/******************************************************************* 
 * 名称：                UART0_Init() 
 * 功能：                串口初始化 
 * 入口参数：        fd       :  文件描述符    
 *               speed  :  串口速度 
 *                              flow_ctrl  数据流控制 
 *               databits   数据位   取值为 7 或者8 
 *                           stopbits   停止位   取值为 1 或者2 
 *                           parity     效验类型 取值为N,E,O,,S 
 *                       
 * 出口参数：        正确返回为1，错误返回为0 
 *******************************************************************/  
int UART0_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
	int err;  
	//设置串口数据帧格式  
	if (UART0_Set(fd,speed,0,8,1,'N') == FALSE)  
	{                                                           
		return FALSE;  
	}  
	else  
	{  
		return  TRUE;  
	}  
}  

/******************************************************************* 
 * 名称：                  UART0_Recv 
 * 功能：                接收串口数据 
 * 入口参数：        fd                  :文件描述符     
 *                              rcv_buf     :接收串口中数据存入rcv_buf缓冲区中 
 *                              data_len    :一帧数据的长度 
 * 出口参数：        正确返回为1，错误返回为0 
 *******************************************************************/  
int UART0_Recv(int fd, char *rcv_buf,int data_len)  
{  
	int len,fs_sel;  
	fd_set fs_read;  

	struct timeval time;  

	FD_ZERO(&fs_read);  
	FD_SET(fd,&fs_read);  

	time.tv_sec = 10;  
	time.tv_usec = 0;  

	//使用select实现串口的多路通信  
	fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);  
	if(fs_sel)  
	{  
		len = read(fd,rcv_buf,data_len);  
		printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);  
		return len;  
	}  
	else  
	{  
		printf("Sorry,I am wrong!");  
		return FALSE;  
	}       
}  
/******************************************************************** 
 * 名称：                  UART0_Send 
 * 功能：                发送数据 
 * 入口参数：        fd                  :文件描述符     
 *                              send_buf    :存放串口发送数据 
 *                              data_len    :一帧数据的个数 
 * 出口参数：        正确返回为1，错误返回为0 
 *******************************************************************/  
int UART0_Send(int fd, char *send_buf,int data_len)  
{  
	int len = 0;  

	len = write(fd,send_buf,data_len);  
	if (len == data_len )  
	{  
		return len;  
	}       
	else     
	{  

		tcflush(fd,TCOFLUSH);  
		return FALSE;  
	}  

}  

#define uint8 unsigned char
#define uint16 unsigned short 
#define uint32 unsigned int


#define CRC(crc,byte) (((crc) >> 8 ) ^ tabel[((crc) ^ (unsigned int) (byte)) & 0XFF])
static const unsigned short tabel[256] = {
0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};

unsigned short CalcCRC(uint8 *data, uint32 size)//&dat1 + 8
{
	uint32 i;
	unsigned short crc = 0;
	for (i = 0; i < size; i++) {
		crc = CRC(crc, data[i]);
	}
	return crc;
}

struct led_data{
	uint8 data1[20];
	uint8 len1[2];
	uint8 data2[7];
	uint8 len2[2];
	uint8 data3[23];
	uint8 len3[4];
};

struct led_data2{
	uint16 crc_16;
	uint8 tail;
};


int send_buffer_maker(char *str, char *dat, int len)
{
	//unsigned char send_buf2[]={0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0x01,0x00,0x00,0x80,\
	//0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x02,0x32,0x00,0xA3,0x06,0x01,0x2D,0x00,\
	//0x00,0x01,0x29,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x20,0x00,0x00,0x00,\
	//0x00,0x02,0x00,0x00,0x00,0x00,0x02,0x02,0x03,0x00,0x04,0x05,0x0E,0x00,0x00,\
	//0x00,0x30,0x31,0x32,0x33,0x34,0x35,0x51,0x52,0x53,0x54,0x54,0x56,0x27,0x42,0xA8,0x7D,0x5A};

	unsigned char send_buf[]={0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0x01,0x00,0x00,0x80,0x00,\
	0x00,0x00,0x00,0x00,0x00,0xFE,0x02,0x4A,0x00,0xA3,0x06,0x01,0x2D,0x00,0x00,0x01,0x29,0x00,\
	0x00,0x00,0x00,0x0C,0x00,0x20,0x01,0x30,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x02,\
	0x02,0x03,0x00,0x04,0x00,0x26,0x00,0x00,0x00,0x5C,0x46,0x45,0x30,0x30,0x32,0x30,0x31,0x32,\
	0x33,0x34,0x35,0x51,0x52,0x53,0x54,0x54,0x56,0x27,0x42,0x5C,0x46,0x4F,0x30,0x30,0x32,0xCE,0xD2,0xC3,\
	0xC7,0xCA,0xC7,0xD2,0xBB,0xBC,0xD2,0xC8,0xCB,0xC3,0x95,0x5A};

	struct led_data dat1;
	struct led_data2 dat2;
	
	memcpy(&dat1, send_buf, sizeof(struct led_data));

	unsigned char a[] = {0x00,0x29};
	*(unsigned short *)dat1.len3 = len;
	*(unsigned short *)dat1.len2 = *(unsigned short *)dat1.len3 + 27;
	*(unsigned short *)dat1.len1 = *(unsigned short *)dat1.len2 + 9;
	
	printf("len1 = %d\n", *(unsigned short *)dat1.len1);//len1 = len2 + 9
	printf("len2 = %d\n", *(unsigned short *)dat1.len2);//len2 = len3 +27
	printf("len3 = %d\n", *(unsigned short *)dat1.len3);//data length
	memcpy(dat, &dat1, sizeof(struct led_data));
	//printf("--------------------->%x\n", *((unsigned char *)&(dat1.len3)+2));
	memcpy(dat + sizeof(struct led_data), str, *(unsigned short *)dat1.len3);
	dat2.crc_16 = CalcCRC(dat + 8, *(unsigned short *)dat1.len1 + 14);//len3 + 14   dat + 8
	dat2.tail = 0x5A;
	printf("crc_16 = %d\n", dat2.crc_16);
	memcpy(dat + sizeof(struct led_data) + *(unsigned short *)dat1.len3, &dat2, sizeof(struct led_data2));

	return 0;
}

int uart_led_send_str_1(char *str)  
{  
	int fd;                            //文件描述符  
	int err;                           //返回调用函数的状态  
	int len;                          
	int i;  
	unsigned char dat[2048];  
	if(str == NULL)
		return -1;
	

	send_buffer_maker(str, dat, strlen(str));






	fd = UART0_Open(fd,"/dev/ttySAC0"); //打开串口，返回文件描述符  
	do{  
		err = UART0_Init(fd,57600,0,8,1,'N');  
		printf("Set Port Exactly!\n");  
	}while(FALSE == err || FALSE == fd);  



	//len = UART0_Send(fd,(unsigned char *)((dat1.data2)),7);
		//len = UART0_Send(fd,(unsigned char *)(&(dat1.len2)),2);  
		len = UART0_Send(fd,dat,sizeof(struct led_data) + sizeof(struct led_data2) + strlen(str) - 1); 
		if(len > 0)  
			printf(" %d send data successful\n",i);  
		else  
			printf("send data failed!\n");  


	UART0_Close(fd);               

}  

/*********************************************************************                            End Of File                          ** 
 *******************************************************************/  
