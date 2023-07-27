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
		TePredictProjHandle nProjHandle = NULL;//项目
		TeImage SrcImage;//初始化图片
		TeString* pstrNodeName;//节点名称
		TeNodeRst* pstNodeRst;//结果节点集合
		int iRstNodeNum = 0;
		TeStatus eStatus;
		TeString stErrStr;
		
		bool cppInitAI(char* projectPath, char* nodeNames, int nodeNum, char* imagePath);

		void cppReleaseAI();
			 
		void cppPredictImage(unsigned char* imageData);

		void cppOutputResult(unsigned char* resultmask,int l);

		//字符串的分割，根据子串分割字符串
		//参数：
		//str被分割的字符串
		//sub子串
		//sub_before为str中sub所在位置的之前部分
		//sub_after为str中sub所在位置的之后部分，包括sub
		void static str_split(const char* str/*in*/, char* sub/*in*/, char** sub_before/*out*/, char** sub_after/*out*/);

	};



}
