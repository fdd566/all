/**************************************************************
** Copyright (C) 2016, TG Corporation
** All rights reserved.  
** 
** @file  main.c
** @brief 
**
** 头文件： ubuntu 64上通过USB操作指静脉设备
**  
** @author  liangshuangchou
** @date    2016/08/18 10:00 
** @note    
** @version 1.0
** @todo
** @history
** Date             Name             Change Reason
** 2016/08/18       liangshuangchou  New create
*****************************************************************/

/*****************************************************************************/
/*                             include                                       */
/*****************************************************************************/
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include <termios.h>
#include <fcntl.h> 
#include <pthread.h>

#include"XG_Protocol.h"
#include "utf8_gb2312.h"

#include "mycurl.h"


/*****************************************************************************/
/*                              define                                       */
/*****************************************************************************/
/**
 * 声音提示
 */
#define XG_CMD_EXT_CTRL        0x4B    //扩展控制

#define FV_V_ASYN              0       // 异步
#define FV_V_SYN               1       // 同步

#define	FV_V_INPUT_FINGE_A     23	    // 请再放一次
#define	FV_V_INPUT_FINGE       27		// 请自然轻放手指

#define FV_V_REG_SUCCESS	    0	  // 登记成功
#define	FV_V_REG_FAIL	        2     // 登记失败
#define	FV_V_VERIFY_SUCCESS	   33     // 验证失败			
#define	FV_V_VERIFY_FAIL       32     // 验证成功			


#define PACKET_LEN             24


/**
 * 指静脉信息和注册模板buffer的大小
 */
#define FV_KEY_SIZE  2048
#define FV_TMP_SIZE  8192


typedef int (*SendDataCallBack)(int total, int sent);
/*****************************************************************************/
/*                          struct / union / enum                            */
/*****************************************************************************/
//指静脉模块设备操作相关函数
typedef struct _FUN_COM
{  
	//获取版本号
	int (*XG_GetVeinLibVer) (char *pVer); 
	
	//获取当前连接的USB指静脉设备的个数，USB驱动必须是HID模式
	int (*XG_DetectUsbDev) ();
	
	//打开并连接指静脉设备
	int (*XG_OpenVeinDev) (char* pDev, int Baud, int Addr, UINT8 Password[16], int Len, PHANDLE pHandle);
	
	//关闭指静脉设备
	int (*XG_CloseVeinDev) (UINT8 Addr, HANDLE Handle);
	
	//发送一个指令包
	int (*XG_SendPacket) (UINT8 Addr, UINT8 Cmd, UINT8 Encode, UINT8 Len, UINT8* pData, HANDLE Handle);
	
	//接收一个指令包
	int (*XG_RecvPacket)(UINT8 Addr, UINT8* pData, HANDLE Handle);
	
	//写入数据
	int (*XG_WriteData) (UINT8 Addr, UINT8 Cmd, UINT8* pData, UINT32 size, HANDLE Handle);
	
	//读取数据
	int (*XG_ReadData) (UINT8 Addr, UINT8 Cmd, UINT8* pData, UINT32 size, HANDLE Handle);
	
	//更新指静脉固件
	int (*XG_Upgrade) (int iAddr, const char* fname, HANDLE Handle);
	
	//写入指静脉登记数据
	int (*XG_WriteDevEnrollData) (int iAddr, UINT32 User, UINT8 *pBuf, HANDLE Handle);
	
	//读取指静脉登记数据
	int (*XG_ReadDevEnrollData) (int iAddr, UINT32 User, UINT8 *pBuf, UINT32 *pSize, HANDLE Handle);
	
	//获取数据包发送状态，固件升级时间较长用于进度条
	int (*XG_SetCallBack) (HANDLE Handle, SendDataCallBack pSendData);	

	/*********************************************************************
	功能：从设备获取指静脉特征
	参数：
	Addr(输入):设备地址,0[广播，所有设备都接收]，1-255[指定接收设备地址]
	pBuf(输出):特征数据缓冲区
	pSize(输出):特征数据大小，多少字节
	返回值：
	XG_ERR_SUCCESS:特征获取成功
	XG_ERR_INVALID_PARAM:参数非法
	XG_ERR_COM:设备或通信错误
	XG_ERR_TIME_OUT:操作超时
	XG_ERR_FAIL:其他错误
	**********************************************************************/
	int	(*XG_GetVeinChara) (int iAddr, UINT8* pBuf, UINT32 *pSize, HANDLE Handle);

	//功能：检测是否有手指放入，如检测到手指放入返回值大于0
	int	(*XG_GetFingerStatus) (int iAddr, HANDLE Handle);
} FunCom_t, *pFunCom_t;


//指静脉算法库操作相关函数
typedef struct _FUN_VEIN
{
	//创建算法库实例
	int (*XGV_CreateVein) (PHANDLE pHandle, int UserNum); 
	
	//销毁算法库实例
	int (*XGV_DestroyVein) (HANDLE hHandle);
	
	//登记
	int (*XGV_Enroll) (HANDLE hHandle, UINT32 User, UINT8* pBuf, UINT32 size, UINT8 CheckSameFinger, UINT16* pQuality);
	
	//验证，一般1：N识别比对时使用
	int (*XGV_Verify) (HANDLE hHandle, UINT32* pUser, UINT32 Num, UINT8* pBuf, UINT32 size, UINT8 Group, UINT16* pQuality);
	
	// 1:1验证
	UINT8* (*XGV_CharaVerify) (HANDLE hHandle, UINT8* pEnroll, int EnrollSize, UINT8* pChara, int CharaSize);
	
	//获取1:1验证成功后自学习后的登记数据，可替换原有的登记数据
	int (*XGV_GetCharaVerifyLearn) (HANDLE hHandle, UINT8* pBuf);
	
	//设置安全等级，1,2,3，安全等级越高误识率越低
	int (*XGV_SetSecurity) (HANDLE hHandle, UINT8 Level);
	
	//获取登记数据
	int (*XGV_GetEnrollData) (HANDLE hHandle, UINT32 User, UINT8* pData, UINT32* pSize);
	
	//获取登记数据登记时候指定的用户ID
	int (*XGV_GetEnrollUserId) (HANDLE hHandle, UINT8* pData);
	
	//保存登记数据
	int (*XGV_SaveEnrollData) (HANDLE hHandle, UINT32 User, UINT8 Group, UINT8* pData, UINT16 Size);
	
	//删除登记数据
	int (*XGV_DelEnrollData) (HANDLE hHandle, UINT32 User);
	
	//获取登记用户数
	int (*XGV_GetEnrollNum) (HANDLE hHandle, UINT32 *pMaxUser, UINT32 *pMaxTempNum, UINT32 *pEnrollUser, UINT32 *pEnrollTempNum);
	
	//获取空ID
	int (*XGV_GetEnptyID) (HANDLE hHandle, UINT32 *pEnptyId, UINT32 StartId, UINT32 EndId);
	
	//获取指定用户登记的模板数
	int (*XGV_GetUserTempNum) (HANDLE hHandle, UINT32 User, UINT32 *pTempNum);
} FunVein_t, *pFunVein_t;

//指静脉算法库操作相关函数
typedef struct
{
	int	(*EncodeBase64)(const unsigned char* pSrc, char* pDst, int nSrcLen);
	int (*DecodeBase64)(const char* pSrc, unsigned char* pDst, int nSrcLen);
}B64_Fun_t;



struct Thread_Argv
{
	UINT8 iDevAddress;
	HANDLE hDevHandle;
};


// 在小端系统上可以直接作为buf是在用，因为无填充

/*包数据结构 24字节*/
typedef struct _XG_PACKET_
{
	unsigned short  wPrefix; //包标识0xAABB
	unsigned char   bAddress; //设备地址， 0为广播
	unsigned char   bCmd; //命令码
	unsigned char   bEncode; //数据编码，默认为 0 
	unsigned char   bDataLen; //有效数据长度（字节）
	unsigned char   bData[16]; //包数据
	unsigned short  wCheckSum; //包检验， 0-22字节和
} XG_PACKET, *PXG_PACKET;

/*****************************************************************************/
/*                             Global Data                                   */
/*****************************************************************************/
// 动态链接库
static char* so_file = "./VeinApiLinux.so";

// 动态链接库中方法
static FunCom_t m_ComFun;
static FunVein_t m_VeinFun;
static B64_Fun_t m_B64Fun;

// 句柄
static HANDLE m_hDevHandle;    // 指静脉设备文件句柄
static HANDLE m_hVeinHandle;   // 指静脉算法库文件句柄   

// 全局存放指静脉信息
static unsigned char  fv_data[4096]  = {0};
static unsigned char  fv_reg_data[8192]  = {0};
static  char  fv_base64[4096] = {0};
static  char  reg_base64[8192] = {0};

// 设备密码
static char Password[16] = "00000000";
static unsigned char iDevAddress = 0;

/****************************************************************************/
/*                                Prototype                                 */
/****************************************************************************/
static void trace( const unsigned char* databuf, unsigned int bufLen, const char* comment );
static int init_so();
static void* GetDlFun(void *Handle, char *pFunName);
static int openDevAndVein();
static int speak(unsigned char bSound, unsigned char bPlay);
static int waitFingerInput( int* rst );
static int getFingerVeinData(unsigned char* fvData, 
                        unsigned int size, 
                        unsigned int* len, 
                        unsigned int g_num,
                        unsigned int interval,
                        unsigned int prompt);
static int getRegTemp(unsigned char* rTemp, unsigned int size, unsigned int* len);
static int verify_1_1( UINT8 * regTemp, int regTemLen,  UINT8 * fvData, int fvDataLen);

/*****************************************************************  
** @brief      操作串口的Main函数
** @author     liangshuangchou
** @param      <in>  <int>           参数数目
** @param      <in> <char**>         参数
** @exception
** @return     0 :正常
**             -1:异常
** @note       
**
** @see 
** @date       2014/03/27
******************************************************************/
#if 0
int main( int argc, char** argv )
{
    int    i_ret          = -1;      /* 调用函数返回值 */
    unsigned int reg_len = 0;
    unsigned int fv_len =0;

    // 打开动态链接库
    i_ret = init_so();
    if( 0 != i_ret )
    {
        printf("init_so Error.\n");
        return -1;
    }


   // 打开设备，创建算法库
    i_ret = openDevAndVein();
    if( 0 != i_ret )
    {
        printf("openDevAndVein Error.\n");
        return -1;
    }


    printf("\n\n\n");
    printf("====================================================================\n");
    printf("=======================开始注册指静脉===============================\n");
    printf("====================================================================\n");

    sleep(3);

    /**
     *  注册一根手指的指静脉
     */
     i_ret = getRegTemp( fv_reg_data, sizeof(fv_reg_data), &reg_len );
     if( 0 != i_ret )
     {
        printf("登记失败!\n");
        speak(FV_V_REG_FAIL, FV_V_SYN);
        printf("getRegTemp Error.\n");
        return -1;
     }
     printf("登记成功!\n");
     speak(FV_V_REG_SUCCESS, FV_V_SYN);
     trace(fv_reg_data, reg_len, "注册指静脉(二进制)为: ");
     m_B64Fun.EncodeBase64(fv_reg_data, reg_base64, reg_len);
     printf("注册指静脉(Base64)为 : %s \n", reg_base64);
   

    printf("\n\n\n");
    printf("====================================================================\n");
    printf("===========================开始认证=================================\n");
    printf("====================================================================\n");

    sleep(3);
    
    /**
    *  采集一个指静脉，进行认证
    */
    speak(FV_V_INPUT_FINGE, FV_V_SYN);
    i_ret = getFingerVeinData( fv_data, sizeof(fv_data), &fv_len, 3, 1000, FV_V_INPUT_FINGE_A);
    if( 0 != i_ret )
    {
        printf("getFingerVeinData Error.\n");

        return -1;
    }
    trace(fv_data, fv_len, "指静脉特征(二进制)为: ");
    m_B64Fun.EncodeBase64(fv_data, fv_base64, fv_len);
    printf("指静脉特征(Base64)为 : %s \n", fv_base64);


    // 认证
    i_ret = verify_1_1((UINT8*)reg_base64, strlen(reg_base64), (UINT8*)fv_base64, strlen(fv_base64));
    if( 0 != i_ret )
    {
        printf("验证失败!\n");
        speak(FV_V_VERIFY_FAIL, FV_V_SYN);
        printf("verify_1_1 Error.\n");

        return -1;
    }
    printf("验证成功!\n");
    speak(FV_V_VERIFY_SUCCESS, FV_V_SYN);  

    return 0;
}
#endif




void* heart_beat(void* arg)
{

	char buffer[1024] = {0};
	char str_2312[2048] = {0};
	while(1){
		buffer[0] = 0;
		GetLed(buffer, "111", 0);
		printf("buff_led == %s\n",buffer);
		if(buffer[0] == '1'){
			char str[2048] = {0x5C,0x46,0x45,0x30,0x30,0x32,0x5C,0x46,0x4F,0x30,0x30,0x32};
			strcpy(&str[12],&buffer[2]);
			Utf82Gb2312(str, strlen(str), str_2312, 2048);
			printf("buff_str==%s\n",str_2312);
			uart_led_send_str_1(str_2312);
		}
		sleep(10);
	}
}



int init()
{


	pthread_t p_led;
	pthread_create(&p_led, NULL, heart_beat, NULL);

	return 0;
}





static UINT8 fvData[4096] = {0};
static UINT32 fvData_len = 0;
int main( int argc, char** argv )
{

	init();

	//char buffer[1024] = {0};
	//ToServerGetAuth(buffer, "1111111111111111", 0);
	//GetLed(buffer, "111", 0);
	//printf("buff == %s\n",buffer);
	//buffer[0] = 0;
	//ToServerGetAuth(buffer, "1111111111111111", 0);
	//printf("buff == %s\n",buffer);
    int    i_ret          = -1;      /* 调用函数返回值 */
    unsigned int reg_len = 0;
    unsigned int fv_len =0;
    int i = 0;
    int input_rst = -1;

    // 打开动态链接库
    i_ret = init_so();
    if( 0 != i_ret )
    {
        printf("init_so Error.\n");
        return -1;
    }


   // 打开设备，创建算法库
    i_ret = openDevAndVein();
    if( 0 != i_ret )
    {
        printf("openDevAndVein Error.\n");
        return -1;
    }


    printf("\n\n\n");
    printf("====================================================================\n");
    printf("=======================开始注册指静脉===============================\n");
    printf("====================================================================\n");


    while(1)
    {
        printf("--------请自然轻放手指(%d)------ \n", i+1);
        
        //sleep(1);
        
        input_rst = -1;
        i_ret = waitFingerInput(&input_rst);
        if( 0 != i_ret )
        {
            printf("waitFingerInput Error.\n");
        }
        else if( 1 != input_rst )
        {
            printf("未找到手指.\n");
        }
        else
        {
             printf("检测到手指.\n");
             
             memset(fvData, 0x00, sizeof(fvData));
             memset(fv_base64, 0x00, sizeof(fv_base64));
             i_ret = 0;
             fvData_len = 0;
             i_ret = m_ComFun.XG_GetVeinChara(iDevAddress, fvData, &fvData_len, m_hDevHandle);
            if (i_ret <= 0)
            {
                printf("m_ComFun.XG_GetVeinChara Error(%d:%d).\n", i_ret, fvData_len);
            }
            else
            {
                fvData_len = i_ret;
                trace(fvData, fvData_len, "指静脉特征(二进制)为: ");
				fv_base64[0] = 0;
                m_B64Fun.EncodeBase64(fvData, fv_base64, fvData_len);
                printf("指静脉特征(Base64)为 : %s \n", fv_base64);
				if(fv_base64[0] == 0)
					continue;
				char buffer[1024] = {0};
				ToServerGetAuth(buffer, fv_base64, 0);
				printf("buff == %s\n",buffer);
				if(buffer[0] == '0'){
					speak(FV_V_VERIFY_SUCCESS, 1);
					system("./o1.sh");
				}
				else{
					speak(FV_V_VERIFY_FAIL, 1);
				}
					
            }
            
        }
        
        printf("--------请拿开手指(%d)------ \n", i+1);
        
        printf("\n");
    }

    return 0;
}





/*****************************************************************  
** @brief      初始化动态链接库，获得符号链接
** @author     liangshuangchou
** @param      无
** @exception
** @return     0 :正常
**             -1:异常
** @note       
**
** @see 
** @date       2016/08/18
******************************************************************/
int init_so()
{
	// int i;
	char* eInfo;
	void *hLibHandle = NULL;

    printf("Open so start:%s\n", so_file);
	
	memset(&m_ComFun, 0, sizeof(m_ComFun));
	memset(&m_VeinFun, 0, sizeof(m_VeinFun));

	
	hLibHandle = dlopen(so_file, RTLD_LAZY);
	if(hLibHandle == NULL)
	{
		eInfo = dlerror();
		printf("libVeinApi.so error...%s\n",eInfo);
		return -1;
	}

    // 操作设备方法
	m_ComFun.XG_GetVeinLibVer = GetDlFun(hLibHandle, "XG_GetVeinLibVer");
	m_ComFun.XG_DetectUsbDev = GetDlFun(hLibHandle, "XG_DetectUsbDev");
	m_ComFun.XG_OpenVeinDev = GetDlFun(hLibHandle, "XG_OpenVeinDev");
	m_ComFun.XG_CloseVeinDev = GetDlFun(hLibHandle, "XG_CloseVeinDev");
	m_ComFun.XG_SendPacket = GetDlFun(hLibHandle, "XG_SendPacket");
	m_ComFun.XG_RecvPacket = GetDlFun(hLibHandle, "XG_RecvPacket");
	m_ComFun.XG_WriteData = GetDlFun(hLibHandle, "XG_WriteData");
	m_ComFun.XG_ReadData = GetDlFun(hLibHandle, "XG_ReadData");
	m_ComFun.XG_Upgrade = GetDlFun(hLibHandle, "XG_Upgrade");
	m_ComFun.XG_WriteDevEnrollData = GetDlFun(hLibHandle, "XG_WriteDevEnrollData");
	m_ComFun.XG_ReadDevEnrollData = GetDlFun(hLibHandle, "XG_ReadDevEnrollData");
	m_ComFun.XG_SetCallBack = GetDlFun(hLibHandle, "XG_SetCallBack");
    m_ComFun.XG_GetVeinChara = GetDlFun(hLibHandle, "XG_GetVeinChara");
    m_ComFun.XG_GetFingerStatus = GetDlFun(hLibHandle, "XG_GetFingerStatus");


    // 算方法库方法
	m_VeinFun.XGV_CreateVein = GetDlFun(hLibHandle, "XGV_CreateVein");
	m_VeinFun.XGV_DestroyVein = GetDlFun(hLibHandle, "XGV_DestroyVein");
	m_VeinFun.XGV_SetSecurity = GetDlFun(hLibHandle, "XGV_SetSecurity");
	m_VeinFun.XGV_Enroll = GetDlFun(hLibHandle, "XGV_Enroll");
	m_VeinFun.XGV_Verify = GetDlFun(hLibHandle, "XGV_Verify");
	m_VeinFun.XGV_CharaVerify = GetDlFun(hLibHandle, "XGV_CharaVerify");
	m_VeinFun.XGV_GetCharaVerifyLearn = GetDlFun(hLibHandle, "XGV_GetCharaVerifyLearn");
	m_VeinFun.XGV_SaveEnrollData = GetDlFun(hLibHandle, "XGV_SaveEnrollData");
	m_VeinFun.XGV_GetEnrollData = GetDlFun(hLibHandle, "XGV_GetEnrollData");
	m_VeinFun.XGV_DelEnrollData = GetDlFun(hLibHandle, "XGV_DelEnrollData");
	m_VeinFun.XGV_GetEnrollNum = GetDlFun(hLibHandle, "XGV_GetEnrollNum");
	m_VeinFun.XGV_GetEnptyID = GetDlFun(hLibHandle, "XGV_GetEnptyID");
	m_VeinFun.XGV_GetUserTempNum = GetDlFun(hLibHandle, "XGV_GetUserTempNum");
	m_VeinFun.XGV_GetEnrollUserId = GetDlFun(hLibHandle, "XGV_GetEnrollUserId");


    // Base64转换
    m_B64Fun.EncodeBase64 = GetDlFun(hLibHandle, "EncodeBase64");;

    printf("Open so successful:%s\n", so_file);

	return 0;
}

/*****************************************************************  
** @brief      检测设备，打开设备，并初始化算法库
** @author     liangshuangchou
** @param      无
** @exception
** @return     0 :正常
**             -1:异常
** @note       
**
** @see 
** @date       2016/08/18
******************************************************************/
int openDevAndVein()
{
    int usbNum = 0;
    int i_ret = -1;
    char Ver[100] = {0};

    printf( "Open dev and create Vein Start.\n");

    if( NULL == m_ComFun.XG_OpenVeinDev )
    {
         printf( "m_ComFun.XG_OpenVeinDev is NULL.\n");
         return -1;
    }

    if( NULL == m_ComFun.XG_DetectUsbDev )
    {
         printf( "m_ComFun.XG_DetectUsbDev is NULL.\n");
         return -1;
    }

    // 检测USB指静脉设备
    usbNum = m_ComFun.XG_DetectUsbDev();
    if( usbNum <= 0 )
    {
         printf( "没有发现指静脉USB设备.\n");
         return -1;
    }

    printf("检测到的USB设备个数为: %d . \n", usbNum);

    // 打开设备
    i_ret = m_ComFun.XG_OpenVeinDev("USB", 0, iDevAddress, (UINT8*)Password, strlen(Password), &m_hDevHandle);
    if( XG_ERR_SUCCESS != i_ret )
    {
         printf( "m_ComFun.XG_OpenVeinDev Error(%d).\n", i_ret);
         return -1;
    }

    // 获得设备版本号
	if(m_ComFun.XG_GetVeinLibVer(Ver) == XG_ERR_SUCCESS)
	{
		printf("XG_GetVeinLibVer %s\n", Ver);
	}
    else
    {
        printf("XG_GetVeinLibVer ERROR.\n");
    }


    // 创建算法库(近存储计算一个用户)
    i_ret =  m_VeinFun.XGV_CreateVein(&m_hVeinHandle, 1);
    if( XG_ERR_SUCCESS != i_ret )
    {
         printf( "m_ComFun.XGV_CreateVein Error(%d).\n", i_ret);
         return -1;
    }


    i_ret = m_VeinFun.XGV_SetSecurity(m_hVeinHandle, 1);
    if( XG_ERR_SUCCESS != i_ret )
    {
         printf( "m_ComFun.XGV_SetSecurity Error(%d).\n", i_ret);
         return -1;
    }

    printf( "Open dev and create Vein Successful.\n");
    
    return 0;
    
}


void closeDevVein()
{
    if( NULL != m_hVeinHandle )
    {
        m_VeinFun.XGV_DestroyVein(m_hVeinHandle);
        m_hVeinHandle = NULL;
    }

    if( NULL != m_hDevHandle )
    {
       m_ComFun.XG_CloseVeinDev(iDevAddress, m_hDevHandle);
       m_hDevHandle = NULL;
    }

    return;
}



/*****************************************************************  
** @brief      设备语音提示
** @author     liangshuangchou
** @param      [in] <unsigned char>     声音模板定义序号
** @param      [in] <unsigned char>     同步/异步方式
** @exception
** @return     0 :正常
**             -1:异常
** @note       
**
** @see 
** @date       2016/08/18
******************************************************************/    
int speak(unsigned char bSound, unsigned char bPlay)
{
	unsigned char buf[16] = { 0 };
    unsigned char rev_buf[PACKET_LEN] = { 0 };
    
	buf[0] = 0x01;//播放语音
	buf[1] = bSound; //语音序号
	buf[2] = bPlay; //播放方式，0异步，1同步
	if(m_ComFun.XG_SendPacket(iDevAddress, XG_CMD_EXT_CTRL, 0, 3, buf, m_hDevHandle ) == XG_ERR_SUCCESS)
	{
		if(m_ComFun.XG_RecvPacket(iDevAddress, rev_buf, m_hDevHandle) == XG_ERR_SUCCESS)
		{
/*				if(pack.bData[0] == XG_ERR_SUCCESS)
			{
				return XG_ERR_SUCCESS;
			} 
			else 
			{
				return pack.bData[1];
			}*/
		}
        else
        {
            return 0;
        }
	}
    else
    {
        return -1;
    }

    return 0;
}


/*****************************************************************  
** @brief      检测手指是否放入
** @author     liangshuangchou
** @param      [in] <unsigned char>     声音模板定义序号
** @param      [in] <unsigned char>     同步/异步方式
** @exception
** @return     0 :正常
**             -1:异常
** @note       
**
** @see 
** @date       2016/08/18
******************************************************************/   
int waitFingerInput( int* rst )
{
#if 0
    XG_PACKET pack = {0}; 

	int ret = m_ComFun.XG_SendPacket(iDevAddress, XG_CMD_FINGER_STATUS, 0, 0, NULL, m_hDevHandle);
    if( XG_ERR_SUCCESS != ret )
    {
        return -1;
    }

	ret = m_ComFun.XG_RecvPacket(iDevAddress, (UINT8*)(&pack), m_hDevHandle);
    if( XG_ERR_SUCCESS != ret )
    {
        return -1;
    }

	if(pack.bData[0] == XG_ERR_SUCCESS)
	{
		*rst = pack.bData[1];
	}
    else
    {
       *rst = -1; 
    }
#endif    

    int i_ret = 0;
    *rst = -1;
    i_ret = m_ComFun.XG_GetFingerStatus(iDevAddress, m_hDevHandle);
    if(  i_ret > 0 )
    {
        *rst = 1;
    }

    
    return 0;
    
}



/*****************************************************************  
** @brief      获得指静脉信息
** @author     liangshuangchou
** @param      [in/out]  <unsigned char*>    存放指静脉信息的buffer
** @param      [in]      <unsigned int>      buffer大小
** @param      [in]      <unsigned int*>     数据长度
** @param      [in]      <unsigned int>      采集次数
** @param      [in]      <unsigned int>      时间间隔，毫秒
** @param      [in]      <unsigned int>      失败后再次提示音
** @exception
** @return     0: 成功  -1: 错误
** @note       fvData一般是2048大小;
**             循环判断g_num次，每次等待时间interval(ms)
**
** @see 
** @date       2016/08/08
******************************************************************/
int getFingerVeinData(unsigned char* fvData, 
                        unsigned int size, 
                        unsigned int* len, 
                        unsigned int g_num,
                        unsigned int interval,
                        unsigned int prompt)
{
    int i_ret = -1;
    int input_rst = -1;
    unsigned int num = 0;

    for( num = 0; num < g_num; ++num )
    { 
        usleep(interval * 1000);
        
        // 判断是否放入手指
        input_rst = -1;
        i_ret = waitFingerInput(&input_rst);
        //Trace(TRACE_LEVEL_ERROR, "waitFingerInput:%d \n", i_ret);
        if( 0 != i_ret )
        {
            printf("waitFingerInput Error.\n");
            return -1;
        }
        else if( 1 != input_rst )
        {
            // 请再放一次
            if( (num +1) >=  g_num )
            {
                break;
            }
            
            i_ret = speak(prompt, FV_V_SYN);
            if( 0 != i_ret )
            {
                printf("speak Error.\n");
                return -1;
            }
            
            continue;
        }
            
        // 获取指静脉信息
        i_ret = m_ComFun.XG_GetVeinChara(iDevAddress, fvData, len, m_hDevHandle);
        //Trace(TRACE_LEVEL_ERROR, "XG_GetVeinChara:%d \n", i_ret);
        if (i_ret <= 0)
        {
            printf("m_ComFun.XG_GetVeinChara Error(%d:%d).\n", i_ret, *len);
            return -1;
        }
        else
        {
            *len = i_ret;
            return 0;
        }
    }


    printf("没有检测到手指.\n");
	return -1;
    
}




void* GetDlFun(void *Handle, char *pFunName)
{
	char* eInfo = NULL;
	void* fun = NULL;
	
	fun = dlsym(Handle, pFunName);
	eInfo = dlerror();
	if(eInfo != NULL) 
	{
		printf("FUN:%s error:%s\n", pFunName, eInfo);
		return NULL;
	}
	return fun;
}


/*****************************************************************  
** @brief      登记注册指静脉信息时，获得指静脉模板
** @author     liangshuangchou
** @param      [in/out]  <unsigned char*>   存放指静脉信息的buffer
** @param      [in]      <unsigned int>     buffer大小
** @param      [in]      <unsigned int*>     数据长度
** @exception
** @return     0:              打开成功  
**             -1:             错误
**             1:              不是同一根手指
** @note       fvData一般是大于6K的buffer
**
** @see 
** @date       2016/08/08
******************************************************************/
int getRegTemp(unsigned char* rTemp, unsigned int size, unsigned int* len)
{
    int            i_ret     = -1;
    int            ret_flag  = 0;;
    unsigned char* fvkey_p   = NULL;
    unsigned int   fvkey_len = 0;
    UINT32         u32Index  = 0;
    int            i         = 0;
    

    if( ( NULL == rTemp )  || ( size < 8192)  || ( NULL == len) )
    {
        return -1;
    }


    // 删除算法库中id为1的数据，并获得一个空闲的id
    m_VeinFun.XGV_DelEnrollData(m_hVeinHandle, 1);
    i_ret =  m_VeinFun.XGV_GetEnptyID(m_hVeinHandle, &u32Index, 1, 2001);
    if( XG_ERR_SUCCESS != i_ret )
    {
        printf("m_VeinFun.XGV_GetEnptyID ERROR(%d) \n", i_ret);
        return -1;
    }
    

    // 动态分配空间
    fvkey_p = (unsigned char*)malloc(FV_KEY_SIZE);
    if( NULL == fvkey_p )
    {
        return -1;
    }

    // 请自然放入手指
    i_ret = speak(FV_V_INPUT_FINGE, FV_V_SYN);
    if( 0 != i_ret )
    {
        if( NULL != fvkey_p )
        {
            free(fvkey_p);
            fvkey_p = NULL;
        }

        
        return -1;
    }
    
    // 采集三次指静脉，并放到算法库中
    ret_flag = 0;
    for( i = 0; i < 3; ++i )
    {
         //Sleep(FV_WAIT_TIME);
    
         memset(fvkey_p, 0x00, FV_KEY_SIZE );
         fvkey_len = 0;
         i_ret = getFingerVeinData(fvkey_p, FV_KEY_SIZE, &fvkey_len, 3, 1000, FV_V_INPUT_FINGE_A);
         if( 0 != i_ret )
         {
              printf("getFingerVeinData ERROR(%d) \n", i_ret);
              ret_flag =  -1;
              break;
         }

         
         i_ret = m_VeinFun.XGV_Enroll(m_hVeinHandle, u32Index, fvkey_p, fvkey_len, 1, NULL);
         if( XG_ERR_SUCCESS == i_ret )
         {
            // 最后一次后不再提示放手指
            if( 3  == i+1 )
            {
                continue;
            }
         
            i_ret = speak(FV_V_INPUT_FINGE, FV_V_SYN);
            if( 0 != i_ret )
            {
                ret_flag =  -1;
                break;
            }
         
             continue;
         }
         else if( XG_ERR_NO_SAME_FINGER == i_ret )
         {
             ret_flag=  1;
             break;
         }
         else
         {
             ret_flag =  -1;
             break;
         }
    }


    // 采集指静脉时出错，释放内存，返回
    if( 0 != ret_flag )
    {
        if( NULL != fvkey_p )
        {
            free(fvkey_p);
            fvkey_p = NULL;
        }

        return ret_flag;
    }

    i_ret = m_VeinFun.XGV_SaveEnrollData(m_hVeinHandle, u32Index, 0, NULL, 0);
    if( XG_ERR_SUCCESS != i_ret )
    {  
        printf("m_VeinFun.XGV_SaveEnrollData ERROR(%d) \n", i_ret);
    
        if( NULL != fvkey_p )
        {
            free(fvkey_p);
            fvkey_p = NULL;
        }

        return -1;
    }

    i_ret = m_VeinFun.XGV_GetEnrollData(m_hVeinHandle, u32Index, rTemp, len);
    if( XG_ERR_SUCCESS != i_ret )
    {
        printf("m_VeinFun.XGV_GetEnrollData ERROR(%d) \n", i_ret);
        
        if( NULL != fvkey_p )
        {
            free(fvkey_p);
            fvkey_p = NULL;
        }

        return -1;
    }

    if( NULL != fvkey_p )
    {
        free(fvkey_p);
        fvkey_p = NULL;
    }
    
	return 0;  
}


/*****************************************************************  
** @brief      1:1认证检验
** @author     liangshuangchou
** @param      [in]      <unsigned UINT8*>   注册的指静脉模板信息
** @param      [in]      <int>               长度
** @param      [in]      <unsigned UINT8*>   采集的指静脉特征信息
** @param      [in]      <int>               长度
** @exception
** @return     0:           通过 
**             -1:          错误
** @note       可以为二进制数据，也可以为base64数据
**
** @see 
** @date       2016/08/18
******************************************************************/
int verify_1_1(UINT8* regTemp, int regTemLen, UINT8* fvData, int fvDataLen )
{
     UINT8 * p_ret = NULL;

     p_ret = m_VeinFun.XGV_CharaVerify(m_hVeinHandle, regTemp, regTemLen, fvData, fvDataLen);
     if( NULL == p_ret )
     {
         return -1;
     }

     // 如果通过，还可以有自我学习能力
     // XGV_GetEnrollData

     return 0;
}




static char    str_buf[4048] = {0};
void trace( const unsigned char* databuf, unsigned int bufLen, const char* comment )
{
    if( 1 )
    {
        int            i             = 0;
        const char*    dataFormat    = "%02X "; 
    	char           temp[10]      = {0};
    	unsigned int   str_len       = 0;

    	memset(str_buf, 0x00, sizeof(str_buf));
    	
        if( NULL  == databuf || 0 == bufLen )
        {
            return;
        }

        // 数据是16进制，再加一个空格三个
    	for( i = 0; i < bufLen; i++ )
    	{
    	    memset( (void*)temp, 0x00, sizeof(temp) );
    		
    		sprintf(temp, dataFormat, databuf[i]);
    	    if( str_len + strlen(temp) >= 4048-1 )
    	    {
    	        printf( "%s(%d)::%s  too many data.\n", __FILE__, __LINE__, __FUNCTION__ );
    	        break;
    	    }
    		strcat(str_buf, temp);
            str_len += strlen(temp);
    	}

    	if( NULL == comment )
        {
           printf( "%s.\n",  str_buf );
        }
        else
        {
           printf( "%s: %s.\n", comment, str_buf);
        }

        return;
    }

	return;

}

/****************************** End of file **********************************/

