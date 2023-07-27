// AI_User_End_Class.h
#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <windows.h>
//#include<vcclr.h>

//#include<opencv2/opencv.hpp>

#include <te.h>
#include <teCalcuTime.h>
#include <teTreePredict.h>
#include <teTreePredictDataType.h>

using namespace std;
//using namespace System;
//using namespace Runtime::InteropServices;

namespace AI_User_End_Classes {

class cppAI_User_End
	{
	public:

		TeDllHandle pDLLHandle = NULL;//dll
		TePredictProjHandle nProjHandle = NULL;//��Ŀ
		TeImage SrcImage;//��ʼ��ͼƬ
		TeString* pstrNodeName;//�ڵ�����
		TeNodeRst* pstNodeRst;//����ڵ㼯��
		int iRstNodeNum = 0;
		TeStatus eStatus;
		TeString stErrStr;
		
		bool cppInitAI(char* projectPath, char* nodeNames, int nodeNum, char* imagePath);

		void cppReleaseAI();
			 
		void cppPredictImage(unsigned char* imageData);

		void cppOutputResult(unsigned char* resultmask,int l);

		//�ַ����ķָ�����Ӵ��ָ��ַ���
		//������
		//str���ָ���ַ���
		//sub�Ӵ�
		//sub_beforeΪstr��sub����λ�õ�֮ǰ����
		//sub_afterΪstr��sub����λ�õ�֮�󲿷֣�����sub
		void static str_split(const char* str/*in*/, char* sub/*in*/, char** sub_before/*out*/, char** sub_after/*out*/);

	};



}
