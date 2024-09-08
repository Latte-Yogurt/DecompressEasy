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

    // 獲取文件所在目錄
    GetFullPathNameA(filename, MAX_PATH, work_path, NULL);

    // 去掉文件名
    char* path = strrchr(work_path, '\\');

    if (path)
    {
        *path = '\0'; //將最後一個斜杠后的字符替換爲字符串結束符
    }

    free(path_copy);
    return work_path;
}

int IS_DIRECTORY(const char* path)
{
    DWORD attributes = GetFileAttributesA(path);

    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        return 0; // 不是文件夾
    }

    // 判斷是否是文件夾
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

const char* CHECK_FILE_TYPE(const char* filename)
{
    char* path_copy = strdup(filename);
    const char* dot = strrchr(path_copy, '.');

    if (!dot || dot == path_copy)
    {
        return ""; // 沒有後綴名
    }

    free(path_copy);
    return dot + 1; // 返回後綴名
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
            *dot = '\0'; // 用字符串結束符替換 '.'
        }
    }

    free(path_copy);
    return temp_file_name;
}

const char* GET_FOLDER_PATH(const char* filename)
{
    char* path_copy = strdup(filename);
    static char folder_path[MAX_PATH];

    // 獲取文件所在目錄
    GetFullPathNameA(filename, MAX_PATH, folder_path, NULL);

    // 去掉文件名
    char* path = strrchr(folder_path, '\\');
    if (path)
    {
        *path = '\0'; //將最後一個斜杠后的字符替換爲字符串結束符
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
    //printf("構造臨時批處理路徑爲：%s\n", bat_path);

    GET_BAT_PATH(bat_path);

    // 創建一個臨時批處理并執行
    FILE* batFile = fopen(bat_path, "w");

    if (batFile == NULL)
    {
        printf("出現問題，創建bat失敗。\n");
        system("pause");
        return 0;
    }

    fprintf(batFile, "%s", command);
    fclose(batFile);

    char final_path[256];
    sprintf(final_path, "\"%s\"", bat_path); // system執行cmd命令時加入雙引號以避免空格帶來的影響
    int result = system(final_path);

    if (result == 0)
    {
        printf("解壓成功。\n");
        remove(bat_path);
        return 0;
    }

    else
    {
        printf("解壓失敗，錯誤代碼為：%d。\n", result);
        remove(bat_path);
        return result;
    }
}

BOOL WINAPI ConsoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
    {
        // 刪除臨時文件
        if (remove(bat_path) == 0)
        {
            printf("臨時文件已成功刪除。\n");
        }

        else
        {
            printf("刪除臨時文件失敗或臨時文件不存在。\n");
        }
    }
    return TRUE; // 表示已經處理該事件
}

int main(int argc, char* argv[])
{
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);

    if (argc < 2)
    {
        printf("請將文件拖拽到此程式上以進行解壓。\n");
        system("pause");
        return 0;
    }

    // 獲取傳入的文件路徑
    const char* file = argv[1];
    //const char* file = "C:\\Users\\Mocha\\Desktop\\DepressEasy\\dez\\test.7z";
    char* file_copy = strdup(file);

    const char* dir = argv[0];
    char* dir_copy = strdup(dir);

    const char* work_path = GET_WORK_PATH(dir_copy);
    printf("dez主程式工作路徑為：%s\n", work_path);

    char seven_z_exe_path[256];
    char seven_z_dll_path[256];
    const char* seven_z_exe = "7z.exe";
    const char* seven_z_dll = "7z.dll";
    sprintf(seven_z_exe_path, "%s\\%s", work_path, seven_z_exe);
    sprintf(seven_z_dll_path, "%s\\%s", work_path, seven_z_dll);
    printf("7z主程式工作路徑為：%s\n\n", seven_z_exe_path);

    if (!IS_DIRECTORY(file_copy))
    {
        const char* temp_file_type = CHECK_FILE_TYPE(file_copy);
        char file_type[256];
        strcpy(file_type, temp_file_type);
        printf("文件類型為：%s\n", file_type);

        const char* temp_file_name = GET_FILE_NAME(file_copy);
        char file_name[256];
        strcpy(file_name, temp_file_name);
        printf("文件名稱為：%s\n", file_name);

        const char* temp_folder_path = GET_FOLDER_PATH(file_copy);
        char folder_path[256];
        strcpy(folder_path, temp_folder_path);
        printf("文件所在路徑為：%s\n\n", folder_path);

        if (_access(seven_z_exe_path, 0) == 0 && _access(seven_z_dll_path, 0) == 0)
        {
            if (strcmp(file_type, "") != 0)
            {
                // 生成7z.exe的解壓命令
                char command[256];
                char full_path[256];
                sprintf(full_path, "%s\\%s.%s", folder_path, file_name, file_type);
                //printf("構造路徑爲：%s\n", full_path);

                sprintf(command, "@echo off\n\"%s\" x -aos \"%s\" -o\"%s\"", seven_z_exe_path, full_path, folder_path);
                //printf("構造命令為：%s\n\n", command);

                EXECUTE_BAT(work_path, command);

                free(file_copy);
                free(dir_copy);
            }

            else
            {
                printf("未知文件類型。\n");
            }
        }

        else
        {
            printf("7z主程式不完整，檢查dez主程式工作路徑下是否存在7z.exe和7z.dll。\n");
        }
    }

    else
    {
        printf("傳入參數爲文件夾，無法進行解壓操作。\n");
    }

    system("pause");
    return 0;
}
