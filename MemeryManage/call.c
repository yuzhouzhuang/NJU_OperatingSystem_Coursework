//
// Created by Apple on 2017/6/11.
//

#include "call.h"
#include "bottom.h"
#include "stdio.h"
#include "MemeryPage.h"

/* 每一页1KB 有10位偏移位
 * 页表大小(128 + 512)MB/1KB = 5 * 2的17次方项
 * 每一项记录  4Bpid 有18位(大于15位的时候需要去磁盘调度)
 * 页表一共5*2的15*4 B = 5 * 2 的19 共2.5MB
 * 在内存中的地址为0x00300000 ~ 0x00580000
 * 每一项大小为0x00000004
 *
 *
 * 内存128MB 硬盘512MB 内存中留1MB的交换缓冲区
 * 在内存中的地址为0x00000000 ~ 0x00100000
 *
 * 需要链表记录各次申请 4Bpid 4B虚拟开始地址页表项号 4B结束地址页表项号 一条记录12B,留2MB的链表空间
 * 在内存中的地址为0x00100000 ~ 0x00300000
 */



void init() {

    for (int i = Page_Start; i < Page_End; i = i + Page_Length) {
        writeInt(0, i);
    }

    for (int i = Page_Start; i < Page_Start + 0x00580000 / Page_Size * Page_Length; i = i + Page_Length) {
        writeInt(1000, i);
    }
}

int read(data_unit *data, v_address address, m_pid_t pid) {
    return getContentByVAddress(data, address, pid);
}

int write(data_unit data, v_address address, m_pid_t pid) {
    return writeContentByVAddress(data, address, pid);
}

int allocate(v_address *address, m_size_t size, m_pid_t pid) {
    if (size <= getRemainStorage()) {
        *address = getFirstFitVAddress(size, pid);
//        printf("%d\n", *address);
        return 0;
    }
    return -1;
}

int free(v_address address, m_pid_t pid) {
    return freeMemory(address, pid);
}

//int main() {
//    printf("helloworld\n");
//    init();
//    printf("hi %d\n", getRemainStorage());
//}