// This is the main DLL file.
//#include "stdafx.h"
#include "AI_User_End_Class.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
//using namespace cv;

namespace AI_User_End_Classes {
	
	bool cppAI_User_End::cppInitAI(char* projectPath, char* nodeNames, int nodeNum, char* imagePath) {
		//��̬����DLL
		if (NULL == pDLLHandle)
			teInitDll(&pDLLHandle);
		if (NULL == pDLLHandle) {
			cout << "DLL����ʧ��" << endl;
			return false;
		}

		//�����Ԥ��ͼƬ
		TeString stImagePath;
		sprintf_s(stImagePath.pData, imagePath);
		SrcImage = teReadImageFromHD(&stImagePath);
		if (1 > SrcImage.width || 1 > SrcImage.height) {
			cout << "����ͼƬ·��" << endl;
			return false;
		}

		//����ڵ�����
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

		//������Ŀ����
		TeString strProjPath;
		sprintf_s(strProjPath.pData, projectPath);

		TeSize stSize;
		stSize.height = SrcImage.height;
		stSize.width = SrcImage.width;
		TeStatus eStatus;

		//��ʼ����Ŀ
		eStatus = teInitProj(&nProjHandle, &strProjPath, pstrNodeName, stSize, iRstNodeNum, 0);
		teGetError(eStatus, &stErrStr);

		cout << "��ʼ����Ŀ teInitProj ��" << stErrStr.pData << endl;

		//�Զ���ʵ�ʵ�ǰҪ��ȡ����Ľڵ㼯�ϣ�
		pstNodeRst = new TeNodeRst[iRstNodeNum];
		memset(pstNodeRst, 0, sizeof(TeNodeRst) * iRstNodeNum);
		for (int i = 0; i < iRstNodeNum; i++)
		{
			//�������طָ�ڵ���Ҫ��ȡ��ǩͼ�Ļ���ҪΪ������ڴ�..������ΪNULL
			pstNodeRst[i].pnLabelMat = new unsigned char[sizeof(unsigned char) * SrcImage.width * SrcImage.height]; 
		}

		return true;
	}

	void cppAI_User_End::cppReleaseAI() {

		if (NULL != pstNodeRst) {
			for (int i = 0; i < iRstNodeNum; i++)
			{
				delete[] pstNodeRst[i].pnLabelMat; //�������طָ�ڵ���Ҫ��ȡ��ǩͼ�Ļ���ҪΪ������ڴ�..������ΪNULL
			}
			//ɾ���ڵ㼯��
			delete[] pstNodeRst;
			pstNodeRst = NULL;
			//�ͷ�ͼƬ
			teReleaseImage(&SrcImage);
		}

		//�ͷ���Ŀ
		if (NULL != nProjHandle) {
			eStatus = teReleaseProj(nProjHandle);
			nProjHandle = NULL;
			teGetError(eStatus, &stErrStr);
			cout << "�ͷ�Project teReleaseProj ��" << stErrStr.pData << endl;
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

//		////�����Ԥ��ͼƬ
//        TeString stImagePath;
//        sprintf_s(stImagePath.pData, ".\\AI_Initial_Data\\atl_train.bmp");
//        TeImage SrcImage = teReadImageFromHD(&stImagePath);
//        if (1 > SrcImage.width || 1 > SrcImage.height)
//        {
//            printf("����ͼƬ·��\n");

//            system("pause");
//        }

		if (NULL != imageData)
			memcpy(SrcImage.imageData, imageData, SrcImage.width*SrcImage.height*SrcImage.nChannels);

//        ////��ʾͼƬ
//        teShowImage(&SrcImage);

		//return;

		eStatus = tePredictProj(nProjHandle, &SrcImage, pstNodeRst, iRstNodeNum, NULL, pstrNodeName);
		//teGetError(eStatus, &stErrStr);
		//printf_s("Ԥ��Project tePredictProj ��%s\n", stErrStr.pData);

//        ///��ָ���ڵ��Ԥ��������ͼ�ϲ���ʾ
//        for (int i = 0; i < iRstNodeNum; i++)
//        {
//            teDrawRstInSrcImageByRect(&SrcImage, &pstNodeRst[i], NULL, 0.6f, 1);

//            teDrawRstInSrcImageByFilling(&SrcImage, &pstNodeRst[i]);
//        }

//        ///��ʾͼƬ
//        teShowImage(&SrcImage);
	}

	void cppAI_User_End::cppOutputResult(unsigned char* resultmask, int l)
	{
		memcpy(resultmask, pstNodeRst[l].pnLabelMat, sizeof(unsigned char) * SrcImage.width * SrcImage.height);
	}


	//�ַ����ķָ�����Ӵ��ָ��ַ���
	//������
	//str���ָ���ַ���
	//sub�Ӵ�
	//sub_beforeΪstr��sub����λ�õ�֮ǰ���֣�������sub
	//sub_afterΪstr��sub����λ�õ�֮�󲿷֣�������sub
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
		//�ָ��ַ�����ʱ��ʹ��strstr�����������ǵ�һ��sub��ַ,���û��ƥ����򷵻�NULL
		char* temp = (char*)malloc(strlen(str));
		strcpy(temp, str);
		*sub_after = strstr(temp, sub);
		//û��ƥ����Ӵ�
		if (*sub_after == NULL || **sub_after == '\0') {
			printf("the str without sub");
			return;
		}
		//��ӽ�����־��
		*(*sub_after + strlen(*sub_after)) = '\0';
		*sub_after = *sub_after + 1;
		//���ַ�����sub֮ǰ���ַ���ֵ��sub_before
		int i = 0;
		while (i < (strlen(temp) - strlen(*sub_after))) {
			*(*sub_before + i) = *(temp + i);
			i++;
		}
		//��ӽ�����־��,��ӽ�����־��Ŀ����Ϊ�˷�ֹ����
		*(*sub_before + i-1) = '\0';
	}


}
