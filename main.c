#include <Windows.h>

#include "syntax_analysis.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR szCmdLine,
	int iCmdShow
) {
	static TCHAR szClassName[] = TEXT("HelloWin");  //��������
	HWND     hwnd;  //���ھ��
	MSG      msg;  //��Ϣ
	WNDCLASS wndclass;  //������
	/**********�ڢٲ���ע�ᴰ����**********/
	//Ϊ������ĸ����ֶθ�ֵ
	wndclass.style = CS_HREDRAW | CS_VREDRAW;  //���ڷ��
	wndclass.lpfnWndProc = WndProc;  //���ڹ���
	wndclass.cbClsExtra = 0;  //��ʱ����Ҫ���
	wndclass.cbWndExtra = 0;  //��ʱ����Ҫ���
	wndclass.hInstance = hInstance;  //��ǰ���ھ��
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);  //����ͼ��
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);  //�����ʽ
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //���ڱ�����ˢ
	wndclass.lpszMenuName = NULL;  //���ڲ˵�
	wndclass.lpszClassName = szClassName;  //��������
	//ע�ᴰ��
	RegisterClass(&wndclass);
	/*****�ڢڲ�����������(���ô�����ʾ����)*****/
	hwnd = CreateWindow(
		szClassName,  //�����������
		TEXT("Welcome"),  //���ڱ��⣨�����ڱ�������
		WS_OVERLAPPEDWINDOW,  //���ڷ��
		CW_USEDEFAULT,  //��ʼ��ʱx���λ��
		CW_USEDEFAULT,  //��ʼ��ʱy���λ��
		1000,  //���ڿ��
		1000,  //���ڸ߶�
		NULL,  //�����ھ��
		NULL,  //���ڲ˵����
		hInstance,  //��ǰ���ڵľ��
		NULL  //��ʹ�ø�ֵ
	);
	//��ʾ����
	ShowWindow(hwnd, iCmdShow);
	//���£����ƣ�����
	UpdateWindow(hwnd);
	/**********�ڢ۲�����Ϣѭ��**********/
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);  //������Ϣ
		DispatchMessage(&msg);  //������Ϣ
	}
	return msg.wParam;
}
/**********�ڢܲ������ڹ���**********/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	char* file_name = "C:\\Users\\18253\\Desktop\\test.txt";
	switch (message) {
		//���ڻ�����Ϣ
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		Parser(file_name);
		EndPaint(hwnd, &ps);
		return 0;
		//����������Ϣ
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}