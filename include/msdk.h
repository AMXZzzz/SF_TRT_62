//------------------------------------------------------------------------------//
//    文盒单双头应用编程接口(API)说明                                           //
//                                                                              //
//    本API适用于                                                               //
//    1. 文盒单双头                                                             //
//------------------------------------------------------------------------------//
#pragma once
#ifndef  _MSDK_H
#define  _MSDK_H

#include <windows.h>

#ifdef __cplusplus
extern "C"{
#endif

/***********设备操作函数;以下函数中的m_hdl是指M_Open返回的句柄*******/
//打开端口获取句柄;
//  Nbr是端口号，无论是双头模块还是单头模块，都是从1开始，依次为2/3/4...，最大126。电脑上插入一个就始终是1；插入n个 端口分别是1、2....n
//  使用M_Open接口打开默认vid pid的盒子
HANDLE WINAPI M_Open(int Nbr);
//使用M_Open_VidPid接口可以打开指定vid pid的单头盒子或者双头盒子的主控端。必须保证电脑上只插有一个这种盒子。
HANDLE WINAPI M_Open_VidPid(int Vid, int Pid);
//扫描硬件列表，打开扫描到的第一个单头或者双头盒子
HANDLE WINAPI M_ScanAndOpen(void);
//关闭端口；在程序退出前再关闭端口; 返回 0: 成功；!0: 失败
int WINAPI M_Close(HANDLE m_hdl);

//获取设备序列号
//     参数:
//         dwp_LenResponse: 设备序列号的长度(单位: 字节)
//         ucp_Response: 设备序列号buf(buf由调用该API的应用程序分配)
//     返回 0: 成功；-1: 失败
int WINAPI M_GetDevSn(HANDLE m_hdl, DWORD *dwp_LenResponse, unsigned char *ucp_Response);
//写用户数据
//     参数:
//         dwp_LenResponse: 数据长度(单位: 字节)，不能超过256字节
//         ucp_UserData   : 数据
//     返回 0: 成功；非0: 失败
int WINAPI M_SetUserData(HANDLE m_hdl, DWORD dw_LenUserData, unsigned char *ucp_UserData);
//验证用户数据 //参数定义同上
int WINAPI M_VerifyUserData(HANDLE m_hdl, DWORD dw_LenUserData, unsigned char *ucp_UserData);
//检查盒子是否是可修改盒子; 返回: 1:是可修改； 其他:不可修改
int WINAPI M_ChkSupportMdy(HANDLE m_hdl);
//读取当前绝对移动功能的状态 返回: 1:打开; 0:关闭; 其他: 错误
int WINAPI M_GetAbsCfg(HANDLE m_hdl);
//设置绝对移动功能: e1d0=1 打开; =0;关闭
int WINAPI M_SetAbsCfg(HANDLE m_hdl, int e1d0);
//设置新VID/PID
//  只支持可修改的单头、双头。普通单头、双头不支持
//     参数:
//         mVid: 主控端Vid;
//               不能是C216或C217或FFFF;
//               如果是0，表示mVid、mPid不需要更改; mPid的值将被忽略。
//         mPid: 主控端Pid;
//               如果mVid=0，该参数将被忽略;
//               不能是0000或FFFF
//         sVid: 被控端Vid;
//               如果是单头，该参数将被忽略.
//               不能是C216或C217或FFFF;
//               如果是0，表示sVid、sPid不需要更改; sPid的值将被忽略。
//         sVid: 被控端Pid;
//               如果是单头，该参数将被忽略.
//               如果sVid=0，该参数将被忽略;
//               不能是0000或FFFF
//     返回  0: 成功；
//           -2：该盒子不支持修改；
//           -10: mVID不符合规则  -11: mPID不符合规则
//           -20: sVID不符合规则  -21: sPID不符合规则
//           其他: 修改失败
int WINAPI M_SetNewVidPid(HANDLE m_hdl, int mVid, int mPid, int sVid, int sPid);
int WINAPI M_ResetVidPid(HANDLE m_hdl);
//读取VID/PID; IdIndex=1/2/3/4 分别对应 主控端Vid, 主控端Pid, 被控端Vid, 被控端Pid
int WINAPI M_GetVidPid(HANDLE m_hdl, int IdIndex);
//是否支持修改产品名称, 返回值1=支持，0=不支持；-1，失败
int WINAPI M_ChkSupportProdStrMdy(HANDLE m_hdl);
//读取产品名称
//m0s1: 0=主控端; 1=被控端
//LenOfStr: 产品名称的长度
//ucp_Str: 产品名称，非Unicode字符串
int WINAPI M_GetProdStr(HANDLE m_hdl, int m0s1, int *LenOfStr, unsigned char *ucp_Str);
//设置产品名称
//m0s1: 0=主控端; 1=被控端
//LenOfStr: 不超过16字节; 如果是unicode编码，长度不超过32; a
//ucp_Str: 不超过16个ascii的char数组, 支持unicode编码
int WINAPI M_SetProdStr(HANDLE m_hdl, int m0s1, int LenOfStr, unsigned char *ucp_Str);
//是否支持加密功能, 返回值1=支持，0=不支持；-1，失败
int WINAPI M_ChkSupportEnc(HANDLE m_hdl);
//检测密码状态; 返回值: <0 错误；255=无密码; 0~16:密码剩余有效次数
int WINAPI M_ChkPwStat(HANDLE m_hdl);
//设置新密码
//PwLen: 密码长度，要和ucp_pw长度对应
//PwMaxCnt: 密码验证错误允许尝试次数 取值范围1~16
//ucp_pw: 密码字符串, 长度必须是PwLen
int WINAPI M_SetPw(HANDLE m_hdl, int PwLen, int PwMaxCnt, unsigned char *ucp_pw);
//验证密码
//PwLen: 密码长度，要和ucp_pw长度对应
//ucp_pw: 密码字符串, 长度必须是PwLen
//返回: >=0密码剩余有效次数; <0 其他错误
int WINAPI M_VerifyPw(HANDLE m_hdl, int PwLen, unsigned char *ucp_pw);
//写入数据
//DataLen:数据长度(单位:字节)(最大1000)
//ucp_data: 待写入数据;
//返回值: <0: 错误; 0: 成功; 其他
int WINAPI M_WrEncData(HANDLE m_hdl, int DataLen, unsigned char *ucp_data);
//读取数据
//DataLen:期望读取的最大数据长度(单位:字节)(最大1000)
//ucp_data: 返回的数据;
//返回值: <0: 读取错误; 1000> >0: 实际读取的数据长度
int WINAPI M_RdEncData(HANDLE m_hdl, int DataLen, unsigned char *ucp_data);
//DLL内部参数恢复默认值
int WINAPI M_InitParam(HANDLE m_hdl);
//设置DLL内部参数
//ParamType = 0: 单击按键，按下和弹起之间的延时值，Param1=最小值，Param2=最大值，实际延时时间在最小最大值之间随机//默认值是50,80
//ParamType = 2: 多个按键，每个按键之间的延时值，Param1=最小值，Param2=最大值，实际延时时间在最小最大值之间随机//默认值是150,600
//ParamType = 8: 单击鼠标(左中右键)，按下和弹起之间的延时值，Param1=最小值，Param2=最大值，实际延时时间在最小最大值之间随机//默认值是50,80
//ParamType = 10: 多次单击鼠标(左中右键)，每次单击之间的延时值，Param1=最小值，Param2=最大值，实际延时时间在最小最大值之间随机//默认值是500,900
//ParamType = 12: 双击鼠标(左键)，两次单击之间的延时值，Param1=最小值，Param2=最大值，实际延时时间在最小最大值之间随机//默认值是60,110
//ParamType = 14: 多次双击鼠标(左键)，每次双击之间的延时值，Param1=最小值，Param2=最大值，实际延时时间在最小最大值之间随机//默认值是500,800
//ParamType = 20: 鼠标移动轨迹由多条直线组成，每条直线移动之间的延时值，Param1=最小值，Param2=最大值，实际延时时间在最小最大值之间随机//默认值是10,20
int WINAPI M_SetParam(HANDLE m_hdl, int ParamType, int Param1, int Param2);
/***********键盘操作函数;以下函数中的m_hdl是指M_Open返回的句柄*******/
/***********以下所有命令返回 0: 成功；-1: 失败*******/
///////////以下接口采用的HidKeyCode是USB键盘的KeyCode，不是windows的KeyCode，新开发程序不建议再使用
//单击(按下后立刻弹起)按键  //HidKeyCode: 键盘码; Nbr: 按下次数
int WINAPI M_KeyPress(HANDLE m_hdl, int HidKeyCode, int Nbr);
//按下某个按键不弹起        //HidKeyCode: 键盘码
int WINAPI M_KeyDown(HANDLE m_hdl, int HidKeyCode);
//弹起某个按键              //HidKeyCode: 键盘码
int WINAPI M_KeyUp(HANDLE m_hdl, int HidKeyCode);
//读取按键状态              //HidKeyCode: 键盘码 //返回 0: 弹起状态；1:按下状态；-1: 失败(端口未打开)
//使用该接口，不允许手工操作键盘，否则该接口返回值有可能不正确
int WINAPI M_KeyState(HANDLE m_hdl, int HidKeyCode);
///////////以上接口采用的HidKeyCode是USB键盘的KeyCode，不是windows的KeyCode，新开发程序不建议再使用
///////////以下接口采用的KeyCode是windows的KeyCode，新开发程序建议再使用
//单击(按下后立刻弹起)按键  //KeyCode: 键盘码; Nbr: 按下次数
int WINAPI M_KeyPress2(HANDLE m_hdl, int KeyCode, int Nbr);
//按下某个按键不弹起        //KeyCode: 键盘码
int WINAPI M_KeyDown2(HANDLE m_hdl, int KeyCode);
//弹起某个按键              //KeyCode: 键盘码
int WINAPI M_KeyUp2(HANDLE m_hdl, int KeyCode);
//读取按键状态              //KeyCode: 键盘码 //返回 0: 弹起状态；1:按下状态；-1: 失败(端口未打开)
//使用该接口，不允许手工操作键盘，否则该接口返回值有可能不正确
int WINAPI M_KeyState2(HANDLE m_hdl, int KeyCode);
///////////以上接口采用的KeyCode是windows的KeyCode，新开发程序建议再使用
//弹起所有按键
int WINAPI M_ReleaseAllKey(HANDLE m_hdl);
//读取小键盘NumLock灯的状态 //返回 0:灭；1:亮；-1: 失败
int WINAPI M_NumLockLedState(HANDLE m_hdl);
//读取CapsLock灯的状态 //返回 0:灭；1:亮；-1: 失败
int WINAPI M_CapsLockLedState(HANDLE m_hdl);
//读取ScrollLock灯的状态 //返回 0:灭；1:亮；-1: 失败
int WINAPI M_ScrollLockLedState(HANDLE m_hdl);
//输入一串ASCII字符串，如"ABCdef012,.<>"，在InputLen个字节内将忽略非ASCII字符，  //InputStr: 输入缓冲区首地址; 注意：不支持解析\n\r等转义字符！ InputLen:输出的长度
int WINAPI M_KeyInputString(HANDLE m_hdl, char *InputStr, int InputLen);
//输入一串字符串，支持中文(GBK编码)英文混合，如"啊啊啊ABCdef012,.<>"，在InputLen个字节内将忽略非ASCII和中文字符，  //InputStr: 输入缓冲区首地址; 注意：不支持解析\n\r等转义字符！ InputLen:输出的长度
int WINAPI M_KeyInputStringGBK(HANDLE m_hdl, char *InputStr, int InputLen);
//输入一串字符串，支持中文(Unicode编码)英文混合，如"啊啊啊ABCdef012,.<>"，在InputLen个字节内将忽略非ASCII和中文字符，  //InputStr: 输入缓冲区首地址; 注意：不支持解析\n\r等转义字符！ InputLen:输出的长度
int WINAPI M_KeyInputStringUnicode(HANDLE m_hdl, char *InputStr, int InputLen);
/***********鼠标操作函数;以下函数中的m_hdl是指M_Open返回的句柄*******/
/***********以下所有命令返回 0: 成功；-1: 失败*******/
//左键单击   //Nbr: 左键在当前位置单击次数
int WINAPI M_LeftClick(HANDLE m_hdl,int Nbr);
//左键双击   //Nbr: 左键在当前位置双击次数
int WINAPI M_LeftDoubleClick(HANDLE m_hdl,int Nbr);
//按下左键不弹起
int WINAPI M_LeftDown(HANDLE m_hdl);
//弹起左键
int WINAPI M_LeftUp(HANDLE m_hdl);
//右键单击   //Nbr: 左键在当前位置单击次数
int WINAPI M_RightClick(HANDLE m_hdl,int Nbr);
//按下右键不弹起
int WINAPI M_RightDown(HANDLE m_hdl);
//弹起右键
int WINAPI M_RightUp(HANDLE m_hdl);
//中键单击   //Nbr: 左键在当前位置单击次数
int WINAPI M_MiddleClick(HANDLE m_hdl,int Nbr);
//按下中键不弹起
int WINAPI M_MiddleDown(HANDLE m_hdl);
//弹起中键
int WINAPI M_MiddleUp(HANDLE m_hdl);
//弹起鼠标的所有按键
int WINAPI M_ReleaseAllMouse(HANDLE m_hdl);
//读取鼠标左中右键状态   //MouseKeyCode: 1=左键 2=右键 3=中键  //返回 0: 弹起状态；1:按下状态；-1: 失败
//只能读取盒子中鼠标的状态，读取不到实体鼠标的状态
int WINAPI M_MouseKeyState(HANDLE m_hdl, int MouseKeyCode);
//滚动鼠标滚轮;  Nbr: 滚动量,  为正,向上滚动；为负, 向下滚动;
int WINAPI M_MouseWheel(HANDLE m_hdl,int Nbr);
//将鼠标移动到原点(0,0)  在出现移动出现异常时，可以用该函数将鼠标复位
int WINAPI M_ResetMousePos(HANDLE m_hdl);
//从当前位置移动鼠标    x: x方向（横轴）的距离（正:向右; 负值:向左）; y: y方向（纵轴）的距离（正:向下; 负值:向上）
int WINAPI M_MoveR(HANDLE m_hdl,int x, int y);
//移动鼠标到指定坐标    x: x方向（横轴）的坐标; y: y方向（纵轴）的坐标。坐标原点(0, 0)在屏幕左上角
//注意：如果出现过将鼠标移动的距离超过屏幕大小，再次MoveTo可能会出现无法正确移动到指定坐标的问题，如果出现该问题，需调用ResetMousePos复位
int WINAPI M_MoveTo(HANDLE m_hdl,int x, int y);
//读取当前鼠标所在坐标  返回坐标在x、y中。
//注意：该函数必须在执行一次MoveTo或ResetMousePos函数后才能正确执行！
//注意：如果曾经出现过将鼠标移动的距离超过屏幕大小，这里读取到的坐标值有可能是不正确的！如果出现该问题，需调用ResetMousePos复位
int WINAPI M_GetCurrMousePos(HANDLE m_hdl,int *x, int *y);
//返回当前鼠标坐标X值
int WINAPI M_GetCurrMousePosX(HANDLE m_hdl); //由于某些语言不支持指针，增加此接口
//返回当前鼠标坐标Y值
int WINAPI M_GetCurrMousePosY(HANDLE m_hdl); //由于某些语言不支持指针，增加此接口

////////以下接口仅适用主控机和被控机是同一台电脑的使用方式(单头模块；双头模块的两个USB头都连接到同一台电脑)
////////以下接口将调用系统的API来获取当前鼠标位置，DLL将不记录鼠标移动的位置
//移动鼠标到指定坐标    x: x方向（横轴）的坐标; y: y方向（纵轴）的坐标。
int WINAPI M_MoveR2(HANDLE m_hdl,int x, int y);
//移动鼠标到指定坐标    x: x方向（横轴）的坐标; y: y方向（纵轴）的坐标。坐标原点(0, 0)在屏幕左上角
int WINAPI M_MoveTo2(HANDLE m_hdl,int x, int y);
//读取当前鼠标所在坐标  返回坐标在x、y中。
int WINAPI M_GetCurrMousePos2(int *x, int *y);

////////以下接口将使用绝对移动功能。该功能目前还不能支持安卓
//输入被控机分辨率
//  在使用绝对移动功能前，必须先输入被控机的屏幕分辨率，打开端口获取句柄后，只需要调用一次该接口就可以
//  x: x方向（横轴）的坐标; y: y方向（纵轴）的坐标。坐标原点(0, 0)在屏幕左上角
//  返回值如果是-10，表示该盒子不支持绝对移动功能。返回0表示执行正确。可以用该接口判断盒子是否支持绝对移动功能
int WINAPI M_ResolutionUsed(HANDLE m_hdl, int x, int y);
//将鼠标移动到指定坐标。
//  绝对移动功能，鼠标移动到指定位置时，在某些坐标上最大会有±2的误差
//  使用该接口后，可以调用M_GetCurrMousePos读取鼠标位置
int WINAPI M_MoveTo3(HANDLE m_hdl, int x, int y);
//一步到位将鼠标移动到指定坐标。
//  使用绝对移动功能，鼠标一步到位移动（没有移动轨迹）到指定位置，在某些坐标上最大会有±2的误差
//  使用该接口后，可以调用M_GetCurrMousePos读取鼠标位置
int WINAPI M_MoveTo3_D(HANDLE m_hdl, int x, int y);
/*******************通用操作函数****************************/
//延时指定时间  time:单位ms
int WINAPI M_Delay(int time);
//在指定的最小最大值之间延时随机时间  Min_time:最小延时时间; Max_time: 最大延时时间 （单位：ms）
int WINAPI M_DelayRandom(int Min_time, int Max_time);
//在最小最大值之间取随机数
int WINAPI M_RandDomNbr(int Min_V, int Max_V);

#ifdef __cplusplus
}
#endif
#endif
