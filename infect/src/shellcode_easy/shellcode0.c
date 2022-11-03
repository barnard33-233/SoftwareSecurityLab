// compile:

#include <Windows.h>

#pragma section(".virus", read, execute)

__declspec(allocate(".virus")) int x;

#pragma code_seg(".virus")

__forceinline int in(){
    return 1;
}

__declspec(dllexport) int export_this(){
    x = in();
    return x;
}