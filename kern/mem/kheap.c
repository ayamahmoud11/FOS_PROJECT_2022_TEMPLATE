#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//==================================================================//
//==================================================================//
//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)//
//==================================================================//
//==================================================================//

void initialize_dyn_block_system()
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] initialize_dyn_block_system
			// your code is here, remove the panic and write your code
			//kpanic_into_prompt("initialize_dyn_block_system() is not implemented yet...!!");
			//[1] Initialize two lists (AllocMemBlocksList & FreeMemBlocksList) [Hint: use LIST_INIT()]
			LIST_INIT(&AllocMemBlocksList);
			LIST_INIT(&FreeMemBlocksList);

		#if STATIC_MEMBLOCK_ALLOC
			//DO NOTHING
		#else
			uint32 s = KERNEL_HEAP_START ,h = KERNEL_HEAP_MAX;
			MAX_MEM_BLOCK_CNT = NUM_OF_KHEAP_PAGES;
		    MemBlockNodes = (struct MemBlock*)s;
		    uint32 hesi = h - s;
			uint32 si = ROUNDUP(NUM_OF_KHEAP_PAGES * sizeof(struct MemBlock), PAGE_SIZE);
			uint32 rems =hesi-si;
			allocate_chunk(ptr_page_directory ,s,si ,PERM_WRITEABLE);
			/*[2] Dynamically allocate the array of MemBlockNodes
			 * 	remember to:
			 * 		1. set MAX_MEM_BLOCK_CNT with the chosen size of the array
			 * 		2. allocation should be aligned on PAGE boundary
			 * 	HINT: can use alloc_chunk(...) function
             */
		#endif
			initialize_MemBlocksList(MAX_MEM_BLOCK_CNT);
			struct MemBlock *p2=NULL;
			p2= LIST_LAST(&AvailableMemBlocksList);
			p2->size = rems;
			p2->sva = s + si;
			LIST_REMOVE(&AvailableMemBlocksList,p2);
			LIST_INSERT_HEAD(&FreeMemBlocksList , p2);

			//[3] Initialize AvailableMemBlocksList by filling it with the MemBlockNodes
			//[4] Insert a new MemBlock with the remaining heap size into the FreeMemBlocksList
}

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kmalloc
	// your code is here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	//change this "return" according to your answer
	 struct MemBlock *block,*block1,*block2; size = ROUNDUP(size,PAGE_SIZE);

          if(isKHeapPlacementStrategyBESTFIT())
  		    	{
  			 block1 = alloc_block_BF(size);
  			 if(block1!=NULL)
  				{
  				int res = allocate_chunk(ptr_page_directory,ROUNDDOWN(block1->sva,PAGE_SIZE),size,PERM_WRITEABLE);

  				if(res!=-1)
  				  {

  					uint32 si=ROUNDDOWN(block1->sva,PAGE_SIZE);
  					LIST_INSERT_TAIL(&AllocMemBlocksList,block1);
  					return (void*)si;
  				  }
  				}

  			}

			if(isKHeapPlacementStrategyFIRSTFIT())
			{
				block = alloc_block_FF(size);
				if(block!=NULL)
				{
				int res = allocate_chunk(ptr_page_directory,ROUNDDOWN(block->sva,PAGE_SIZE),size,PERM_WRITEABLE);

				if(res!=-1)
				{

					uint32 si=ROUNDDOWN(block->sva,PAGE_SIZE);
					LIST_INSERT_TAIL(&AllocMemBlocksList,block);
					return (void*)si;
				}

				}
			}
			if(isKHeapPlacementStrategyNEXTFIT())
						{
							block2 = alloc_block_NF(size);
							if(block2!=NULL)
							{
							int res = allocate_chunk(ptr_page_directory,ROUNDDOWN(block2->sva,PAGE_SIZE),size,PERM_WRITEABLE);

							if(res!=-1)
							{

								uint32 si=ROUNDDOWN(block2->sva,PAGE_SIZE);
								LIST_INSERT_TAIL(&AllocMemBlocksList,block2);
								return (void*)si;
							}

							}
						}

			return NULL;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kfree
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
	uint32 va=(uint32)virtual_address;
	uint32 ss=KERNEL_HEAP_MAX , hh=KERNEL_HEAP_START;
	struct MemBlock *bppolc = find_block(&AllocMemBlocksList,va);

		if(bppolc!=NULL && bppolc->sva!=0)
		{
			for (uint32 ll=va; ll<ROUNDUP((va+(bppolc->size)),PAGE_SIZE);ll+=PAGE_SIZE)
			{
			unmap_frame(ptr_page_directory,ll);
			}
			LIST_REMOVE(&AllocMemBlocksList,bppolc);
			insert_sorted_with_merge_freeList(bppolc);

		}

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");



	  struct FrameInfo*v_add;
      v_add=to_frame_info(physical_address);
      return v_add->va;



	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details
	 uint32* ptr_page_table = NULL;
	 get_page_table(ptr_page_directory, virtual_address, &ptr_page_table);
	 uint32 x,l;
	 int hh =PTX(virtual_address);
	 l=(ptr_page_table[hh]);
	 x=l>>12;
	 uint32 kk =x * PAGE_SIZE;
	 return kk;

}


void kfreeall()
{
	panic("Not implemented!");

}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT MS2 - BONUS] [KERNEL HEAP] krealloc
	// Write your code here, remove the panic and write your code
	//panic("krealloc() is not implemented yet...!!");

	uint32 va=(uint32)virtual_address;
	if(virtual_address==NULL)
	{
		return (void*)kmalloc(new_size);
	}
	if(new_size==0 )
	{
	 kfree(virtual_address);
	 return virtual_address;
	}
	else
	{
		struct MemBlock *bppolc = find_block(&AllocMemBlocksList,va);
		struct MemBlock *block1;
        if (bppolc!=NULL)
        {
        if(new_size==bppolc->size || new_size<(bppolc->size))
        {

        	return virtual_address;
        }
        if(new_size>=bppolc->size)
        	{
        	bppolc->size=new_size;
        	 block1 = alloc_block_BF(new_size);
        	  if(block1!=NULL)
        	  				{
        	int res = allocate_chunk(ptr_page_directory,ROUNDDOWN(block1->sva,PAGE_SIZE),new_size,PERM_WRITEABLE);

        	  				if(res!=-1)
        	  				  {
        	  					kfree(virtual_address);
        	  					uint32 si=ROUNDDOWN(block1->sva,PAGE_SIZE);
        	  					LIST_INSERT_TAIL(&AllocMemBlocksList,block1);
        	  					 return virtual_address;
        	  				  }
        	  				}

            }
        else {
        	return virtual_address;
        }
        }

	}
	  return virtual_address;

}
