//参考链接：https://blog.csdn.net/wangting627/article/details/22931337
//https://blog.csdn.net/fyxichen/article/details/50577580
//https://blog.csdn.net/chenlycly/article/details/88842619
#define _WIN32_DCOM
#include "pch.h"
#include <conio.h>
#include <comdef.h>
#include <Wbemidl.h>

# pragma comment(lib, "wbemuuid.lib")
#include "CCPUUseRate.h"
#include <iostream>
using namespace std;


CCPUUseRate::CCPUUseRate()
{
}


CCPUUseRate::~CCPUUseRate()
{
}


double CCPUUseRate::FileTimeToDouble(FILETIME* pFiletime)
{
	return (double)((*pFiletime).dwHighDateTime * 4.294967296E9) + (double)(*pFiletime).dwLowDateTime;
}


bool CCPUUseRate::Initialize()
{
	FILETIME ftIdle, ftKernel, ftUser;
	BOOL flag = FALSE;
	if (flag = GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
	{
		m_fOldCPUIdleTime = FileTimeToDouble(&ftIdle);
		m_fOldCPUKernelTime = FileTimeToDouble(&ftKernel);
		m_fOldCPUUserTime = FileTimeToDouble(&ftUser);

	}
	return flag;
}


/*
cpu整体使用率
*/
int CCPUUseRate::GetCPUUseRate()
{
	int nCPUUseRate = -1;
	FILETIME ftIdle, ftKernel, ftUser;
	//检索系统定时信息，对于多处理器系统，返回值是各处理器总和的时间
	//参数1：空闲时间
	//参数2：内核时间
	//参数3：用户时间
	//函数成功，返回值为非零。函数失败，返回值为零
	if (GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
	{
		double fCPUIdleTime = FileTimeToDouble(&ftIdle);
		double fCPUKernelTime = FileTimeToDouble(&ftKernel);
		double fCPUUserTime = FileTimeToDouble(&ftUser);
		//CPU使用率 = 100 - 空闲时间 / 系统时间
		double sys = fCPUKernelTime - m_fOldCPUKernelTime + fCPUUserTime - m_fOldCPUUserTime;
		//nCPUUseRate = (int)(100 - (fCPUIdleTime - m_fOldCPUIdleTime) / sys*100.0);
		nCPUUseRate = (int)((sys - (fCPUIdleTime - m_fOldCPUIdleTime)) / sys*100.0);
		m_fOldCPUIdleTime = fCPUIdleTime;
		m_fOldCPUKernelTime = fCPUKernelTime;
		m_fOldCPUUserTime = fCPUUserTime;
	}
	return nCPUUseRate;
}

//获取CPU的总核心数
int CCPUUseRate::GetCPUCoreNum() {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	//printf("now system cpu num is %d\n", sysInfo.dwNumberOfProcessors);
	return sysInfo.dwNumberOfProcessors;
}

//WMI方法获取每个核的CPU使用率，不过和Task里面的值有出入，这个值会低些
bool CCPUUseRate::getCpuLoadInfo() {
	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl;
		return 0;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities
		NULL                         // Reserved
	);


	if (FAILED(hres)) {
		cout << "Failed to initialize security. Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 0;                    // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres)) {
		cout << "Failed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 0;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres)) {
		cout << "Could not connect. Error code = 0x" << hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return 0;                // Program has failed.
	}

	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities
	);

	if (FAILED(hres)) {
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 0;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	IWbemClassObject *pclsObj;
	int i;
	//while (1) {
		i = 1;
		hres = pSvc->ExecQuery(
			bstr_t("WQL"),
			bstr_t("SELECT * FROM Win32_PerfFormattedData_PerfOS_Processor"),

			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);

		if (FAILED(hres)) {
			cout << "Query for operating system name failed."
				<< " Error code = 0x"
				<< hex << hres << endl;
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			return 0;               // Program has failed.
		}

		// Step 7: -------------------------------------------------
		// Get the data from the query in step 6 -------------------


		ULONG uReturn = 0;

		while (pEnumerator) {
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
				&pclsObj, &uReturn);

			if (0 == uReturn) {
				break;
			}

			VARIANT vtProp;

			// Get the value of the Name property
			//hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
			hr = pclsObj->Get(L"PercentProcessorTime", 0, &vtProp, 0, 0);
			wcout << " CPU Usage of CPU " << i << " : " << vtProp.bstrVal << endl;
			hr = pclsObj->Get(L"PercentIdleTime", 0, &vtProp, 0, 0);
			wcout << " CPU Idle of CPU " << i << " : " << vtProp.bstrVal << endl;
			VariantClear(&vtProp);

			//IMPORTANT!!
			pclsObj->Release();

			i++;
		}

	//}

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	//pclsObj->Release();
	CoUninitialize();

	return 0;   // Program successfully completed.
}