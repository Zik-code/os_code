#include <stdio.h>  // 包含标准输入输出函数库头文件，用于printf、scanf等函数
#include <stdlib.h> // 包含标准库函数头文件，用于malloc、free等内存管理函数

// 定义宏getpch，用于分配指定类型的内存空间并返回指向该空间的指针
#define getpch(type) (type*)malloc(sizeof(type)) 

// 定义进程控制块PCB结构体
// 进程控制块用于存储进程的相关信息，是操作系统管理进程的重要数据结构
struct pcb { 
    char name[10]; // 进程名，最多可存储10个字符，包含字符串结束符'\0'
    char state;    // 进程状态：'W'-就绪态，'R'-运行态
    int nice;      // 进程优先级，数值越大表示优先级越高
    float ntime;   // 需要运行时间，修改为float类型以支持小数输入
    float rtime;   // 已经运行的时间，修改为float类型
    struct pcb *link; // 指向下一个PCB结构体的指针，用于构建链表，形成就绪队列
} *ready = NULL, *p; 

// 为struct pcb定义别名PCB，方便后续使用
typedef struct pcb PCB; 

// 函数声明
// 这些函数声明提前告知编译器函数的名称、参数和返回值类型
PCB* sort(); // 对进程进行优先级排列函数，优先数大者优先
void input(); // 建立进程控制函数，用于输入进程信息并将进程加入就绪队列
int space();  // 计算就绪队列中进程数量的函数
void disp(PCB* pr); // 建立进程显示函数，用于显示当前进程信息
void check(); // 建立进程查看函数，显示当前运行进程和就绪队列中的进程信息
void destroy(); // 建立进程撤消函数，进程运行结束时撤销进程
void running(); // 建立进程就绪函数，进程运行时间到后置就绪状态

// 对进程进行优先级排列函数，优先数大者优先
// 该函数将新进程按照优先级插入到就绪队列中
PCB* sort() { 
    PCB *new_p = p; 
    // 如果就绪队列为空，直接将新进程作为队列的第一个元素
    if (ready == NULL) { 
        ready = new_p; 
        new_p->link = NULL; 
        return ready;
    } 
    // 如果新进程的优先级大于队列头进程的优先级，将新进程插入到队列头部
    if (new_p->nice > ready->nice) { 
        new_p->link = ready; 
        ready = new_p; 
        return ready;
    } 
    PCB *prev = NULL; // 用于记录当前节点的前一个节点
    PCB *current = ready; // 用于遍历队列
    // 遍历队列，找到新进程应该插入的位置
    while (current!= NULL && new_p->nice <= current->nice) { 
        prev = current;
        current = current->link;
    }
    // 如果prev为NULL，说明新进程优先级最低，应插入队尾
    if (prev == NULL) {  
        ready->link = new_p;
        new_p->link = NULL;
    } else {
        // 否则，将新进程插入到prev和current之间
        prev->link = new_p;
        new_p->link = current;
    }
    return ready;
}

// 建立进程控制函数
// 该函数用于输入进程的相关信息，并将进程加入到就绪队列中
void input() { 
    int num; 
    printf("\n请输入被调度的进程数目:"); 
    // 读取用户输入的进程数目，如果输入不是整数，输出错误信息并退出程序
    if (scanf("%d", &num) != 1) {
        fprintf(stderr, "输入进程数目错误，请输入整数\n");
        exit(1);
    }
    // 清除输入缓冲区中的换行符，避免影响后续输入
    while (getchar() != '\n'); 

    for (int i = 0; i < num; i++) { 
        printf("\n进程号No.%d:", i + 1); 
        p = getpch(PCB); // 为新进程分配内存空间
        if (p == NULL) {
            // 如果内存分配失败，输出错误信息并退出程序
            fprintf(stderr, "内存分配失败\n");
            exit(1);
        }
        printf("\n输入进程名:"); 
        // 增加输入校验，确保输入为字符串且长度合适
        while (1) {
            if (scanf("%9s", p->name) == 1) {
                int len = 0;
                // 计算输入字符串的长度
                while (p->name[len] != '\0') {
                    len++;
                }
                // 如果字符串长度在合法范围内，跳出循环
                if (len > 0 && len <= 9) {
                    break;
                }
            }
            // 如果输入不合法，输出错误信息并要求重新输入
            fprintf(stderr, "进程名输入错误，请输入合法字符串（长度不超过9）\n");
            while (getchar() != '\n'); 
        }
        // 清除输入缓冲区中的换行符
        while (getchar() != '\n'); 
        printf("输入进程优先数:"); 
        // 读取用户输入的进程优先级，如果输入不是整数，输出错误信息并退出程序
        if (scanf("%d", &p->nice) != 1) {
            fprintf(stderr, "输入进程优先数错误，请输入整数\n");
            exit(1);
        }
        // 清除输入缓冲区中的换行符
        while (getchar() != '\n'); 
        printf("输入进程运行时间:"); 
        // 读取用户输入的进程运行时间，如果输入不是浮点数，输出错误信息并退出程序
        if (scanf("%f", &p->ntime) != 1) {
            fprintf(stderr, "输入进程运行时间错误，请输入浮点数\n");
            exit(1);
        }
        // 清除输入缓冲区中的换行符
        while (getchar() != '\n'); 
        p->state = 'W'; // 将进程状态设置为就绪态
        p->rtime = 0.0; // 初始化进程已运行时间为0
        p->link = NULL; 
        sort(); // 将新进程插入到就绪队列中
    }
}

// 计算就绪队列中进程数量的函数
int space() { 
    int i = 0; 
    PCB *pr = ready; 
    // 遍历就绪队列，统计进程数量
    while (pr!= NULL) { 
        i++; 
        pr = pr->link; 
    }
    return i; 
}

// 建立进程显示函数，用于显示当前进程信息
void disp(PCB* pr) { 
    printf("\nname\tstate\tnice\tntime\truntime\n"); 
    printf("%s\t", pr->name); 
    printf("%c\t", pr->state); 
    printf("%d\t", pr->nice); 
    printf("%.1f\t", pr->ntime); 
    printf("%.1f\t", pr->rtime); 
    printf("\n"); 
}

// 建立进程查看函数
// 该函数用于显示当前正在运行的进程和就绪队列中的进程信息
void check() { 
    PCB *pr; 
    if (p != NULL) { 
        printf("\n***当前正在运行的进程是: %s", p->name); 
        disp(p); 
    }
    pr = ready; 
    if (pr != NULL) { 
        printf("\n***当前就绪队列状态为:"); 
        // 遍历就绪队列，显示每个进程的信息
        while (pr != NULL) { 
            disp(pr); 
            pr = pr->link; 
        }
    } else { 
        printf("\n***当前就绪队列状态为: 空\n"); 
    }
}

// 建立进程撤消函数，进程运行结束时撤销进程
void destroy() { 
    if (p != NULL) {
        printf("进程[%s]已完成\n", p->name); 
        free(p); // 释放进程占用的内存空间
        p = NULL; 
    }
}

// 建立进程就绪函数，进程运行时间到后置就绪状态
void running() { 
    if (p != NULL) {
        p->rtime += 1.0; // 进程运行时间加1
        if (p->rtime >= p->ntime) { 
            destroy(); // 如果进程已运行时间达到需要运行时间，撤销该进程
        } else { 
            p->nice--; // 进程优先级减1
            p->state = 'W'; // 将进程状态设置为就绪态
            // 重新插入就绪队列前，先从当前位置移除
            PCB *prev = NULL;
            PCB *current = ready;
            while (current != NULL && current != p) {
                prev = current;
                current = current->link;
            }
            if (prev == NULL) {  // p是队首
                ready = p->link;
            } else {
                prev->link = p->link;
            }
            // 尝试直接插入队尾
            PCB *temp = ready;
            if (temp == NULL) {
                ready = p;
                p->link = NULL;
            } else {
                while (temp->link != NULL) {
                    temp = temp->link;
                }
                temp->link = p;
                p->link = NULL;
            }
        }
    }
}

int main() { 
    int len, h = 0; 
    char ch; 
    input(); // 调用input函数，输入进程信息并建立就绪队列
    len = space(); // 计算就绪队列中进程的数量
    while (len != 0 && ready != NULL) { 
        // 清除输入缓冲区中的换行符
        while (getchar() != '\n'); 
        h++; 
        printf("\n第%d次执行\n", h); 
        p = ready; // 取出就绪队列的第一个进程
        if (p != NULL) {
            ready = p->link; // 更新就绪队列的头指针
            p->link = NULL; 
            p->state = 'R'; // 将进程状态设置为运行态
            running(); // 调用running函数，模拟进程运行
            check(); // 调用check函数，显示当前运行进程和就绪队列中的进程信息
            printf("\n按任一键继续..."); 
            // 清除输入缓冲区中的换行符
            while (getchar() != '\n'); 
        }
        len = space(); // 重新计算就绪队列中进程的数量
    }
    printf("\n所有进程已经运行完成"); 
    // 清除输入缓冲区中的换行符
    while (getchar() != '\n'); 
    return 0;
}