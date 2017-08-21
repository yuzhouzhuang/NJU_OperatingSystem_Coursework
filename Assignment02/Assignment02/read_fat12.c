////
////  read_fat12.c
////  Assignment02
////
////  Created by Apple on 2017/4/18.
////  Copyright © 2017年 Apple. All rights reserved.
////
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <strings.h>
//#include <string.h>
//struct Time{
//    int year;
//    int month;
//    int day;
//    int hour;
//    int minute;
//    int second;
//};
//
//struct FileInfo{
//    char DIR_Name[16];//文件名
//    int DIR_Attr;//文件属性,文本文件为0,文件夹为32
//    int DIR_FstClus;//开始簇号
//    int DIR_FileSize;//文件大小
//    struct Time DIR_Time;//文件最后修改时间
//    char *fileContent;//文件内容
//};
//
//struct FileTreeNode{
//    struct FileInfo* fileInfo;
//    struct FileTreeNode* father;
//    struct FileTreeNode* firstChild;
//    struct FileTreeNode* nextBrother;
//};
//
////global variables
//FILE* file;
//struct FileTreeNode* root;
//int BPB_BytsPerSec;//每扇区字节数
//int BPB_SecPerClus;//每簇扇区数
//int BPB_RsvdSecCnt;//Boot记录占用多少扇区
//int BPB_BumFATs;//共有多少FAT表
//int BPB_RootEntCnt;//根目录文件数最大值
//int BPB_TotSec16;//扇区总数
//int BPB_FATSz16;//每FAT扇区数
//int validFile = 1;
//int endFile = 0;
//
//
//
////initate methods
//void readSingleFolder(struct FileTreeNode* parent, int clus);
//
//struct FileTreeNode* addTreeNode(struct FileInfo* fileInfo, struct FileTreeNode* father) {
//    struct FileTreeNode* nodeTemp = malloc(sizeof(struct FileTreeNode));
//    nodeTemp->fileInfo = fileInfo;
//    nodeTemp->father = father;
//    nodeTemp->nextBrother = NULL;
//    nodeTemp->firstChild = NULL;
//    if (father->firstChild != NULL) {
//        struct FileTreeNode* childNode = father->firstChild;
//        while (childNode->nextBrother != NULL) {
//            childNode = childNode->nextBrother;
//        }
//        childNode->nextBrother = nodeTemp;
//    } else {
//        father->firstChild = nodeTemp;
//    }
//    return nodeTemp;
//}
//
//int isSingleLineEnd(unsigned char buf[32]){
//    for (int i = 0; i < 32; i ++) {
//        if (buf[i] != 0) {
//            return 0;
//        }
//    }
//    return 1;
//}
//
//int isFileValid(unsigned char buf[32]) {
//    for (int i = 0; i < 8; i ++) {
//        if (!(((buf[i] >= 48)&&(buf[i] <= 57)) || ((buf[i] >= 65)&&(buf[i] <= 90)) ||
//              ((buf[i] >= 97)&&(buf[i] <= 122)) || (buf[i] == 32))){
//            return 1;
//        }
//    }
//    return 0;
//}
//
//void convertHighToLow(struct FileInfo* fileInfo) {
//    for (int i = 0; i < 16; i ++) {
//        if (65 <= fileInfo->DIR_Name[i] && fileInfo->DIR_Name[i] <= 90){
//            fileInfo->DIR_Name[i] = fileInfo->DIR_Name[i] + 32;
//        }
//        if (fileInfo->DIR_Name[i] == 0){
//            break;
//        }
//    }
//}
//
//void readSingleFileContent(struct FileInfo* fileInfo)
//{
//    fileInfo->fileContent = malloc(0x200);
//    long current_off=ftell(file);
//    if (fileInfo->DIR_FstClus == 0) {
//        return;
//    }
//    int markPoint = 0x2600 + BPB_RootEntCnt * 0x20 + (fileInfo->DIR_FstClus - 2) * 0x200;
//    fseek(file, markPoint, SEEK_SET);
//    fread(fileInfo->fileContent, 0x200, 1, file);
//    fseek(file,current_off,SEEK_SET);
//}
//
//struct FileInfo* readSingleFileInfo() {
//    unsigned char singleLine[32];
//    fread(singleLine, 32, 1, file);
//    
//    validFile = 1;
//    if (isFileValid(singleLine)) {
//        validFile = 0;
//    }
//    endFile = 0;
//    if (isSingleLineEnd(singleLine)) {
//        endFile = 1;
//    }
//    
//    struct FileInfo* fileInfo = malloc(sizeof(struct FileInfo));
//    fileInfo->fileContent = NULL;
//    if (singleLine[0xB] == 0x10) {
//        fileInfo->DIR_Attr = 1;
//    } else {
//        fileInfo->DIR_Attr = 0;
//    }
//    
//    if (!fileInfo->DIR_Attr) {
//        int count;
//        for (count = 0; count < 8; count ++) {
//            if (singleLine[count] == 0x20) {
//                break;
//            }
//            fileInfo->DIR_Name[count] = singleLine[count];
//        }
//        fileInfo->DIR_Name[count] = '.';
//        count++;
//        int i;
//        for (i = 8; i < 0xB; i ++) {
//            if (singleLine[i] == 20) {
//                break;
//            }
//            fileInfo->DIR_Name[count] = singleLine[i];
//            count++;
//        }
//        fileInfo->DIR_Name[count] = 0;
//    } else {
//        int count;
//        for (count = 0; count < 0xB; count ++) {
//            if (singleLine[count] == 0x20) {
//                break;
//            }
//            fileInfo->DIR_Name[count] = singleLine[count];
//        }
//        fileInfo->DIR_Name[count] = 0;
//    }
//    
//    fileInfo->DIR_FstClus = singleLine[0x1A] + (singleLine[0x1B] << 8);
//    fileInfo->DIR_FileSize = *((int*)&singleLine[0x1C]);
//    if (!fileInfo->DIR_Attr) {
//        readSingleFileContent(fileInfo);
//    }
//    return fileInfo;
//}
//
//long readSingleLine(long markPoint, struct FileTreeNode* father) {
//    fseek(file, markPoint, SEEK_SET);
//    struct FileInfo* fileInfo = readSingleFileInfo();
//    markPoint = ftell(file);
//    if (endFile) {
//        return 0;
//    }
//    if (!validFile) {
//        return markPoint;
//    }
//    convertHighToLow(fileInfo);
//    struct FileTreeNode* node = addTreeNode(fileInfo, father);
//    if (fileInfo->DIR_Attr) {
//        readSingleFolder(node, fileInfo->DIR_FstClus);
//    }
//    return markPoint;
//}
//
//void readSingleFolder(struct FileTreeNode* father, int DIR_FstClus) {
//    long markPoint = 0x2600 + BPB_RootEntCnt * 0x20 + (DIR_FstClus - 2) * 0x200 + 0x40;
//    while (1) {
//        markPoint = readSingleLine(markPoint, father);
//        if (markPoint == 0) {
//            break;
//        }
//    }
//}
//
//
//int initiateParameters(){
//    file = fopen("/Users/apple/Documents/Code/OS/Assignment02/a.img","r");
//    if (file == NULL) {
//        printf("File Read Failed!\n");
//        return 0;
//    }else{
//        printf("File Read Success!\n");
//        unsigned char loader[25];
//        fread(loader, 25, 1, file);
//        BPB_BytsPerSec = loader[11] + (loader[12] << 8);
//        BPB_SecPerClus = loader[13];
//        BPB_RsvdSecCnt = loader[14] + (loader[15] << 8);
//        BPB_BumFATs = loader[16];
//        BPB_RootEntCnt = loader[17] + (loader[18] << 8);
//        BPB_TotSec16 = loader[19] + (loader[20] << 8);
//        BPB_FATSz16 = loader[22] + (loader[23] << 8);
//        
//        root = malloc(sizeof(struct FileTreeNode));
//        root->firstChild = NULL;
//        root->nextBrother = NULL;
//        root->father = NULL;
//        struct FileInfo* rootFileInfo = malloc(sizeof(struct FileInfo));
//        strcpy(rootFileInfo->DIR_Name , "root");
//        rootFileInfo->fileContent = "null";
//        root->fileInfo = rootFileInfo;
//        
//        long markPoint = 0x2600;
//        while (1) {
//            markPoint = readSingleLine(markPoint, root);
//            if (markPoint == 0) {
//                break;
//            }
//        }
//        fclose(file);
//        return 1;
//    }
//}
//
////输出文件以及文件夹
//void printSingleNode(struct FileTreeNode* nodeTemp){
//    if (nodeTemp == root) {
//        return;
//    }
//    if (nodeTemp->father != root) {
//        printSingleNode(nodeTemp->father);
//    }
//    if (nodeTemp->fileInfo->DIR_Attr == 0) {
//        printf("%s\n",nodeTemp->fileInfo->DIR_Name);
//    } else {
//        printf("%s",nodeTemp->fileInfo->DIR_Name);
//        if (nodeTemp->firstChild == NULL) {
//            printf("\n");
//        } else {
//            printf("/");
//        }
//        fflush(stdout);
//    }
//}
//
////输出以nodeTemp为root的树
//void showNode(struct FileTreeNode* nodeTemp){
//    if (nodeTemp == NULL) {
//        return;
//    }
//    struct FileTreeNode* firstChildNode = nodeTemp->firstChild;
//    
//    if (firstChildNode == NULL) {
//        printSingleNode(nodeTemp);
//    }
//    
//    while (firstChildNode != NULL) {
//        showNode(firstChildNode);
//        firstChildNode = firstChildNode->nextBrother;
//    }
//}
//
//int getDirNum(struct FileTreeNode* nodeTemp) {
//    struct FileTreeNode* childNode = nodeTemp->firstChild;
//    int sum = 0;
//    while (childNode != NULL) {
//        if(childNode->fileInfo->DIR_Attr){
//            sum += 1;
//        }
//        childNode = childNode->nextBrother;
//    }
//    return sum;
//}
//
//int getFileNum(struct FileTreeNode* nodeTemp) {
//    struct FileTreeNode* childNode = nodeTemp->firstChild;
//    int sum = 0;
//    while (childNode != NULL) {
//        if(!childNode->fileInfo->DIR_Attr){
//            sum += 1;
//        }
//        childNode = childNode->nextBrother;
//    }
//    return sum;
//}
//
//void showDir(struct FileTreeNode* nodeTemp, int index) {
//    struct FileTreeNode* childNode = nodeTemp->firstChild;
//    if (nodeTemp->fileInfo->DIR_Attr == 0) {
//        return;
//    } else {
//        for (int i = 0; i < index; i++) {
//            printf("--");
//        }
//        printf("%s: %d files, %d directories\n",nodeTemp->fileInfo->DIR_Name,getFileNum(nodeTemp),getDirNum(nodeTemp));
//        while (childNode != NULL) {
//            showDir(childNode, index + 1);
//            childNode = childNode->nextBrother;
//        }
//    }
//}
//
//struct FileTreeNode* searchNode(char filePath[16][16],int index,int maxNum,struct FileTreeNode* nodeTemp){
//    if (index == maxNum) {
//        return nodeTemp;
//    }
//    struct FileTreeNode* firstChildNode = nodeTemp->firstChild;
//    while (firstChildNode != NULL) {
//        if (strcmp(firstChildNode->fileInfo->DIR_Name,filePath[index]) == 0) {
//            return searchNode(filePath, index + 1, maxNum, firstChildNode);
//        }
//        firstChildNode = firstChildNode->nextBrother;
//    }
//    return NULL;
//}
//
//int formFilePath(char filePath[16][16], char* input){
//    int index = 0;
//    char* pathTemp= strtok(input, "/");
//    while(pathTemp){
//        strcpy(filePath[index], pathTemp);
//        pathTemp = strtok(NULL, "/");
//        index++;
//    }
//    return index;
//}
//
//int main(){
//    if (initiateParameters()) {
//        //TODO: actions after reading file
//        char input[32];
//        char filePath[16][16];
//        showNode(root);
//        while (1) {
//            printf("Please input your order:\n");
//            scanf("%s",input);
//            if (strncasecmp(input,"count",5) == 0) {
//                scanf("%s",input);
//                int maxNum = formFilePath(filePath, input);
//                struct FileTreeNode* dirNode = searchNode(filePath, 0, maxNum, root);
//                if (dirNode != NULL && dirNode->fileInfo->DIR_Attr != 0) {
//                    showDir(dirNode,0);
//                } else {
//                    printf("Not a directory!\n");
//                }
//            } else {
//                int maxNum = formFilePath(filePath, input);
//                struct FileTreeNode* node = searchNode(filePath, 0, maxNum, root);
//                if (node == NULL) {
//                    printf("Unknown Path!\n");
//                } else if (node->fileInfo->DIR_Attr != 0) {
//                    showNode(node);
//                } else {
//                    printf("%s",node->fileInfo->fileContent);
//                }
//            }
//        }
//    }
//}
