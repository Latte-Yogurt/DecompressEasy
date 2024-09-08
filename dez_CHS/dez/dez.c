#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <windows.h>

char bat_path[256];

void GET_BAT_PATH(const char* filepath)
{
    strcpy(bat_path, filepath);
}

const char* GET_WORK_PATH(const char* filename)
{
    char* path_copy = strdup(filename);
    static char work_path[MAX_PATH];

    // 获取文件所在目录
    GetFullPathNameA(filename, MAX_PATH, work_path, NULL);

    // 去掉文件名
    char* path = strrchr(work_path, '\\');

    if (path)
    {
        *path = '\0'; // 将最后一个斜杠后的字符替换为字符串结束符
    }

    free(path_copy);
    return work_path;
}

int IS_DIRECTORY(const char* path)
{
    DWORD attributes = GetFileAttributesA(path);

    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        return 0; // 不是文件夹
    }

    // 判断是否是文件夹
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

const char* CHECK_FILE_TYPE(const char* filename)
{
    char* path_copy = strdup(filename);
    const char* dot = strrchr(path_copy, '.');

    if (!dot || dot == path_copy)
    {
        return ""; // 没有后缀名
    }

    free(path_copy);
    return dot + 1; // 返回后缀名
}

const char* GET_FILE_NAME(const char* filename)
{
    char* path_copy = strdup(filename);
    const char* temp_file_name = strrchr(path_copy, '\\');

    if (temp_file_name)
    {
        temp_file_name++;
    }

    if (temp_file_name != NULL)
    {
        char* dot = strrchr(temp_file_name, '.');

        if (dot)
        {
            *dot = '\0'; // 用字符串结束符替换 '.'
        }
    }

    free(path_copy);
    return temp_file_name;
}

const char* GET_FOLDER_PATH(const char* filename)
{
    char* path_copy = strdup(filename);
    static char folder_path[MAX_PATH];

    // 获取文件所在目录
    GetFullPathNameA(filename, MAX_PATH, folder_path, NULL);

    // 去掉文件名
    char* path = strrchr(folder_path, '\\');
    if (path)
    {
        *path = '\0'; // 将最后一个斜杠后的字符替换为字符串结束符
    }

    free(path_copy);
    return folder_path;
}

int EXECUTE_BAT(const char* work_path, const char* command)
{
    char bat_path[256];
    srand((unsigned int)time(NULL));
    int rand_num = rand() % 100000;

    sprintf(bat_path, "%s\\%d.bat", work_path, rand_num);
    //printf("构造临时批处理路径为：%s\n", bat_path);

    GET_BAT_PATH(bat_path);

    // 创建一个临时批处理并执行
    FILE* batFile = fopen(bat_path, "w");

    if (batFile == NULL)
    {
        printf("出现问题，创建bat失败。\n");
        system("pause");
        return 0;
    }

    fprintf(batFile, "%s", command);
    fclose(batFile);

    char final_path[256];
    sprintf(final_path, "\"%s\"", bat_path); // system执行cmd命令时加入双引号以避免空格带来的影响
    int result = system(final_path);

    if (result == 0)
    {
        printf("解压成功。\n");
        remove(bat_path);
        return 0;
    }

    else
    {
        printf("解压失败，错误代码为：%d。\n", result);
        remove(bat_path);
        return result;
    }
}

BOOL WINAPI ConsoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
    {
        // 删除临时文件
        if (remove(bat_path) == 0)
        {
            printf("临时文件已成功删除。\n");
        }

        else
        {
            printf("删除临时文件失败或临时文件不存在。\n");
        }
    }
    return TRUE; // 表示已经处理该事件
}

int main(int argc, char* argv[])
{
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    if (argc < 2)
    {
        printf("请将文件拖拽到此程序上以进行解压。\n");
        system("pause");
        return 0;
    }

    // 获取传入的文件路径
    const char* file = argv[1];
    //const char* file = "C:\\Users\\Mocha\\Desktop\\DepressEasy\\dez\\test.7z";
    char* file_copy = strdup(file);

    const char* dir = argv[0];
    char* dir_copy = strdup(dir);

    const char* work_path = GET_WORK_PATH(dir_copy);
    printf("dez主程序工作路径为：%s\n", work_path);

    char seven_z_exe_path[256];
    char seven_z_dll_path[256];
    const char* seven_z_exe = "7z.exe";
    const char* seven_z_dll = "7z.dll";
    sprintf(seven_z_exe_path, "%s\\%s", work_path, seven_z_exe);
    sprintf(seven_z_dll_path, "%s\\%s", work_path, seven_z_dll);
    printf("7z主程序工作路径为：%s\n\n", seven_z_exe_path);

    if (!IS_DIRECTORY(file_copy))
    {
        const char* temp_file_type = CHECK_FILE_TYPE(file_copy);
        char file_type[256];
        strcpy(file_type, temp_file_type);
        printf("文件类型为：%s\n", file_type);

        const char* temp_file_name = GET_FILE_NAME(file_copy);
        char file_name[256];
        strcpy(file_name, temp_file_name);
        printf("文件名称为：%s\n", file_name);

        const char* temp_folder_path = GET_FOLDER_PATH(file_copy);
        char folder_path[256];
        strcpy(folder_path, temp_folder_path);
        printf("文件所在路径为：%s\n\n", folder_path);

        if (_access(seven_z_exe_path, 0) == 0 && _access(seven_z_dll_path, 0) == 0)
        {
            if (strcmp(file_type, "") != 0)
            {
                // 生成7z.exe的解压命令
                char command[256];
                char full_path[256];
                sprintf(full_path, "%s\\%s.%s", folder_path, file_name, file_type);
                //printf("构造路径为：%s\n", full_path);

                sprintf(command, "@echo off\n\"%s\" x -aos \"%s\" -o\"%s\"", seven_z_exe_path, full_path, folder_path);
                //printf("构造命令为：%s\n\n", command);

                EXECUTE_BAT(work_path, command);

                free(file_copy);
                free(dir_copy);
            }

            else
            {
                printf("未知文件类型。\n");
            }
        }

        else
        {
            printf("7z主程序不完整，检查dez主程序工作路径下是否存在7z.exe和7z.dll。\n");
        }
    }

    else
    {
        printf("传入参数为文件夹，无法进行解压操作。\n");
    }

    system("pause");
    return 0;
}
