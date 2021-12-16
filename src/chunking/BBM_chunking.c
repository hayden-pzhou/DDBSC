//
// Created by 60458 on 2021/10/26.
//
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define BIT8 0x01
#define BIT7 0x02
#define BIT6 0x04
#define BIT5 0x08
#define BIT4 0x10
#define BIT3 0x20
#define BIT2 0x40
#define BIT1 0x80


#define _repeat do
#define _until(CONDITION) while (!(CONDITION))

static pthread_t MBM_t;
static pthread_t BMBM_t;

char bits[8] = {BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7, BIT8};

char* P={"0101011001"};

int g_min_chunk_size;
int g_max_chunk_size;
int g_expect_chunk_size;

struct thread_data{
    char* T;
    int n;
    char* P;
    int m;
};



//static void stringToBinary(unsigned char* input ,int n,unsigned char* output){
//    for(int i=0;i<n;i++){
//        output[i]= input[i]&bits[7] ? '1':'0';
//    }
//}


static int Modified_Boyer_Moore(char* T,int n,char* P,int m){
    int d1 = 0;
    while ((n-d1) >= m ){
        int i =0 ;
        int j =0;
        while (j < m && (T[d1+i]&bits[7]) == (P[j]&bits[7])){
            ++i;
            ++j;
        }
        if(j == m){
            return d1+m; //找到切点 直接返回
        }else{
            _repeat{
                i = i+1;
            }_until((d1+i==n+j+1-m || ( (P[j]&bits[7])==(T[d1+i]&bits[7]) && ((2*j-i)<0 || (T[d1+j]&bits[7]) == (P[2*j-i]&bits[7])))));
            d1 = d1+i-j;
        }
        //end if
    }
    return -1;
}

static int back_Modified_Boyer_Moore(char* T,int n,char* P,int m){
    int d1=n-1;
    while (d1 >= m-1){
        int i =0;
        int j =m-1;
        while (j>=0 && (T[d1-i]&bits[7]) == (P[j]&bits[7])){
            ++i;
            --j;
        }
        if(j==-1){
            return d1;
        }else{
            _repeat{
                i++;
            }_until( d1-i == j-1 || ((P[j]&bits[7]) == (T[d1-i]&bits[7])  && ((2*m-2*j-i-2) < 0 || (T[d1-m+j+1]&bits[7]) == (P[i-m+2*j+1]&bits[7]))));
            d1 = d1-i-j+m-1;
        }
    }
    return -1;
}

static void* forward_BM_th(void *args){
    struct thread_data * params = (struct thread_data *)args;
    int cut_point = Modified_Boyer_Moore(params->T,params->n,params->P,params->m);
    return (void*)cut_point;
}

static void* backward_BM_th(void *args){
    struct thread_data * params = (struct thread_data *)args;
    int cut_point = back_Modified_Boyer_Moore(params->T,params->n,params->P,params->m);
    return (void*)cut_point;
}

/**
 *
 * @param p 待分块字节流
 * @param n 字节流长度
 * @return 0-i的切点 在[0,n)中长度 0-i
 */
int BBM_chunk_data(unsigned char *p,int n){
    /**
     * 三种情况
     * n小于最短分块长度  直接返回n
     * n小于期望分块长度  从区间[min,n]找切点
     * n小于最大分块长度  左边依然按照[min,expect],右边则按照[expect,n]的方式寻找切点
     */
    if(n<=g_min_chunk_size) return n;

    void *lret=NULL;
    void *rret=NULL;
    int left_point =-1;
    int right_point=-1;

    struct thread_data * b_params = (struct thread_data *) malloc(sizeof(struct thread_data));
    b_params->T = p+g_min_chunk_size;
    b_params->n = n>=g_expect_chunk_size ? g_expect_chunk_size - g_min_chunk_size : n-g_min_chunk_size; //if n<chunk_expect size but n > min size
    b_params->P = P;
    b_params->m = strlen(P);
    pthread_create(&BMBM_t,NULL,backward_BM_th,b_params);

    /*
     * if min_chunk_size < n < expect_chunk_size
     * 则不需要进行右边搜索
     */
    if(n > g_expect_chunk_size) {
        struct thread_data *f_params = (struct thread_data *) malloc(sizeof(struct thread_data));
        f_params->T = p+g_expect_chunk_size; // 起点 if n < expect_size 溢出
        f_params->n = n >= g_max_chunk_size ? g_max_chunk_size - g_expect_chunk_size : n -
                                                                                       g_expect_chunk_size; //如果n<chunk_max size but n > expect size
        f_params->P = P;
        f_params->m = strlen(P);
        pthread_create(&MBM_t, NULL, forward_BM_th, f_params);
        pthread_join(MBM_t,(void*)&rret);
        right_point = (int)rret;
        free(f_params);
    }
    pthread_join(BMBM_t,(void*)&lret);
    left_point = (int)lret;


    free(b_params);

    if(left_point == -1 && right_point == -1 ){
        //都没找到 左右两边匹配失败
        return n<g_expect_chunk_size ? n : g_expect_chunk_size;
    }
    if(left_point!=-1&&right_point!=-1){
        return g_expect_chunk_size-g_min_chunk_size-left_point-1 > right_point ? g_expect_chunk_size + right_point : g_min_chunk_size + left_point+1;
        //左右两边都匹配成功,返回距离最近的
    }
    if(left_point!=-1){
        //左边匹配成功
        return g_min_chunk_size+left_point+1;
    }
    if(right_point!=-1) {
        //右边匹配成功
        g_expect_chunk_size+right_point;
    }
}

void BBM_init(int expectCS){
    g_min_chunk_size = 2048;
    g_max_chunk_size = 64*1024;
    g_expect_chunk_size = expectCS;
}



