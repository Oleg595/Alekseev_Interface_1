#pragma once
#pragma warning (disable:4996)

#ifdef INTERFACE_H_EXPORTS
#define INTERFACE_H __declspec(dllexport)
#else
#define INTERFACE_H __declspec(dllimport)
#endif

#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<commdlg.h>

INTERFACE_H enum PAINT_MODE {
    ORDINARY,
    LAYOUT,
    NODATA
};

INTERFACE_H struct Str_data_t {//���������, ������� ������ ����� ������� � ���������� �������� ������
    int start_str;//����� �������, � �������� ���������� ������, � ����� ������� ��������
    int end_str;//����� �������, �� ������� �������������� ������, � ����� ������� ��������
};

INTERFACE_H struct data_t {
    char* str_data;//������ ���� ��������� ��������
    int cxChar, cyChar;//������� ��������
    int num_str;//���������� ��������� �����
    int num_layout_str;//���������� ����� � ������ ������
    struct Str_data_t* start_str;// ���������, �������� ������� ������ � ����� ������ ��������� ������
    int start_layout, start;//����������, �������� ������� ��� ������ ������ � ������ ������ � ��� ������ ������������ ������� ������
    int iVscrollPos, iLayoutVscrollPos;//������� ������� � ������� ������ � ������ ������ �� ���������
    int iHscrollPos;//������� ������� �� �����������
    int cxClient, cyClient;//������� ����
    int iVscrollMax, iVscrollInc;//������������ �������� ��� ������� � �������� �������� ������� ��� ��������������� ��������� �� ���������
    int iHscrollMax, iHscrollInc;//������������ �������� ��� ������� � �������� �������� ������� ��� ��������������� ��������� �� �����������
    int iMaxWidth;//���������� ������ ������
    int Sc_pos;//����������, ������������, �� ������� ����� ����� ���������� ��� ��������� �� 1 ������
    enum PAINT_MODE p_m;//����������, � ������� ������������ ����� ���������: ������ ��� ������
    OPENFILENAME* ofn;//��������� ��� ������ �� ����������� ��������� ����� Open
};

/**
*@brief �������, ����������� ������ � ����� � ������������ �� ��������� � data_t
*@param szFileName[] �������� �����, � �������� ����������� ������
*@param data ��������������� ���������, � ������� ������������ ���������� ������
*/

INTERFACE_H void Read(char szFileName[], struct data_t* data);

/**
*@brief ������� ��� ������� ������ ��������� data_t
*@param data ���� ���������, ������ ������� ����� ��������
*/

INTERFACE_H void Clear(struct data_t* data);

/**
*@brief �������, ��������� ���������� ����� � ������ ������
*@param start_str ������ ��������, � ������� ���������� ������ � ������ � ����� ������ ������ � ������� ������
*@param num_str ���������� �����
*@param size ������ ���� �� ��� x
*@return ���������� ����� � ������ ������ ��� ������� ������� ����
*/

INTERFACE_H int Layout_str(struct Str_data_t* start_str, int num_str, int size);

/**
*@brief �������, ������������ ��������� ������� �� ��� y ��� ������ ������
*@param start_str ������ ��������, � ������� ���������� ������ � ������ � ����� ������ ������ � ������� ������
*@param scroll_pos ���������� ������� ������� �� ��� y
*@param size ������ ���� �� ��� x
*@param num_str ���������� ����� � ������� ������
*@return ����� ��������� �������
*/

INTERFACE_H int Scroll_Pos(struct Str_data_t* start_str, int scroll_pos, int size, int num_str);

/**
*@brief �������, ��������������� ��������� ��������� OPENFILENAME
*@param hwnd ��������� ��������� ����������
*@param ofn ��������� ��� ������ �� ����������� ���������� ����� Open
*/

INTERFACE_H void PopFileInitialize(HWND hwnd, OPENFILENAME* ofn);

/**
*@brief �������, ��������� ����������� ���������� ���� "�������"
*@param hwnd ��������� ��������� ����������
*@param ofn ��������� ��� ������ �� ����������� ���������� ����� Open
*@param pstrFileName ������ ��� �������� �����
*@param pstrTitleName ��� �������� �����
*/

INTERFACE_H BOOL PopFileOpenDlg(HWND hwnd, OPENFILENAME* ofn, PSTR pstrFileName, PSTR pstrTitleName);
