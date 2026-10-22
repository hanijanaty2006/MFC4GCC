// =====================================================================
//  afxtempl.h  --  MFC style collection templates (CArray/CList/CMap)
//                  for the MFC-GCC clone.  C++11, no MSVC required.
// =====================================================================
#ifndef __AFXTEMPL_H__
#define __AFXTEMPL_H__
#pragma once

#ifndef __AFX_H__
#include "afx.h"
#endif

// MFC uses an opaque pointer for enumeration positions.
typedef void* POSITION;

// ---------------------------------------------------------------------
//  CArray -- indexable array (std::vector based)
// ---------------------------------------------------------------------
template <class TYPE, class ARG_TYPE>
class CArray
{
public:
    CArray() {}
    ~CArray() {}

    int GetSize() const          { return (int)m_data.size(); }
    int GetCount() const         { return GetSize(); }
    int GetUpperBound() const    { return (int)m_data.size() - 1; }
    BOOL IsEmpty() const         { return m_data.empty() ? TRUE : FALSE; }
    void FreeExtra()             { m_data.shrink_to_fit(); }
    void RemoveAll()             { m_data.clear(); }

    void SetSize(int nNewSize, int nGrowBy = -1)
    {
        if (nGrowBy > 0) m_nGrowBy = nGrowBy;
        if (nNewSize < 0) nNewSize = 0;
        m_data.resize((size_t)nNewSize);
    }

    const TYPE& GetAt(int nIndex) const  { return m_data[(size_t)nIndex]; }
    TYPE& GetAt(int nIndex)              { return m_data[(size_t)nIndex]; }
    const TYPE& ElementAt(int nIndex) const { return m_data[(size_t)nIndex]; }
    TYPE& ElementAt(int nIndex)             { return m_data[(size_t)nIndex]; }
    const TYPE& operator[](int nIndex) const { return m_data[(size_t)nIndex]; }
    TYPE& operator[](int nIndex)             { return m_data[(size_t)nIndex]; }
    void SetAt(int nIndex, ARG_TYPE newElement) { m_data[(size_t)nIndex] = newElement; }
    void SetAtGrow(int nIndex, ARG_TYPE newElement);

    int Add(ARG_TYPE newElement) { m_data.push_back(newElement); return (int)m_data.size() - 1; }
    int Append(const CArray<TYPE, ARG_TYPE>& src);
    void Copy(const CArray<TYPE, ARG_TYPE>& src) { m_data = src.m_data; }

    void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);
    void InsertAt(int nIndex, CArray<TYPE, ARG_TYPE>* pNewArray);
    void RemoveAt(int nIndex, int nCount = 1);

    TYPE* GetData()              { return m_data.empty() ? NULL : &m_data[0]; }
    const TYPE* GetData() const  { return m_data.empty() ? NULL : &m_data[0]; }

    TYPE* begin() { return m_data.empty() ? NULL : &m_data[0]; }
    TYPE* end()   { return m_data.empty() ? NULL : &m_data[0] + m_data.size(); }

protected:
    std::vector<TYPE> m_data;
    int m_nGrowBy = 0;
};

template <class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
    if (nIndex >= (int)m_data.size()) SetSize(nIndex + 1);
    m_data[(size_t)nIndex] = newElement;
}

template <class TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::Append(const CArray<TYPE, ARG_TYPE>& src)
{
    int nIndex = (int)m_data.size();
    m_data.insert(m_data.end(), src.m_data.begin(), src.m_data.end());
    return nIndex;
}

template <class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount)
{
    if (nIndex < 0) nIndex = 0;
    if (nIndex > (int)m_data.size()) nIndex = (int)m_data.size();
    m_data.insert(m_data.begin() + nIndex, (size_t)nCount, newElement);
}

template <class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::InsertAt(int nIndex, CArray<TYPE, ARG_TYPE>* pNewArray)
{
    if (pNewArray == NULL) return;
    if (nIndex < 0) nIndex = 0;
    if (nIndex > (int)m_data.size()) nIndex = (int)m_data.size();
    m_data.insert(m_data.begin() + nIndex, pNewArray->m_data.begin(), pNewArray->m_data.end());
}

template <class TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::RemoveAt(int nIndex, int nCount)
{
    if (nIndex < 0 || nIndex >= (int)m_data.size()) return;
    if (nCount < 0) nCount = 1;
    if (nIndex + nCount > (int)m_data.size()) nCount = (int)m_data.size() - nIndex;
    m_data.erase(m_data.begin() + nIndex, m_data.begin() + nIndex + nCount);
}

// ---------------------------------------------------------------------
//  CList -- doubly linked list, POSITION == internal node pointer
// ---------------------------------------------------------------------
template <class TYPE, class ARG_TYPE>
class CList
{
public:
    CList(int nBlockSize = 10) : m_pHead(NULL), m_pTail(NULL), m_nCount(0), m_nBlockSize(nBlockSize) {}
    ~CList() { RemoveAll(); }

    int GetCount() const  { return m_nCount; }
    int GetSize() const   { return m_nCount; }
    BOOL IsEmpty() const  { return m_nCount == 0 ? TRUE : FALSE; }

    const TYPE& GetHead() const { return m_pHead->data; }
    TYPE& GetHead()             { return m_pHead->data; }
    const TYPE& GetTail() const { return m_pTail->data; }
    TYPE& GetTail()             { return m_pTail->data; }

    POSITION GetHeadPosition() const { return (POSITION)m_pHead; }
    POSITION GetTailPosition() const { return (POSITION)m_pTail; }
    TYPE& GetNext(POSITION& rPosition);
    const TYPE& GetNext(POSITION& rPosition) const;
    TYPE& GetPrev(POSITION& rPosition);
    const TYPE& GetPrev(POSITION& rPosition) const;

    POSITION AddHead(ARG_TYPE newElement);
    POSITION AddTail(ARG_TYPE newElement);
    void AddHead(CList<TYPE, ARG_TYPE>* pNewList);
    void AddTail(CList<TYPE, ARG_TYPE>* pNewList);

    TYPE RemoveHead();
    TYPE RemoveTail();
    void RemoveAt(POSITION position);
    void RemoveAll();

    TYPE& GetAt(POSITION position);
    const TYPE& GetAt(POSITION position) const;
    void SetAt(POSITION position, ARG_TYPE newElement);
    POSITION InsertBefore(POSITION position, ARG_TYPE newElement);
    POSITION InsertAfter(POSITION position, ARG_TYPE newElement);

    POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL) const;
    POSITION FindIndex(int nIndex) const;

protected:
    struct CNode
    {
        CNode* pNext;
        CNode* pPrev;
        TYPE   data;
        CNode() : pNext(NULL), pPrev(NULL), data() {}
        CNode(ARG_TYPE d) : pNext(NULL), pPrev(NULL), data(d) {}
    };
    CNode* m_pHead;
    CNode* m_pTail;
    int    m_nCount;
    int    m_nBlockSize;
};

template <class TYPE, class ARG_TYPE>
TYPE& CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition)
{
    CNode* pNode = (CNode*)rPosition;
    rPosition = (POSITION)pNode->pNext;
    return pNode->data;
}

template <class TYPE, class ARG_TYPE>
const TYPE& CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) const
{
    CNode* pNode = (CNode*)rPosition;
    rPosition = (POSITION)pNode->pNext;
    return pNode->data;
}

template <class TYPE, class ARG_TYPE>
TYPE& CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition)
{
    CNode* pNode = (CNode*)rPosition;
    rPosition = (POSITION)pNode->pPrev;
    return pNode->data;
}

template <class TYPE, class ARG_TYPE>
const TYPE& CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) const
{
    CNode* pNode = (CNode*)rPosition;
    rPosition = (POSITION)pNode->pPrev;
    return pNode->data;
}

template <class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
    CNode* pNode = new CNode(newElement);
    pNode->pNext = m_pHead;
    if (m_pHead != NULL) m_pHead->pPrev = pNode;
    else m_pTail = pNode;
    m_pHead = pNode;
    ++m_nCount;
    return (POSITION)pNode;
}

template <class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
    CNode* pNode = new CNode(newElement);
    pNode->pPrev = m_pTail;
    if (m_pTail != NULL) m_pTail->pNext = pNode;
    else m_pHead = pNode;
    m_pTail = pNode;
    ++m_nCount;
    return (POSITION)pNode;
}

template <class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddHead(CList<TYPE, ARG_TYPE>* pNewList)
{
    if (pNewList == NULL) return;
    POSITION pos = pNewList->GetTailPosition();
    while (pos != NULL) AddHead(pNewList->GetPrev(pos));
}

template <class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddTail(CList<TYPE, ARG_TYPE>* pNewList)
{
    if (pNewList == NULL) return;
    POSITION pos = pNewList->GetHeadPosition();
    while (pos != NULL) AddTail(pNewList->GetNext(pos));
}

template <class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveHead()
{
    CNode* pNode = m_pHead;
    TYPE data = pNode->data;
    RemoveAt((POSITION)pNode);
    return data;
}

template <class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveTail()
{
    CNode* pNode = m_pTail;
    TYPE data = pNode->data;
    RemoveAt((POSITION)pNode);
    return data;
}

template <class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAt(POSITION position)
{
    CNode* pNode = (CNode*)position;
    if (pNode == NULL) return;
    if (pNode->pPrev != NULL) pNode->pPrev->pNext = pNode->pNext;
    else m_pHead = pNode->pNext;
    if (pNode->pNext != NULL) pNode->pNext->pPrev = pNode->pPrev;
    else m_pTail = pNode->pPrev;
    --m_nCount;
    delete pNode;
}

template <class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAll()
{
    CNode* pNode = m_pHead;
    while (pNode != NULL) { CNode* pNext = pNode->pNext; delete pNode; pNode = pNext; }
    m_pHead = m_pTail = NULL;
    m_nCount = 0;
}

template <class TYPE, class ARG_TYPE>
TYPE& CList<TYPE, ARG_TYPE>::GetAt(POSITION position) { return ((CNode*)position)->data; }
template <class TYPE, class ARG_TYPE>
const TYPE& CList<TYPE, ARG_TYPE>::GetAt(POSITION position) const { return ((CNode*)position)->data; }
template <class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::SetAt(POSITION position, ARG_TYPE newElement) { ((CNode*)position)->data = newElement; }

template <class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertBefore(POSITION position, ARG_TYPE newElement)
{
    CNode* pOld = (CNode*)position;
    if (pOld == NULL) return AddHead(newElement);
    CNode* pNode = new CNode(newElement);
    pNode->pNext = pOld;
    pNode->pPrev = pOld->pPrev;
    if (pOld->pPrev != NULL) pOld->pPrev->pNext = pNode; else m_pHead = pNode;
    pOld->pPrev = pNode;
    ++m_nCount;
    return (POSITION)pNode;
}

template <class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertAfter(POSITION position, ARG_TYPE newElement)
{
    CNode* pOld = (CNode*)position;
    if (pOld == NULL) return AddTail(newElement);
    CNode* pNode = new CNode(newElement);
    pNode->pPrev = pOld;
    pNode->pNext = pOld->pNext;
    if (pOld->pNext != NULL) pOld->pNext->pPrev = pNode; else m_pTail = pNode;
    pOld->pNext = pNode;
    ++m_nCount;
    return (POSITION)pNode;
}

template <class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSITION startAfter) const
{
    CNode* pNode = (startAfter == NULL) ? m_pHead : ((CNode*)startAfter)->pNext;
    for (; pNode != NULL; pNode = pNode->pNext)
        if (pNode->data == searchValue) return (POSITION)pNode;
    return NULL;
}

template <class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::FindIndex(int nIndex) const
{
    if (nIndex < 0 || nIndex >= m_nCount) return NULL;
    CNode* pNode = m_pHead;
    for (int i = 0; i < nIndex && pNode != NULL; ++i) pNode = pNode->pNext;
    return (POSITION)pNode;
}

// key hashing: overloaded so that pointer, integral and string keys all work
AFX_INLINE UINT AfxHashKey(const void* p)   { return ((UINT)(UINT_PTR)p) >> 4; }
AFX_INLINE UINT AfxHashKey(UINT_PTR n)      { return ((UINT)n) >> 4; }
AFX_INLINE UINT AfxHashKey(const CStringA& key)
{
    UINT nHash = 0;
    const char* p = key;
    while (*p) nHash = (nHash * 33) + (UINT)(unsigned char)*p++;
    return nHash;
}
AFX_INLINE UINT AfxHashKey(const CStringW& key)
{
    UINT nHash = 0;
    const wchar_t* p = key;
    while (*p) nHash = (nHash * 33) + (UINT)*p++;
    return nHash;
}

// ---------------------------------------------------------------------
//  CMap -- hash table with MFC semantics
// ---------------------------------------------------------------------
template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMap
{
public:
    CMap(int nBlockSize = 17)
        : m_pHashTable(NULL), m_nHashTableSize(0), m_nCount(0), m_pFreeList(NULL), m_nBlockSize(nBlockSize) {}
    ~CMap() { RemoveAll(); }

    int GetCount() const  { return m_nCount; }
    int GetSize() const   { return m_nCount; }
    BOOL IsEmpty() const  { return m_nCount == 0 ? TRUE : FALSE; }

    BOOL Lookup(ARG_KEY key, VALUE& rValue) const;
    VALUE& operator[](ARG_KEY key);
    void SetAt(ARG_KEY key, ARG_VALUE newValue);
    BOOL RemoveKey(ARG_KEY key);
    void RemoveAll();

    POSITION GetStartPosition() const;
    void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

    UINT GetHashTableSize() const { return m_nHashTableSize; }
    void InitHashTable(UINT nHashSize, BOOL bAllocNow = TRUE);

protected:
    struct CAssoc
    {
        CAssoc* pNext;
        UINT    nHashCode;
        KEY     key;
        VALUE   value;
    };
    CAssoc** m_pHashTable;
    UINT     m_nHashTableSize;
    int      m_nCount;
    CAssoc*  m_pFreeList;
    int      m_nBlockSize;
    std::vector<CAssoc*> m_pBlocks;

    CAssoc* NewAssoc();
    void FreeAssoc(CAssoc* pAssoc);
    CAssoc* GetAssocAt(ARG_KEY key, UINT& nHash) const;
    static UINT PASCAL HashKey(ARG_KEY key) { return AfxHashKey(key); }
};

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc()
{
    if (m_pFreeList == NULL) {
        CAssoc* pNew = new CAssoc[m_nBlockSize];
        m_pBlocks.push_back(pNew);
        for (int i = m_nBlockSize - 1; i >= 0; --i) { pNew[i].pNext = m_pFreeList; m_pFreeList = &pNew[i]; }
    }
    CAssoc* pAssoc = m_pFreeList;
    m_pFreeList = m_pFreeList->pNext;
    ++m_nCount;
    return pAssoc;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(CAssoc* pAssoc)
{
    pAssoc->pNext = m_pFreeList;
    m_pFreeList = pAssoc;
    --m_nCount;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHash) const
{
    nHash = HashKey(key);
    if (m_pHashTable == NULL) return NULL;
    CAssoc* pAssoc = m_pHashTable[nHash % m_nHashTableSize];
    for (; pAssoc != NULL; pAssoc = pAssoc->pNext)
        if (pAssoc->nHashCode == nHash && pAssoc->key == key) return pAssoc;
    return NULL;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
    UINT nHash;
    CAssoc* pAssoc = GetAssocAt(key, nHash);
    if (pAssoc == NULL) return FALSE;
    rValue = pAssoc->value;
    return TRUE;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
{
    UINT nHash;
    CAssoc* pAssoc = GetAssocAt(key, nHash);
    if (pAssoc == NULL) {
        if (m_pHashTable == NULL) InitHashTable(17);
        pAssoc = NewAssoc();
        pAssoc->nHashCode = nHash;
        pAssoc->key = key;
        pAssoc->value = newValue;
        pAssoc->pNext = m_pHashTable[nHash % m_nHashTableSize];
        m_pHashTable[nHash % m_nHashTableSize] = pAssoc;
    } else {
        pAssoc->value = newValue;
    }
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
    UINT nHash;
    CAssoc* pAssoc = GetAssocAt(key, nHash);
    if (pAssoc == NULL) {
        if (m_pHashTable == NULL) InitHashTable(17);
        pAssoc = NewAssoc();
        pAssoc->nHashCode = nHash;
        pAssoc->key = key;
        pAssoc->value = VALUE();
        pAssoc->pNext = m_pHashTable[nHash % m_nHashTableSize];
        m_pHashTable[nHash % m_nHashTableSize] = pAssoc;
    }
    return pAssoc->value;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
{
    if (m_pHashTable == NULL) return FALSE;
    UINT nHash = HashKey(key);
    CAssoc** ppAssocPrev = &m_pHashTable[nHash % m_nHashTableSize];
    while (*ppAssocPrev != NULL) {
        CAssoc* pAssoc = *ppAssocPrev;
        if (pAssoc->nHashCode == nHash && pAssoc->key == key) {
            *ppAssocPrev = pAssoc->pNext;
            FreeAssoc(pAssoc);
            return TRUE;
        }
        ppAssocPrev = &pAssoc->pNext;
    }
    return FALSE;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
    for (UINT i = 0; i < m_nHashTableSize; ++i) {
        CAssoc* pAssoc = m_pHashTable[i];
        while (pAssoc != NULL) { CAssoc* pNext = pAssoc->pNext; FreeAssoc(pAssoc); pAssoc = pNext; }
        m_pHashTable[i] = NULL;
    }
    for (size_t b = 0; b < m_pBlocks.size(); ++b) delete[] m_pBlocks[b];
    m_pBlocks.clear();
    m_pFreeList = NULL;
    delete[] m_pHashTable;
    m_pHashTable = NULL;
    m_nHashTableSize = 0;
    m_nCount = 0;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(UINT nHashSize, BOOL bAllocNow)
{
    if (m_pHashTable != NULL) { RemoveAll(); }
    if (nHashSize == 0) nHashSize = 17;
    m_nHashTableSize = nHashSize;
    if (bAllocNow) {
        m_pHashTable = new CAssoc*[m_nHashTableSize];
        for (UINT i = 0; i < m_nHashTableSize; ++i) m_pHashTable[i] = NULL;
    }
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
POSITION CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
{
    if (m_nCount == 0) return NULL;
    for (UINT nBucket = 0; nBucket < m_nHashTableSize; ++nBucket) {
        CAssoc* pAssoc = m_pHashTable[nBucket];
        for (; pAssoc != NULL; pAssoc = pAssoc->pNext)
            if (pAssoc != m_pFreeList) return (POSITION)pAssoc;
    }
    return NULL;
}

template <class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const
{
    CAssoc* pAssocRet = (CAssoc*)rNextPosition;
    rKey = pAssocRet->key;
    rValue = pAssocRet->value;

    // advance
    CAssoc* pAssoc = pAssocRet->pNext;
    if (pAssoc != NULL) { rNextPosition = (POSITION)pAssoc; return; }
    // next bucket
    UINT nBucket;
    for (nBucket = pAssocRet->nHashCode % m_nHashTableSize + 1; nBucket < m_nHashTableSize; ++nBucket) {
        pAssoc = m_pHashTable[nBucket];
        if (pAssoc != NULL) break;
    }
    rNextPosition = (nBucket < m_nHashTableSize) ? (POSITION)pAssoc : NULL;
}

// ---------------------------------------------------------------------
//  The non-template "legacy" collections MFC code expects
// ---------------------------------------------------------------------
class CStringArray : public CArray<CString, LPCTSTR>
{
public:
    CStringArray() {}
};
class CStringList : public CList<CString, LPCTSTR>
{
public:
    CStringList(int nBlockSize = 10) : CList<CString, LPCTSTR>(nBlockSize) {}
};
class CPtrArray : public CArray<void*, void*>
{
public:
    CPtrArray() {}
};
class CPtrList : public CList<void*, void*>
{
public:
    CPtrList(int nBlockSize = 10) : CList<void*, void*>(nBlockSize) {}
};
class CObArray : public CArray<CObject*, CObject*>
{
public:
    CObArray() {}
};
class CObList : public CList<CObject*, CObject*>
{
public:
    CObList(int nBlockSize = 10) : CList<CObject*, CObject*>(nBlockSize) {}
};
class CWordArray : public CArray<WORD, WORD>
{
public:
    CWordArray() {}
};
class CDWordArray : public CArray<DWORD, DWORD>
{
public:
    CDWordArray() {}
};
class CUIntArray : public CArray<UINT, UINT>
{
public:
    CUIntArray() {}
};
class CIntArray : public CArray<int, int>
{
public:
    CIntArray() {}
};

class CMapStringToPtr : public CMap<CString, LPCTSTR, void*, void*>
{
public:
    CMapStringToPtr(int nBlockSize = 17) : CMap<CString, LPCTSTR, void*, void*>(nBlockSize) {}
    BOOL Lookup(LPCTSTR key, void*& rValue) const
    { return CMap<CString, LPCTSTR, void*, void*>::Lookup(CString(key), rValue); }
    void SetAt(LPCTSTR key, void* newValue)
    { CMap<CString, LPCTSTR, void*, void*>::SetAt(CString(key), newValue); }
    BOOL RemoveKey(LPCTSTR key)
    { return CMap<CString, LPCTSTR, void*, void*>::RemoveKey(CString(key)); }
};
class CMapStringToOb : public CMap<CString, LPCTSTR, CObject*, CObject*>
{
public:
    CMapStringToOb(int nBlockSize = 17) : CMap<CString, LPCTSTR, CObject*, CObject*>(nBlockSize) {}
};
class CMapWordToPtr : public CMap<WORD, WORD, void*, void*>
{
public:
    CMapWordToPtr(int nBlockSize = 17) : CMap<WORD, WORD, void*, void*>(nBlockSize) {}
};
class CMapPtrToWord : public CMap<void*, void*, WORD, WORD>
{
public:
    CMapPtrToWord(int nBlockSize = 17) : CMap<void*, void*, WORD, WORD>(nBlockSize) {}
};
class CMapPtrToPtr : public CMap<void*, void*, void*, void*>
{
public:
    CMapPtrToPtr(int nBlockSize = 17) : CMap<void*, void*, void*, void*>(nBlockSize) {}
};
class CMapWordToOb : public CMap<WORD, WORD, CObject*, CObject*>
{
public:
    CMapWordToOb(int nBlockSize = 17) : CMap<WORD, WORD, CObject*, CObject*>(nBlockSize) {}
};

#endif /* __AFXTEMPL_H__ */
