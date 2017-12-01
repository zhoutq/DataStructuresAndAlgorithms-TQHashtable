//
//  TQHashtable.h
//  TQHashtable
//
//  Created by ztq on 2017/11/28.
//  Copyright © 2017年 ztq. All rights reserved.
//

#ifndef TQHashtable_h
#define TQHashtable_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef void * tq;

typedef void (*DataDestroy)(tq); // 销毁函数

typedef struct TableElement {
    
    char *m_pKey; // hash key
    int m_nKeyLength; // key 长度
    tq m_pData; // hash节点数据
    struct TableElement *m_pNext; // 下一个节点指针（采用“链地址法”解决冲突，参考：http://blog.csdn.net/w_fenghui/article/details/2010387）
}TQTableElement, *TQTableElementList;

typedef struct HashTableItem {
    
    int m_nTableAllocSize; // hash table 开辟大小
    int m_mElementCount; // 元素个数
    TQTableElementList *m_ppElementArray; // hash table 数组地址
    
}TQHashTableItem, *TQHashTable;


/**
 创建hashtable

 @param nTableAllocSize 开辟空间大小
 @return 创建的hashtable
 */
TQHashTable TQHashTableCreate(int nTableAllocSize);

/**
 通过key获取hash值

 @param pKey key
 @param nKeyLength key 长度
 @param nTableAllocSize hashtable开辟空间大小
 @return key 对应的 hash 值
 */
int TQHashTableGetHashValue(char *pKey, int  nKeyLength, int nTableAllocSize);

/**
 通过key插入数据，已经存在key,则替换数据；不存在则创建

 @param pHashTable hash table
 @param pKey key
 @param nKeyLength key 长度
 @param tqData 待插入数据
 @return 结果反馈， true 成功， false 失败
 */
bool TQHashTableInsert(TQHashTable pHashTable, char *pKey, int  nKeyLength, tq tqData);

/**
 通过key查找数据节点
 
 @param pKey key
 @param nKeyLength key长度
 @param pHashTable hashTable
 @return hash 数据节点
 */
TQTableElement* TQHashTableFindElement(TQHashTable pHashTable, char *pKey, int  nKeyLength);

/**
 通过key删除元素

 @param pHashTable hash table
 @param pKey key
 @param nKeyLength key 长度
 @param pDataDestroy 数据销毁函数
 @return 结果反馈， true 成功， false 失败
 */
bool TQHashTableDelete(TQHashTable pHashTable, char *pKey, int  nKeyLength, DataDestroy pDataDestroy);

/**
 获取hash table 长度 （元素个数）

 @param pHashTable hash table
 @return table 长度 （元素个数）
 */
int TQHashTableLength(TQHashTable pHashTable);

/**
 销毁hash table

 @param pHashTable hash table
 @param pDataDestroy 销毁函数
 */
void TQHashTableDestroy(TQHashTable pHashTable, DataDestroy pDataDestroy);

#endif /* TQHashtable_h */
