#include "AutomaticTrackingMap.h"

AutomaticTrackingMap::AutomaticTrackingMap()
{
	getSystemInfo();
	SLF.setIndex(SystemUserFileIndex);
	//SLF.load();
	loadLocal();
}

AutomaticTrackingMap::~AutomaticTrackingMap()
{
	Exit();
	SLF.save();
}

void AutomaticTrackingMap::Init(HWND mapWindowsHandle)
{
	//����״̬��ʼ��
	MET.zerosMinMap = zerosMinMap;
	MET.offGiMinMap = offGiMinMap;
	SST.setPort(23333);//6666

	getGiHandle();
	getThisHandle(mapWindowsHandle);
	//SetWindowLong(thisHandle, GWL_EXSTYLE, (GetWindowLong(thisHandle, GWL_EXSTYLE) | WS_EX_TOOLWINDOW) );
	//������ݸ���
	BackEndUpdata();
}

void AutomaticTrackingMap::Exit()
{
}

//ǰ����ʾ
void AutomaticTrackingMap::FrontEndUpdata()
{
	//���ò���
	setThisState();

	setThreadMatchMat();

	//��ʾ����

	//��ȡ��ʾ�����ͼ
	MainMat = getViewMap();
	//�����Ʒͼ��
	drawObjectLists();
	drawFlagLists();
	//��ӵ�ǰλ��ͼ��
	drawAvatar();
	//��ʾ�������

	//ͼƬ�����Qt��ʾ
	Mat2QImage();
}

//���״̬����
void AutomaticTrackingMap::BackEndUpdata()
{
	
	//���ݴ�����
	if (isAutoMode)
	{
		//���̼߳�����
		TMS.CheckThread();
		TMS.GetMatchResults();
		//���̳߳�ʼ��
		TMS.cThreadSurfMapInit(RES.GIMAP);
		TMS.cThreadSurfMapMatch();
		TMS.cThreadTemplatePaimonMatch(RES.GIPAIMON[GIS.resIdPaimon]);
		TMS.cThreadOrbAvatarInit(RES.GIAVATAR);
		TMS.cThreadOrbAvatarMatch();
		TMS.cThreadTemplateUIDInit(RES.GINUMUID);
		TMS.cThreadTemplateUIDMatch();
		TMS.cThreadTemplateStarInit(RES.GISTAR);
		TMS.cThreadTemplateStarMatch();
		


		if (TMS.tIsEndSurfMapInit)
		{
			isAutoInitFinish = true;
			zerosMinMap = TMS.pos;
			MET.zerosMinMap = zerosMinMap;
			GIS.isPaimonVisible = TMS.isPaimonVisial;
			if (TMS.isPaimonVisial)
			{
				TMS.continuityState.setState(TMS.isContinuity);
				if(TMS.isContinuity)
				{
					SST.AutoMapUdpSocketSend(zerosMinMap.x, zerosMinMap.y, TMS.rotationAngle, TMS.uid);
				}
			}
		}
	}
	//��ͫʶ�𲿷�
	if (isAutoMode)
	{
		bool isSave = false;
		if (TMS.tIsEndSurfMapInit)
		{
			if (TMS.continuityState.getIsContinuity())
			{

				//����������Ϊ��ɫ��ǰλ�ý�Ϊ׼ȷ
				//�ж���Χ�Ƿ���������ǣ�����ͫ�Ƿ�Ӧ�ó�����С��ͼ��Ұ֮��
				//���Ӧ�ô��ڣ���ô��ȥ����ƥ�䣬ƥ�������Ϊû�У���ô��Ϊ���ռ���������Ϊ���֣����һ�������ꡣ
				//�����ǰ�ñ��״̬��emmm�Ȳ�����
				if (TMS.isStarVisible||TMS.isStarExist)
				{
					//��ƥ�䵽��star��Ӧ����Ӧ��id
					vector<int> starPosKlass;
					vector<int> starPosId;
					vector<int> starPosDis;
					for (int i = 0; i < TMS.starPos.size(); i++)
					{
						Point p = TMS.starPos[i] * 1.3 + (Point)zerosMinMap;
						double minDis = 999;
						vector<double> dis;
						int klass = 0, id = 0;
						for (int ii = 0; ii < OLS.visualStarIdList.size(); ii++)
						{
							dis.push_back(ATM_Modules::dis(p, OLS.p(OLS.visualStarKlassList[ii], OLS.visualStarIdList[ii])));
							if (dis[ii] < minDis)
							{
								minDis = dis[ii];
								klass = OLS.visualStarKlassList[ii];
								id = OLS.visualStarIdList[ii];
							}
						}
						starPosKlass.push_back(klass);
						starPosId.push_back(id);
						starPosDis.push_back(minDis);
					}
					//������ƥ����Եĵ�ͼ������ݵ�
					//�����ѷ���
					for (int i = 0; i < TMS.starPos.size(); i++)
					{
						switch (OLS.getCollectionState(starPosKlass[i], starPosId[i]))
						{
							case 0:
							{
								if (starPosDis[i] <24)
								{
									OLS.setCollectionState(starPosKlass[i], starPosId[i], 2);
									isSave = true;
								}
								break;
							}
							case 1:
							{
								if (starPosDis[i] <24)
								{
									OLS.setCollectionState(starPosKlass[i], starPosId[i], 2);
									isSave = true;
								}
								break;
							}
							case 2:
							{
								//OLS.setCollectionState(starPosKlass[i], starPosId[i], 2);
								break;
							}
							default:
								break;
						}
					}
					//��Ӧ�ܿ�����star,ɾ���ѷ��ֵģ���Щ��Ϊ�ѷ��ֵ�û���˼����ռ�
					//����δ����
					for (int i = 0; i < OLS.visualStarIdList.size(); i++)
					{
						//�ѷ��֣��Ѿ�����һ������
						bool isFind = false;
						for (int ii = 0; ii < TMS.starPos.size(); ii++)
						{
							if (OLS.visualStarKlassList[i]== starPosKlass[ii] && OLS.visualStarIdList[i] == starPosId[ii])
							{
								isFind = true;
							}
						}
						if (isFind)
						{
							;//break;
						}
						else
						{
							switch (OLS.getCollectionState(OLS.visualStarKlassList[i], OLS.visualStarIdList[i]))
							{
								case 0:
								{
									if (OLS.visualStarDisList[i] < 24)
									{
										OLS.setCollectionState(OLS.visualStarKlassList[i], OLS.visualStarIdList[i], 1);
										isSave = true;
									}
									break;
								}
								case 1:
								{
									//OLS.setCollectionState(OLS.visualStarKlassList[i], OLS.visualStarIdList[i], 1);
									break;
								}
								case 2:
								{
									if (OLS.visualStarDisList[i] < 24)
									{
										OLS.setCollectionState(OLS.visualStarKlassList[i], OLS.visualStarIdList[i], 1);
										isSave = true;
									}
									break;
								}
								default:
									break;
							}
						}
					}
				}
			}
		}

		if (isSave)
		{
			saveLocal();
		}
	}


	if (SLF._uid != TMS.uid)
	{
		SLF.setUID(TMS.uid);
		if (SLF.tryLoad())
		{
			SLF.load();
			loadLocal();
		}
		else
		{
			SLF.save();
		}
	}

	//��ȡ����
	
	//��ȡԭ�񴰿�״̬
	getGiState();
	getKYJGState();
	//��ȡ�������ԭ��λ��
	getThisOffset();
	getThisState();

	//״̬ת�Ʋ���

	//�жϵ�ǰ������״̬
	thisStateMode = getThisState();
}

void AutomaticTrackingMap::Mat2QImage()
{
	std::vector<Mat> mv0;
	std::vector<Mat> mv1;
	//ͨ������
	split(MainMat, mv0);
	split(RES.MAINMASK, mv1);
	mv0.push_back(mv1[0]);
	merge(mv0, MainMat);
	MainImg = QImage((uchar*)(MainMat.data), MainMat.cols, MainMat.rows, MainMat.cols*(MainMat.channels()), QImage::Format_ARGB32);
}

void AutomaticTrackingMap::CustomProcess(int i)
{

}

Mat AutomaticTrackingMap::getViewMap()
{
	static Mat minMap;

	Point minMapPoint = Point(0, 0);

	Size reMapSize = autoMapSize;
	Point2d reAutoMapCenter = autoMapCenter;
	reMapSize.width = (reMapSize.width * MET.scale);
	reMapSize.height = (reMapSize.height * MET.scale);
	reAutoMapCenter = autoMapCenter * MET.scale;

	Point2d LT = zerosMinMap - reAutoMapCenter;
	Point2d RB = zerosMinMap +Point2d(reMapSize)- reAutoMapCenter;

	minMapPoint = LT;

	if (LT.x < 0)
	{
		minMapPoint.x = 0;
	}
	if (LT.y < 0)
	{
		minMapPoint.y = 0;
	}
	if (RB.x > mapSize.width)
	{
		minMapPoint.x = mapSize.width - reMapSize.width;
	}
	if (RB.y > mapSize.height)
	{
		minMapPoint.y = mapSize.height - reMapSize.height;
	}
	minMapRect = Rect(minMapPoint, reMapSize);

	resize(RES.GIMAP(minMapRect), minMap, autoMapSize);
	return minMap;

}

void AutomaticTrackingMap::getGiState()
{
	GIS.getGiState();
	if (GIS.giRectMode != 0)
	{
		GIS.getGiFrame();
	}
}

void AutomaticTrackingMap::setThisState_Normal()
{
	//���ô���λ��
	SetWindowPos(thisHandle, HWND_TOP, GIS.giRect.left + offGiMinMap.x, GIS.giRect.top + offGiMinMap.y, 0, 0, SWP_NOSIZE);
}

void AutomaticTrackingMap::setThisState_Minimize()
{
	//��С����ʾ����
	ShowWindow(thisHandle, SW_MINIMIZE);
	//����ԭ�񴰿�Ϊǰ̨
	SetForegroundWindow(GIS.giHandle);/* ��ԭ�񴰿ڵĲ��� */

}

void AutomaticTrackingMap::setThisState_Top()
{
	//���ô���λ��
	setWindowsPos();
	//��ԭ��ʾ����
	ShowWindow(thisHandle, SW_RESTORE);
}

void AutomaticTrackingMap::setThisState_TopShow()
{
	//���ô���λ��
	setWindowsPos();
	//��ԭ��ʾ����
	ShowWindow(thisHandle, SW_RESTORE);
	//����ԭ�񴰿�Ϊǰ̨
	SetForegroundWindow(GIS.giHandle);/* ��ԭ�񴰿ڵĲ��� */
}

void AutomaticTrackingMap::getThisOffset()
{
	static Point offGiMinMapTmp;
	if (GIS.giRectMode > 0)
	{
		offGiMinMapTmp = GIS.getOffset();
		if (offGiMinMap != offGiMinMapTmp)
		{
			offGiMinMap = offGiMinMapTmp;
		}
	}
}

void AutomaticTrackingMap::getGiHandle()
{
	GIS.getHandle();
}

void AutomaticTrackingMap::getThisHandle()
{
	thisHandle = FindWindowA(NULL, "GenshinImpact_AutoMap");
}

void AutomaticTrackingMap::getThisHandle(HWND _thisHandle)
{
	thisHandle = _thisHandle;
}

void AutomaticTrackingMap::setWindowsPos()
{
	SetWindowPos(thisHandle, HWND_TOPMOST, GIS.giRect.left + offGiMinMap.x, GIS.giRect.top + offGiMinMap.y, 0, 0, SWP_NOSIZE);
}

void AutomaticTrackingMap::setWindowsPos(HWND _thisHandle)
{
	SetWindowPos(_thisHandle, HWND_TOPMOST, GIS.giRect.left + offGiMinMap.x, GIS.giRect.top + offGiMinMap.y, 0, 0, SWP_NOSIZE);
}

void AutomaticTrackingMap::setMoveMapDownPos(int x, int y)
{
	MET.setMouseLeftDownPos(x, y);
}

void AutomaticTrackingMap::setMoveMapUpPos(int x, int y)
{
	MET.setMouseLeftUpPos(x, y);
	MET.normalizationZerosMinMap(Rect(0, 0, mapSize.width, mapSize.width));
	zerosMinMap = MET.zerosMinMap;
}

void AutomaticTrackingMap::setMoveMapMovePos(int x, int y)
{
	MET.setMouseLeftMovePos(x, y);
	MET.normalizationZerosMinMap(Rect(0,0,mapSize.width,mapSize.width));
	zerosMinMap = MET.zerosMinMap;
	
}

void AutomaticTrackingMap::setOffsetDownPos(int x, int y)
{
	MET.setMouseMidDownPos(x, y);
}

void AutomaticTrackingMap::setOffsetUpPos(int x, int y)
{
	MET.setMouseMidUpPos(x, y);
	offGiMinMap = MET.offGiMinMap;
}

void AutomaticTrackingMap::setOffsetMovePos(int x, int y)
{
	MET.setMouseMidMovePos(x, y);
	offGiMinMap = MET.offGiMinMap;
}

void AutomaticTrackingMap::setScaleMapDelta(int x, int y,int delta)
{
	double dx = (x - autoMapCenter.x)*MET.scale;
	double dy = (y - autoMapCenter.y)*MET.scale;

	if (delta > 0) 
	{
		if (MET.scale > 0.5)
		{
			MET.scale /= 1.2;
			MET.zerosMinMap.x += dx * 0.2;//1.2-1
			MET.zerosMinMap.y += dy * 0.2;//1.2-1
			zerosMinMap = MET.zerosMinMap;
		}
	}
	else 
	{
		if (MET.scale < 6)
		{
			MET.scale *= 1.2;
			MET.zerosMinMap.x -= dx * 0.2;//1.2-1
			MET.zerosMinMap.y -= dy * 0.2;//1.2-1
			zerosMinMap = MET.zerosMinMap;
		}
	}
}

void AutomaticTrackingMap::setAutoMode()
{
	isAutoMode = !isAutoMode;
}

bool AutomaticTrackingMap::getAutoMode()
{
	return isAutoMode;
}

void AutomaticTrackingMap::setObjIsShow(int klass)
{
	OLS.setShow(klass);
}

void AutomaticTrackingMap::setObjFlagIsShow()
{
	OLS.setShowFlag();
}

void AutomaticTrackingMap::setAddFlagOnPos()
{
	if (OLS.isSelect)
	{
		OLS.deleteFlag(OLS.selectID);
	}
	else
	{
		OLS.appendFlag(zerosMinMap.x, zerosMinMap.y);
	}
	saveLocal();
}

void AutomaticTrackingMap::setKongYingJiuGuanState()
{
	if (AKY.isRunKYJG)
	{
		AKY.setState(GIS.giHandle);
	}
}

void AutomaticTrackingMap::setGenshinImpactWndHandle(HWND giHandle)
{
	GIS.setGiHandle(giHandle);
}

int AutomaticTrackingMap::getUID()
{
	return TMS.uid;
}

void AutomaticTrackingMap::saveLocal()
{
	if (SLF._uid != TMS.uid)
	{
		SLF.setUID(TMS.uid);
	}
	CopyToLocal();
	SLF.save();
}

void AutomaticTrackingMap::loadLocal()
{
	if (SLF._uid != TMS.uid)
	{
		SLF.setUID(TMS.uid);
	}
	SLF.load();
	CopyToThis();
	OLS.reAppendFlag();
}

void AutomaticTrackingMap::getSystemInfo()
{
	char strBuffer[256] = { 0 };
	DWORD dwSize = 256;
	GetUserNameA(strBuffer, &dwSize);
	SystemUserName = strBuffer;
	SystemUserLocalLow.append("C:\\Users\\");
	SystemUserLocalLow.append(SystemUserName);
	SystemUserLocalLow.append("\\AppData\\LocalLow\\");
	SystemUserFileIndex = SystemUserLocalLow;
	SystemUserFileIndex.append(ApplicationCompanyName);
	SystemUserFileIndex.append("\\");
	SystemUserFileIndex.append(ApplicationName);
	SystemUserFileIndex.append("\\");

	SystemLanguageID = GetSystemDefaultLangID();
	GIS.setGiNameClass(SystemLanguageID);
	switch (SystemLanguageID)
	{
		case 0X0804:
		{
			SystemLanguage = "Chinese";
			break;
		}
		case 0x0409:
		{
			SystemLanguage = "English";
			break;
		}
		default:
		{
			SystemLanguage = "English";
			break;
		}
	}
}

int AutomaticTrackingMap::getThisState()
{
	//����״̬���Ա����Ƿ���������״̬���ã���ֹ��������ԭ�񴰿ڣ���꽹���޷�ת�ơ�
	thisStateModeNext = thisStateMode;
	//����ԭ��״̬�ж���һ֡ʱ������״̬
	if (GIS.isRunning)
	{
		if (isAutoMode)
		{
			if (TMS.tIsEndSurfMapInit == true)
			{
				if (GIS.isPaimonVisible)
				{
					thisStateModeNext = ThisWinState::TopShow;
				}
				else
				{
					thisStateModeNext = ThisWinState::Minimize;
				}
			}
			else
			{
				thisStateModeNext = ThisWinState::TopShow;
			}

			if (AKY.isActivationMap)
			{
				thisStateModeNext = ThisWinState::Minimize;
			}
		}
		else
		{
			thisStateModeNext = ThisWinState::Top;
		}
	}
	else
	{
		thisStateModeNext = ThisWinState::Normal;
	}
	//���״̬����һ֡��ͬ��������״̬����������
	if (isFristChangeThisState||thisStateModeNext != thisStateMode)
	{
		thisStateMode = thisStateModeNext;
		isFristChangeThisState = true;
	}
	else
	{
		thisStateModeNext = ThisWinState::Wait;
	}
	return thisStateMode;
}

void AutomaticTrackingMap::getKYJGState()
{
	AKY.getKYJGHandle();
}

void AutomaticTrackingMap::setThisState()
{
	if (isFristChangeThisState != true)
	{
		return;
	}
	else
	{
		isFristChangeThisState = false;
	}
	switch (thisStateMode)
	{
		case ThisWinState::Normal:
		{
			setThisState_Normal();
			break;
		}
		case ThisWinState::Minimize:
		{
			setThisState_Minimize();
			break;
		}
		case ThisWinState::Top:
		{
			//���ñ����ö���ǰ����״̬
			setThisState_Top();
			break;
		}
		case ThisWinState::TopShow:
		{
			//���ñ����ö���ǰ����״̬
			setThisState_TopShow();
			break;
		}
		default:
		{
			setThisState_Normal();
			break;
		}
	}
}

void AutomaticTrackingMap::setThreadMatchMat()
{
	if (GIS.isRunning&&GIS.giRectMode > 0)
	{
		Mat matRGB2GRAY;
		cvtColor(GIS.giFrameMap, matRGB2GRAY, CV_RGB2GRAY);
		TMS.getObjMinMap(GIS.giFrameMap);
		//TMS.getObjMinMap(matRGB2GRAY);
		cvtColor(GIS.giFramePaimon, matRGB2GRAY, CV_RGB2GRAY);
		TMS.getObjPaimon(matRGB2GRAY);
		TMS.getObjUID(GIS.giFrameUID);
	}
	else
	{
		TMS.isExistObjMinMap = false;
		TMS.isExistObjPaimon = false;
		TMS.isExistObjUID = false;
	}
}

void AutomaticTrackingMap::drawObjectLists()
{
	//draw Star
	static Point avatarPos = zerosMinMap;
	int x = 0, y = 0;
	Point p;
	Mat ObjIconROIMat;
	const int dx = 16, dy = 16;//ͼ�궥�㵽ͼ�����ĵ�ƫ��
	double minDist = 9999;
	OLS.visualStarKlassList.clear();
	OLS.visualStarIdList.clear();
	OLS.visualStarDisList.clear();
	for (int objKlass = 0; objKlass < OLS.objectListsNumber(); objKlass++)
	{
		if (OLS.isShow(objKlass))
		{
			for (int objOrder = 0; objOrder < OLS.objectsNumber(objKlass); objOrder++)
			{
				//0 δ���� 1 δ���ּ����ռ� 2 �ѷ���δ�ռ�  
				switch (OLS.getCollectionState(objKlass, objOrder))
				{
					case 0:
					{
						p = OLS.p(objKlass, objOrder);
						if (ATM_Modules::isContains(minMapRect, p))
						{
							x = (int)((p.x - minMapRect.x) / MET.scale) - dx;
							y = (int)((p.y - minMapRect.y) / MET.scale) - dy;
							//��x��y��ΧҪ���㹻�Ŀռ������ͼ��
							if (x > 0 && y > 0 && x + RES.GIOBJICON[objKlass].cols < autoMapSize.width&&y + RES.GIOBJICON[objKlass].rows < autoMapSize.height)
							{
								ObjIconROIMat = MainMat(Rect(x, y, RES.GIOBJICON[objKlass].cols, RES.GIOBJICON[objKlass].rows));
								addWeightedAlpha(ObjIconROIMat, RES.GIOBJICON[objKlass], RES.GIOBJICONMASK[objKlass]);
							}
						}
						break;
					}
					case 1:
					{
						p = OLS.p(objKlass, objOrder);
						if (ATM_Modules::isContains(minMapRect, p))
						{
							x = (int)((p.x - minMapRect.x) / MET.scale) - dx;
							y = (int)((p.y - minMapRect.y) / MET.scale) - dy;
							//��x��y��ΧҪ���㹻�Ŀռ������ͼ��
							if (x > 0 && y > 0 && x + RES.GIOBJICON[objKlass].cols < autoMapSize.width&&y + RES.GIOBJICON[objKlass].rows < autoMapSize.height)
							{
								ObjIconROIMat = MainMat(Rect(x, y, RES.GIOBJICON[objKlass].cols, RES.GIOBJICON[objKlass].rows));
								addWeightedAlpha(ObjIconROIMat, RES.GIOBJICON[objKlass], RES.GIOBJICONMASK[objKlass],0.75);
							}
						}
						break;
					}
					case 2: 
					{
						p = OLS.p(objKlass, objOrder);
						if (ATM_Modules::isContains(minMapRect, p))
						{
							x = (int)((p.x - minMapRect.x) / MET.scale) - dx;
							y = (int)((p.y - minMapRect.y) / MET.scale) - dy;
							//��x��y��ΧҪ���㹻�Ŀռ������ͼ��
							if (x > 0 && y > 0 && x + RES.GIOBJICON[objKlass].cols < autoMapSize.width&&y + RES.GIOBJICON[objKlass].rows < autoMapSize.height)
							{
								ObjIconROIMat = MainMat(Rect(x, y, RES.GIOBJICON[objKlass].cols, RES.GIOBJICON[objKlass].rows));
								addWeightedAlpha(ObjIconROIMat, RES.GIOBJICON[objKlass], RES.GIOBJICONMASK[objKlass], 0.2);
							}
						}
						break;
					}
					default:
						break;
					
				}
				double dis = ATM_Modules::dis(TMS.pos, p);
				if (dis < 64)
				{
					TMS.isStarExist = true;
					OLS.visualStarKlassList.push_back(objKlass);
					OLS.visualStarIdList.push_back(objOrder);
					OLS.visualStarDisList.push_back(dis);
				}
				if (dis < minDist)
				{
					minDist = dis;
					OLS.selectObjKlass = objKlass;
					OLS.selectObjID = objOrder;
				}
			}
			if (minDist < 64 && isAutoMode)
			{
				for (int i = 0; i < OLS.visualStarIdList.size(); i++)
				{
					p = OLS.p(OLS.visualStarKlassList[i], OLS.visualStarIdList[i]);
					x = (int)((p.x - minMapRect.x) / MET.scale) - RES.GISTAR.cols / 2;
					y = (int)((p.y - minMapRect.y) / MET.scale) - RES.GISTAR.rows / 2;
					ObjIconROIMat = MainMat(Rect(x, y, RES.GISTAR.cols, RES.GISTAR.rows));
					addWeightedAlpha(ObjIconROIMat, RES.GISTAR, RES.GISTARMASK, 0.5);
				}
				TMS.isStarExist = true;
				OLS.isSelectObj = true;

			}
			else
			{
				TMS.isStarExist = false;
				OLS.isSelectObj = false;
				OLS.selectObjKlass = -1;
				OLS.selectObjID = -1;
			}
		}
	}
	if (isAutoMode)
	{
		avatarPos = zerosMinMap;
	}
	for (int i = 0; i < TMS.starPos.size(); i++)
	{
		p = TMS.starPos[i] * 1.3 + avatarPos;
		x = (int)((p.x - minMapRect.x) / MET.scale) - RES.GISTAR.cols / 2;
		y = (int)((p.y - minMapRect.y) / MET.scale) - RES.GISTAR.rows / 2;
		//��x��y��ΧҪ���㹻�Ŀռ������ͼ��
		if (x > 0 && y > 0 && x + RES.GISTAR.cols < autoMapSize.width&&y + RES.GISTAR.rows < autoMapSize.height)
		{
			ObjIconROIMat = MainMat(Rect(x, y, RES.GISTAR.cols, RES.GISTAR.rows));
			addWeightedAlpha(ObjIconROIMat, RES.GISTAR, RES.GISTARMASK);
		}
	}

}

void AutomaticTrackingMap::drawFlagLists()
{
	//draw Flag
	int x = 0, y = 0;
	Point p;
	Mat ObjIconROIMat;
	const int dx = 16, dy = 16;//ͼ�궥�㵽ͼ�����ĵ�ƫ��
	double minDist = 9999;
	if (OLS.isShowFlag())
	{
		for (int objOrder = 0; objOrder < OLS.flagNumber(); objOrder++)
		{
			p = OLS.fp(objOrder);
			if (ATM_Modules::isContains(minMapRect, p))
			{
				x = (int)((p.x - minMapRect.x) / MET.scale) - dx;
				y = (int)((p.y - minMapRect.y) / MET.scale) - dy;
				//��x��y��ΧҪ���㹻�Ŀռ������ͼ��
				if (x > 0 && y > 0 && x + RES.GIOBJFLAGICON[0].cols < autoMapSize.width&&y + RES.GIOBJFLAGICON[0].rows < autoMapSize.height)
				{
					ObjIconROIMat = MainMat(Rect(x, y, RES.GIOBJFLAGICON[0].cols, RES.GIOBJFLAGICON[0].rows));
					addWeightedAlpha(ObjIconROIMat, RES.GIOBJFLAGICON[0], RES.GIOBJFLAGICONMASK[0]);

					double dis = ATM_Modules::dis(zerosMinMap, p);
					if (dis < minDist)
					{
						minDist = dis;
						OLS.selectID = objOrder;
					}

				}
			}
		}
		if (minDist < 16)
		{
			p = OLS.fp(OLS.selectID);
			x = (int)((p.x - minMapRect.x) / MET.scale) - dx;
			y = (int)((p.y - minMapRect.y) / MET.scale) - dy;
			ObjIconROIMat = MainMat(Rect(x, y, RES.GIOBJFLAGICON[1].cols, RES.GIOBJFLAGICON[1].rows));
			addWeightedAlpha(ObjIconROIMat, RES.GIOBJFLAGICON[1], RES.GIOBJFLAGICONMASK[1]);

			OLS.isSelect = true;
		}
		else
		{
			OLS.isSelect = false;
			OLS.selectID = -1;

		}
	}
}

void AutomaticTrackingMap::drawAvatar()
{
	if (isAutoMode)
	{
		Mat avatar= rotateAvatar(TMS.rotationAngle,1.0/1.3);//���ͼ��С��ͼ֮��
		
		addWeightedPNG(MainMat(Rect(autoMapCenter.x - RES.GIAVATARMASK.cols / 2, autoMapCenter.y - avatar.rows / 2, avatar.cols, avatar.rows)), avatar);
	}
}

void AutomaticTrackingMap::CopyToThis()
{
	if (SLF._stateFST.row() != 0)
	{
		OLS._collectionStateFST = SLF._stateFST;
	}
	if (SLF._stateFST.row() != 0)

	{
		OLS._collectionStateYST = SLF._stateYST;
	}
	if (SLF._stateFST.row() != 0)
	{
		OLS._collectionStateFHYS = SLF._stateFHYS;
	}
	if (SLF._stateFST.row() != 0)
	{
		OLS._collectionStateFlag = SLF._stateFlag;
	}
}

void AutomaticTrackingMap::CopyToLocal()
{
	SLF._stateFST = OLS._collectionStateFST;
	SLF._stateYST = OLS._collectionStateYST;
	SLF._stateFHYS = OLS._collectionStateFHYS;
	SLF._stateFlag = OLS._collectionStateFlag;
}

void AutomaticTrackingMap::addWeightedAlpha(Mat & backgroundImage, Mat & Image, Mat &maskImage)
{
	std::vector<cv::Mat>scr_channels;
	std::vector<cv::Mat>dstt_channels;
	std::vector<cv::Mat>alpha_channels;
	split(Image, scr_channels);
	split(backgroundImage, dstt_channels);
	split(maskImage, alpha_channels);

	Mat Alpha = alpha_channels[0];

	for (int i = 0; i < 3; i++)
	{
		dstt_channels[i] = dstt_channels[i].mul(Alpha, 1.0 / 255.0);
		dstt_channels[i] += scr_channels[i].mul(~Alpha, 1.0 / 255.0);
	}
	merge(dstt_channels, backgroundImage);
}

void AutomaticTrackingMap::addWeightedAlpha(Mat & backgroundImage, Mat & Image, Mat & maskImage, double alpha)
{
	std::vector<cv::Mat>scr_channels;
	std::vector<cv::Mat>dstt_channels;
	std::vector<cv::Mat>alpha_channels;
	split(Image, scr_channels);
	split(backgroundImage, dstt_channels);
	split(maskImage, alpha_channels);

	Mat Alpha = alpha_channels[0]+(unsigned char)(alpha*255);

	for (int i = 0; i < 3; i++)
	{
		dstt_channels[i] = dstt_channels[i].mul(Alpha,1.0 / 255.0 );
		dstt_channels[i] += scr_channels[i].mul(~Alpha, 1.0/ 255.0);
	}
	merge(dstt_channels, backgroundImage);
}

void AutomaticTrackingMap::addWeightedPNG(Mat & backgroundImage, Mat & Image)
{
	std::vector<cv::Mat>scr_channels;
	std::vector<cv::Mat>dstt_channels;
	split(Image, scr_channels);
	split(backgroundImage, dstt_channels);

	Mat Alpha = scr_channels[3];

	for (int i = 0; i < 3; i++)
	{
		dstt_channels[i] = dstt_channels[i].mul(~Alpha,1.0 / 255.0);
		dstt_channels[i] += scr_channels[i].mul(Alpha, 1.0/ 255.0);
	}
	merge(dstt_channels, backgroundImage);
}

Mat AutomaticTrackingMap::rotateAvatar(double angle)
{
	Mat avatar;
	Point2f pt(RES.GIAVATARMASK.cols / 2., RES.GIAVATARMASK.rows / 2.);
	Mat r = getRotationMatrix2D(pt, angle, 1.0);
	warpAffine(RES.GIAVATARMASK, avatar, r, Size(pt*2));
	return avatar;
}

Mat AutomaticTrackingMap::rotateAvatar(double angle, double scale)
{
	Mat avatar;
	Point2f pt(RES.GIAVATARMASK.cols / 2., RES.GIAVATARMASK.rows / 2.);
	Mat r = getRotationMatrix2D(pt, angle, scale);
	warpAffine(RES.GIAVATARMASK, avatar, r, Size(pt * 2));
	return avatar;
}
