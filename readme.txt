//
//ˢ�̼�����
//
1.����ˢ��
	ID   Len  Data
			  CMD DeviceID	 FirmWare Length  
    7E   08   01  **         ** ** ** **      00 00   
  ��Ӧ
	ID   Len  Data  
			  CMD  DeviceID  SerialNum
    7D   08   81   **        ** ** ** ** 00 00
	
2.ȷ������
	ID   Len  Data
			  CMD DeviceID	 SerialNum  
    7E   08   02  **         ** ** ** **      00 00   
  ��Ӧ
	ID   Len  Data  
			  CMD  DeviceID  SerialNum
    7D   08   82   **        ** ** ** ** 00 00
	
3.��ʼˢ��
	ID   Len  Data
    7E   08   ** ** ** ** ** ** ** **  
  ����������1K������1K���ְ�ʵ�ʳ��ȷ��ͣ��豸�յ�1K����Ӧ��
  
  ��Ӧ
	ID   Len  Data
			  CMD DeviceID   Revice Length
    7D   08   83  **         ** ** ** **   00 00
  ���жϳ�������뷢��һ�¼������ͣ���һ�����·��ͣ�
  
4.ˢ�����
  ��λ�����豸��Ӧ�������ܳ�һ�£�ˢ����ɲ��ɹ���
  �豸�ˣ�ʵ���յ�������������Ϣ����һ�£�ˢ����ɣ��Զ���ת��APP
	
Լ����
	1.�̼����ݰ�Bin�ļ�Ĭ��˳���շ�
	2.�̼�����Ϊ���ֽڣ���λ��ǰ
	3.��ʱʱ��ͳһΪ500ms
	4.Ĭ���ط�����Ϊ3��

  ������Ӧ
	ID   Len  Data
			  CMD DeviceID   	Revice Length
    7D   08   83  ** ** ** ** 	** ** **
	
	
								01 00 00	//У��ʹ���
								02 ** **	//��������1024�ֽ�,�������ֽ�Ϊ�ֽ���
								03 00 00	//FLASH���޳���1
								
  �ɹ���Ӧ
	ID   Len  Data
			  CMD DeviceID   	Revice Length
    7D   08   84  ** ** ** ** 	** ** **
	
	
								01 00 00	//�ɹ�
								02 ** **	//ʧ��

	
	

//
//��д��������
//	
	
1.���豸��Ϣ
	ID   Len  Data
			  CMD 
    7D   08   01 00 00 00 00 00 00 00     
  ��Ӧ
	ID   Len  Data  
			  CMD  SerialNum   CANID  DeviceType   RunMode
    7B   08   81   ** ** ** ** **     **           **                    
	
Լ����
	1.DeviceType
		�豸���ͣ�00:δ����
			      01:����
				  02:�ŵ���-8
				  03:�ŵ���-16
				  04:�ر�
				  05:������
				  06:ң����
	2.RunMode
		����ģʽ��01:BootLoader 
				  02:APP	
	
	
	
	