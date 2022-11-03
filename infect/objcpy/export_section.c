#include <winnt.h>
#include <Windows.h>
#include <stdio.h>
#include <commdlg.h>

char * usage = "usage: export_section file_name section_name\n";

int get_sec(char * pe_file, char* section, int * off, char ** buffer){
    char* _buffer;
    // TODO
    char szFilePath[PATH]
    OPENFILENAME ofn;

    HANDLE hFile;// 文件句柄
    HANDLE hMapping;// 映射文件句柄
    LPVOID ImageBase;// 映射基址
 
    PIMAGE_DOS_HEADER  pDH = NULL;//指向IMAGE_DOS结构的指针
    PIMAGE_NT_HEADERS  pNtH = NULL;//指向IMAGE_NT结构的指针
    PIMAGE_FILE_HEADER pFH = NULL;//指向IMAGE_FILE结构的指针
    PIMAGE_OPTIONAL_HEADER pOH = NULL;//指向IMAGE_OPTIONALE结构的指针

    memset(szFilePath, 0, MAX_PATH);
    memset(&ofn, 0, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.hInstance = GetModuleHandle(NULL);
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = ".";
    ofn.lpstrFile = szFilePath;
    ofn.lpstrTitle = "选择 PE文件打开 by For";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrFilter = "*.exe\0*.exe\0";//过滤器
    
    //     if (!GetOpenFileName(&ofn))//调用打开对话框，选择要分析的文件
    // {
    //     MessageBox(NULL, "打开文件错误", NULL, MB_OK);
    //     return 0;
    // }
    //文件内核对象
    hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    //     if (!hFile)
    // {
    //     MessageBox(NULL, "打开文件错误", NULL, MB_OK);
    //     return 0;
    // }
    //文件映射内核对象
    hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    //     if (!hMapping)
    // {
    //     CloseHandle(hFile);
    //     return FALSE;
    // }
    //返回的映射基址
    ImageBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    // if (!ImageBase)
    // {
    //     CloseHandle(hMapping);
    //     CloseHandle(hFile);
    //     return FALSE;
    // }

    //IMAGE_DOS Header结构指针
    pDH = (PIMAGE_DOS_HEADER)ImageBase;
    //IMAGE_NT Header结构指针
    pNtH = (PIMAGE_NT_HEADERS)((DWORD)pDH + pDH->e_lfanew);
    //IMAGE_File Header结构指针
    pFH = &pNtH->FileHeader;
    //IMAGE_Optional Header结构指针
    pOH = &pNtH->OptionalHeader;
    
    printf("AddressOfEntryPoint:%08X\n", pOH->AddressOfEntryPoint);
    printf("SectionAlignment:   %08X\n", pOH->SectionAlignment);
    *buffer = _buffer;
    return 0;
}

int main(int argc, char ** argv){
    // argv[1]: file name
    // argv[2]: section name
    if(argc != 3){fputs(usage, stderr);}

    char * file = argv[1];
    char * section = argv[2];
    FILE * f = fopen(file, "r");
    if(!f){fputs(usage, stderr);}
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    char * pe_buffer = malloc(size);
    fread(pe_buffer, 1, size, f);
    fclose(f);

    char * file_name;
    char * delim = '.';
    file_name = strtok(file, delim);
    char * output = malloc(strlen(file_name) + strlen(section) + 2);
    strcpy(output, file_name);
    strcat(output, ".");
    strcat(output, section);

    char * section_data; // allocate in get_sec
    int off;
    int response = get_sec(pe_buffer, section, &off, section_data);

    printf("");

    free(pe_buffer);
    free(output);
    free(section_data);
    return  0;
}