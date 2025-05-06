#include <unistd.h>  // 包含了许多系统调用函数，如 fork、getpid、getppid、sleep 等
#include <sys/types.h>  // 定义了许多系统数据类型，如 pid_t
#include <errno.h>  // 用于处理错误信息
#include <sys/wait.h>  // 提供了 wait 等进程等待相关的函数
#include <stdlib.h>  // 包含了标准库函数，如 exit
#include <stdio.h>  // 包含标准输入输出函数，如 printf、scanf

int main() {
    // 定义一个 pid_t 类型的变量 childpid，用于存储 fork 函数的返回值
    pid_t childpid; 
    // 定义一个整型变量 retval，用于存储用户输入的子进程退出值
    int retval;    
    // 定义一个整型变量 status，用于存储子进程的退出状态
    int status;    

    // 调用 fork 函数创建一个新的子进程，fork 函数会返回两次
    childpid = fork();
    // 判断 fork 函数的返回值是否大于等于 0，如果是则表示 fork 成功
    if (childpid >= 0) { 
        // 如果 childpid 等于 0，说明当前处于子进程中
        if (childpid == 0) { 
            // 输出子进程的提示信息
            printf("CHILD: I am the child process! \n");
            // 输出子进程的进程 ID
            printf("CHILD: Here's my PID: %d\n", getpid());
            // 输出子进程的父进程 ID
            printf("CHILD: My parent's PID is: %d\n", getppid());
            // 输出 fork 函数在子进程中的返回值
            printf("CHILD: The value of fork return is: %d\n", childpid);
            // 提示子进程将休眠 1 秒
            printf("CHILD: Sleep for 1 second...\n");
            // 子进程休眠 1 秒
            sleep(1);
            // 提示用户输入一个 0 到 255 之间的退出值
            printf("CHILD: Enter an exit value (0~255): ");
            // 读取用户输入的退出值
            scanf("%d", &retval);
            // 输出子进程即将退出的提示信息
            printf("CHILD: Goodbye! \n");
            // 子进程以用户输入的退出值退出
            exit(retval); 
        } else { 
            // 输出父进程的提示信息
            printf("PARENT: I am the parent process! \n");
            // 输出父进程的进程 ID
            printf("PARENT: Here's my PID: %d\n", getpid());
            // 输出子进程的进程 ID
            printf("PARENT: The value of my child's PID is: %d\n", childpid);
            // 提示父进程将等待子进程退出
            printf("PARENT: I will now wait for my child to exit.\n");
            // 父进程调用 wait 函数等待子进程退出，并将子进程的退出状态存储在 status 中
            wait(&status); 
            // 输出子进程的退出码
            printf("PARENT: Child's exit code is: %d\n", WEXITSTATUS(status));
            // 输出父进程即将退出的提示信息
            printf("PARENT: Goodbye! \n");
            // 父进程以退出码 0 退出
            exit(0); 
        }
    } else { 
        // 如果 fork 函数返回 -1，说明 fork 失败，输出错误信息
        perror("fork error!"); 
        // 程序以退出码 0 退出
        exit(0);
    }
}