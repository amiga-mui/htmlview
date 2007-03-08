#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <proto/dos.h>
#include <proto/exec.h>

//#define MEMWATCH
#define THREAD_SAFE
#define POOL_SIZE (10*1024)

#ifndef PROGRAM
#define PROGRAM FindTask(NULL)->tc_Node.ln_Name
#endif

#ifdef THREAD_SAFE
struct SignalSemaphore MemorySemaphore;
#endif

APTR MemoryPool = NULL;

struct Pool
{
	VOID Init (ULONG size)
	{
//		Allocated = Freed = 0;
		PoolSize = size;
	}

	ULONG Allocated;
	ULONG Freed;
	ULONG PoolSize;
	UBYTE Data[POOL_SIZE];
};

struct Memory
{
	struct Pool *SrcPool;
	ULONG Size;
//	UBYTE Tag[8];
	UBYTE DataStart;
};

struct Pool *CurrentPool;

inline struct Pool* NewPool(ULONG size)
{
  ENTER();

	struct Pool *result;
	do
  {
		result = (Pool*)AllocPooled(MemoryPool, size);
	}
  while(!result);
	
  result->Init(size);

  RETURN(result);
	return(result);
}

APTR operator new(size_t bytes)
{
  ENTER();

	bytes = (offsetof(Memory, DataStart)+bytes+8) & ~7;

  SHOWVALUE(DBF_STARTUP, bytes);

#ifdef THREAD_SAFE
	ObtainSemaphore(&MemorySemaphore);
#endif

	struct Pool *pool = CurrentPool;
	if(bytes > POOL_SIZE)
	{
		ULONG pool_size = offsetof(Pool, Data)+bytes;
		pool = NewPool(pool_size);
	}
	else if(CurrentPool->Allocated + bytes > POOL_SIZE)
	{
		pool = NewPool(sizeof(Pool));
		CurrentPool = pool;
	}

	struct Memory *mem = (struct Memory *)&pool->Data[pool->Allocated];
	pool->Allocated += bytes;

#ifdef THREAD_SAFE
	ReleaseSemaphore(&MemorySemaphore);
#endif

	mem->SrcPool = pool;
	mem->Size = bytes;
//	strncpy(&mem->Tag[0], "C0DEDBAD", 8);

  RETURN(&mem->DataStart);
	return(&mem->DataStart);
}

VOID operator delete (APTR mem, size_t bytes)
{
	if(mem)
	{
		struct Memory *chunk = (struct Memory *)((ULONG)mem - offsetof(Memory, DataStart));
		struct Pool *pool = chunk->SrcPool;

/*		if(strncmp(chunk->Tag, "C0DEDBAD", 8))
			printf("*** broken: %s\n", &chunk->DataStart);
*/
#ifdef THREAD_SAFE
		ObtainSemaphore(&MemorySemaphore);
#endif
		pool->Freed += chunk->Size;
		if(pool->Freed == pool->Allocated)
		{
			if(pool != CurrentPool)
			{
				FreePooled(MemoryPool, pool, pool->PoolSize);
			}
			else
			{
				pool->Allocated = pool->Freed = 0;
				memset(pool->Data, 0, pool->PoolSize - offsetof(Pool, Data));
			}
		}
#ifdef THREAD_SAFE
		ReleaseSemaphore(&MemorySemaphore);
#endif
	}
}

VOID _INIT_4_InitMem ()
{
#ifdef THREAD_SAFE
	memset(&MemorySemaphore, 0, sizeof(struct SignalSemaphore));
	InitSemaphore(&MemorySemaphore);
#endif
	MemoryPool = CreatePool(MEMF_CLEAR | MEMF_ANY, 12*1024, 6*1024);

	CurrentPool = (Pool *)AllocPooled(MemoryPool, sizeof(Pool));
	CurrentPool->Init(sizeof(Pool));
}

VOID _EXIT_4_CleanupMem ()
{
	if(MemoryPool)
	{
		FreePooled(MemoryPool, CurrentPool, CurrentPool->PoolSize);
		DeletePool(MemoryPool);
	}
}
