// CPUUsage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "chart.h"
#include "CPUUsage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

typedef struct MyData
{
	int val1;
	int val2;
	//char key[32];
}MYDATA;

DWORD WINAPI FunProc(LPVOID lpParameter)
{
	//达到100%
	//float * p = 0;

	//while (p == p)
	//{
	//	(*p++) + 1.144564542315345;
	//}
	//return 0;



	MYDATA *pmd = (MYDATA *)lpParameter;
	int beishu = pmd->val2;
	//printf("%d\n", beishu);

	//达到90%,问题是读的CPU占用率和Task显示有出入，Task显示100%
	//while (true)

	//{

	//	//for 循环运行x次，使它的时间和sleep的时间相等
	//	//8000
	//	for (int i = 0; i < 6600000 * beishu * 8; i++);

	//	Sleep(100);

	//}

	int busytime = 100;
	int idletime = busytime;
	long starttime = 0;
	while (true)
	{
		starttime = GetTickCount();
		//std::cout<<starttime<<std::endl;//调试用
		while ((GetTickCount() - starttime) <= busytime * 9)
			;
		Sleep(idletime);
	}

}

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
			CCPUUseRate cpu0;
			int num_core = cpu0.GetCPUCoreNum();
			MYDATA mydt[8];
			HANDLE hThread;
			for (int i = 0; i < num_core; i++)
			{
				mydt[i].val1 = i;
				mydt[i].val2 = num_core;
				hThread = CreateThread(NULL, 0, FunProc, &mydt[i], 0, NULL);
				DWORD offset = 1;
				if (i < num_core) {
					offset = 1 << i;
				}
				//将线程分配到每一个核
				SetThreadAffinityMask(hThread, offset);
				CloseHandle(hThread);
			}
			chart *chartdialog = new chart;
			int ReturnValue = chartdialog->DoModal(); // Show the dialog
			printf("%d", ReturnValue);
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
