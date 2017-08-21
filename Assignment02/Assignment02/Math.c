////
////  Math.c
////  Assignment02
////
////  Created by Apple on 2017/5/1.
////  Copyright © 2017年 Apple. All rights reserved.
////
//
//#include <stdio.h>
//
//int table[6][6] = {
//    {13, 24, 31, 19, 40, 29},
//    {18, 25, 30, 15, 43, 22},
//    {20, 20, 27, 25, 34, 33},
//    {23, 26, 28, 18, 37, 30},
//    {28, 33, 34, 17, 38, 20},
//    {19, 36, 25, 27, 45, 24},
//};
//int maxSum = 0;
//int maxSumResult[6];
//int maxMin = 0;
//int maxMinResult[6];
//
//void calculate(int count, int* result, int* mark) {
//    if (count == 6) {
//        int max = result[0];
//        int min = result[0];
//        for (int i = 0; i < 6; i++) {
//            max += result[i];
//            if (result[i] < min) {
//                min = result[i];
//            }
//        }
//        if (max > maxSum) {
//            maxSum = max;
//            for (int i = 0; i < 6; i++) {
//                maxSumResult[i] = result[i];
//            }
//        }
//        if (min > maxMin) {
//            maxMin = min;
//            for (int i = 0; i < 6; i++) {
//                maxMinResult[i] = result[i];
//            }
//        }
//    }
//    for (int j = 0; j < 6; j++) {
//        if (mark[j] > 0) {
//            result[count] = table[count][j];
//            mark[j] = 0;
//            calculate(count + 1, result, mark);
//            mark[j] = 1;
//        }
//    }
//}
//
//int main() {
//    int result[6] = {13, 25, 27, 18, 38, 24};
//    int mark[6] = {1, 1, 1, 1, 1, 1};
//    calculate(0, result, mark);
//    printf("第一题结果为%d\n",maxSum);
//    for (int i = 0; i < 6; i++) {
//        printf("%d ",maxSumResult[i]);
//    }
//    printf("\n");
//    printf("第一题结果为%d\n",maxMin);
//    for (int i = 0; i < 6; i++) {
//        printf("%d ",maxMinResult[i]);
//    }
//    printf("\n");
//}
