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

INTERFACE_H struct Str_data_t {//структура, которая хранит номер первого и последнего элемента строки
    int start_str;//номер символа, с которого начинается строка, в общем массиве символов
    int end_str;//номер символа, на котором заканчиавается строка, в общем массиве символов
};

INTERFACE_H struct data_t {
    char* str_data;//массив всех считанных символов
    int cxChar, cyChar;//размеры символов
    int num_str;//количество считанных строк
    int num_layout_str;//количество строк в режиме вёрстки
    struct Str_data_t* start_str;// структура, хранящая индексы начала и конца каждой считанной строки
    int start_layout, start;//переменные, хранящая позицию для начала печати в режиме вёрстки и без вёрстки относительно текущей строки
    int iVscrollPos, iLayoutVscrollPos;//позиции бегунка в обычном режиме и режиме вёрстки по вертикали
    int iHscrollPos;//позиция бегунка по горизонтали
    int cxClient, cyClient;//размеры окна
    int iVscrollMax, iVscrollInc;//максимальное значение для бегунка и значение смещения бегунка при соответствующих действиях по вертикали
    int iHscrollMax, iHscrollInc;//максимальное значение для бегунка и значение смещения бегунка при соответствующих действиях по горизонтали
    int iMaxWidth;//наибольшая ширина строки
    int Sc_pos;//переменная, определяющая, на сколько строк нужно опуститься при прокрутке на 1 строку
    enum PAINT_MODE p_m;//переменная, в которой определяется режим отрисовки: обычны или вёрстки
    OPENFILENAME* ofn;//структура для работы со стандартным диалоовым окном Open
};

/**
*@brief Функция, считывающая данные с файла и записывающая их структуру в data_t
*@param szFileName[] название файла, с которого считываются данные
*@param data непосредственно структура, в которую записываются полученные данные
*/

INTERFACE_H void Read(char szFileName[], struct data_t* data);

/**
*@brief функция для очистки данных структуры data_t
*@param data сама структура, данные которой нужно очистить
*/

INTERFACE_H void Clear(struct data_t* data);

/**
*@brief функция, возвращая количество строк в режиме вёрстки
*@param start_str массив структур, в которых содержатся данные о начале и конце каждой строки в обычном режиме
*@param num_str количество строк
*@param size размер окна по оси x
*@return количество строк в режиме вёрстки для данного размера окна
*/

INTERFACE_H int Layout_str(struct Str_data_t* start_str, int num_str, int size);

/**
*@brief функция, возвращающая положение бегунка по оси y для режима вёрстки
*@param start_str массив структур, в которых содержатся данные о начале и конце каждой строки в обычном режиме
*@param scroll_pos предыдущая позиция бегунка по оси y
*@param size размер окна по оси x
*@param num_str количество строк в обычном режиме
*@return новое положение бегунка
*/

INTERFACE_H int Scroll_Pos(struct Str_data_t* start_str, int scroll_pos, int size, int num_str);

/**
*@brief функция, инициализиующая экземпляр структуры OPENFILENAME
*@param hwnd описатель контекста устройства
*@param ofn структура для работы со стандартным диалоговым окном Open
*/

INTERFACE_H void PopFileInitialize(HWND hwnd, OPENFILENAME* ofn);

/**
*@brief функция, создающая стандартное диалоговое окно "Открыть"
*@param hwnd описатель контекста устройства
*@param ofn структура для работы со стандартным диалоговым окном Open
*@param pstrFileName полное имя текущего файла
*@param pstrTitleName имя текущего файла
*/

INTERFACE_H BOOL PopFileOpenDlg(HWND hwnd, OPENFILENAME* ofn, PSTR pstrFileName, PSTR pstrTitleName);
