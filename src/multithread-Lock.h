#pragma once
#include "lock.h"
#include "algorithm.h"


typedef VOID(*CustomizeMove)(int, int);
typedef VOID(*CustomizeMoveFree)();

class MultithreadLock : public  Lock{
public:
	//! 初始化移动方式
	bool initLock() override;
	//! 触发移动
	void triggerMove() override;
	//! 释放
	void Release() override;
	//! 获取处理指针
	Process* getProcessPtr();
	//! 获取参数
	IParameter* getParamePtr();
	//! 获取坐标信息
	IPoint* getIPointPtr();
	//! 获取移动指针
	CustomizeMove getMoveRPtr();
	//! 获取移动释放指针
	CustomizeMoveFree getMoveClosePtr();
	//! 获取句柄
	HANDLE getHandle();

	MultithreadLock(LOCKINFO* info) : Lock(info) {};

private:
	//! 创建事件
	bool CreateLockEvent();
	//! 移动
	CustomizeMove MoveR;
	CustomizeMoveFree MoveClose;
	HANDLE lock_event = NULL;
	HANDLE lock_thread = NULL;
};

