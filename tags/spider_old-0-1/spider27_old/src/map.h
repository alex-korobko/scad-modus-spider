#ifndef __MAP_H__
#define __MAP_H__

#include <assert.h>
#include "goodies.h"

template <class T> class Map
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
	dword	*MapTable;
public:
	Map();
	virtual ~Map();	
	T* operator[](int i);
	void Remove(T* pData);
	void Push(T* pData, dword Key = 0);
	T* Pop();
	void Traverse(void (*func)(T*));
	int Search(int (*func)(T*));
	T* GetByKey(dword Key);
	dword GetSize() { return Size; }
	T* First();
	T* Last();
	T* Next();
	void Reset() { Cursor = Head; }
};

template <class T> Map<T>::Map()
{
	Head = Tail = new Node;
	assert(Head);
	Head->Next = Head->Prev = NULL;
	Head->Data = NULL;
	HashTable = NULL;
	MapTable = NULL;
	HashSize = Size = 0;
}

template <class T> Map<T>::~Map()
{
	while(Head->Next)
	{
		Node*	Kill = Head;	
		Head = Head->Next;
		delete Kill;
	}

	delete[] HashTable;
	delete[] MapTable;
	delete Head;
}

template <class T> T* Map<T>::operator[](int i)
{
	if (Size)
		return HashTable[i];
	else
		return NULL;
}

template <class T> void Map<T>::Push(T* pData, dword Key)
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
				
		dword* MapTemp = new dword[2*(HashSize + 64)];
		assert(MapTemp);
		memcpy(MapTemp, MapTable, 2*HashSize*sizeof(dword));
		delete[] MapTable;
		MapTable = MapTemp;
		
		HashSize += 64;
	}
	HashTable[Size - 1] = pData;
	MapTable[2*(Size - 1)] = Key;
	MapTable[2*(Size - 1) + 1] = (dword)pData;
}

template <class T> void Map<T>::Remove(T* pData)
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

template <class T> T* Map<T>::Pop()
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

template <class T> void Map<T>::Traverse(void (*func)(T*))
{
	Node* Current = Head;

	while((Current = Current->Next) != NULL)
	{
		func(Current->Data);
	}
}

template <class T> int Map<T>::Search(int (*func)(T*))
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

template <class T> T* Map<T>::GetByKey(dword Key)
{
	for(dword i=0; i<Size*2; i+=2)
	{
		if (MapTable[i] == Key && MapTable[i]!= 0)
			return (T*)MapTable[i+1];
	}
	
	return NULL;
}

template <class T> T* Map<T>::Next()
{
	if(!Cursor->Next)
		return NULL;

	Cursor = Cursor->Next;

	return Cursor->Data;
}

template <class T> T* Map<T>::First()
{
	Cursor = Head->Next;

	if (Cursor)
		return Cursor->Data;
	else
		return NULL;
}

template <class T> T* Map<T>::Last()
{
	Cursor = Tail;

	return Cursor->Data;
}

#endif
