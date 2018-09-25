//
//刷固件流程
//
1.请求刷固
	ID   Len  Data
			  CMD DeviceID	 FirmWare Length  
    7E   08   01  **         ** ** ** **      00 00   
  响应
	ID   Len  Data  
			  CMD  DeviceID  SerialNum
    7D   08   81   **        ** ** ** ** 00 00
	
2.确认请求
	ID   Len  Data
			  CMD DeviceID	 SerialNum  
    7E   08   02  **         ** ** ** **      00 00   
  响应
	ID   Len  Data  
			  CMD  DeviceID  SerialNum
    7D   08   82   **        ** ** ** ** 00 00
	
3.开始刷固
	ID   Len  Data
    7E   08   ** ** ** ** ** ** ** **  
  （连续发送1K，不足1K部分按实际长度发送，设备收到1K后响应）
  
  响应
	ID   Len  Data
			  CMD DeviceID   Revice Length
    7D   08   83  **         ** ** ** **   00 00
  （判断长度如果与发送一致继续发送，不一致重新发送）
  
4.刷固完成
  上位机：设备响应长度与总长一致，刷固完成并成功。
  设备端：实际收到长度与请求信息长度一致，刷固完成，自动跳转到APP
	
约定：
	1.固件数据按Bin文件默认顺序收发
	2.固件长度为四字节，高位在前
	3.超时时间统一为500ms
	4.默认重发次数为3次

  错误响应
	ID   Len  Data
			  CMD DeviceID   	Revice Length
    7D   08   83  ** ** ** ** 	** ** **
	
	
								01 00 00	//校验和错误
								02 ** **	//接收少于1024字节,后两个字节为字节数
								03 00 00	//FLASH内无程序1
								
  成功响应
	ID   Len  Data
			  CMD DeviceID   	Revice Length
    7D   08   84  ** ** ** ** 	** ** **
	
	
								01 00 00	//成功
								02 ** **	//失败

	
	

//
//读写配置流程
//	
	
1.读设备信息
	ID   Len  Data
			  CMD 
    7D   08   01 00 00 00 00 00 00 00     
  响应
	ID   Len  Data  
			  CMD  SerialNum   CANID  DeviceType   RunMode
    7B   08   81   ** ** ** ** **     **           **                    
	
约定：
	1.DeviceType
		设备类型：00:未定义
			      01:主板
				  02:磁导航-8
				  03:磁导航-16
				  04:地标
				  05:超声波
				  06:遥控器
	2.RunMode
		运行模式：01:BootLoader 
				  02:APP	
	
	
	
	