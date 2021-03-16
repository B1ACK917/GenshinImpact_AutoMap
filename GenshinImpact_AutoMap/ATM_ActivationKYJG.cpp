#include "ATM_ActivationKYJG.h"

void ATM_ActivationKYJG::setActivation()
{
	if (kyHandle != NULL)
	{
		if (IsIconic(kyHandle))
		{
			ShowWindow(kyHandle, SW_RESTORE);
		}
		SetForegroundWindow(kyHandle);
	}
}

void ATM_ActivationKYJG::setUnActivation(HWND giHandle)
{
	if (giHandle != NULL)
	{
		SetForegroundWindow(giHandle);
	}
}

void ATM_ActivationKYJG::getKYJGHandle()
{
	kyHandle = FindWindowA(NULL, "����ӫ�ƹݡ�ԭ���ͼ");
	if (kyHandle != NULL)
	{
		isRunKYJG = true;
	}
	else
	{
		isRunKYJG = false;
	}
}

void ATM_ActivationKYJG::setState(HWND giHandle)
{
	isActivationMap = !isActivationMap;
	if (isActivationMap)
	{
		setActivation();
	}
	else
	{
		setUnActivation(giHandle);
	}
}
