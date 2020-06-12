#pragma once
#include <windows.h>

class CCPUUseRate
{
public:
	CCPUUseRate();
	~CCPUUseRate();
public:
	double m_fOldCPUIdleTime;
	double m_fOldCPUKernelTime;
	double m_fOldCPUUserTime;
	double FileTimeToDouble(FILETIME* pFiletime);
	bool Initialize();
	int GetCPUUseRate();
	int GetCPUCoreNum();
	bool getCpuLoadInfo();
};

