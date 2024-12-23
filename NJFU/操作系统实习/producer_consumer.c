#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define K 5  // 缓存区大小

// 缓存区
int buffer[K];
int in = 0;    // 记录下一个放入缓冲区的位置
int out = 0;   // 记录下一个取出缓冲区的位置

sem_t s1;      // 空缓冲区信号量
sem_t s2;      // 满缓冲区信号量
pthread_mutex_t mutex; // 互斥锁

void produce(int* item) {
    *item = rand() % 100; // 生产一个随机数
    printf("Produced: %d\n", *item);
}

void* producer(void* arg) {
    while (1) {
        int item;
        produce(&item);
        
        // 等待空缓冲区
        sem_wait(&s1);  
        pthread_mutex_lock(&mutex); // 进入临界区
        
        // 放入缓存区
        buffer[in] = item;
        in = (in + 1) % K;
        
        pthread_mutex_unlock(&mutex); // 离开临界区
        sem_post(&s2); // 增加满缓冲区信号量
        
        usleep(rand() % 1000000); // 随机延时
    }
}

void* consumer(void* arg) {
    while (1) {
        int item;
        
        // 等待满缓冲区
        sem_wait(&s2);
        pthread_mutex_lock(&mutex); // 进入临界区
        
        // 从缓冲区取出一个产品
        item = buffer[out];
        out = (out + 1) % K;
        printf("Consumed: %d\n", item);
        
        pthread_mutex_unlock(&mutex); // 离开临界区
        sem_post(&s1); // 增加空缓冲区信号量
        
        usleep(rand() % 1000000); // 随机延时
    }
}

int main() {
    pthread_t prod_thread, cons_thread;

    // 初始化信号量
    sem_init(&s1, 0, K);  // 初始值为K，表示缓冲区中有K个空位
    sem_init(&s2, 0, 0);  // 初始值为0，表示缓冲区中没有产品
    pthread_mutex_init(&mutex, NULL); // 初始化互斥锁

    // 创建生产者和消费者线程
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // 等待线程结束（此处无法停止线程，实际应用中可能需要更优雅的结束方式）
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // 清理资源
    sem_destroy(&s1);
    sem_destroy(&s2);
    pthread_mutex_destroy(&mutex);

    return 0;
}