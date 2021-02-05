#include"Interface.h"

int Layout_str(struct Str_data_t* start_str, int num_str, int size) {
    int num, i = 0;
    int count = 0;
    for (i = 0; i < num_str; i++) {
        num = start_str[i].end_str - start_str[i].start_str;//без учёта знака перевода строки
        if(num == 0){
            count++;
        }
        else{
            count += (num / size) + ((num % size > 0) ? 1 : 0);
        }
    }
    return count;
}

int Scroll_Pos(struct Str_data_t* start_str, int scroll_pos, int size, int num_str) {
    int pos = 0;
    int i = 0;
    if(num_str == 0){
        return pos;
    }
    for (; i < scroll_pos; i++) {
        if((start_str[i].end_str == start_str[i].start_str) && (i != num_str)){
            pos++;
        }
        else{
            pos += (start_str[i].end_str - start_str[i].start_str) / size + (((start_str[i].end_str - start_str[i].start_str) % size != 0) ? 1 : 0);
        }
    }
    return pos;
}

void Clear(struct data_t* data) {//функция для очистки данных структуры data_t. Используется при закрытии файла и программы. На вход принимает указатель на структуру data_t
    if (data->start_str != NULL) {
        free(data->start_str);
        data->start_str = NULL;
    }
    if (data->str_data != NULL) {
        free(data->str_data);
        data->str_data = NULL;
    }
    data->num_str = 0;
    data->num_layout_str = 0;
    data->iHscrollPos = 0;
    data->iVscrollPos = 0;
    data->iHscrollMax = 0;
    data->iVscrollMax = 0;
    data->iMaxWidth = 0;
}

void Read(char szFileName[], struct data_t* data) {//Функция, считывающая данные с файла и записывающая их структуру data_t, которая хранит данные окна. На вход принимает имя файла и указатель на структуру.
    FILE* file;
    int i, j, size;
    data->start_layout = 0;
    data->iHscrollPos = 0;
    file = fopen(szFileName, "r");
    if (file == NULL) {
        data->num_str = 0;
        data->iVscrollPos = 0;
        if(data->start_str != NULL){
            free(data->start_str);
        }
        data->start_str = (struct Str_data_t*)malloc(sizeof(struct Str_data_t));
        data->start_str[0].end_str = 0;
        data->start_str[0].start_str = 0;
        if(data->str_data != NULL){
            free(data->str_data);
        }
        data->str_data = (char*)malloc(sizeof(char));
        data->str_data[0] = '\0';
        data->num_str = 0;
        data->iHscrollMax = 0;
        data->iHscrollPos = 0;
        data->iVscrollMax = 0;
        data->num_layout_str = 0;
        data->iMaxWidth = 0;
        return;
    }
    fseek(file, 0, SEEK_END);
    size = (int)ftell(file);
    data->str_data = (char*)malloc(sizeof(char) * (size + 1));
    fseek(file, 0, SEEK_SET);
    size = fread(data->str_data, 1, size, file);
    j = 0;
    data->num_str = 1;
    data->iMaxWidth = 0;
    for (i = 0; i < size; i++) {
        j++;
        if ((data->str_data[i] == '\n')) {
            if (j > data->iMaxWidth) {
                data->iMaxWidth = j;
            }
            j = 0;
            data->num_str++;
        }
    }
    data->str_data[i] = '\0';
    data->Sc_pos = (data->num_str > pow(2, 16)) ? data->num_str / pow(2, 16) + 1 : 1;
    data->start_str = (struct Str_data_t*)malloc(sizeof(struct Str_data_t) * (data->num_str));
    data->start_str[0].start_str = 0;
    j = 0;
    for (i = 1; i < size; i++) {
        if (data->str_data[i] == '\n') {
            data->start_str[j].end_str = i;
            data->start_str[j + 1].start_str = i + 1;
            j++;
        }
    }
    data->iHscrollMax = max(0, data->iMaxWidth - data->cxClient / data->cxChar);
    data->iHscrollPos = min(data->iHscrollPos, data->iHscrollMax);
    data->start_str[j].end_str = size + 1;
    data->iVscrollMax = min(pow(2, 16), (max(0, data->num_str - data->cyClient / data->cyChar) / data->Sc_pos));
    if (data->p_m == LAYOUT) {
        data->iLayoutVscrollPos = 0;
        data->num_layout_str = Layout_str(data->start_str, data->num_str, data->cxClient / data->cxChar);
        data->iHscrollMax = 0;
        data->iVscrollMax = (int)(max(0, data->num_layout_str - data->cyClient / data->cyChar) / data->Sc_pos);
    }
}

void PopFileInitialize(HWND hwnd, OPENFILENAME* ofn) {
    static char szFilter[] = "Text Files(*.TXT)\0*.txt\0";
    ofn->lStructSize = sizeof(OPENFILENAME);
    ofn->hwndOwner = hwnd;
    ofn->hInstance = NULL;
    ofn->lpstrFilter = szFilter;
    ofn->lpstrCustomFilter = NULL;
    ofn->nMaxCustFilter = 0;
    ofn->nFilterIndex = 0;
    ofn->lpstrFile = NULL; // Set in Open and Close functions
    ofn->nMaxFile = _MAX_PATH;
    ofn->lpstrFileTitle = NULL; // Set in Open and Close functions
    ofn->nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
    ofn->lpstrInitialDir = NULL;
    ofn->lpstrTitle = NULL;
    ofn->Flags = 0; // Set in Open and Close functions
    ofn->nFileOffset = 0;
    ofn->nFileExtension = 0;
    ofn->lpstrDefExt = "txt";
    ofn->lCustData = 0L;
    ofn->lpfnHook = NULL;
    ofn->lpTemplateName = NULL;
}

BOOL PopFileOpenDlg(HWND hwnd, OPENFILENAME* ofn, PSTR pstrFileName, PSTR pstrTitleName) {
    ofn->hwndOwner = hwnd;
    ofn->lpstrFile = pstrFileName;
    ofn->lpstrFileTitle = pstrTitleName;
    ofn->Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
    return GetOpenFileName(&(*ofn));
}
