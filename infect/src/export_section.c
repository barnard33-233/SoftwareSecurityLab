#include <winnt.h>
#include <Windows.h>
#include <stdio.h>

char * usage = "usage: export_section file_name section_name\n";

int get_sec(char * pe_file, char* section, int * off, char ** buffer){
    char* _buffer;
    // TODO
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