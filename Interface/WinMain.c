#include"Menu.h"
#include"Interface.h"
#include<windows.h>
#include<commdlg.h>
#include<stdio.h>
#include<stdlib.h>
#define EDITID 1
#define MAX_SIZE pow(2, 16)
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static HWND hDlgModeless;
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static char szAppName[] = "HelloWin";
    HWND hwnd;
    MSG msg;
    HACCEL hAccel;
    WNDCLASSEX wndclass;
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground =(HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = "Menu";
    wndclass.lpszClassName = szAppName;
    wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wndclass);
    hwnd = CreateWindow(szAppName, "System Metrics",
        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, szCmdLine);
    hInst = hInstance;
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);
    hAccel = LoadAccelerators(hInstance, szAppName);
    while(GetMessage(&msg, NULL, 0, 0)){
        if(hDlgModeless == NULL || !IsDialogMessage(hDlgModeless, &msg)){
            if(!TranslateAccelerator(hwnd, hAccel, &msg)){
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam){
    HDC hdc;
    static HMENU hMenu;
    static char szFileName[_MAX_PATH];
    static char szTitleName[_MAX_FNAME + _MAX_EXT];
    static HINSTANCE hInst;
    static HWND hwndEdit;
    static UINT iMsgFindReplace;
    RECT* lprect;
    int i, j, q, num;
    int size = 0;
    char szAppName[] = "PopMenu";//название меню
    static struct data_t data;
    int iPaintBeg, iPaintEnd, y, x, iPaintLeft, iPaintRight;
    CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    FILE* file;
    RECT rect;
    switch(iMsg)
    {
        case WM_CREATE:
            hInst =((LPCREATESTRUCT) lParam) -> hInstance;
            //создаём дочернее окно
                hwndEdit = CreateWindow("edit", NULL,
                WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                WS_BORDER | ES_LEFT | ES_MULTILINE |
                ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                0, 0, 0, 0,
                hwnd,(HMENU) EDITID, hInst, NULL);
            SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);
            data.ofn = (OPENFILENAME*)malloc(sizeof(OPENFILENAME));
            PopFileInitialize(hwnd, data.ofn);
            iMsgFindReplace = RegisterWindowMessage(FINDMSGSTRING);

            data.p_m = ORDINARY;
            hMenu = LoadMenu(hInst, szAppName);
            hMenu = GetSubMenu(hMenu, 0);
            hdc = GetDC(hwnd);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            GetTextMetrics(hdc, &tm);
            data.cxChar = tm.tmAveCharWidth;
            data.cyChar = tm.tmHeight + tm.tmExternalLeading;
            data.cxClient = LOWORD(lParam);//ширина окна
            data.cyClient = HIWORD(lParam);//высота окна
            ReleaseDC(hwnd, hdc);
            Read((char*)cs->lpCreateParams, &data);
            data.start = 0;
            return 0;
        case WM_SIZE :
            j = data.cxClient / data.cxChar;
            q = data.cyClient / data.cyChar;
            data.cxClient = LOWORD(lParam);//ширина окна
            data.cyClient = HIWORD(lParam);//высота окна
            if(data.p_m == ORDINARY){
                data.Sc_pos = (data.num_str > MAX_SIZE) ? data.num_str / MAX_SIZE + 1 : 1;
                data.iVscrollMax = min(MAX_SIZE, (max(0, data.num_str - data.cyClient / data.cyChar) / data.Sc_pos));
                data.iVscrollPos = (int)(min(data.iVscrollPos, data.iVscrollMax));
                SetScrollRange(hwnd, SB_VERT, 0, data.iVscrollMax, FALSE);
                SetScrollPos(hwnd, SB_VERT, data.iVscrollPos, TRUE);
                data.iHscrollMax = max(0, data.iMaxWidth - data.cxClient / data.cxChar);
                data.iHscrollPos = min(data.iHscrollPos, data.iHscrollMax);
                SetScrollRange(hwnd, SB_HORZ, 0, data.iHscrollMax, FALSE);
                SetScrollPos(hwnd, SB_HORZ, data.iHscrollPos, TRUE);
                if((data.iVscrollPos == data.iVscrollMax) && (data.iVscrollMax != 0)){
                    data.start = data.num_str - (data.iVscrollPos * data.Sc_pos) - (data.cyClient / data.cyChar);
                }
            }
            if(data.p_m == LAYOUT){
                data.iHscrollMax = 0;
                SetScrollRange(hwnd, SB_HORZ, 0, data.iHscrollMax, FALSE);
                data.num_layout_str = Layout_str(data.start_str, data.num_str, data.cxClient / data.cxChar);
                data.Sc_pos = (data.num_layout_str > MAX_SIZE) ? data.num_layout_str / MAX_SIZE + 1 : 1;
                data.iVscrollMax = (int)(max(0, data.num_layout_str - data.cyClient / data.cyChar) / data.Sc_pos);
                data.iLayoutVscrollPos = (Scroll_Pos(data.start_str, data.iVscrollPos, data.cxClient / data.cxChar, data.num_str) + (data.start_layout / (data.cxClient / data.cxChar))) / data.Sc_pos;
                SetScrollRange(hwnd, SB_VERT, 0, data.iVscrollMax, TRUE);
                ScrollWindow(hwnd, 0, -data.cyChar * data.iVscrollInc * data.Sc_pos, NULL, NULL);
                if(data.iVscrollMax == 0){
                    data.iVscrollPos = 0;
                    data.start_layout = 0;
                }
                if((data.iLayoutVscrollPos >= data.iVscrollMax) && (data.num_str != 0) && (data.iVscrollMax != 0) && ((j < data.cxClient / data.cxChar) ||
                                                                        ((data.cxClient / data.cxChar == j) && (q < data.cyClient / data.cyChar)))){
                    data.iVscrollPos = data.num_str;
                    data.start_layout = 0;
                    for(i = data.cyClient / data.cyChar; i > 0; i--){
                        if(data.start_layout >= data.cxClient / data.cxChar){
                            data.start_layout -= data.cxClient / data.cxChar;
                        }
                        else{
                            data.iVscrollPos--;
                            data.start_layout = data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str;// -
                            //((data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str) % (data.cxClient / data.cxChar));
                            if((data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str) % (data.cxClient / data.cxChar) == 0){
                                data.start_layout -= data.cxClient / data.cxChar;
                            }
                            else{
                                data.start_layout -= (data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str)
                                 % (data.cxClient / data.cxChar);
                            }
                        }
                    }
                    SetScrollPos(hwnd, SB_VERT, data.iLayoutVscrollPos, TRUE);
                    InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
                }
                SetScrollPos(hwnd, SB_VERT, data.iLayoutVscrollPos, TRUE);
                UpdateWindow(hwnd);
            }
            return 0;
        case WM_VSCROLL :
            switch(LOWORD(wParam))
            {
                case SB_TOP :
                    if(data.p_m == ORDINARY){
                        data.iVscrollInc = -data.iVscrollPos;
                    }
                    if(data.p_m == LAYOUT){
                        data.iVscrollInc = -data.iLayoutVscrollPos;
                    }
                    break;
                case SB_BOTTOM :
                    if(data.p_m == ORDINARY){
                        data.iVscrollInc = data.iVscrollMax - data.iVscrollPos;
                    }
                    if(data.p_m == LAYOUT){
                        data.iVscrollInc = data.iVscrollMax - data.iLayoutVscrollPos;
                    }
                    break;
                case SB_LINEUP :
                    data.iVscrollInc = -1;
                    break;
                case SB_LINEDOWN :
                    data.iVscrollInc = 1;
                    break;
                case SB_PAGEUP :
                    data.iVscrollInc = min(-1, -data.cyClient / data.cyChar);
                    break;
                case SB_PAGEDOWN :
                    data.iVscrollInc = max(1, data.cyClient / data.cyChar);
                    break;
                case SB_THUMBTRACK :
                    if(data.p_m == ORDINARY){
                        data.iVscrollInc = HIWORD(wParam) - data.iVscrollPos;
                    }
                    if(data.p_m == LAYOUT){
                        data.iVscrollInc = HIWORD(wParam) - data.iLayoutVscrollPos;
                    }
                    break;
                default :
                    data.iVscrollInc = 0;
            }
            if(data.p_m == ORDINARY){
                data.iVscrollInc = max(
                -data.iVscrollPos,
                min(data.iVscrollInc, data.iVscrollMax - data.iVscrollPos)
                );
                if((data.iVscrollPos + data.iVscrollInc == 0) && (data.iVscrollInc != 0) && (data.start != 0)){
                    data.start = 0;
                    data.iVscrollPos = 0;
                    SetScrollPos(hwnd, SB_VERT, data.iVscrollPos, TRUE);
                    InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
                }
                if((data.iVscrollPos + data.iVscrollInc == data.iVscrollMax) && (data.iVscrollInc != 0)){
                    data.iVscrollPos = data.iVscrollMax;
                    data.start = (data.num_str - data.start - (data.iVscrollPos * data.Sc_pos) - (data.cyClient / data.cyChar)) % data.Sc_pos;
                    SetScrollPos(hwnd, SB_VERT, data.iVscrollPos, TRUE);
                    InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
                }
            }
            if(data.p_m == LAYOUT){
                data.iVscrollInc = max(
                -data.iLayoutVscrollPos,
                min(data.iVscrollInc, data.iVscrollMax - data.iLayoutVscrollPos)
                );
                if((data.iLayoutVscrollPos + data.iVscrollInc == 0) && ((data.iVscrollPos > 0) || (data.start_layout > 0))){
                    data.iVscrollPos = 0;
                    data.start_layout = 0;
                    data.iLayoutVscrollPos = 0;
                    SetScrollPos(hwnd, SB_VERT, data.iLayoutVscrollPos, TRUE);
                    InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
                }
                if((data.iLayoutVscrollPos + data.iVscrollInc == data.iVscrollMax) && (data.num_layout_str - data.iLayoutVscrollPos
                    * data.Sc_pos > data.cyClient / data.cyChar) && (data.iVscrollInc != 0)){
                    data.iVscrollPos = data.num_str;
                    data.start_layout = 0;
                    for(i = 0; i < data.cyClient / data.cyChar; i++){
                        if(data.start_layout - data.cxClient / data.cxChar < 0){
                            data.iVscrollPos--;
                            if(data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str >= data.cxClient / data.cxChar){
                                if((data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str)
                                    % (data.cxClient / data.cxChar) != 0){
                                    data.start_layout = data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str
                                     - ((data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str)
                                         % (data.cxClient / data.cxChar));
                                }
                                else{
                                    data.start_layout = data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str
                                     - data.cxClient / data.cxChar;
                                }
                            }
                            else{
                                data.start_layout = 0;
                            }
                        }
                        else{
                            data.start_layout -= data.cxClient / data.cxChar;
                        }
                    }
                    data.iLayoutVscrollPos = data.iVscrollMax;
                    SetScrollPos(hwnd, SB_VERT, data.iLayoutVscrollPos, TRUE);
                    InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
                }
            }
            if (data.iVscrollInc != 0)
            {
                if(data.p_m == ORDINARY){
                    data.iVscrollPos += data.iVscrollInc;
                    ScrollWindow(hwnd, 0, -data.cyChar * data.iVscrollInc * data.Sc_pos, NULL, NULL);
                    SetScrollPos(hwnd, SB_VERT, data.iVscrollPos, TRUE);
                }
                if(data.p_m == LAYOUT){
                    j = data.iVscrollInc * data.Sc_pos;
                    if(j > 0){
                        while(j != 0){
                            if(data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str <= data.start_layout + data.cxClient / data.cxChar){
                                data.iVscrollPos++;
                                data.start_layout = 0;
                            }
                            else{
                                data.start_layout += data.cxClient / data.cxChar;
                            }
                            j--;
                        }
                        data.iLayoutVscrollPos += data.iVscrollInc;
                    }
                    else{
                        while((j != 0) && (data.iVscrollPos + data.start_layout != 0)){
                            if(data.start_layout - data.cxClient / data.cxChar < 0){
                                data.iVscrollPos--;
                                if(data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str >= data.cxClient / data.cxChar){
                                    if((data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str) % (data.cxClient / data.cxChar) != 0){
                                    data.start_layout = data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str - ((data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str) % (data.cxClient / data.cxChar));
                                    }
                                    else{
                                        data.start_layout = data.start_str[data.iVscrollPos].end_str - data.start_str[data.iVscrollPos].start_str - data.cxClient / data.cxChar;
                                    }
                                }
                                else{
                                    data.start_layout = 0;
                                }
                            }
                            else{
                                data.start_layout -= data.cxClient / data.cxChar;
                            }
                            j++;
                        }
                        data.iLayoutVscrollPos += data.iVscrollInc - j / data.Sc_pos;
                    }
                    ScrollWindow(hwnd, 0, -data.cyChar * (data.iVscrollInc * data.Sc_pos - j), NULL, NULL);
                    SetScrollPos(hwnd, SB_VERT, data.iLayoutVscrollPos, TRUE);
                }
                hdc = BeginPaint(hwnd, &ps);
                lprect = &ps.rcPaint;
                lprect->top -= (lprect->top % data.cyChar + 1);
                EndPaint(hwnd, &ps);
                InvalidateRect(hwnd, lprect, TRUE);
            }
            return 0;
        case WM_HSCROLL :
        switch(LOWORD(wParam))
        {
        case SB_LINEUP :
            data.iHscrollInc = -1;
            break;
        case SB_LINEDOWN :
            data.iHscrollInc = 1;
            break;
        case SB_PAGEUP :
            data.iHscrollInc = -8;
            break;
        case SB_PAGEDOWN :
            data.iHscrollInc = 8;
            break;
        case SB_THUMBTRACK :
            data.iHscrollInc = HIWORD(wParam) - data.iHscrollPos;
            break;
        default :
            data.iHscrollInc = 0;
        }
        data.iHscrollInc = max(
        -data.iHscrollPos,
        min(data.iHscrollInc, data.iHscrollMax - data.iHscrollPos)
        );
        if (data.iHscrollInc != 0)
        {
        data.iHscrollPos += data.iHscrollInc;
        ScrollWindow(hwnd, -data.cxChar * data.iHscrollInc, 0, NULL, NULL);
        SetScrollPos(hwnd, SB_HORZ, data.iHscrollPos, TRUE);
        hdc = BeginPaint(hwnd, &ps);
        lprect = &ps.rcPaint;
        lprect->left -= (lprect->left % data.cxChar + data.cxChar);
        EndPaint(hwnd, &ps);
        InvalidateRect(hwnd, lprect, TRUE);
        }
        return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rect);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            GetTextMetrics(hdc, &tm);
            data.cxChar = tm.tmAveCharWidth;
            data.cyChar = tm.tmHeight + tm.tmExternalLeading;
            switch(data.p_m){
                case ORDINARY:
                    if(ps.rcPaint.left % data.cxChar != 0){
                        lprect = &ps.rcPaint;
                        lprect->left -= (lprect->left % data.cxChar + data.cxChar);
                        EndPaint(hwnd, &ps);
                        InvalidateRect(hwnd, lprect, TRUE);
                    }
                    iPaintBeg = max(0, data.iVscrollPos * data.Sc_pos + data.start + ps.rcPaint.top / data.cyChar);
                    iPaintEnd = min(data.num_str, data.iVscrollPos * data.Sc_pos + data.start + ps.rcPaint.bottom / data.cyChar);
                    iPaintLeft = ps.rcPaint.left / data.cxChar + data.iHscrollPos;
                    iPaintRight = ps.rcPaint.right / data.cxChar + data.iHscrollPos;
                    for(i = iPaintBeg; (i <= iPaintEnd) && (i < data.num_str); i++){
                        y = data.cyChar * (i - (data.iVscrollPos * data.Sc_pos + data.start));
                        TextOut(hdc, ps.rcPaint.left, y, data.str_data + data.start_str[i].start_str + iPaintLeft,
                                 min(data.start_str[i].end_str - data.start_str[i].start_str - iPaintLeft, iPaintRight - iPaintLeft + 1));
                    }
                    EndPaint(hwnd, &ps);
                    break;
                case LAYOUT:
                    iPaintBeg = max(0, ps.rcPaint.top / data.cyChar - 1);
                    iPaintEnd = ps.rcPaint.bottom / data.cyChar;
                    num = data.start_layout;
                    j = 0;
                    q = data.iVscrollPos;
                    while(j < iPaintBeg){
                        if(data.start_str[q].end_str - data.start_str[q].start_str - num > data.cxClient / data.cxChar){
                            num += data.cxClient / data.cxChar;
                        }
                        else{
                            q++;
                            num = 0;
                        }
                        j++;
                    }
                    for(i = iPaintBeg; (i <= iPaintEnd) && (q < data.num_str); i++){
                        y = data.cyChar * i;
                        TextOut(hdc, 0, y, data.str_data + data.start_str[q].start_str + num,
                                min(data.start_str[q].end_str - data.start_str[q].start_str - num, data.cxClient / data.cxChar));
                        if(data.start_str[q].end_str - data.start_str[q].start_str - num > data.cxClient / data.cxChar){
                            num += data.cxClient / data.cxChar;
                        }
                        else{
                            q++;
                            num = 0;
                        }
                    }
                    EndPaint(hwnd, &ps);
                    break;
                case NODATA:
                    EndPaint(hwnd, &ps);
                    break;
            }
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            switch(wParam){
            case VK_UP:
                SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0L);
                break;
            case VK_DOWN:
                SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
                break;
            case VK_LEFT:
                SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, 0L);
                break;
            case VK_RIGHT:
                SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, 0L);
                break;
            case VK_PRIOR:
                SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0L);
                break;
            case VK_NEXT:
                SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0L);
                break;
            }
            return 0;

    case WM_COMMAND :
        hMenu = GetMenu(hwnd);
        switch(LOWORD(wParam)){
        case IDM_OPEN:
            if(PopFileOpenDlg(hwnd, data.ofn, szFileName, szTitleName)){
                if(data.start_str != NULL){
                    free(data.start_str);
                    data.start_str = NULL;
                }
                if(data.str_data != NULL){
                    free(data.str_data);
                    data.str_data = NULL;
                }
                data.num_str = 0;
                data.num_layout_str = 0;
                data.iHscrollPos = 0;
                data.iVscrollPos = 0;
                Read(szFileName, &data);
            }
            SetScrollRange(hwnd, SB_VERT, 0, data.iVscrollMax, TRUE);
            SetScrollPos(hwnd, SB_VERT, 0, TRUE);
            SetScrollRange(hwnd, SB_HORZ, 0, data.iHscrollMax, TRUE);
            SetScrollPos(hwnd, SB_HORZ, data.iHscrollPos, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        case IDM_CLOSE:
            Clear(&data);
            InvalidateRect(hwnd, NULL, TRUE);
            SendMessage(hwnd, WM_SIZE, 0, data.cyClient);
            return 0;
        case IDM_EXIT:
            Clear(&data);
            SendMessage(hwnd, WM_DESTROY, 0, 0L);
            return 0;
        case IDM_ORDINARY_MODE:
            EnableMenuItem(hMenu, IDM_ORDINARY_MODE, MF_GRAYED);
            EnableMenuItem(hMenu, IDM_LAYOUT_MODE, MF_ENABLED);
            data.p_m = ORDINARY;
            data.Sc_pos = (data.num_str > MAX_SIZE) ? data.num_str / MAX_SIZE + 1 : 1;
            data.start = data.iVscrollPos % data.Sc_pos;
            data.iVscrollPos = data.iVscrollPos / data.Sc_pos;
            data.iVscrollMax = (int)(max(0, data.num_str - data.cyClient / data.cyChar) / data.Sc_pos);
            SetScrollRange(hwnd, SB_VERT, 0, data.iVscrollMax, TRUE);
            SetScrollPos(hwnd, SB_VERT, data.iVscrollPos, TRUE);
            data.iHscrollMax = max(0, data.iMaxWidth - data.cxClient / data.cxChar);
            data.iHscrollPos = 0;
            SetScrollRange(hwnd, SB_HORZ, 0, data.iHscrollMax, TRUE);
            SetScrollPos(hwnd, SB_HORZ, data.iHscrollPos, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        case IDM_LAYOUT_MODE:
            EnableMenuItem(hMenu, IDM_ORDINARY_MODE, MF_ENABLED);
            EnableMenuItem(hMenu, IDM_LAYOUT_MODE, MF_GRAYED);
            data.p_m = LAYOUT;
            data.iVscrollPos = data.iVscrollPos * data.Sc_pos + data.start;
            data.iLayoutVscrollPos = Scroll_Pos(data.start_str, data.iVscrollPos, data.cxClient / data.cxChar, data.num_str);
            data.start_layout = 0;
            data.num_layout_str = Layout_str(data.start_str, data.num_str, data.cxClient / data.cxChar);
            data.Sc_pos = (data.num_layout_str > MAX_SIZE) ? data.num_layout_str / MAX_SIZE + 1 : 1;
            data.iLayoutVscrollPos /= data.Sc_pos;
            data.iVscrollMax = (int)(max(0, data.num_layout_str - data.cyClient / data.cyChar) / data.Sc_pos);
            SetScrollRange(hwnd, SB_VERT, 0, data.iVscrollMax, TRUE);
            SetScrollPos(hwnd, SB_VERT, data.iLayoutVscrollPos, TRUE);
            data.iHscrollMax = 0;
            SetScrollRange(hwnd, SB_HORZ, 0, data.iHscrollMax, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        default:
            break;
        }
        break;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
