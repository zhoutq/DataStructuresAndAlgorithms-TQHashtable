//
//  TQHashtable.c
//  TQHashtable
//
//  Created by ztq on 2017/11/28.
//  Copyright © 2017年 ztq. All rights reserved.
//

#include "TQHashtable.h"

#define MAX_HASH_PRIME_ARRAY_SIZE    7

static int nPrime[MAX_HASH_PRIME_ARRAY_SIZE] = {17, 37, 79, 163, 331, 673, 1361};

TQHashTable TQHashTableCreate(int nTableAllocSize)
{
    TQHashTable pHashTable = NULL;
    
    if (nTableAllocSize < 1) {
        return NULL;
    }
    
    pHashTable = (TQHashTable)malloc(sizeof(TQHashTableItem));
    if (!pHashTable) {
        
        return NULL;
    }
    
    memset(pHashTable, 0, sizeof(TQHashTableItem));
    
    pHashTable->m_nTableAllocSize = nTableAllocSize;
    
    // 减少hash冲突，hash表大小最好为素数，按照nPrime对应分配空间。小于17，则大小为17；大于17，小于37，则大小为37.
    // 可参考：http://blog.csdn.net/maoliran/article/details/52082829， http://blog.51cto.com/thuhak/1352903
    for (int i = 0; i < MAX_HASH_PRIME_ARRAY_SIZE; i++) {
        
        if (nPrime[i] > pHashTable->m_nTableAllocSize) {
            
            pHashTable->m_nTableAllocSize = nPrime[i];
            break;
        }
    }
    
    pHashTable->m_ppElementArray = (TQTableElementList *)malloc(pHashTable->m_nTableAllocSize * sizeof(TQTableElement));
    if (!pHashTable->m_ppElementArray) {
        
        free(pHashTable);
        return NULL;
    }
    
    memset(pHashTable->m_ppElementArray, 0, pHashTable->m_nTableAllocSize * sizeof(TQTableElement));
    
    return pHashTable;
}

int TQHashTableGetHashValue(char *pKey, int  nKeyLength, int nTableAllocSize)
{
    // ELF hash算法，参考：http://blog.csdn.net/mycomputerxiaomei/article/details/7641221
    int h = 0;
    int g = 0;
    
    if(!pKey || nKeyLength<1 || nTableAllocSize<1)
    {
        return -1;
    }
    while(nKeyLength--) // 获取key中的每个字符，进行hash计算
    {
        h = (h<<4) + *pKey++;
        g = h & 0xF0000000L;
        if(g)
        {
            h ^= g>>24;
        }
        h &= ~g;
    }
    return h % nTableAllocSize;
}

bool TQHashTableInsert(TQHashTable pHashTable, char *pKey, int  nKeyLength, tq tqData)
{
    int nPos = 0;
    TQTableElement *pTableElement = NULL;
    TQTableElement **pTableEndElement = NULL;
    
    if (!pHashTable || !pKey  || nKeyLength < 1) {
        return false;
    }
    
    pTableElement = TQHashTableFindElement(pHashTable, pKey, nKeyLength);
    if (pTableElement) // 存在，替换数据
    {
        pTableElement->m_pData = tqData;
    }
    else // 不存在，追加
    {
        // 创建元素
        pTableElement = (TQTableElementList)malloc(sizeof(TQTableElement));
        memset(pTableElement, 0, sizeof(TQTableElement));
        
        pTableElement->m_pKey = (char *)malloc(sizeof(nKeyLength));
        memset(pTableElement->m_pKey, 0, sizeof(nKeyLength));
        memcpy(pTableElement->m_pKey, pKey, nKeyLength);
        
        pTableElement->m_nKeyLength = nKeyLength;
        pTableElement->m_pData = tqData;
        pTableElement->m_pNext = NULL;
        
        pHashTable->m_mElementCount++;
        
        // 插入最后
        nPos = TQHashTableGetHashValue(pKey, nKeyLength, pHashTable->m_nTableAllocSize);
        pTableEndElement = &pHashTable->m_ppElementArray[nPos];
        if (!*pTableEndElement)
        {
            pHashTable->m_ppElementArray[nPos] = pTableElement;
            
            return true;
        }
        
        while ((*pTableEndElement)->m_pNext) {
            pTableEndElement = &((*pTableEndElement)->m_pNext);
        }
        
        (*pTableEndElement)->m_pNext = pTableElement;
        
    }
    
    return true;
}

TQTableElement* TQHashTableFindElement(TQHashTable pHashTable, char *pKey, int  nKeyLength)
{
    int nPos = 0;
    TQTableElement *pCur = NULL;
    
    if (!pHashTable || !pKey || nKeyLength < 1) {
        return NULL;
    }
    
    // 通过 key 获取 hash值（在hash表中的位置）
    nPos = TQHashTableGetHashValue(pKey, nKeyLength, pHashTable->m_nTableAllocSize);
    if (nPos >= 0) {
        // 通过位置获取到数据链表地址
        pCur = pHashTable->m_ppElementArray[nPos];
    }
    
    while (pCur) {
        // 在链表中通过key查找指定节点
        if (pCur->m_nKeyLength == nKeyLength && memcmp(pCur->m_pKey, pKey, nKeyLength) == 0) {
            
            return pCur;
        }
        
        pCur = pCur->m_pNext;
    }
    
    return NULL;
}

bool TQHashTableDelete(TQHashTable pHashTable, char *pKey, int  nKeyLength, DataDestroy pDataDestroy)
{
    int nPos = 0;
    TQTableElement **pCur = NULL;
    TQTableElement **pCurTmp = NULL;
    
    if (!pHashTable || !pKey || nKeyLength < 1) {
        
        return false;
    }
    
    nPos = TQHashTableGetHashValue(pKey, nKeyLength, pHashTable->m_nTableAllocSize);
    pCur = &pHashTable->m_ppElementArray[nPos];
    if (!(*pCur)) {
        
        return false;
    }
    
    if ((*pCur)->m_nKeyLength == nKeyLength && memcmp((*pCur)->m_pKey, pKey, nKeyLength) == 0)
    {
       // 链表首个元素
        pHashTable->m_mElementCount--;
        free((*pCur)->m_pKey);
        (*pCur)->m_pKey = NULL;
        
        if (pDataDestroy) {
            pDataDestroy((*pCur)->m_pData);
        }
        (*pCur)->m_pData = NULL;
        
        if ((*pCur)->m_pNext) { // 如果next有元素，则前移
            
            pHashTable->m_ppElementArray[nPos] = (*pCur)->m_pNext;
        }
        
        (*pCur)->m_pNext = NULL;
        free(*pCur);
        (*pCur) = NULL;
        
        return true;
    }
    else
    {
        while ((*pCur)->m_pNext) {
            
            *pCurTmp = *pCur;
            (*pCur) = (*pCur)->m_pNext;
            
            if ((*pCur)->m_nKeyLength == nKeyLength && memcmp((*pCur)->m_pKey, pKey, nKeyLength) == 0)
            {
                pHashTable->m_mElementCount--;
                free((*pCur)->m_pKey);
                (*pCur)->m_pKey = NULL;
                
                if (pDataDestroy) {
                    pDataDestroy((*pCur)->m_pData);
                }
                (*pCur)->m_pData = NULL;
                
                if ((*pCur)->m_pNext) { // 如果next有元素,则 前面的next等于后面的next
                    
                    (*pCurTmp)->m_pNext = (*pCur)->m_pNext;
                }
                
                (*pCur)->m_pNext = NULL;
                free(*pCur);
                (*pCur) = NULL;
                
                return true;
                
            }
        }
    }
    
    return false;
}

int TQHashTableLength(TQHashTable pHashTable)
{
    return pHashTable ? pHashTable->m_mElementCount : -1;
}

void TQHashTableDestroy(TQHashTable pHashTable, DataDestroy pDataDestroy)
{
    TQTableElement **pCur = NULL;
    TQTableElement **pCurTmp = NULL;
    
    if (!pHashTable) {
        return;
    }
    
    for (int i = 0; i <pHashTable->m_nTableAllocSize; i++) {
        
        pCur = &pHashTable->m_ppElementArray[i];
        if (!(*pCur)) {
            continue;
        }
        
        while ((*pCur)->m_pNext) {
            
            *pCurTmp = *pCur;
            (*pCur) = (*pCur)->m_pNext;
            
            free((*pCur)->m_pKey);
            (*pCur)->m_pKey = NULL;
            
            if (pDataDestroy) {
                pDataDestroy((*pCur)->m_pData);
            }
            (*pCur)->m_pData = NULL;
            
            free(*pCur);
            (*pCur) = NULL;
            
            (*pCur) = (*pCur)->m_pNext;
            (*pCurTmp)->m_pNext = NULL;
        }
    }
    
    free(pHashTable->m_ppElementArray);
    pHashTable->m_ppElementArray = NULL;
    free(pHashTable);
    pHashTable = NULL;
}
