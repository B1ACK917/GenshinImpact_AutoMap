#pragma once
#include <iostream>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
using namespace std;
using namespace cv;

enum RectMode
{
	FW_UNDIFINDE = -1,
	FW_MINIMIZE = 0,
	F_1920x1080 = 1,
	W_1920x1080 = 2,
	F_1680x1050 = 3,
	W_1680x1050 = 4,
	F_1600x900 = 5,
	W_1600x900 = 6,
	F_1440x900 = 7,
	W_1440x900 = 8,
	F_1400x1050 = 9,
	W_1400x1050 = 10,
	F_1366x768 = 11,
	W_1366x768 = 12,

};

class ATM_GiState
{
public:
	//ԭ����Ϸ���ھ��
	HWND giHandle;
	//ԭ�񴰿�����
	RECT giRect;
	//ԭ�񴰿����򱣴�Ա���
	RECT giRectTmp;

	//ԭ����Ϸ���ڽ�ͼ
	Mat giFrame;
	//ԭ����Ϸ���ڽ�ͼ��Ч����
	//Mat giFrameROI;
	//
	Mat giFrameRect;
	//
	Mat giFramePaimon;
	//
	Mat giFrameMap;

	Mat giFrameUID;

	string giWndClass = "UnityWndClass";
	string giName = "ԭ��";

	//ԭ���Ƿ�������
	bool isRunning = false;
	//�����Ƿ�ɼ����Ƿ���������
	bool isPaimonVisible = false;
	//ԭ���Ƿ�ȫ��
	bool isFullScreen = false;
	//���ڷֱ���ģʽ
	int giRectMode = RectMode::FW_MINIMIZE;

	int resIdPaimon = 0;
	//���ڴ�С
	Size giSize;

	bool isRun();
	void getHandle();
	void getRect();
	int getGiRectMode();
	int getGiState();
	Point getOffset();

	void getAllScreen();

	void getGiScreen();
	void getGiFrame();
	void getGiFramePaimon();
	void getGiFrameMap();
	void getGiFrameUID();

	void setGiNameClass(LANGID SystemLanguageID);

	void setGiHandle(HWND GiHandle);
};

