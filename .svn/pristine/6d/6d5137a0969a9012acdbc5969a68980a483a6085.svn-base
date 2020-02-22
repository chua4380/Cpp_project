#ifndef _CELLObjectPool_hpp_
#define _CELLObjectPool_hpp_
#include <stdlib.h>
#include <mutex>
#include <assert.h>
#ifdef _DEBUG
	#ifndef xPrintf
		#include<stdio.h>
		#define xPrintf(...) printf(__VA_ARGS__)
	#endif // !xPrintf
#else
	#ifndef xPrintf
		#define xPrintf(...)
	#endif // !xPrintf
#endif

template<class Type, size_t nPoolSize>
class CELLObjectPool
{
public:
	CELLObjectPool()
	{
		_pBuf = nullptr;
		initPool();
	}

	~CELLObjectPool()
	{
		if (_pBuf)
			delete[] _pBuf;
	}
private:
	class NodeHeader
	{
	public:
		//下一块位置
		NodeHeader* pNext;
		//内存块编号
		int nID;
		//引用次数
		char nRef;
		//是否在内存池中
		bool bPool;
	private:
		//预留值
		char c1;
		char c2;
	};
public:
	//释放对象内存
	void freeObjMemory(void* pMem)
	{
		NodeHeader*  pBlock = (NodeHeader*)((char*)pMem - sizeof(NodeHeader));
		assert(1 == pBlock->nRef);
		if (pBlock->bPool)
		{
			std::lock_guard<std::mutex> lg(_mutex);
			//回收的内存再次利用
			if (--pBlock->nRef != 0)
			{
				return;
			}
			pBlock->pNext = _pHeader;
			_pHeader = pBlock;
			xPrintf("freeObjMemory: %llx, id=%d\n", pBlock, pBlock->nID);
		}
		else {
			if (--pBlock->nRef != 0)
			{
				return;
			}
			xPrintf("freeObj by system: %llx, id=%d\n", pBlock, pBlock->nID);
			delete[] pBlock;  //释放一个完整的内存块
		}
	}
	//申请对象内存
	void* allocObjMemory(size_t nSize)
	{
		std::lock_guard<std::mutex> lg(_mutex);
		NodeHeader* pReturn = nullptr;
		//如果内存池中没有可用的内存块，向系统重新申请
		if (nullptr == _pHeader)
		{
			pReturn = (NodeHeader*)new char[sizeof(Type) + sizeof(NodeHeader)];
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pNext = nullptr;
		}
		else {
			pReturn = _pHeader;     //_pHeader指向当前可用内存
			_pHeader = _pHeader->pNext;
			assert(0 == pReturn->nRef);
			pReturn->nRef = 1;
		}
		xPrintf("allocObjMemory: %llx, id=%d, size=%d\n", pReturn, pReturn->nID, (int)nSize);
		return ((char*)pReturn + sizeof(NodeHeader));
	}
private:
	//初始化对象池
	void initPool()
	{
		//断言
		assert(nullptr == _pBuf);
		if (_pBuf)
			return;
		//计算对象池的大小
		size_t realSize = sizeof(Type) + sizeof(NodeHeader);
		size_t n = nPoolSize * realSize;
		//申请池的内存
		_pBuf = new char[n];
		//初始化内存池
		_pHeader = (NodeHeader*)_pBuf;
		_pHeader->bPool = true;
		_pHeader->nID = 0;
		_pHeader->nRef = 0;
		_pHeader->pNext = nullptr;
		//遍历内存块并进行初始化
		NodeHeader* pTemp1 = _pHeader;
		for (size_t n = 1; n < nPoolSize; n++)
		{
			NodeHeader* pTemp2 = (NodeHeader*)(_pBuf + (n * realSize));
			pTemp2->bPool = true;
			pTemp2->nID = (int)n;
			pTemp2->nRef = 0;
			pTemp2->pNext = nullptr;
			pTemp1->pNext = pTemp2;
			pTemp1 = pTemp2;

		}
	}
private:
	NodeHeader* _pHeader;
	//对象池内存缓冲区地址
	char* _pBuf;
	std::mutex _mutex;
};

template<class Type, size_t nPoolSize>
class ObjectPoolBase
{
public:
	void* operator new(size_t nSize)
	{
		return objectPool().allocObjMemory(nSize);
	}

	void operator delete(void* p)
	{
		objectPool().freeObjMemory(p);
	}
	template<typename ...Args>
	static Type* createObject(Args ... args)
	{   //不定参数 可变参数
		Type* obj = new Type(args...);
		//可以做点想做的事
		return obj;
	}

	static void destoryObject(Type* obj)
	{
		delete obj;
	}

private:

	typedef CELLObjectPool<Type, nPoolSize> ClassTypePool;

	static ClassTypePool& objectPool()
	{   //静态对象池对象
		static ClassTypePool sPool;
		return sPool;
	}
};

#endif // !_CELLObjectPool_hpp_
