#ifndef __LIST_H__
#define __LIST_H__

#include <assert.h>
#include "types.h"

template <class T> class List
{
protected:	
	typedef struct tagNode
	{
		tagNode*	Prev;
		tagNode*	Next;
		T*			Data;
	} Node;
	Node*	Head;
	Node*	Tail;
	Node*	Cursor;
	dword	Size;
	dword   HashSize;
	T** HashTable;
public:
	List();
	virtual ~List();	
	T* operator[](int i);
	void Remove(T* pData);
	void Push(T* pData);
	T* Pop();
	T* PopHead();
	void Traverse(void (*func)(T*));
	int Search(int (*func)(T*));
	dword GetSize() { return Size; }
	void Reset() { Cursor = Head; }
	T* LastItem() { return Tail->Data; }
	T* NextItem();
};

template <class T> List<T>::List()
{
	Head = Tail = new Node;
	assert(Head);
	Head->Next = Head->Prev = NULL;
	Head->Data = NULL;
	HashTable = NULL;
	HashSize = Size = 0;
}

template <class T> List<T>::~List()
{
	while(Head->Next)
	{
		Node*	Kill = Head;	
		Head = Head->Next;
		delete Kill;
	}

	delete[] HashTable;
	delete Head;
}

template <class T> T* List<T>::operator[](int i)
{
	return HashTable[i];	
}

template <class T> void List<T>::Push(T* pData)
{
	Node* Prev = Tail;
	Tail->Next = new Node;
	assert(Tail->Next);
	Tail = Tail->Next;
	Tail->Data = pData;
	Tail->Prev = Prev;
	Tail->Next = NULL;
	Size++;
	if (Size > HashSize)
	{
		T** Temp = new (T*)[HashSize + 64];
		assert(Temp);
		memcpy(Temp, HashTable, HashSize*sizeof(T*));
		delete[] HashTable;
		HashTable = Temp;
		HashSize += 64;
	}
	HashTable[Size - 1] = pData;
}

template <class T> void List<T>::Remove(T* pData)
{
	dword Count = 0;
	Node* Current = Head;

	while((Current->Data != pData) && Current->Next)
	{
		Current = Current->Next;
		Count++;
	}
	if (Current)
	{
		Current->Next->Prev = Current->Prev;
		Current->Prev->Next = Current->Next;
		delete Current;
		Size--;
		memcpy(HashTable[Count], HashTable[Count+1], Size - Count);
		HashTable[Size] = NULL;
	}
}

template <class T> T* List<T>::Pop()
{
	Node* Current = Tail;
	if (!Tail->Prev)
		return NULL;
	Tail = Tail->Prev;
	Tail->Next = NULL;
	T* pData = Current->Data;	
	delete Current;
	Size--;
	HashTable[Size] = NULL;

	return pData;
}

template <class T> T* List<T>::PopHead()
{
	Node* Current = Head->Next;
	if (!Current)
	{
		printf("**** %lu\n", Size);
		return NULL;
	}
	Head->Next = Current->Next;
	if (Head->Next != NULL)
		Head->Next->Prev = Head;
		
	T* pData = Current->Data;	
	delete Current;
	Size--;
	HashTable[Size] = NULL;

	if (!pData)
		printf("++++\n");

	return pData;
}

template <class T> void List<T>::Traverse(void (*func)(T*))
{
	Node* Current = Head;

	while((Current = Current->Next) != NULL)
	{
		func(Current->Data);
	}
}

template <class T> int List<T>::Search(int (*func)(T*))
{
	Node* Current = Head;
	int Count = 0;

	while((Current = Current->Next) != NULL)
	{
		if (func(Current->Data))
			return Count;
		Count++;
	}
	
	return -1;
}

template <class T> T* List<T>::NextItem()
{
	if(!Cursor->Next)
		return NULL;

	Cursor = Cursor->Next;

	return Cursor->Data;
}

#endif
