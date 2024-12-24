#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define K 5  // 缓冲区大小
#define PRODUCER_NUM 3  // 生产者数量
#define CONSUMER_NUM 2  // 消费者数量
#define PRODUCER_LOOP 4 // 每个生产者生产次数 
#define CONSUMER_LOOP 6 // 每个消费者消费次数

// 缓冲区相关数据结构
int buffer[K];    // 缓冲区数组
int in = 0;       // 生产者放入位置
int out = 0;      // 消费者取出位置

// 信号量定义
sem_t s1;         // 同步信号量,控制缓冲区不满
sem_t s2;         // 同步信号量,控制缓冲区不空
sem_t mutex;      // 互斥信号量,控制缓冲区互斥访问

// 生产者线程函数
void* producer(void* arg) {
    int id = *(int*)arg;
    int item;
    
    for(int i = 0; i < PRODUCER_LOOP; i++) {
        item = rand() % 100;  // 生产一个随机数
        
        sem_wait(&s1);        // P(s1) 
        sem_wait(&mutex);     // P(mutex)
        
        buffer[in] = item;
        printf("producer %d produced %d in %d\n", id, item, in);
        in = (in + 1) % K;
        
        sem_post(&mutex);     // V(mutex)
        sem_post(&s2);        // V(s2)
        
        sleep(rand() % 3);    // 随机等待一段时间
    }
    pthread_exit(NULL);
}

// 消费者线程函数  
void* consumer(void* arg) {
    int id = *(int*)arg;
    int item;
    
    for(int i = 0; i < CONSUMER_LOOP; i++) {
        sem_wait(&s2);        // P(s2)
        sem_wait(&mutex);     // P(mutex)
        
        item = buffer[out];
        printf("consumer %d consumed %d in %d\n", id, item, out);
        out = (out + 1) % K;
        
        sem_post(&mutex);     // V(mutex)
        sem_post(&s1);        // V(s1)
        
        sleep(rand() % 3);    // 随机等待一段时间
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t pid[PRODUCER_NUM], cid[CONSUMER_NUM];
    int i, producer_id[PRODUCER_NUM], consumer_id[CONSUMER_NUM];
    
    // 初始化信号量
    sem_init(&s1, 0, K);      // 初始值为K
    sem_init(&s2, 0, 0);      // 初始值为0
    sem_init(&mutex, 0, 1);   // 初始值为1
    
    // 创建生产者线程
    for(i = 0; i < PRODUCER_NUM; i++) {
        producer_id[i] = i;
        pthread_create(&pid[i], NULL, producer, &producer_id[i]);
    }
    
    // 创建消费者线程
    for(i = 0; i < CONSUMER_NUM; i++) {
        consumer_id[i] = i;
        pthread_create(&cid[i], NULL, consumer, &consumer_id[i]);
    }
    
    // 等待所有线程结束
    for(i = 0; i < PRODUCER_NUM; i++) {
        pthread_join(pid[i], NULL);
    }
    for(i = 0; i < CONSUMER_NUM; i++) {
        pthread_join(cid[i], NULL);
    }
    
    // 销毁信号量
    sem_destroy(&s1);
    sem_destroy(&s2);
    sem_destroy(&mutex);
    
    return 0;
}