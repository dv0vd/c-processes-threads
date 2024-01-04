#include<Windows.h>
#include"resource.h"
#include <WindowsX.h>
#include<Psapi.h>
#include<strsafe.h>

#define LOAD_PROCESS_BUTTON 8
#define TERMINATE_PROCESS 9
#define PAUSE 10
#define PROCESS_RADIO1 101
#define PROCESS_RADIO2 102
#define PROCESS_RADIO3 103
#define PROCESS_RADIO4 104
#define PROCESS_RADIO5 105
#define PROCESS_RADIO6 106
#define THREAD_RADIO1 201
#define THREAD_RADIO2 202
#define THREAD_RADIO3 203
#define THREAD_RADIO4 204
#define THREAD_RADIO5 205
#define THREAD_RADIO6 206
#define THREAD_RADIO7 207
#define IAM_IN_TASK 11
#define NEW_JOB 12
#define PROCESSES_IN_JOB 13

HINSTANCE handle;
HWND ProcessesListBox, ModulesListBox, hWnd = NULL, Seconds, Answer;
DWORD ProcessInJobIds[1024], cbNeededInJob = 0;
HANDLE job=NULL;

VOID Exit(HWND hWnd) {
	int mbResult = MessageBox(hWnd, TEXT("Действительно выйти?"), TEXT("Выход"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
	if (IDYES == mbResult)
		PostQuitMessage(EXIT_SUCCESS);
}

VOID CreatingElements(HINSTANCE hInstance, HWND hWnd)
{
	CreateWindow("static", "Задаваемое время ( 1-999 с)", WS_CHILD | WS_VISIBLE, 780, 30, 200, 30, hWnd, (HMENU)1, hInstance, NULL);
	Seconds = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_BORDER, 1000, 30, 30, 20, hWnd, (HMENU)-1, hInstance, NULL);
	CreateWindow("BUTTON", "Приоритет приложения", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 770, 110, 265, 150, hWnd, (HMENU)-1, hInstance, NULL);
	CreateWindow("BUTTON", "Реального времени", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 130, 250, 20, hWnd, (HMENU)PROCESS_RADIO1, hInstance, NULL);
	CreateWindow("BUTTON", "Высокий", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 150, 250, 20, hWnd, (HMENU)PROCESS_RADIO2, hInstance, NULL);
	CreateWindow("BUTTON", "Выше среднего", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 170, 250, 20, hWnd, (HMENU)PROCESS_RADIO3, hInstance, NULL);
	CreateWindow("BUTTON", "Средний", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 190, 250, 20, hWnd, (HMENU)PROCESS_RADIO4, hInstance, NULL);
	CreateWindow("BUTTON", "Ниже среднего", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 210, 250, 20, hWnd, (HMENU)PROCESS_RADIO5, hInstance, NULL);
	CreateWindow("BUTTON", "Низкий", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 230, 230, 20, hWnd, (HMENU)PROCESS_RADIO6, hInstance, NULL);
	CheckRadioButton(hWnd, PROCESS_RADIO1, PROCESS_RADIO6, PROCESS_RADIO4);
	CreateWindow("BUTTON", "Приоритет главного потока", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 770, 270, 265, 170, hWnd, (HMENU)-1, hInstance, NULL);
	CreateWindow("BUTTON", "Критичный по времени", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 290, 250, 20, hWnd, (HMENU)THREAD_RADIO1, hInstance, NULL);
	CreateWindow("BUTTON", "Максимальный", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 310, 250, 20, hWnd, (HMENU)THREAD_RADIO2, hInstance, NULL);
	CreateWindow("BUTTON", "Выше среднего", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 330, 250, 20, hWnd, (HMENU)THREAD_RADIO3, hInstance, NULL);
	CreateWindow("BUTTON", "Средний", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 350, 250, 20, hWnd, (HMENU)THREAD_RADIO4, hInstance, NULL);
	CreateWindow("BUTTON", "Ниже среднего", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 370, 250, 20, hWnd, (HMENU)THREAD_RADIO5, hInstance, NULL);
	CreateWindow("BUTTON", "Минимальный", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 390, 250, 20, hWnd, (HMENU)THREAD_RADIO6, hInstance, NULL);
	CreateWindow("BUTTON", "Простаивающий", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 775, 410, 250, 20, hWnd, (HMENU)THREAD_RADIO7, hInstance, NULL);
	CheckRadioButton(hWnd, THREAD_RADIO1, THREAD_RADIO7, THREAD_RADIO4);
	CreateWindow("BUTTON", "Процессы", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 10, 360, 470, hWnd, (HMENU)-1, hInstance, NULL);
	CreateWindow("BUTTON", "Приостановка работы", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 770, 10, 265, 85, hWnd, (HMENU)-1, hInstance, NULL);
	CreateWindow("BUTTON", "Пауза", WS_CHILD | WS_VISIBLE, 855, 60, 70, 30, hWnd, (HMENU)PAUSE, hInstance, NULL);
	CreateWindow("BUTTON", "Задания", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 1040, 10, 250, 170, hWnd, (HMENU)-1, hInstance, NULL);
	CreateWindow("BUTTON", "Я в задании?", WS_CHILD | WS_VISIBLE, 1045, 40, 100, 30, hWnd, (HMENU)IAM_IN_TASK, hInstance, NULL);
	Answer = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 1160, 50, 30, 20, hWnd, (HMENU)1, hInstance, NULL);
	ProcessesListBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY, 15, 55, 350, 400, hWnd, NULL, NULL, NULL);
	ModulesListBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL, 400, 35, 350, 440, hWnd, NULL, NULL, NULL);
	SendMessage( ModulesListBox, LB_SETHORIZONTALEXTENT, 900, 0);
	CreateWindow("BUTTON", "Модули", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 395, 10, 360, 470, hWnd, (HMENU)-1, hInstance, NULL);
	CreateWindow("BUTTON", "Создать задание", WS_CHILD | WS_VISIBLE, 1045, 80, 240, 30, hWnd, (HMENU)NEW_JOB, hInstance, NULL);
	CreateWindow("BUTTON", "Процессы в созданном задании", WS_CHILD | WS_VISIBLE, 1045, 120, 240, 30, hWnd, (HMENU)PROCESSES_IN_JOB, hInstance, NULL);
	CreateWindow("BUTTON", "Принудительно завершить", WS_CHILD | WS_VISIBLE, 1070, 200, 200, 30, hWnd, (HMENU)TERMINATE_PROCESS, hInstance, NULL);
	CreateWindow("BUTTON", "Загрузить список всех процессов", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 445, 250, 30, hWnd, (HMENU)LOAD_PROCESS_BUTTON, hInstance, NULL);
	CreateWindow("static", "PID", WS_CHILD | WS_VISIBLE, 20, 30, 100, 20, hWnd, (HMENU)1, hInstance, NULL);
	CreateWindow("static", "Название", WS_CHILD | WS_VISIBLE, 120, 30, 100, 20, hWnd, (HMENU)1, hInstance, NULL);
}

VOID LoadProcesses()
{
	ListBox_ResetContent(ProcessesListBox);
	DWORD aProcessIds[1024], cbNeeded; 
	if (EnumProcesses(aProcessIds, sizeof(aProcessIds), &cbNeeded))
	{
		TCHAR szName[MAX_PATH], szBuffer[300]; 
		DWORD n = cbNeeded / sizeof(DWORD);
		for (DWORD i = 0; i < n; ++i)
		{
			DWORD dwProcessId = aProcessIds[i], cch = 0;
			if (0 == dwProcessId) continue; 
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
			if (NULL != hProcess)
			{
				cch = GetModuleBaseName(hProcess, NULL, szName, MAX_PATH);
				CloseHandle(hProcess);
			} 
			if (0 == cch)
				StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));
			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%.6u      %s"), dwProcessId,szName);
			int iItem = ListBox_AddString(ProcessesListBox, szBuffer);
			ListBox_SetItemData(ProcessesListBox, iItem, dwProcessId);
		} 
	} 
}

VOID LoadProcessesModules()
{
	int index = ListBox_GetCurSel(ProcessesListBox);
	DWORD ProcessId = ListBox_GetItemData(ProcessesListBox, index);
	ListBox_ResetContent(ModulesListBox);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, ProcessId);
	if(NULL != hProcess)
	{
		DWORD cb ;
		EnumProcessModulesEx(hProcess, NULL, 0, &cb, LIST_MODULES_ALL);
		DWORD nCount = cb / sizeof(HMODULE);
		HMODULE *hModule = new HMODULE[nCount];
		cb = nCount * sizeof(HMODULE);
		if(EnumProcessModulesEx(hProcess, hModule, cb, &cb, LIST_MODULES_ALL))
		{
			TCHAR szFileName[MAX_PATH]; 
			for (DWORD i = 0; i < nCount; ++i)
			{
				if(GetModuleFileNameEx(hProcess, hModule[i], szFileName, MAX_PATH))
					ListBox_AddString(ModulesListBox, szFileName); 
			} 
		} 
		delete[]hModule; 
	    CloseHandle(hProcess); 
	} 
	else
		MessageBox(hWnd, TEXT("Для данного процесса не удалось загрузить список модулей((("), TEXT("Ошибка"), MB_OK | MB_ICONERROR);

}

VOID TerminateProc()
{
	int index = ListBox_GetCurSel(ProcessesListBox);
	if (index != -1)
	{
		DWORD ProcessId = ListBox_GetItemData(ProcessesListBox, index);
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, ProcessId);
		if (NULL != hProcess)
		{
			if (FALSE != TerminateProcess(hProcess, 0))
			{
				MessageBox(hWnd, TEXT("Процесс завершён!"), TEXT("Успех"), MB_OK | MB_ICONINFORMATION);
				CloseHandle(hProcess);
			}
			else
				MessageBox(hWnd, TEXT("Процесс завершить не удалось((("), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
		}
		else
			MessageBox(hWnd, TEXT("Процесс завершить не удалось((("), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
	}
	else
		MessageBox(hWnd, TEXT("Вы не выбрали процесс для завершения!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

BOOL CALLBACK PauseProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendMessage(hwndDlg, WM_SETTEXT, 0, (LPARAM)"Пауза");
		SetDlgItemText(hwndDlg, IDABORT, "Пожалуйста, ждите...");
		return TRUE;
	}
	return FALSE;
}

VOID Waiting(DWORD time)
{
	int index = ListBox_GetCurSel(ProcessesListBox);
	DWORD ProcessId = ListBox_GetItemData(ProcessesListBox, index);
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, ProcessId);
	if (NULL != hProcess)
	{
		HWND dialog = CreateDialogA(handle, MAKEINTRESOURCE(IDD_DIALOG), hWnd, PauseProc);
		ShowWindow(dialog, SW_SHOW);
		UpdateWindow(dialog);
		WaitForSingleObject(hProcess, time * 1000);
		DestroyWindow(dialog);
		CloseHandle(hProcess);
		LoadProcesses();
		MessageBox(hWnd, TEXT("Пауза окончена!"), TEXT("Успех"), MB_OK | MB_ICONINFORMATION);
	}
	else
		MessageBox(hWnd, TEXT("Не выбран процесс!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

VOID ProcessPriority(DWORD wParam)
{
	DWORD Priority;
	switch (wParam)
	{
	case PROCESS_RADIO1:
		Priority = REALTIME_PRIORITY_CLASS;
		break;
	case PROCESS_RADIO2:
		Priority = HIGH_PRIORITY_CLASS;
		break;
	case PROCESS_RADIO3:
		Priority = ABOVE_NORMAL_PRIORITY_CLASS;
		break;
	case PROCESS_RADIO4:
		Priority = NORMAL_PRIORITY_CLASS;
		break;
	case PROCESS_RADIO5:
		Priority = BELOW_NORMAL_PRIORITY_CLASS;
		break;
	case PROCESS_RADIO6:
		Priority = IDLE_PRIORITY_CLASS;
		break;
	}
	if (SetPriorityClass(GetCurrentProcess(), Priority))
	{
		CheckRadioButton(hWnd, PROCESS_RADIO1, PROCESS_RADIO6, LOWORD(wParam));
		MessageBox(hWnd, TEXT("Приоритет изменён!"), TEXT("Успех"), MB_OK | MB_ICONINFORMATION);
	}
	else
		MessageBox(hWnd, TEXT("Приоритет не изменён!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

VOID ThreadPriority(DWORD wParam)
{
	DWORD Priority;
	switch (wParam)
	{
	case THREAD_RADIO1:
		Priority = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	case THREAD_RADIO2:
		Priority = THREAD_PRIORITY_HIGHEST;
		break;
	case THREAD_RADIO3:
		Priority = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case THREAD_RADIO4:
		Priority = THREAD_PRIORITY_NORMAL;
		break;
	case THREAD_RADIO5:
		Priority = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case THREAD_RADIO6:
		Priority = THREAD_PRIORITY_LOWEST;
		break;
	case THREAD_RADIO7:
		Priority = THREAD_PRIORITY_IDLE;
		break;
	}
	if (SetThreadPriority(GetCurrentThread(), Priority))
	{
		CheckRadioButton(hWnd, THREAD_RADIO1, THREAD_RADIO7, LOWORD(wParam));
		MessageBox(hWnd, TEXT("Приоритет изменён!"), TEXT("Успех"), MB_OK | MB_ICONINFORMATION);
	}
	else
		MessageBox(hWnd, TEXT("Приоритет не изменён!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

BOOL EnumProcessesInJob(HANDLE hJob, ULONG_PTR *lpidProcess, DWORD cb, LPDWORD lpcbNeeded)
{
	DWORD nCount = cb / sizeof(ULONG_PTR);
	if (NULL != lpidProcess && nCount > 0)
	{
		DWORD cbJobPIL = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + (nCount -1) * sizeof(ULONG_PTR);
		JOBOBJECT_BASIC_PROCESS_ID_LIST *pJobPIL = (JOBOBJECT_BASIC_PROCESS_ID_LIST *)malloc(cbJobPIL);
		if (NULL != pJobPIL)
		{
			pJobPIL->NumberOfAssignedProcesses = nCount;
			BOOL bRet= QueryInformationJobObject(hJob, JobObjectBasicProcessIdList, pJobPIL, cbJobPIL, NULL);
			if (FALSE != bRet)
			{
				nCount = pJobPIL->NumberOfProcessIdsInList;
				CopyMemory(lpidProcess, pJobPIL->ProcessIdList, nCount*sizeof(ULONG_PTR));
				if (NULL != lpcbNeeded)
					*lpcbNeeded = nCount * sizeof(ULONG_PTR);
			}
			free(pJobPIL);
			return bRet;
		} 
	} 
	return FALSE;
}

BOOL CALLBACK ProcessesInJobProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		SendMessage(hwndDlg, WM_SETTEXT, 0, (LPARAM)"Процессы в задании");
		SetDlgItemText(hwndDlg, IDYES, "PID");
		SetDlgItemText(hwndDlg, IDNO, "Название");
		TCHAR szName[MAX_PATH], szBuffer[300];
		DWORD n = cbNeededInJob / sizeof(DWORD);
		for (DWORD i = 0; i < n; ++i)
		{
			DWORD dwProcessId = ProcessInJobIds[i], cch = 0;
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
			if (NULL != hProcess)
			{
				cch = GetModuleBaseName(hProcess, NULL, szName, _countof(szName));
				CloseHandle(hProcess);
			}
			if (0 == cch)
				StringCchCopy(szName, MAX_PATH, TEXT("Неизвестный процесс"));
			StringCchPrintf(szBuffer, _countof(szBuffer), TEXT("%.6u      %s"), dwProcessId, szName);
			SendDlgItemMessage(hwndDlg, IDHELP, LB_ADDSTRING, 0, (LPARAM)szBuffer);
		}
		return TRUE;
	}
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		return TRUE;
	case WM_COMMAND: 
		if (LOWORD(wParam) == IDOK)
			EndDialog(hwndDlg, 0);
		return TRUE;
	}
	return FALSE;
}

VOID TaskChecking(HANDLE hJob)
{
	BOOL checking=FALSE;
	IsProcessInJob(GetCurrentProcess(), hJob, &checking);
	if (!checking)
		SetWindowText(Answer, "нет");
	else
		SetWindowText(Answer, "да");
		if (EnumProcessesInJob(hJob, ProcessInJobIds, sizeof(ProcessInJobIds), &cbNeededInJob))
		{
			DialogBox(handle, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, ProcessesInJobProc);
		}
		else
			MessageBox(hWnd, TEXT("Ошибка при получении "), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
}

BOOL StartGroupProcessesAsJob(HANDLE hJob, LPCTSTR lpszCmdLine[], DWORD nCount, BOOL bInheritHandles, DWORD dwCreationFlags)
{
	BOOL bInJob = FALSE;
	IsProcessInJob(GetCurrentProcess(), NULL, &bInJob);
	if (FALSE != bInJob) 
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 }; 
		QueryInformationJobObject(NULL, JobObjectBasicLimitInformation, &jobli, sizeof(jobli), NULL); 
		DWORD dwLimitMask = JOB_OBJECT_LIMIT_BREAKAWAY_OK | JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK; 
		if ((jobli.LimitFlags & dwLimitMask) == 0)
		{
			return FALSE;
		} 
	}
	TCHAR szCmdLine[MAX_PATH];
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	for (DWORD i = 0; i < nCount; ++i)
	{
		StringCchCopy(szCmdLine, MAX_PATH, lpszCmdLine[i]);
		BOOL bRet= CreateProcess(NULL, szCmdLine, NULL, NULL, bInheritHandles, dwCreationFlags| CREATE_SUSPENDED| CREATE_BREAKAWAY_FROM_JOB, NULL, NULL, &si, &pi);
		if (FALSE != bRet)
		{
			AssignProcessToJobObject(hJob, pi.hProcess);
			ResumeThread(pi.hThread);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		} 
	}
	return TRUE;
}

VOID CreateNewJob()
{
	TCHAR szFileName[1024] = TEXT("");
	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	ofn.hwndOwner = hWnd;
	ofn.hInstance = GetWindowInstance(hWnd);
	ofn.lpstrFilter = TEXT("Программы (*.exe)\0*.exe\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _countof(szFileName);
	ofn.lpstrTitle = TEXT("Выбор программ");
	ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	ofn.lpstrDefExt = TEXT("exe");
	if (GetOpenFileName(&ofn) != FALSE) 
	{
		BOOL bRet = FALSE;
		UINT nCount = 0;
		for (LPCTSTR p = szFileName; (*p) != 0; p += strlen(p) + 1) 
			nCount++;
		if (nCount-- > 1) 
		{
			LPCTSTR lpszName = szFileName + strlen(szFileName) + 1;
			LPTSTR *aCmdLine = new LPTSTR[nCount];
			for (UINT i = 0; i < nCount; ++i)
			{
				aCmdLine[i] = new TCHAR[MAX_PATH];
				StringCchPrintf(aCmdLine[i], MAX_PATH, TEXT("%s\\%s"), szFileName, lpszName);
				lpszName += strlen(lpszName) + 1;
			}
			bRet = StartGroupProcessesAsJob(job, (LPCTSTR *)aCmdLine, nCount, FALSE, 0);
			for (UINT i = 0; i < nCount; ++i) 
				delete[] aCmdLine[i];
			delete[] aCmdLine;
		} 
		else
		{
			LPCTSTR aCmdLine[1] = { szFileName };
			bRet = StartGroupProcessesAsJob(job, aCmdLine, 1, FALSE, 0);
		} 
		if (FALSE == bRet)
			MessageBox(hWnd, TEXT("Ошибка((("), TEXT("Ошибка"), MB_ICONERROR | MB_OK);
	} 
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		Exit(hWnd);
		return 0;
	case WM_QUIT:
		DestroyWindow(hWnd);
		return 0;
	case WM_CREATE:
		CreatingElements(handle, hWnd);
		return 0;
	case WM_COMMAND:
		SetWindowText(Answer, "   ");
		CHAR buffer[4];
		if (LOWORD(wParam) == PAUSE)
		{
			GetWindowText(Seconds, buffer, 4);
			DWORD time = atoi(buffer);
			if (time < 1)
				MessageBox(hWnd, TEXT("Некорректное число!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
			else
				Waiting(time);
		}
		if (LOWORD(wParam) == LOAD_PROCESS_BUTTON)
			LoadProcesses();
		if (HIWORD(wParam) == LBN_SELCHANGE)
			LoadProcessesModules();
		if (LOWORD(wParam) == TERMINATE_PROCESS)
		{
			if (IDYES == MessageBox(hWnd, TEXT("Действительно завершить данный процесс?"), TEXT("Вы уверены?"), MB_YESNO | MB_ICONQUESTION))
			{
				TerminateProc();
				LoadProcesses();
			}
		}
		if (PROCESS_RADIO1 <= LOWORD(wParam) && LOWORD(wParam) <= PROCESS_RADIO6)
			ProcessPriority(wParam);
		if (THREAD_RADIO1 <= LOWORD(wParam) && LOWORD(wParam) <= THREAD_RADIO7)
			ThreadPriority(wParam);
		if (LOWORD(wParam) == IAM_IN_TASK)
			TaskChecking(NULL);
		if (LOWORD(wParam) == PROCESSES_IN_JOB)
			if (job==NULL)
				MessageBox(hWnd, TEXT("Вы не создавали задания!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
			else
				TaskChecking(job);
		if (LOWORD(wParam) == NEW_JOB)
		{
			job = CreateJobObject(NULL, TEXT("Моё задание"));
			CreateNewJob();
		}
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WNDCLASSEX RegisterWindowClass(HINSTANCE hInstance) 
{
	WNDCLASSEX window = { sizeof(WNDCLASSEX) };
	window.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	window.lpfnWndProc = WindowProcedure;
	window.hInstance = hInstance;
	window.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	window.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	window.lpszMenuName = NULL;
	window.lpszClassName = TEXT("WindowClass");
	window.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	return window;
}

VOID MessagesProcessing() {
	MSG  msg;
	BOOL bRet;
	while (bRet=GetMessage(&msg, NULL, 0, 0) != FALSE)
	{
		if (bRet != -1)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpszCmdLine, int nCmdShow)
{
	handle = hInstance;
	if (!RegisterClassEx(&RegisterWindowClass(hInstance)))
		return EXIT_FAILURE;
	LoadLibrary(TEXT("ComCtl32.dll"));
	hWnd = CreateWindowEx(0, TEXT("WindowClass"), TEXT("Моя прога"), WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 1330, 530, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
		return EXIT_FAILURE;
	ShowWindow(hWnd, nCmdShow);
	MessagesProcessing();
	return EXIT_SUCCESS;
}