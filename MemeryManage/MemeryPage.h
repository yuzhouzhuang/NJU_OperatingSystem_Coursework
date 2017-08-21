//
// Created by Apple on 2017/6/11.
//
#include "type.h"

#ifndef MEMERYMANAGE_MEMERYPAGE_H
#define MEMERYMANAGE_MEMERYPAGE_H
#define AllocateList_Start   0x00100000
#define AllocateList_End     0x00300000
#define AllocateList_Length  0x0000000C

#define SawpArea_Start       0x00000000
#define SawpArea_End         0x00100000
#define SawpArea_Length      0x00000008

#define Page_Start           0x00300000
#define Page_End             0x00580000
#define Page_Length          0x00000004
#define Page_Size            0x00000400

v_address getFirstFitVAddress(m_size_t size, int pid);

void my_mem_write(data_unit data, v_address vAddressHigh, v_address vAddressLow);

data_unit my_mem_read(v_address vAddressHigh, v_address vAddressLow);

int getContentByVAddress(data_unit *data, v_address vAddress, int pid);

int writeContentByVAddress(data_unit data, v_address vAddress, int pid);

void initPAddress(v_address vAddress, p_address pAddress);

count_t getRemainStorage();

int readByte(int start_address);

int readInt(int start_address);

void writeByte(int value, int start_address);

void writeInt(int value, int start_address);

int freeMemory(v_address address, m_pid_t pid);

void addSwap(v_address address1, int address2);

v_address useSwap(v_address address);

void addAssociate(v_address address, int size, int pid);

#endif //MEMERYMANAGE_MEMERYPAGE_H
