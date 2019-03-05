#include "stdafx.h"
#include "Impedance.h"
#include <conio.h> //ʹ�������п���
///////////////////////////////////////////////////////////
struct RobotData
{
	double JointsNow[4];
	double JointsNext[4];
	double JointsVelNow[4];
	double JointsVelNext[4];
	double Origin6axisForce[6];
	double JointsTorque[4];
	double CartesianPositionNow[4];
	double CartesianPositionNext[4];
	double CartesianVelNow[4];
	double CartesianVelNext[4];
};

extern SOCKET sockClient; //ȫ�ֱ������ͻ��˵��׽���
//////���嶨ʱ����
#define Tms (10)   
#define T (Tms*0.001)
////////////////////////
int testNUM = 0;
int timenum = 0;
bool UpOrDown = 0;  //0Ϊup,1Ϊdown
HANDLE hSyncEvent;//ͬ���¼����
bool ImpedenceControllerStopflag; //�߳̽�����־
//�迹���ƴ�Home�㿪ʼ�˶�����Home���ʱ��GaptoPositive=0;
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	CImpedance *pImpedence = (CImpedance *)lpParam;  //��ȡ��ָ��
	RobotData MyRobotData;
	ResetEvent(hSyncEvent);  //ȷ���¼��������ź�״̬
//	AllocConsole();//ע���鷵��ֵ   ����ʹ������̨����Ϊ����ں�����ʹ������ǳ�������ʱ�䣬������ʾ�����11ms
	LARGE_INTEGER litmp;
	LONGLONG qt1=0, qt2=0,qt1last;
	double dft, dfm1, dfm2, dff;
	//���ʱ��Ƶ��  
	QueryPerformanceFrequency(&litmp);//���ʱ��Ƶ��  
	dff = (double)litmp.QuadPart;
	while (1)
	{	
////////////////////////////////////////////////����ʱ����뿪ʼ
		////��ó�ʼֵ  
		//QueryPerformanceCounter(&litmp);
		//qt1 = litmp.QuadPart;//��õ�ǰʱ��t1��ֵ	
		//dfm1 = (double)(qt1 - qt2);
		//dft = dfm1 / dff;
		//_cprintf("t1-t2=%.3f\n", dft * 1000);
///////////////////////////////////////////////����ʱ��������
		WaitForSingleObject(hSyncEvent, INFINITE);
////////////////////////////////////////////////����ʱ����뿪ʼ
		QueryPerformanceCounter(&litmp);
		qt1last = qt1;
		qt1 = litmp.QuadPart;//��õ�ǰʱ��t2��ֵ 
		//��ö�Ӧ��ʱ��ֵ��ת�����뵥λ��  
		dfm1 = (double)(qt1 - qt2);
		dft = dfm1 / dff;
//		_cprintf("waited time:t1-t2=%.3f\n", dft * 1000);
		TRACE("waited time:t1-t2=%.3f\n", dft * 1000);
//		TRACE("testNUM=%d\n", testNUM);	
		dfm1 = (double)(qt1 - qt1last);
		dft = dfm1 / dff;
//		_cprintf("One cycle time:t1-t1last=%.3f\n", dft*1000);
		TRACE("One cycle time:t1-t1last=%.3f\n", dft * 1000);
///////////////////////////////////////////////����ʱ��������

		if (ImpedenceControllerStopflag)
		{
			break;
		}
//////////////////**********����������������ʼ
		if (UpOrDown == 0)    //��������
		{
			timenum++;
			if (timenum == 1000)
			{
				UpOrDown = 1;   //��ߵ㣬����½���
			}
		}
		else     //�������½���
		{
			timenum--;
			if (timenum == 0)
			{
				UpOrDown = 0;   //��͵㣬���������
			}
		}
////////////////***********��������������������
		///////////����������Ҫ����Ĵ���		
		if (!(pImpedence->m_Robot->m_isOnGap))
		{
			pImpedence->GetCurrentState();//��ȡ��ǰ��ʱ�̵Ĺؽڿռ���ٶȣ�λ�ú�ֱ������ռ��λ�ã��ٶ�
			for (int i = 0; i < 4; i++)
			{
				TRACE("the %d��axis theta is: %.3f\n",i,pImpedence->m_thetaImpedPara[i].Now);
				TRACE("the %d' axis angelVel is: %.3f\n",i,pImpedence->m_angularVelImpedPara[i].Now);
			}

			TRACE("timeflag=%d\n", timenum);
			pImpedence->GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile();  //������һ��ʱ�̵ĹؽڵĽǶȺͽ��ٶȲ�ִ��
			
			////������TCP/IP����
			memset(&MyRobotData, 0, sizeof(MyRobotData));
			MyRobotData.JointsNow[0] = pImpedence->m_thetaImpedPara[0].Now;
			MyRobotData.JointsVelNow[0] = pImpedence->m_angularVelImpedPara[0].Now;
			MyRobotData.JointsTorque[0] = timenum / 20.0;

			MyRobotData.JointsNow[1] = pImpedence->m_thetaImpedPara[1].Now;
			MyRobotData.JointsVelNow[1] = pImpedence->m_angularVelImpedPara[1].Now;
			MyRobotData.JointsTorque[1] = timenum / 20.0;

			MyRobotData.JointsNow[2] = pImpedence->m_thetaImpedPara[2].Now;
			MyRobotData.JointsVelNow[2] = pImpedence->m_angularVelImpedPara[2].Now;
			MyRobotData.JointsTorque[2] = timenum / 20.0;
			char buff[sizeof(MyRobotData)];
			memset(buff, 0, sizeof(MyRobotData));
			memcpy(buff, &MyRobotData, sizeof(MyRobotData));
			send(sockClient, buff, sizeof(buff), 0);
		}


		////////////����������
////////////////////////////////////////////����ʱ����뿪ʼ
		QueryPerformanceCounter(&litmp);
		qt2 = litmp.QuadPart;//��õ�ǰʱ��t3��ֵ 
		dfm2 = (double)(qt2 - qt1);
		dft = dfm2 / dff;
//		_cprintf("The program running time:t2-t1=%.3f\n", dft * 1000);
		TRACE("The program running time:t2-t1=%.3f\n", dft * 1000);
////////////////////////////////////////////����ʱ��������
		ResetEvent(hSyncEvent);//��λͬ���¼�
	}
	GT_SetIntSyncEvent(NULL);//֪ͨ�豸ISR �ͷ��¼�
	CloseHandle(hSyncEvent); //�ر�ͬ���¼����
	ExitThread(0);
	return 0;
}

CImpedance::CImpedance(CRobotBase *Robot)
{
	m_Robot = Robot;
	m_RunningFlag = false;
	m_M = 0;
	m_K = 0.5;   //��λ�� N/mm
	m_B = 0.51;
	m_FImpedPara.Last = 0;
	m_FImpedPara.Now = 0;
	m_FImpedPara.Next = 0;

	for (int i = 0; i < m_Robot->m_JointNumber; i++)
	{
		m_xImpedPara[i].Last = 0;
		m_xImpedPara[i].Now = 0;
		m_xImpedPara[i].Next = 0;

		m_vImpedPara[i].Last = 0;
		m_vImpedPara[i].Now = 0;
		m_vImpedPara[i].Next = 0;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)    //����ֻ�������ؽ�
	{
		m_thetaImpedPara[i].Last = 0;
		m_thetaImpedPara[i].Now = 0;
		m_thetaImpedPara[i].Next = 0;
	}
	for (int i = 0; i < m_Robot->m_JointNumber; i++)    //����ֻ�������ؽ�
	{
		m_angularVelImpedPara[i].Last = 0;
		m_angularVelImpedPara[i].Now = 0;
		m_angularVelImpedPara[i].Next = 0;
	}

	m_hControlThread = NULL;	

}


CImpedance::~CImpedance()
{

}


//���Ǿ��ðѻ����˴��ݸ��迹�������ȽϺã���Ϊ�迹������һ�ֱȻ���������ϲ�Ŀ��ƣ����Ի����˲�һ�����迹���ƣ������迹����һ���л�����
bool CImpedance::StartImpedanceController()
{

	m_Robot->UpdateJointArray(); //ˢ�¸����ؽڵ�ֵ

	for (int i = 0; i < m_Robot->m_JointNumber; i++)   //�õ������ؽڵĽǶ�,���������õ�����ֱ������λ�ã��ǶȻ���mm
	{	
		m_thetaImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointPosition;
		m_thetaImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointPositon;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)  //�õ������ؽڵĽ��ٶ�  �˿��������ٶ�
	{
		m_angularVelImpedPara[i].Last = m_Robot->m_JointArray[i].LastJointVelocity;
		m_angularVelImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointVelocity;
	}

	//�������������ã��������õ�ʱ����Ҫֱ�Ӳɼ�������Ϣ
	m_FImpedPara.Last = 0;   //����1N  ����λ��N
	m_FImpedPara.Now = 0;   
	m_FImpedPara.Next = 0;

/////////////////ֱ������ϵ����Ϣ
	m_xImpedPara[0].Last = m_Robot->m_HandCurrTn[0][3];   //��X���ߵ�λ��
	m_xImpedPara[1].Last = m_Robot->m_HandCurrTn[1][3];   //��Y���ߵ�λ��
	m_xImpedPara[2].Last = m_Robot->m_HandCurrTn[2][3];   //��z���ߵ�λ��

//�˴�Ӧ�û����ٶȣ���Ҫ���


//����һ���µ��̣߳�������߳�����ʹ��һ����ʱ��һֱˢ��
	unsigned short Status;
	hSyncEvent = CreateEvent(NULL, true, false, NULL); //WIN32 API ����
	if (hSyncEvent == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("������ʱ�����ʧ��!"), MB_OK);
	}
	GT_SetIntrTm(5*Tms);  //���ö�ʱ���Ķ�ʱ����Ϊ50*200us = 10ms
	GT_TmrIntr();   //���������붨ʱ�ж�
	//GT_GetIntr(&Status);   //���windows������������������ 
//	if (&Status != 0)
	//{
	//	AfxMessageBox(_T("��ʱ���ж����ó���!"), MB_OK);
//		exit(1);
//	}
	GT_SetIntSyncEvent(hSyncEvent);//ΪPCI���ƿ������ж�ͬ���¼������ò���ΪNULLʱ���ú�����λ��ǰ������ֵ
	ImpedenceControllerStopflag = false;
	m_hControlThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)(this), CREATE_SUSPENDED, NULL);  //���춨ʱ����������������ú���,���迹���ƶ����ָ�븳��Ϊ���̺߳����Ĳ���
	SetThreadPriority(m_hControlThread, THREAD_PRIORITY_HIGHEST);
	ResumeThread(m_hControlThread);
	if (m_hControlThread == NULL)
	{
		AfxMessageBox(_T("�����߳�ʧ��!"), MB_OK);
		return false;
	}


	return true;
}

//�ر��迹����������Ҫ�ǽ����϶�����⣬��֤��ֹͣ��ʱ�򣬱�֤��϶GaptoPositive=0;
bool CImpedance::StopImpedanceController()
{
	ImpedenceControllerStopflag = true;
	this->m_Robot->m_pController->wait_motion_finished(0);
	if (this->m_Robot->m_JointGap[0].GapToPositive != 0)
	{
		if (this->m_Robot->m_isGapCorrespond == false)     //�ڸ�-->��ת�۵㴦
		{
			this->m_Robot->m_isOnGap = true;   //��ʼ������϶�ˣ��������ϲ��迹��������ʱʲô������
			long pos;
			double vel1, acc;

			//���ؽ�ֵת��Ϊ����ֵ
			pos = (long)((this->m_Robot->m_JointArray[0].CurrentJointWithoutGapPosition + this->m_Robot->m_JointGap[0].GapLength)* this->m_Robot->m_JointArray[0].PulsePerMmOrDegree);  //�߹���-->��ת�۵㴦
			//���ٶ�תΪ�忨���ܵ��ٶ�,vel�ǽǶ�ÿ�룬������ÿ����   Ĭ�ϳ������������200us,deg/s = 
			vel1 = this->m_Robot->m_JointArray[0].NormalJointVelocity;
			//���ٶ�ֱ�Ӵ���ȥ����λһֱ��Pulse/ST^2
			acc = this->m_Robot->m_JointArray[0].NormalJointAcc;
			if (this->m_Robot->m_pController->AxisMoveToWithTProfile(1, pos, vel1, acc) != 0)  //���������˶�ģʽ
				return -1;
			this->m_Robot->m_pController->wait_motion_finished(1);  //�ȴ����˶���ɺ�ֹͣ
			this->m_Robot->m_isGapCorrespond = true;     //����ƥ������
			this->m_Robot->m_JointGap[0].GapToPositive = 0;
			this->m_Robot->m_JointGap[0].GapToNegative = this->m_Robot->m_JointGap[0].GapLength - this->m_Robot->m_JointGap[0].GapToPositive;
			this->m_Robot->UpdateJointArray();			//@wqqʦ��������ӵ�
			this->m_Robot->m_isOnGap = false;   //��ɼ�϶����ʱ��������迹�����м�������
			TRACE("pass the negetive to positive!\n");
		}
	}
}

bool CImpedance::GetCurrentState(void)
{
	m_Robot->UpdateJointArray(); //ˢ�¸����ؽڵ�ֵ
	for (int i = 0; i < m_Robot->m_JointNumber; i++)   //�õ������ؽڵĽǶ�,���������õ�����ֱ������λ�ã��ǶȻ���mm
	{
		m_thetaImpedPara[i].Last = m_thetaImpedPara[i].Now;
		m_thetaImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointPositon;
	}

	for (int i = 0; i < m_Robot->m_JointNumber; i++)  //�õ������ؽڵĽ��ٶ�  �˿��������ٶ�
	{
		m_angularVelImpedPara[i].Last = m_angularVelImpedPara[i].Now;
		m_angularVelImpedPara[i].Now = m_Robot->m_JointArray[i].CurrentJointVelocity;
	}

	//�������������ã��������õ�ʱ����Ҫֱ�Ӳɼ�������Ϣ
	//m_FImpedPara.Last = 10;   //����1N  ����λ��N
	//m_FImpedPara.Now = 10;
	//m_FImpedPara.Next = 10;

	/////////////////ֱ������ϵ����Ϣ
	m_xImpedPara[0].Last = m_Robot->m_HandCurrTn[0][3];   //��X���ߵ�λ��
	m_xImpedPara[1].Last = m_Robot->m_HandCurrTn[1][3];   //��Y���ߵ�λ��
	m_xImpedPara[2].Last = m_Robot->m_HandCurrTn[2][3];   //��z���ߵ�λ��


	//�˴�Ӧ�û���ֱ������ռ���ٶȣ���Ҫ�õ��ſ˱Ⱦ��󣬵��ǻ�����������ӣ����������ﻹû�ж���

	return true;
}

bool CImpedance::GetNextStateUsingJointSpaceImpendence(void)
{
	//double Torque[3] = { 10, 10, 10 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���
	//for (int i = 0; i < 3; i++)
	//{
	//	m_angularVelImpedPara[i].Next = (Torque[i] - m_K*m_thetaImpedPara[i].Now) / (m_K*0.01 + m_B);
	//	m_thetaImpedPara[i].Next = m_thetaImpedPara[i].Now + m_angularVelImpedPara[i].Next*0.01;
	//}
	return true;
	
}

bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithSpeedWithTProfile(void)
{
	double Torque[3] = { 10, 10, 10 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���

	for (int i = 0; i < 3; i++)
	{
		m_angularVelImpedPara[i].Next = (Torque[i] - m_K*m_thetaImpedPara[i].Now) / (m_K*T + m_B);
		m_thetaImpedPara[i].Next = m_thetaImpedPara[i].Now + m_angularVelImpedPara[i].Next*T;
	}
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
		TRACE("the %d��axis next angelVel is: %.3f\n", i, this->m_angularVelImpedPara[i].Next);
	}
	double GoalPos[4], GoalVel[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		GoalPos[i] = (this->m_thetaImpedPara[i].Next) + 2;   //������ֱ�Ӽ���һ�����Ķ�������ֹ����
		GoalVel[i] = this->m_angularVelImpedPara[i].Next;
	}
	this->m_Robot->JointsTMove(GoalPos, GoalVel);
	return true;
}

//////////////////��������һ��ʱ�̵�λ�ò������Ǹ�λ�ã������ٶȵĹ滮
bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithTProfile(void)
{
	
	double Torque[3] = { 0, 0, 0 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���


		Torque[0] = timenum / 100.0;
		Torque[1] = timenum / 100.0;
		Torque[2] = timenum / 100.0;

	for (int i = 0; i < 3; i++)  //ʹ������ֵ���ʽ
	{
		m_thetaImpedPara[i].Next = 1.0 / (m_B / T + m_K)*Torque[i] + 1.0 / (m_B / T + m_K)*(m_B / T)*m_thetaImpedPara[i].Now;
	}
	//for (int i = 0; i < 3; i++)   //ֱ��ʹ��΢�ַ���
	//{
	//	m_thetaImpedPara[i].Next = 1.0 / (m_B / T + m_K)*Torque[i] + 1.0 / (m_B / T + m_K)*(m_B / T)*m_thetaImpedPara[i].Now;
	//}
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
	}
	double GoalPos[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		GoalPos[i] = (this->m_thetaImpedPara[i].Next);  
	}
	this->m_Robot->JointSynTMove(GoalPos,T);
	return true;
}

bool CImpedance::GetNextStateUsingJointSpaceImpendenceWithoutSpeedWithSProfile(void)
{
	double Torque[3] = { 10, 10, 10 };   //�����ǲ����ã����ÿ���ؽڵ����أ�ֻʹ��ǰ�����ؽڵĲ���
	for (int i = 0; i < 3; i++)
	{
		m_thetaImpedPara[i].Next = 1.0 / (m_B / T + m_K)*Torque[i] + 1.0 / (m_B / T + m_K)*(m_B / T)*m_thetaImpedPara[i].Now;
	}
	for (int i = 0; i < 4; i++)
	{
		TRACE("the %d��axis next theta is: %.3f\n", i, this->m_thetaImpedPara[i].Next);
	}
	double GoalPos[4];
	for (int i = 0; i < this->m_Robot->m_JointNumber; i++)
	{
		GoalPos[i] = (this->m_thetaImpedPara[i].Next);
	}
	return true;
}