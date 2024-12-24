#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define PAGE_SIZE 10 // 每页包含10条指令
#define OUTER_MEMORY_SIZE 400 // 外存大小为400条指令
#define MAX_MEMORY 40 // 内存最多可以容纳40页

// 页面结构体,包含页号、访问次数和使用位等信息
struct p_str {
    int pagenum;
    int count; // 访问次数
    int use_bit; // Clock算法中的使用位
    struct p_str *next;
};

// LRU算法的结构体
struct LRU {
    struct p_str *head;
    int size;
    int max_pages;  // 最大页面数限制
};

// Clock算法的结构体
struct Clock {
    struct p_str *pages[MAX_MEMORY];  // 页面数组
    int pointer;  // 替换指针
    int size;
};

// 外存中的指令序列
int instructions[OUTER_MEMORY_SIZE];

// 初始化指令序列,每页含10条指令
void initializeInstructions() {
    int i;
    for (i = 0; i < OUTER_MEMORY_SIZE; i++) {
        instructions[i] = i / PAGE_SIZE;
    }
}

// 生成随机指令编号
int generateRandomInstruction() {
    return rand() % OUTER_MEMORY_SIZE;
}

// 打印命中率
void printHitRate(int hits, int total) {
    printf("命中率: %.2f%%\n", (float)hits * 100 / total);
}

// 初始化LRU结构体
void LRU_init(struct LRU *lru) {
    lru->head = NULL;
    lru->size = 0;
}

// 向LRU中添加新页面
void LRU_addPage(struct LRU *lru, int page) {
    struct p_str *new_page = (struct p_str*)malloc(sizeof(struct p_str));
    new_page->pagenum = page;
    new_page->count = 0;
    new_page->next = lru->head;
    lru->head = new_page;
    lru->size++;
}

// 检查页面是否在LRU中
bool LRU_isPageInMemory(struct LRU *lru, int page) {
    struct p_str *current = lru->head;
    while (current != NULL) {
        if (current->pagenum == page) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// LRU算法的替换
void LRU_reference(struct LRU *lru, int page) {
    struct p_str *current = lru->head, *prev = NULL;
    // 查找页面是否存在于链表中
    while (current != NULL) {
        if (current->pagenum == page) {
            // 如果页面存在,将其移到链表头部（表示最近使用）
            if (prev != NULL) {
                prev->next = current->next; // 断开当前节点
                current->next = lru->head;   // 将当前节点的下一个指针指向原头节点
                lru->head = current;          // 更新头节点为当前节点
            }
            return; // 页面已存在,直接返回
        }
        prev = current; // 更新前一个节点
        current = current->next; // 移动到下一个节点
    }
    
    // 页面不存在,需要添加新页面
    struct p_str *new_page = (struct p_str*)malloc(sizeof(struct p_str));
    new_page->pagenum = page; // 设置页面编号
    new_page->count = 0; // 初始化计数
    new_page->next = lru->head; // 新页面指向当前头节点
    lru->head = new_page;
    lru->size++;
    
    // 如果超出内存限制, 删除最后一个节点(最久未使用的页面)
    if (lru->size > lru->max_pages) {
        current = lru->head;
        prev = NULL;
        // 找到链表的最后一个节点
        while (current->next != NULL) {
            prev = current; // 更新前一个节点
            current = current->next; // 移动到下一个节点
        }
        // 从链表中删除最后一个节点
        prev->next = NULL;
        free(current);
        lru->size--; 
    }
}

// LRU算法的模拟
int LRU_simulate(int memory_pages) {
    struct LRU lru;
    LRU_init(&lru); // 初始化 LRU 结构
    lru.max_pages = memory_pages;  // 设置最大页面数限制
    int hits = 0; // 记录命中次数
    int total = 0; // 记录总访问次数
    
    // 模拟页面访问过程
    int i;
    for (i = 0; i < OUTER_MEMORY_SIZE; i++) {
        int instruction = generateRandomInstruction(); // 生成随机指令
        total++; // 增加总访问次数
        int page = instructions[instruction]; // 获取访问的页面
        
        //        // 检查页面是否在内存中
        if (LRU_isPageInMemory(&lru, page)) {
            hits++; // 如果在内存中,增加命中次数
        }
        // 更新页面的引用状态
        LRU_reference(&lru, page);
    }
    
    // 清理内存,释放所有页面节点
    struct p_str *current = lru.head;
    while (current != NULL) {
        struct p_str *temp = current; // 保存当前节点
        current = current->next; // 移动到下一个节点
        free(temp); // 释放当前节点的内存
    }
    
    return hits; // 返回命中次数
}

// Clock算法的初始化
void Clock_init(struct Clock *clock) {
    // 初始化指针位置和页面数量
    clock->pointer = 0;
    clock->size = 0;
    // 将所有页面指针初始化为NULL
    int i;
    for (i = 0; i < MAX_MEMORY; i++) {
        clock->pages[i] = NULL;
    }
}

// 向Clock中添加新页面
void Clock_addPage(struct Clock *clock, int page) {
    // 为新页面分配内存
    struct p_str *new_page = (struct p_str*)malloc(sizeof(struct p_str));
    // 设置页面编号、访问计数和使用位
    new_page->pagenum = page;
    new_page->count = 0;
    new_page->use_bit = 1; // 表示该页面被使用
    // 将新页面添加到Clock中
    clock->pages[clock->size] = new_page;
    // 增加页面数量
    clock->size++;
}

// Clock算法的模拟
int Clock_simulate(int memory_pages) {
    struct Clock clock;
    // 初始化
    Clock_init(&clock);
    int hits = 0; // 记录命中次数
    int total = 0; // 记录总访问次数

    // 模拟访问页面的过程
    int i;
    for (i = 0; i < OUTER_MEMORY_SIZE; i++) {
        // 生成随机的指令以访问页面
        int instruction = generateRandomInstruction();
        total++;
        int page = instructions[instruction]; // 获取要访问的页面
        bool pageHit = false; // 标记页面是否命中

        // 检查页面是否在Clock中
        int j;
        for (j = 0; j < clock.size; j++) {
            if (clock.pages[j]->pagenum == page) {
                // 如果命中,更新使用位并增加命中次数
                clock.pages[j]->use_bit = 1;
                hits++;
                pageHit = true;
                break;
            }
        }

        // 如果页面没有命中
        if (!pageHit) {
            // 如果当前页面数量小于内存限制,添加新页面
            if (clock.size < memory_pages) {
                Clock_addPage(&clock, page);
            } else {
                // 如果已满,使用Clock算法替换页面
                while (true) {
                    // 检查指针指向的页面的使用位
                    if (clock.pages[clock.pointer]->use_bit == 0) {
                        // 如果使用位为0,替换该页面
                        clock.pages[clock.pointer]->pagenum = page;
                        clock.pages[clock.pointer]->use_bit = 1; // 设置为使用状态
                        break;
                    } else {
                        // 如果使用位为1,重置为0并移动指针
                        clock.pages[clock.pointer]->use_bit = 0;
                    }
                    // 移动指针,循环使用
                    clock.pointer = (clock.pointer + 1) % clock.size;
                }
            }
        }
    }
    return hits;
}

int main() {
    srand(time(NULL));
    initializeInstructions();
    int memory_pages;
    for (memory_pages = 4; memory_pages <= 40; memory_pages++) {
        printf("\n内存大小: %d pages\n", memory_pages);
        
        int lru_hits = LRU_simulate(memory_pages);
        printf("LRU算法 ");
        printHitRate(lru_hits, OUTER_MEMORY_SIZE);
        
        int clock_hits = Clock_simulate(memory_pages);
        printf("Clock算法 ");
        printHitRate(clock_hits, OUTER_MEMORY_SIZE);
    }
    return 0;
}