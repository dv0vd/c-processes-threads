#include<iostream>
#include<Windows.h>
#include <time.h>
using namespace std;

CRITICAL_SECTION section;
INT rawsCount, columnsCount;
INT doubleDigitCount=0, max=0, min=999;

INT **Input()
{
	while (TRUE)
	{
		cout << "Количество строк: ";
		cin >> rawsCount;
		cout << "Количество столбцов: ";
		cin >> columnsCount;
		if ((rawsCount < 1) || (columnsCount < 1))
			cout << "Ошибка" << endl << "-----------------------" << endl;
		else
			break;
	}
	INT **matrix = new INT*[rawsCount];
	for (INT i = 0; i < rawsCount; i++)
		matrix[i] = new INT[columnsCount];
	return matrix;
}

VOID Output(INT **matrix)
{
	cout << endl << "Исходная матрица" << endl;
	for (INT i = 0; i < rawsCount; i++)
	{
		for (INT j = 0; j < columnsCount; j++)
		{
			cout.width(6);
			cout << matrix[i][j];
		}
		cout << endl;
	}
	cout << endl << "Количество двузначных чисел в матрице: " << doubleDigitCount;
	cout << endl << "Минимальное значение: " << min;
	cout << endl << "Максимальное значение: " << max << endl;
}

VOID Removing(INT **matrix, HANDLE *threadsMassive)
{
	for (int i = 0; i < rawsCount; i++)
	{
		CloseHandle(threadsMassive[i]);
		delete[]matrix[i];
	}
	delete[]matrix;
	delete[]threadsMassive;
}

DWORD CALLBACK ThreadsFunction(LPVOID lpParameter)
{
	INT *matrixRaw = (INT*)lpParameter;
	srand(time(nullptr));
	for (INT j=0; j<columnsCount; j++)
		matrixRaw[j] = rand() % 1000;
	INT tempDoubleDigitCount = 0, tempMax = matrixRaw[0], tempMin = matrixRaw[0];
	for (INT i = 0; i < columnsCount; i++)
	{
		if ((matrixRaw[i] > 9) && (matrixRaw[i] < 100))
			tempDoubleDigitCount++;
		if (matrixRaw[i] > tempMax)
			tempMax = matrixRaw[i];
		if (matrixRaw[i] < tempMin)
			tempMin = matrixRaw[i];
	}
	EnterCriticalSection(&section);
		doubleDigitCount += tempDoubleDigitCount;
		if (tempMin < min)
			min = tempMin;
		if (tempMax > max)
			max = tempMax;
	LeaveCriticalSection(&section);
	return 0;
}

HANDLE *ThreadsCreating(INT **matrix)
{
	HANDLE *threads = new HANDLE[rawsCount];
	for (INT i = 0; i < rawsCount; (i)++)
		threads[i] = CreateThread(NULL, 0, ThreadsFunction, matrix[i], 0, NULL);
	return threads;
}

INT main()
{
	INT **matrix = Input();
	InitializeCriticalSection(&section);
	HANDLE *threadsMassive = ThreadsCreating(matrix);
	WaitForMultipleObjects(rawsCount, threadsMassive, TRUE,INFINITE);
	DeleteCriticalSection(&section);
	Output(matrix);
	Removing(matrix, threadsMassive);
	system("pause");
}