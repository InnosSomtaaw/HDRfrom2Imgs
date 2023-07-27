// This is the main DLL file.
//#include "stdafx.h"
#include "AI_User_End_Class.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
//using namespace cv;

namespace AI_User_End_Classes {
	
	bool cppAI_User_End::cppInitAI(char* projectPath, char* nodeNames, int nodeNum, char* imagePath) {
		//动态载入DLL
		if (NULL == pDLLHandle)
			teInitDll(&pDLLHandle);
		if (NULL == pDLLHandle) {
			cout << "DLL加载失败" << endl;
			return false;
		}

		//载入待预测图片
		TeString stImagePath;
		sprintf_s(stImagePath.pData, imagePath);
		SrcImage = teReadImageFromHD(&stImagePath);
		if (1 > SrcImage.width || 1 > SrcImage.height) {
			cout << "请检查图片路径" << endl;
			return false;
		}

		//载入节点名称
		iRstNodeNum = nodeNum;
		pstrNodeName = new TeString[iRstNodeNum];
		char**  nodeNameArray = new char*[iRstNodeNum];
		char* nodeNamesResidue = nodeNames;
        char nodesplit[]=" ";
		for (int i = 0; i < iRstNodeNum; i++)
		{
			nodeNameArray[i] = NULL;
            cppAI_User_End::str_split(nodeNamesResidue,nodesplit, &nodeNameArray[i], &nodeNamesResidue);
		}
		for (int i = 0; i < iRstNodeNum; i++)
		{
			sprintf_s(pstrNodeName[i].pData, nodeNameArray[i]);
		}
		delete[] nodeNameArray;

		//载入项目名称
		TeString strProjPath;
		sprintf_s(strProjPath.pData, projectPath);

		TeSize stSize;
		stSize.height = SrcImage.height;
		stSize.width = SrcImage.width;
		TeStatus eStatus;

		//初始化项目
		eStatus = teInitProj(&nProjHandle, &strProjPath, pstrNodeName, stSize, iRstNodeNum, 0);
		teGetError(eStatus, &stErrStr);

		cout << "初始化项目 teInitProj ：" << stErrStr.pData << endl;

		//自定义实际当前要获取结果的节点集合，
		pstNodeRst = new TeNodeRst[iRstNodeNum];
		memset(pstNodeRst, 0, sizeof(TeNodeRst) * iRstNodeNum);
		for (int i = 0; i < iRstNodeNum; i++)
		{
			//对于像素分割节点需要获取标签图的话则要为其分配内存..否则置为NULL
			pstNodeRst[i].pnLabelMat = new unsigned char[sizeof(unsigned char) * SrcImage.width * SrcImage.height]; 
		}

		return true;
	}

	void cppAI_User_End::cppReleaseAI() {

		if (NULL != pstNodeRst) {
			for (int i = 0; i < iRstNodeNum; i++)
			{
				delete[] pstNodeRst[i].pnLabelMat; //对于像素分割节点需要获取标签图的话则要为其分配内存..否则置为NULL
			}
			//删除节点集合
			delete[] pstNodeRst;
			pstNodeRst = NULL;
			//释放图片
			teReleaseImage(&SrcImage);
		}

		//释放项目
		if (NULL != nProjHandle) {
			eStatus = teReleaseProj(nProjHandle);
			nProjHandle = NULL;
			teGetError(eStatus, &stErrStr);
			cout << "释放Project teReleaseProj ：" << stErrStr.pData << endl;
		}

		if (NULL != pDLLHandle)
		{
			teReleaseTeDll(&pDLLHandle);
			pDLLHandle = NULL;
		}
	}

	void cppAI_User_End::cppPredictImage(unsigned char* imageData) 
    {
//        Mat img_src = Mat(SrcImage.height, SrcImage.width, CV_8UC3, imageData);

//        imshow("test", img_src);
//        waitKey(0);

		//return;

//		////载入待预测图片
//        TeString stImagePath;
//        sprintf_s(stImagePath.pData, ".\\AI_Initial_Data\\atl_train.bmp");
//        TeImage SrcImage = teReadImageFromHD(&stImagePath);
//        if (1 > SrcImage.width || 1 > SrcImage.height)
//        {
//            printf("请检查图片路径\n");

//            system("pause");
//        }

		if (NULL != imageData)
			memcpy(SrcImage.imageData, imageData, SrcImage.width*SrcImage.height*SrcImage.nChannels);

//        ////显示图片
//        teShowImage(&SrcImage);

		//return;

		eStatus = tePredictProj(nProjHandle, &SrcImage, pstNodeRst, iRstNodeNum, NULL, pstrNodeName);
		//teGetError(eStatus, &stErrStr);
		//printf_s("预测Project tePredictProj ：%s\n", stErrStr.pData);

//        ///将指定节点的预测结果画在图上并显示
//        for (int i = 0; i < iRstNodeNum; i++)
//        {
//            teDrawRstInSrcImageByRect(&SrcImage, &pstNodeRst[i], NULL, 0.6f, 1);

//            teDrawRstInSrcImageByFilling(&SrcImage, &pstNodeRst[i]);
//        }

//        ///显示图片
//        teShowImage(&SrcImage);
	}

	void cppAI_User_End::cppOutputResult(unsigned char* resultmask, int l)
	{
		memcpy(resultmask, pstNodeRst[l].pnLabelMat, sizeof(unsigned char) * SrcImage.width * SrcImage.height);
	}


	//字符串的分割，根据子串分割字符串
	//参数：
	//str被分割的字符串
	//sub子串
	//sub_before为str中sub所在位置的之前部分，不包括sub
	//sub_after为str中sub所在位置的之后部分，不包括sub
	void cppAI_User_End::str_split(const char* str/*in*/, char* sub/*in*/, char** sub_before/*out*/, char** sub_after/*out*/) {
		if (str == NULL || *str == '\0' || sub == NULL) {
			printf("function str_split error:( str == NULL || *str=='\0' || sub==NULL)");
			return;
		}
		if (*sub_before == NULL) {
			*sub_before = (char*)malloc(strlen(str));
		}
		if (*sub_after == NULL) {
			*sub_after = (char*)malloc(strlen(str));
		}
		//分割字符串的时候使用strstr函数，返回是第一个sub地址,如果没有匹配的则返回NULL
		char* temp = (char*)malloc(strlen(str));
		strcpy(temp, str);
		*sub_after = strstr(temp, sub);
		//没有匹配的子串
		if (*sub_after == NULL || **sub_after == '\0') {
			printf("the str without sub");
			return;
		}
		//添加结束标志符
		*(*sub_after + strlen(*sub_after)) = '\0';
		*sub_after = *sub_after + 1;
		//将字符串中sub之前的字符赋值给sub_before
		int i = 0;
		while (i < (strlen(temp) - strlen(*sub_after))) {
			*(*sub_before + i) = *(temp + i);
			i++;
		}
		//添加结束标志符,添加结束标志的目的是为了防止乱码
		*(*sub_before + i-1) = '\0';
	}


}
