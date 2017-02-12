
#ifndef  _XG_PROTOCOL_
#define  _XG_PROTOCOL_

#if	defined(__cplusplus)
extern	"C"	{
#endif	/* defined(__cplusplus) */

#if !defined NULL
	#define NULL            0
#endif

typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef char 				*LPCSTR;
typedef void 				*HANDLE;
typedef HANDLE 				*PHANDLE;

/******************************通讯协议*****************************************/
#define XG_PREFIX_CODE                0xAABB //包标识

#define USB_DATA_PACKET_SIZE          4096-2
#define COM_DATA_PACKET_SIZE          512-2 //串口数据包最大为512字节
#define HID_DATA_PACKET_SIZE          (60*60)-2

/*设备相关指令*/
#define XG_CMD_CONNECTION             0x01 //连接设备,必须带8位以上密码，默认密码全为0(0x30)
#define XG_CMD_CLOSE_CONNECTION       0x02 //关闭连接
#define XG_CMD_GET_SYSTEM_INFO        0x03 //获取版本号和设置信息
#define XG_CMD_FACTORY_SETTING        0x04 //恢复出厂设置
#define XG_CMD_SET_DEVICE_ID          0x05 //设置设备编号0-255
#define XG_CMD_SET_BAUDRATE           0x06 //设置波特率0-4
#define XG_CMD_SET_SECURITYLEVEL      0x07 //设置安全等级0-4
#define XG_CMD_SET_TIMEOUT            0x08 //设置等待手指放入超时1-255秒
#define XG_CMD_SET_DUP_CHECK          0x09 //设置重复登录检查0-1
#define XG_CMD_SET_PASSWORD           0x0A //设置通信密码
#define XG_CMD_CHECK_PASSWORD         0x0B //检查密码是否正确
#define XG_CMD_REBOOT                 0x0C //复位重启设备
#define XG_CMD_SET_SAME_FV            0x0D //登记的时候检查是否为同一根手指
#define XG_CMD_SET_USB_MODE           0x0E //设置USB驱动模式 

/*识别相关指令*/
#define XG_CMD_FINGER_STATUS          0x10 //检测手指放置状态
#define XG_CMD_CLEAR_ENROLL           0x11 //清除指定ID登录数据
#define XG_CMD_CLEAR_ALL_ENROLL       0x12 //清除所有ID登录数据
#define XG_CMD_GET_EMPTY_ID           0x13 //获取空（无登录数据）ID
#define XG_CMD_GET_ENROLL_INFO        0x14 //获取总登录用户数和模板数
#define XG_CMD_GET_ID_INFO            0x15 //获取指定ID登录信息
#define XG_CMD_ENROLL                 0x16 //指定ID登录
#define XG_CMD_VERIFY                 0x17 //1:1认证或1:N识别
#define XG_CMD_IDENTIFY_FREE          0x18 //FREE识别模式，自动识别并发送状态
#define XG_CMD_CANCEL                 0x19 //取消FREE识别模式
#define XG_CMD_RAM_MODE               0x1A //内存操作模式
#define XG_CMD_VERIFY_MULTI           0x1B //连续多个ID验证

/******************************高级指令****************************************/
/*数据读写相关指令*/
#define XG_CMD_READ_DATA              0x20 //从设备读取数据
#define XG_CMD_WRITE_DATA             0x21 //写入数据到设备
#define XG_CMD_READ_ENROLL            0x22 //读取指定ID登录数据
#define XG_CMD_WRITE_ENROLL           0x23 //写入（覆盖）指定ID登录数据
#define XG_CMD_GET_VEIN_IMAGE         0X24 //采集指静脉图像数据到主机
#define XG_CMD_WRITE_IMAGE            0x25 //写入图像数据
#define XG_CMD_GET_DEBUG              0x26 //读取调试信息
#define XG_CMD_UPGRADE                0x27 //写入升级程序，重启后自动升级
#define XG_CMD_GET_CHARA              0x28 //采集并读取特征到主机 
#define XG_CMD_READ_USER_DATA         0x29 //从用户扩展存储区读取数据，最大4K
#define XG_CMD_WRITE_USER_DATA        0x2A //写入数据到用户扩展存储区，最大4K

/******************************扩展指令****************************************/
/*数据读写相关指令*/
#define XG_CMD_GET_DOORCTRL           0x30 //获取门禁设置 
#define XG_CMD_SET_DOORCTRL           0x31 //设置门禁设置
#define XG_CMD_OPEN_DOOR              0x32 //开门
#define XG_CMD_READ_LOG               0x33 //读取门禁出入日志
#define XG_CMD_SET_DEVNAME            0x34 //设置设备名称
#define XG_CMD_GET_DATETIME           0x35 //获取门禁实时时钟
#define XG_CMD_SET_DATETIME           0x36 //设置门禁实时时钟
#define XG_CMD_KEY_CTRL               0x37 //发送按键
#define XG_CMD_ENROLL_EXT             0x38 //扩展语音登记
#define XG_CMD_VERIFY_EXT             0x39 //扩展语音验证
#define XG_CMD_DEL_LOG                0x3a //删除门禁日志
#define XG_CMD_PLAY_VOICE             0x3b //播放语音
#define XG_CMD_GET_USER_NAME          0x3C //读取用户名称
#define XG_CMD_SET_USER_NAME          0x3D //修改写入用户名称
#define XG_CMD_GET_USER_CG            0x3E //读取用户卡号和组号 
#define XG_CMD_SET_USER_CG            0x3F //修改写入用户卡号和组号 
#define XG_CMD_DEL_USER_INFO          0x40 //情况所有用户信息 
#define XG_CMD_GET_ADMIN_PWD          0x41 //读取管理员密码，一个管理员可设置一个密码
#define XG_CMD_SET_ADMIN_PWD          0x42 //修改写入管理员密码 


/******************************错误代码****************************************/
#define	XG_ERR_SUCCESS                0x00 //操作成功
#define	XG_ERR_FAIL                   0x01 //操作失败
#define XG_ERR_COM                    0x02 //通讯错误
#define XG_ERR_DATA                   0x03 //数据校验错误
#define XG_ERR_INVALID_PWD            0x04 //密码错误
#define XG_ERR_INVALID_PARAM          0x05 //参数错误
#define XG_ERR_INVALID_ID             0x06 //ID错误
#define XG_ERR_EMPTY_ID               0x07 //指定ID为空（无登录数据）
#define XG_ERR_NOT_ENOUGH             0x08 //无足够登录空间
#define XG_ERR_NO_SAME_FINGER         0x09 //不是同一根手指
#define XG_ERR_DUPLICATION_ID         0x0A //有相同登录ID
#define XG_ERR_TIME_OUT               0x0B //等待手指输入超时
#define XG_ERR_VERIFY                 0x0C //认证失败
#define XG_ERR_NO_NULL_ID             0x0D //已无空ID
#define XG_ERR_BREAK_OFF              0x0E //操作中断 
#define XG_ERR_NO_CONNECT             0x0F //未连接
#define XG_ERR_NO_SUPPORT             0x10 //不支持此操作
#define XG_ERR_NO_VEIN                0x11 //无静脉数据 
#define XG_ERR_MEMORY                 0x12 //内存不足
#define XG_ERR_NO_DEV                 0x13 //设备不存在
#define XG_ERR_ADDRESS                0x14 //设备地址错误
#define XG_ERR_NO_FILE                0x15 //文件不存在
#define XG_ERR_LICENSE                0x80

/******************************状态代码****************************************/
#define XG_INPUT_FINGER               0x20 //请求放入手指
#define XG_RELEASE_FINGER             0x21 //请求拿开手指

#if	defined(__cplusplus)
}
#endif	/* defined(__cplusplus) */

#endif