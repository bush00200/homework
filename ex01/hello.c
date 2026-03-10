#include<stdio.h>
#include <string.h>
// ==========================================
// 1.input 函数 
// ==========================================
void input(const char* prompt, char* output_buffer, int buffer_size) {
    // 显示提示语
    printf("%s", prompt);
    fflush(stdout); 

    // 安全读取一行 (支持中文、空格)
    if (fgets(output_buffer, buffer_size, stdin) != NULL) {
        // 去掉末尾的换行符，让输出更干净
        output_buffer[strcspn(output_buffer, "\n")] = '\0';
        
        // 【关键步骤】：显示输出键盘输入的内容
        printf(">> 你输入了: %s\n", output_buffer);
    }
}

int main(){
    printf("Hello, World!\n");
    char text[512];
    input("请输入内容 (支持中文/空格): ", text, sizeof(text));
    return 0;
}