#pragma once
#include <Windows.h>
class ATM_ActivationKYJG
{
	//C:\Users\aymb0\AppData\LocalLow\��ӫ�ƹ�\����ӫ�ƹݡ�ԭ���ͼ

	//��ӫ�ƹݴ��ھ��
	HWND kyHandle;

public:
	bool isRunKYJG = false;
	bool isActivationMap = false;

	void getKYJGHandle();
	
	void setState(HWND giHandle);

	void setActivation();
	void setUnActivation(HWND giHandle);

};

