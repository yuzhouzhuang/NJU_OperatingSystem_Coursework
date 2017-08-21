//
// Created by Apple on 2017/6/11.
//

#include "MemeryPage.h"
#include "type.h"
#include <stdio.h>
#include "bottom.h"
#include <stdlib.h>


int readByte(int start_address) {
    int value = 0;
    char *ch = (char *) &value;
    for (int i = 0; i < 2; i++) {
        *(ch + i) = mem_read(start_address + i);
    }
    return value;
}

int readInt(int start_address) {
    int value = 0;
    char *ch = (char *) &value;
    for (int i = 0; i < 4; i++) {
        *(ch + i) = mem_read(start_address + i);
    }
    return value;
}

void writeByte(int value, int start_address) {
    char *ch = (char *) &value;
    for (int i = 0; i < 2; i++) {
        mem_write(*(ch + i), start_address + i);
    }
}

void writeInt(int value, int start_address) {
    char *ch = (char *) &value;
    for (int i = 0; i < 4; i++) {
        mem_write(*(ch + i), start_address + i);
    }
}

int getContentByVAddress(data_unit *data, v_address vAddress, int pid) {
    int mem_pid = readInt(Page_Start + vAddress / 1024 * Page_Length);
    if (vAddress / 1024 >= 0x20000) {
        v_address newAddress = (rand() % ((Page_End - Page_Start) / 5 + 1) + Page_Start + 0x00580000) / 4 / 1024;
//        printf("swap: vaddress :%x address :%x \n", vAddress / 1024, newAddress);
        addSwap(vAddress / 1024, newAddress);
    }
    vAddress = useSwap(vAddress / 1024) * 1024 + vAddress % 1024;
    v_address vAddressHigh = vAddress >> 10;


//    printf("read: vaddress :%x address :%x mem_pid :%d pid :%d\n", vAddress, Page_Start + vAddressHigh * Page_Length,
//           mem_pid, pid);
    if (mem_pid == pid) {
        *data = my_mem_read(vAddressHigh, vAddress - (vAddressHigh << 10));
        return 0;
    } else {
        return -1;
    }
}

int writeContentByVAddress(data_unit data, v_address vAddress, int pid) {
    int mem_pid = readInt(Page_Start + vAddress / 1024 * Page_Length);
    if (vAddress / 1024 >= 0x20000) {
        v_address newAddress = (rand() % ((Page_End - Page_Start) / 5 + 1) + Page_Start + 0x00580000) / 4 / 1024;
//        printf("swap: vaddress :%x address :%x \n", vAddress / 1024, newAddress);
        addSwap(vAddress / 1024, newAddress);
    }
    vAddress = useSwap(vAddress / 1024) * 1024 + vAddress % 1024;
    v_address vAddressHigh = vAddress >> 10;


//    printf("write: address :%x address :%x mem_pid :%d pid :%d\n", vAddress, Page_Start + vAddressHigh * Page_Length,
//           mem_pid, pid);
    if (mem_pid == pid) {
        my_mem_write(data, vAddressHigh, vAddress - (vAddressHigh << 10));
        return 0;
    } else {
        return -1;
    }
}

void initPAddress(v_address vAddress, p_address pAddress) {
    v_address vAddressHigh = vAddress >> 10;

    writeInt(pAddress, Page_Start + vAddressHigh * Page_Length);
}

count_t getRemainStorage() {
    count_t remain = 0;
    for (int i = Page_Start; i < Page_End; i = i + Page_Length) {
        if (readInt(i) == 0) {
            remain++;
        }
    }
    return remain * 1024;
}

void my_mem_write(data_unit data, v_address vAddressHigh, v_address vAddressLow) {
//    printf("my_mem_write: %x vAddressHigh: %x\n", (vAddressHigh << 10) + vAddressLow, vAddressHigh);
    if (vAddressHigh < 0x20000) {
        mem_write(data, (vAddressHigh << 10) + vAddressLow);
    } else {
        mem_write(data, SawpArea_Start);
        disk_save(SawpArea_Start, ((vAddressHigh - 0x020000) << 10) + vAddressLow, 0x01);
    }
}

data_unit my_mem_read(v_address vAddressHigh, v_address vAddressLow) {
//    printf("my_mem_read: %x\n", (vAddressHigh << 10) + vAddressLow);
    data_unit data;
    if (vAddressHigh < 0x20000) {
        data = mem_read((vAddressHigh << 10) + vAddressLow);
    } else {
        disk_load(SawpArea_Start, ((vAddressHigh - 0x020000) << 10) + vAddressLow, 0x01);
        data = mem_read(SawpArea_Start);
    }
    return data;
}

v_address getFirstFitVAddress(m_size_t size, int pid) {
    v_address vAddress;
    count_t count = Page_Start;
    count_t remain = 0;
    int mark = 0;

    for (int i = Page_Start; i < Page_End; i = i + Page_Length) {
//        printf("%d\n",readInt(i));
        if (readInt(i) == 0) {
            count = i;
            remain = 0;
            for (int j = count; j < Page_End; j = j + Page_Length) {
                if (readInt(j) == 0) {
                    remain++;
                } else {
                    break;
                }
            }
            if (remain * Page_Size >= size) {
                mark = 1;
                vAddress = count;
                break;
            }
        }
    }
    if (mark == 1) {
//        printf("allocate: address :%x pid :%d\n", vAddress, pid);
        for (int i = vAddress; i < vAddress + (size * Page_Length) / Page_Size; i = i + Page_Length) {
            writeInt(pid, i);
//            printf("allocating: %d address: %x\n", readInt(i), i);
        }
//        printf("associate address: %d size: %d\n", (vAddress - Page_Start) / 4, size / Page_Size);
        addAssociate((vAddress - Page_Start) / 4, size / Page_Size, pid);
//        printf("address: %x\n", (vAddress - Page_Start) / 4 * Page_Size);
        return (vAddress - Page_Start) / 4 * Page_Size;
    } else {
        return 0;
    }
}

int freeMemory(v_address address, m_pid_t pid) {
    int size = 0;

    int mark = -1;

//    printf("free address: %d \n", address / 1024);
    for (int i = AllocateList_Start; i < AllocateList_End; i = i + AllocateList_Length) {
//        printf("address: %d  i : %d m_pid: %d\n", readInt(i), i, readInt(i + 8));
        if (readInt(i) == address / 1024) {

            if (readInt(i + 8) != pid) {
                return -1;
            } else {
                size = readInt(i + 4);
                break;
            }
        }
    }


//    printf("address: %d size: %d\n", address / Page_Size, size);
    for (int i = Page_Start + address / Page_Size * Page_Length;
         i < Page_Start + address / Page_Size * Page_Length + Page_Length * size; i = i + Page_Length) {
//        printf("free: %x mem_pid: %d pid : %d\n", i, readInt(i), pid);
        if (readInt(i) != pid) {
            break;
        }
        mark = 0;
        writeInt(0, i);
    }
    return mark;

}

void addAssociate(v_address address, int size, int pid) {
    for (int i = AllocateList_Start; i < AllocateList_End; i = i + AllocateList_Length) {

        if (readInt(i) == 0 || readInt(i) == address) {
//            printf("associate write address: %d i : %d read(i): %d\n", address, i, readInt(i));
            writeInt(address, i);
            writeInt(size, i + 4);
            writeInt(pid, i + 8);
            break;
        }
    }
}

void addSwap(v_address address1, int address2) {
    for (int i = SawpArea_Start; i < SawpArea_End; i = i + SawpArea_Length) {
        if (readInt(i) == 0) {
            writeInt(address1, i);
            writeInt(address2, i + 4);
            break;
        }
    }
}

v_address useSwap(v_address address) {
    for (int i = SawpArea_Start; i < SawpArea_End; i = i + SawpArea_Length) {
        if (readInt(i) != 0) {
            v_address address1 = readInt(i);
            v_address address2 = readInt(i + 4);
            if (address == address1) {
                return address2;
            } else if (address == address2) {
                return address1;
            }
        } else {
            return address;
        }
    }
}