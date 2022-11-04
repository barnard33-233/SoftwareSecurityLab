# design

## 流程

1. 要修改的地方

头：
NumberOfSections, 
AddressOfEntryPoint, 
SizeOfImage

节表

新节

## 任务分解

TODO

1. parse

   解析 PE 文件

   需要获取：

   AddressOfEntryPoint, 节表剩余空间，剩余空间的首地址
2. 修改
3. 载荷


## 说明

文件 3个

1. virus: 注入程序
2. shellcode1: 注入的程序段
3. objcpy: 将pe文件目标段和入口copy出来并转换成virus可读的C代码

## 编译

1. virus
```shell
# pwd : infetct/
cmake -S ./virus -B "build_virus"
devenv ./build_virus/virus.sln /build
```

2. shellcode1
```shell
cmake -S ./shellcode1 -A Win32 -B "build_shellcode1"
devenv ./build_shellcode1/shellcode1.sln /build
```

3. export_sections.c

## 使用

1. virus

2. 

