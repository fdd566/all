
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

/******************************ͨѶЭ��*****************************************/
#define XG_PREFIX_CODE                0xAABB //����ʶ

#define USB_DATA_PACKET_SIZE          4096-2
#define COM_DATA_PACKET_SIZE          512-2 //�������ݰ����Ϊ512�ֽ�
#define HID_DATA_PACKET_SIZE          (60*60)-2

/*�豸���ָ��*/
#define XG_CMD_CONNECTION             0x01 //�����豸,�����8λ�������룬Ĭ������ȫΪ0(0x30)
#define XG_CMD_CLOSE_CONNECTION       0x02 //�ر�����
#define XG_CMD_GET_SYSTEM_INFO        0x03 //��ȡ�汾�ź�������Ϣ
#define XG_CMD_FACTORY_SETTING        0x04 //�ָ���������
#define XG_CMD_SET_DEVICE_ID          0x05 //�����豸���0-255
#define XG_CMD_SET_BAUDRATE           0x06 //���ò�����0-4
#define XG_CMD_SET_SECURITYLEVEL      0x07 //���ð�ȫ�ȼ�0-4
#define XG_CMD_SET_TIMEOUT            0x08 //���õȴ���ָ���볬ʱ1-255��
#define XG_CMD_SET_DUP_CHECK          0x09 //�����ظ���¼���0-1
#define XG_CMD_SET_PASSWORD           0x0A //����ͨ������
#define XG_CMD_CHECK_PASSWORD         0x0B //��������Ƿ���ȷ
#define XG_CMD_REBOOT                 0x0C //��λ�����豸
#define XG_CMD_SET_SAME_FV            0x0D //�Ǽǵ�ʱ�����Ƿ�Ϊͬһ����ָ
#define XG_CMD_SET_USB_MODE           0x0E //����USB����ģʽ 

/*ʶ�����ָ��*/
#define XG_CMD_FINGER_STATUS          0x10 //�����ָ����״̬
#define XG_CMD_CLEAR_ENROLL           0x11 //���ָ��ID��¼����
#define XG_CMD_CLEAR_ALL_ENROLL       0x12 //�������ID��¼����
#define XG_CMD_GET_EMPTY_ID           0x13 //��ȡ�գ��޵�¼���ݣ�ID
#define XG_CMD_GET_ENROLL_INFO        0x14 //��ȡ�ܵ�¼�û�����ģ����
#define XG_CMD_GET_ID_INFO            0x15 //��ȡָ��ID��¼��Ϣ
#define XG_CMD_ENROLL                 0x16 //ָ��ID��¼
#define XG_CMD_VERIFY                 0x17 //1:1��֤��1:Nʶ��
#define XG_CMD_IDENTIFY_FREE          0x18 //FREEʶ��ģʽ���Զ�ʶ�𲢷���״̬
#define XG_CMD_CANCEL                 0x19 //ȡ��FREEʶ��ģʽ
#define XG_CMD_RAM_MODE               0x1A //�ڴ����ģʽ
#define XG_CMD_VERIFY_MULTI           0x1B //�������ID��֤

/******************************�߼�ָ��****************************************/
/*���ݶ�д���ָ��*/
#define XG_CMD_READ_DATA              0x20 //���豸��ȡ����
#define XG_CMD_WRITE_DATA             0x21 //д�����ݵ��豸
#define XG_CMD_READ_ENROLL            0x22 //��ȡָ��ID��¼����
#define XG_CMD_WRITE_ENROLL           0x23 //д�루���ǣ�ָ��ID��¼����
#define XG_CMD_GET_VEIN_IMAGE         0X24 //�ɼ�ָ����ͼ�����ݵ�����
#define XG_CMD_WRITE_IMAGE            0x25 //д��ͼ������
#define XG_CMD_GET_DEBUG              0x26 //��ȡ������Ϣ
#define XG_CMD_UPGRADE                0x27 //д�����������������Զ�����
#define XG_CMD_GET_CHARA              0x28 //�ɼ�����ȡ���������� 
#define XG_CMD_READ_USER_DATA         0x29 //���û���չ�洢����ȡ���ݣ����4K
#define XG_CMD_WRITE_USER_DATA        0x2A //д�����ݵ��û���չ�洢�������4K

/******************************��չָ��****************************************/
/*���ݶ�д���ָ��*/
#define XG_CMD_GET_DOORCTRL           0x30 //��ȡ�Ž����� 
#define XG_CMD_SET_DOORCTRL           0x31 //�����Ž�����
#define XG_CMD_OPEN_DOOR              0x32 //����
#define XG_CMD_READ_LOG               0x33 //��ȡ�Ž�������־
#define XG_CMD_SET_DEVNAME            0x34 //�����豸����
#define XG_CMD_GET_DATETIME           0x35 //��ȡ�Ž�ʵʱʱ��
#define XG_CMD_SET_DATETIME           0x36 //�����Ž�ʵʱʱ��
#define XG_CMD_KEY_CTRL               0x37 //���Ͱ���
#define XG_CMD_ENROLL_EXT             0x38 //��չ�����Ǽ�
#define XG_CMD_VERIFY_EXT             0x39 //��չ������֤
#define XG_CMD_DEL_LOG                0x3a //ɾ���Ž���־
#define XG_CMD_PLAY_VOICE             0x3b //��������
#define XG_CMD_GET_USER_NAME          0x3C //��ȡ�û�����
#define XG_CMD_SET_USER_NAME          0x3D //�޸�д���û�����
#define XG_CMD_GET_USER_CG            0x3E //��ȡ�û����ź���� 
#define XG_CMD_SET_USER_CG            0x3F //�޸�д���û����ź���� 
#define XG_CMD_DEL_USER_INFO          0x40 //��������û���Ϣ 
#define XG_CMD_GET_ADMIN_PWD          0x41 //��ȡ����Ա���룬һ������Ա������һ������
#define XG_CMD_SET_ADMIN_PWD          0x42 //�޸�д�����Ա���� 


/******************************�������****************************************/
#define	XG_ERR_SUCCESS                0x00 //�����ɹ�
#define	XG_ERR_FAIL                   0x01 //����ʧ��
#define XG_ERR_COM                    0x02 //ͨѶ����
#define XG_ERR_DATA                   0x03 //����У�����
#define XG_ERR_INVALID_PWD            0x04 //�������
#define XG_ERR_INVALID_PARAM          0x05 //��������
#define XG_ERR_INVALID_ID             0x06 //ID����
#define XG_ERR_EMPTY_ID               0x07 //ָ��IDΪ�գ��޵�¼���ݣ�
#define XG_ERR_NOT_ENOUGH             0x08 //���㹻��¼�ռ�
#define XG_ERR_NO_SAME_FINGER         0x09 //����ͬһ����ָ
#define XG_ERR_DUPLICATION_ID         0x0A //����ͬ��¼ID
#define XG_ERR_TIME_OUT               0x0B //�ȴ���ָ���볬ʱ
#define XG_ERR_VERIFY                 0x0C //��֤ʧ��
#define XG_ERR_NO_NULL_ID             0x0D //���޿�ID
#define XG_ERR_BREAK_OFF              0x0E //�����ж� 
#define XG_ERR_NO_CONNECT             0x0F //δ����
#define XG_ERR_NO_SUPPORT             0x10 //��֧�ִ˲���
#define XG_ERR_NO_VEIN                0x11 //�޾������� 
#define XG_ERR_MEMORY                 0x12 //�ڴ治��
#define XG_ERR_NO_DEV                 0x13 //�豸������
#define XG_ERR_ADDRESS                0x14 //�豸��ַ����
#define XG_ERR_NO_FILE                0x15 //�ļ�������
#define XG_ERR_LICENSE                0x80

/******************************״̬����****************************************/
#define XG_INPUT_FINGER               0x20 //���������ָ
#define XG_RELEASE_FINGER             0x21 //�����ÿ���ָ

#if	defined(__cplusplus)
}
#endif	/* defined(__cplusplus) */

#endif