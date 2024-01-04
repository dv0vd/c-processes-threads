#include<iostream>
#include<Windows.h>
#include <strsafe.h>
#include <tchar.h>
using namespace std;

HANDLE *ProcessesCreating(CONST INT &processesCount, TCHAR *szCmdLine)
{
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE *processes = new HANDLE[processesCount];
	for (INT i = 0; i < processesCount; i++)
		if (CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
			processes[i] = pi.hProcess;
		else
			processes[i] = NULL;
	return processes;
}

VOID ProcessesRemoving(CONST INT &processesCount, HANDLE *processes)
{
	for (INT i = 0; i < processesCount; i++)
		CloseHandle(processes[i]);
	delete[]processes;
}

VOID Input(INT &processesCount)
{
	for (;;)
	{
		cout << "Количество экземляров: ";
		cin >> processesCount;
		if (processesCount > 0)
			break;
	}
}

BOOL CheckingName(LPCTSTR argv)
{
	if (((argv[0]) == 'A') && ((argv[1]) == 'P') && ((argv[2]) == 'P'))
		return TRUE;
	else
		return FALSE;
}

INT main(INT argc, LPCTSTR argv[])
{
	if (argc == 1)
	{
		TCHAR szCmdLine[MAX_PATH];
		INT processesCount;
		Input(processesCount);
		HANDLE hObject = CreateSemaphoreEx(NULL, 1, 1, NULL, 0, SEMAPHORE_ALL_ACCESS);
		if (hObject != NULL)
		{
			StringCchPrintf(szCmdLine, _countof(szCmdLine), TEXT("%s APP %d %p"), argv[0], (INT)GetCurrentProcessId(), hObject);
			HANDLE *processes = ProcessesCreating(processesCount, szCmdLine);
			WaitForMultipleObjects(processesCount, processes, TRUE, INFINITE);
			ProcessesRemoving(processesCount, processes);
			CloseHandle(hObject);
		}
		else
			cout << endl << "ОШИБКА!" << endl;
		system("pause");
	}
	else
	{
		if ((argc == 4) && (CheckingName(argv[1])))
		{
			HANDLE hSemaphore = NULL;
			DWORD dwProcessId = (DWORD)_ttoi(argv[2]);
			HANDLE hObject = (HANDLE)_tcstoui64(argv[3], NULL, 16);
			HANDLE hSourceProcessHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwProcessId);
			if ((hObject != NULL) && (hSourceProcessHandle != NULL))
			{
				DuplicateHandle(hSourceProcessHandle, hObject, GetCurrentProcess(), &hSemaphore, SEMAPHORE_ALL_ACCESS, FALSE, 0);
				CloseHandle(hSourceProcessHandle);
				if (hSemaphore != NULL)
				{
					for (INT i = 1; i < 4; i++)
					{
						WaitForSingleObject(hSemaphore, INFINITE);
						cout << "Счёт до 10 ( " << i << "-ый раз ):   ";
						for (INT j = 1; j < 11; j++)
						{
							cout << "\b" << j;
							Sleep(500);
						}
						cout << endl;
						ReleaseSemaphore(hSemaphore, 1, NULL);
					}
					CloseHandle(hSemaphore);
				}
			}
		}
		else
			cout << endl << "ERROR" << endl;
	}
}