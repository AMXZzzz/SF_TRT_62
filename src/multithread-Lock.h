#pragma once
#include "lock.h"
#include "algorithm.h"


typedef VOID(*CustomizeMove)(int, int);
typedef VOID(*CustomizeMoveFree)();

class MultithreadLock : public  Lock{
public:
	//! ��ʼ���ƶ���ʽ
	bool initLock() override;
	//! �����ƶ�
	void triggerMove() override;
	//! �ͷ�
	void Release() override;
	//! ��ȡ����ָ��
	Process* getProcessPtr();
	//! ��ȡ����
	IParameter* getParamePtr();
	//! ��ȡ������Ϣ
	IPoint* getIPointPtr();
	//! ��ȡ�ƶ�ָ��
	CustomizeMove getMoveRPtr();
	//! ��ȡ�ƶ��ͷ�ָ��
	CustomizeMoveFree getMoveClosePtr();
	//! ��ȡ���
	HANDLE getHandle();

	MultithreadLock(LOCKINFO* info) : Lock(info) {};

private:
	//! �����¼�
	bool CreateLockEvent();
	//! �ƶ�
	CustomizeMove MoveR;
	CustomizeMoveFree MoveClose;
	HANDLE lock_event = NULL;
	HANDLE lock_thread = NULL;
};

