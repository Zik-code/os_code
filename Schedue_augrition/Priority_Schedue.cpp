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









#include<stdio.h>
#include<stdlib.h>
#include<indlude<string.h>
#include<stdbool.h>
typedef struct node {
    char name[10];
    int prio;
    int round;
    int cputime;
    int needtime;
    int count;
}PCB;
struct node *next;
char state;
PCB *finish, *ready, *run;
int N;
void firstin() {
    run = ready;
    run->state = 'R';
    ready = ready->next;
}
void prt1(char a) {
    if (a == 'P' || a == 'p')
        printf("进程号cp时间所需时间优先数状态\n");
    else if (a == 'r' || a == 'R')
        printf("进程号cpu时间所需时间记数时间片状态\n");
    else if (a == 'f' || a == 'F')
        printf("进程号所需时间状态\n");
}
void prt2(char a, PCB *q) {
    // 此函数未实现具体打印逻辑
}
void prt(char a, PCB *q) {
    if (a == 'P' || a == 'p')
        printf("%-10s%-10d%-10d%-10d%c\n", q->name, q->cputime, q->needtime, q->prio, q->state);
    else if (a == 'r' || a == 'R')
        printf("%-10s%-10d%-10d%-10d%-10d%c\n", q->name, q->cputime, q->needtime, q->count, q->round, q->state);
    else if (a == 'f' || a == 'F')
        printf("%-10s%-10d%c\n", q->name, q->needtime, q->state);
}
void prt(char algo) {
    PCB *p;
    if (run != NULL && (algo == 'l' || algo == 'R')) // 此处'algo == 'l''疑似错误
        prt1(algo);
    p = ready;
    while (p != NULL) {
        prt2(algo, run);
        prt2(algo, p);
        p = p->next;
    }
    p = finish;
    while (p != NULL) {
        prt2(algo, p);
        p = p->next;
    }
    getchar();
}
void insert1() {
    PCB p1;
    int b;
    // s和r未定义，以下代码逻辑可能有误
    p1 = ready;
    b = 1;
    r = p1;
    while (p1!= NULL && b) {
        if (p1->pro >= s->prio) {
            p1 = p1->next;
            r = p1;
        } else
            b = 0;
    }
    if (d = p1) {
        r->next = s;
        s->next = p1;
    } else {
        s->next = p1;
        ready = s;
    }
}
void creat1(char alg) {
    PCB *p;
    int i, time;
    char na[10];
    ready = NULL;
    finish = NULL;
    run = NULL;
    printf("输入进程号和运行时间:\n");
    for (i = 1; i <= N; i++) {
        p = (PCB *)malloc(sizeof(PCB));
        scanf("%s", na);
        strcpy(p->name, na);
        scanf("%d", &time);
        p->cputime = 0;
        p->needtime = time;
        p->state = 'W';
        p->prio = 50 - time;
        insert1();
        if (ready!= NULL)
            // 此处逻辑不完整
            ;
        else {
            p->next = ready;
            ready = p;
        }
    }
    //clrscrO; 未定义函数
    if (alg == 'p' || alg == 'P') {
        printf("优先数算法输出信息:\n");
        printf("先来先服务算法输出信息:\n");
        prt(alg);
        run = ready;
        ready = ready->next;
        run->state = 'R';
    }
}
void creat2(char alg) {
    PCB *p;
    int i, time, round;
    char na[10];
    finish = NULL;
    ready = NULL;
    run = NULL;
    printf("请输入时间片:");
    scanf("%d", &round);
    printf("输入进程号和运行时间\n");
    for (i = 1; i <= N; i++) {
        p = (PCB *)malloc(sizeof(PCB));
        scanf("%s", na);
        strcpy(p->name, na);
        scanf("%d", &time);
        p->cputime = 0;
        p->needtime = time;
        p->state = 'W';
        p->count = 0;
        p->round = round;
        p->next = NULL;
        if (i == 1)
            ready = p;
        else {
            PCB *r = ready;
            while (r->next!= NULL)
                r = r->next;
            r->next = p;
        }
    }
    //clrscrO; 未定义函数
    printf("时间片轮转算法输出信息:\n");
    printf("**************************\n");
    prt(alg);
}
void priority(char alg) {
    while (run!= NULL) {
        run->cputime += 1;
        run->needtime -= 1;
        if (run->needtime == 0)
            run->prio -= 3;
        if (run->needtime == 0) {
            finish = run;
            run->next = finish;
            run->state = 'F';
            run = NULL;
            if (ready!= NULL)
                firstin();
        } else if (ready!= NULL && run->prio < ready->prio) {
            run->state = 'W';
            insert1();
            firstin();
        }
        prt(alg);
    }
}
void roundrun(char alg) {
    // 此函数未实现
}
void FCFSrun(char alg) {
    PCB *p;
    while (run!= NULL) {
        run->cputime += run->needtime;
        run->needtime = 0;
        run->next = finish;
        finish = run;
        run->state = 'F';
        run = NULL;
        if (ready!= NULL)
            firstin();
        prt(alg);
    }
}
int main() {
    char algo;
    //clrscro; 未定义函数
    printf("选择算法:P/R/F优先数算法/时间片轮转算法/先来先服务算法\n");
    scanf("%c", &algo);
    printf("输入进程数\n");
    scanf("%d", &N);
    if (algo == 'P' || algo == 'p') {
        priority(algo);
        creat1(algo);
    } else if (algo == 'R' || algo == 'r') {
        roundrun(algo);
        creat2(algo);
    } else if (algo == 'f' || algo == 'F') {
        creat1(algo);
        FCFSrun(algo);
    }
    system("echo按任意键退出");
    system("read -n 1");
    return 0;
}


// 建立进程控制函数
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
        // ...其他代码...
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
        // ...其他代码...
    }
}






PCB* sort() { 
    PCB *new_p = p; 
    // ...其他代码...
    PCB *prev = NULL; // 用于记录当前节点的前一个节点
    PCB *current = ready; // 用于遍历队列
    // 遍历队列，找到新进程应该插入的位置
    while (current!= NULL && new_p->nice <= current->nice) { 
        prev = current;
        current = current->link;
    }
    // ...其他代码...
    return ready;
}




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

// 建立进程就绪函数，进程运行时间到后置就绪状态
void running() { 
    if (p != NULL) {
        p->rtime += 1.0; // 进程运行时间加1
        if (p->rtime >= p->ntime) { 
            destroy(); // 如果进程已运行时间达到需要运行时间，撤销该进程
        } else { 
            p->nice--; // 进程优先级减1
            p->state = 'W'; // 将进程状态设置为就绪态

            // 从当前位置移除p
            if (ready == p) {
                ready = p->link;
            } else {
                PCB *prev = ready;
                while (prev->link != NULL && prev->link != p) {
                    prev = prev->link;
                }
                if (prev->link != NULL) {
                    prev->link = p->link;
                }
            }

            // 插入到就绪队列尾部
            if (ready == NULL) {
                ready = p;
                p->link = NULL;
            } else {
                PCB *temp = ready;
                while (temp->link != NULL) {
                    temp = temp->link;
                }
                temp->link = p;
                p->link = NULL;
            }
        }
    }
}    





run->count = 0;
if (ready == NULL) {
    ready = run;
    run->next = NULL;
} else {
    PCB *p = ready;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = run;
    run->next = NULL;
}



if (finish == NULL) {
    finish = run;
    run->next = NULL;
} else {
    PCB *p = finish;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = run;
    run->next = NULL;
}



// 对进程进行优先级排列函数，优先数大者优先
PCB* sort() { 
    PCB *new_p = p; 
    if (ready == NULL) { 
        ready = new_p; 
        new_p->link = NULL; 
        return ready;
    } 
    if (new_p->nice > ready->nice) { 
        new_p->link = ready; 
        ready = new_p; 
        return ready;
    } 
    PCB *prev = NULL;
    PCB *current = ready;
    while (current!= NULL && new_p->nice <= current->nice) { 
        prev = current;
        current = current->link;
    }
    if (prev == NULL) {  // 说明新进程优先级最低，应插入队尾
        ready->link = new_p;
        new_p->link = NULL;
    } else {
        prev->link = new_p;
        new_p->link = current;
    }
    return ready;
}


// 对进程进行优先级排列函数，优先数大者优先
// 该函数的作用是将新进程按照优先级插入到就绪队列中，返回值为调整后的就绪队列头指针
PCB* sort() { 
    // 获取当前要插入就绪队列的进程指针，这里假设p是指向新进程的全局指针
    PCB *new_p = p; 
    // 如果就绪队列为空，说明还没有进程在队列中
    if (ready == NULL) { 
        // 直接将新进程作为就绪队列的第一个元素
        ready = new_p; 
        // 新进程的下一个节点指针置为空，因为它是队列中唯一的元素
        new_p->link = NULL; 
        // 返回调整后的就绪队列头指针，此时头指针指向新插入的进程
        return ready;
    } 
    // 如果新进程的优先级大于就绪队列头进程的优先级
    if (new_p->nice > ready->nice) { 
        // 将新进程的下一个节点指针指向当前的就绪队列头进程
        new_p->link = ready; 
        // 更新就绪队列的头指针，使其指向新进程
        ready = new_p; 
        // 返回调整后的就绪队列头指针，此时头指针指向新插入的高优先级进程
        return ready;
    } 
    // 用于记录当前节点的前一个节点，初始化为NULL
    PCB *prev = NULL; 
    // 用于遍历队列，初始化为就绪队列的头指针
    PCB *current = ready; 
    // 遍历队列，当当前节点存在且新进程的优先级小于等于当前节点的优先级时
    while (current!= NULL && new_p->nice <= current->nice) { 
        // 记录当前节点，以便后续操作
        prev = current; 
        // 移动到下一个节点
        current = current->link; 
    }
    // 如果prev为NULL，说明新进程在遍历过程中没有找到比它优先级低的节点
    if (prev == NULL) {  
        // 即新进程优先级最低，将其插入到就绪队列的尾部
        ready->link = new_p; 
        // 新进程的下一个节点指针置为空，因为它是队尾元素
        new_p->link = NULL; 
    } else {
        // 将新进程插入到prev和current之间
        prev->link = new_p; 
        new_p->link = current; 
    }
    // 返回调整后的就绪队列头指针，此时就绪队列已按照优先级插入了新进程
    return ready;
}