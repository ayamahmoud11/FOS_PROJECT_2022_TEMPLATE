/*
 * dyn_block_management.c
 *
 *  Created on: Sep 21, 2022
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//===========================
// PRINT MEM BLOCK LISTS:
//===========================

void print_mem_block_lists()
{
	cprintf("\n=========================================\n");
	struct MemBlock* blk ;
	struct MemBlock* lastBlk = NULL ;
	cprintf("\nFreeMemBlocksList:\n");
	uint8 sorted = 1 ;
	LIST_FOREACH(blk, &FreeMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nFreeMemBlocksList is NOT SORTED!!\n") ;

	lastBlk = NULL ;
	cprintf("\nAllocMemBlocksList:\n");
	sorted = 1 ;
	LIST_FOREACH(blk, &AllocMemBlocksList)
	{
		if (lastBlk && blk->sva < lastBlk->sva + lastBlk->size)
			sorted = 0 ;
		cprintf("[%x, %x)-->", blk->sva, blk->sva + blk->size) ;
		lastBlk = blk;
	}
	if (!sorted)	cprintf("\nAllocMemBlocksList is NOT SORTED!!\n") ;
	cprintf("\n=========================================\n");

}

//********************************************************************************//
//********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//===============================
// [1] INITIALIZE AVAILABLE LIST:
//===============================
void initialize_MemBlocksList(uint32 numOfBlocks)
{
	LIST_INIT(&AvailableMemBlocksList);

		    uint32 n;
		    uint32 i;

			for (n=0;n<numOfBlocks; n++) {
			LIST_INSERT_HEAD(&AvailableMemBlocksList, &(MemBlockNodes[n]));
			}

		     for (i=0; i< MAX_MEM_BLOCK_CNT; i++) {
		    	 MemBlockNodes[i].sva=0;
		    	 MemBlockNodes[i].size=0;
		     }
}

//===============================
// [2] FIND BLOCK:
//===============================
struct MemBlock *find_block(struct MemBlock_List *blockList, uint32 va)
{
	struct MemBlock *bl=NULL;
	LIST_FOREACH(bl,blockList){

		if(bl->sva==va){
			return bl;
			break;
		}
	}
	return NULL;
}

//=========================================
// [3] INSERT BLOCK IN ALLOC LIST [SORTED]:
//=========================================
void insert_sorted_allocList(struct MemBlock *blockToInsert)
{
	struct MemBlock *blk;

		//insert at the head if empty
		if (LIST_EMPTY(&AllocMemBlocksList))
		{
			LIST_INSERT_HEAD(&AllocMemBlocksList , blockToInsert);
		}

		//insert at tail if block is bigger than the last block
		if (blockToInsert->sva>LIST_LAST(&AllocMemBlocksList)->sva)
		{
			LIST_INSERT_TAIL(&AllocMemBlocksList , blockToInsert);
		}
		else {

		LIST_FOREACH(blk, &(AllocMemBlocksList))
			{
			   if(blk->sva > blockToInsert->sva)
			   {
				   LIST_INSERT_BEFORE(&AllocMemBlocksList, blk, blockToInsert);
				   break;
			   }
		   	}
}
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
struct MemBlock *alloc_block_FF(uint32 size)
{
	struct MemBlock *p1=NULL;

				LIST_FOREACH(p1,&(FreeMemBlocksList)){
					if(size == p1->size){
					  LIST_REMOVE(&(FreeMemBlocksList),p1);
					  return p1;
					  break;

					}
					if(size < p1->size){
						struct MemBlock *p2=NULL;
						p2= LIST_LAST(&AvailableMemBlocksList);
						p2->size = size;
						p2->sva = p1->sva;
						p1->size -= size;
						p1->sva += size;
						LIST_REMOVE(&AvailableMemBlocksList,p2);
						return p2;
						break;
					}

				}
				return NULL;


}


//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
struct MemBlock *alloc_block_BF(uint32 size)
{
	    struct MemBlock* block ;
		struct MemBlock* allocatedBlock = NULL;
		struct MemBlock* min ;
		uint32 m = 2147483647;
		int x = 0;
		LIST_FOREACH(block, &FreeMemBlocksList)
		{
		    if(block->size == size)
		    {
		    	allocatedBlock = block;
		    	LIST_REMOVE(&FreeMemBlocksList,block);
		    	x=0;
		    	break;

		    }
		    else if(block->size > size)
		    {
		    	if(block->size < m)
		    	{
		    		m = block->size;
		    		min = block;
		    		x=1;
		    	}

		    }
		}
		if(x==1)
		{
			allocatedBlock = LIST_LAST(&AvailableMemBlocksList);
			allocatedBlock->size = size;
			allocatedBlock->sva = min->sva;
			min->sva += size;
			min->size -= size;
			LIST_REMOVE(&AvailableMemBlocksList,allocatedBlock);
		}


	    return allocatedBlock;
}


//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
struct MemBlock *pg = NULL;
struct MemBlock *alloc_block_NF(uint32 size)
{
	//TODO: [PROJECT MS1 - BONUS] [DYNAMIC ALLOCATOR] alloc_block_NF
	// Write your code here, remove the panic and write your code
	//panic("alloc_block_NF() is not implemented yet...!!");
	         if(pg == NULL )
	         {
	        	 struct MemBlock* block;
	        	 LIST_FOREACH(block, &FreeMemBlocksList)
	        	 	{
	        	 	    if(block->size == size)
	        	 	    {
	        	 	    	pg = LIST_NEXT(block);
	        	 	    	LIST_REMOVE(&FreeMemBlocksList,block);
	        	 	    	return block;
	        	 	    }
	        	 	    else if(block->size > size)
	        	 	    {
	        	 	    	struct MemBlock*  B = LIST_LAST(&AvailableMemBlocksList);
	        	 	    	B->size = size;
	        	 	    	B->sva = block->sva;
	        	 	    	block->sva += size;
	        	 	    	block->size -= size;
	        	 	    	pg = block;
	        	 	    	LIST_REMOVE(&AvailableMemBlocksList,B);
	        	 	    	return B;
	        	 	    }
	        	 	}
	         }
		     else
		     {
		    	 struct MemBlock* p1 ;
		    	 LIST_FOREACH(p1, &FreeMemBlocksList)
		    	 {
		    		 p1 = pg;
		    		 if(p1->size == size)
		    		 {
		    			 pg = LIST_NEXT(p1);
		    			 LIST_REMOVE(&FreeMemBlocksList,p1);
		    			 return p1;
		    		 }
		    		 else
		    		 {
		    			 p1 = pg;
		    			 if(p1->size > size)
		    			 {
		    			 struct MemBlock* blk = LIST_LAST(&AvailableMemBlocksList);
		    			 blk->size = size;
		    			 blk->sva = p1->sva;
		    			 p1->size -= size;
		    			 p1->sva += size;
		    			 pg = p1;
		    			 LIST_REMOVE(&AvailableMemBlocksList,blk);
		    			 return blk;
		    			 }
		    		 }

		          }
		    	 //searching from the start of List

	         struct MemBlock* block;
				 LIST_FOREACH(block, &FreeMemBlocksList)
					{
						if(block->size == size)
						{
							pg = LIST_NEXT(block);
							LIST_REMOVE(&FreeMemBlocksList,block);
							return block;
						}

						else if(block->size > size)
							 {
								struct MemBlock*  B = LIST_LAST(&AvailableMemBlocksList);
								B->size = size;
								B->sva = block->sva;
								block->sva += size;
								block->size -= size;
								pg = block;
								LIST_REMOVE(&AvailableMemBlocksList,B);
								return B;
							 }
				       }


		     }

	        return NULL;

}

//===================================================
// [8] INSERT BLOCK (SORTED WITH MERGE) IN FREE LIST:
//===================================================
void insert_sorted_with_merge_freeList(struct MemBlock *blockToInsert)
{
	//cprintf("BEFORE INSERT with MERGE: insert [%x, %x)\n=====================\n", blockToInsert->sva, blockToInsert->sva + blockToInsert->size);
    //print_mem_block_lists() ;



  // Write your code here, remove the panic and write your code
  uint32 sizee=blockToInsert->size;
  uint32 svaa=blockToInsert->sva;


		//insert at the head if empty
		if (LIST_EMPTY(&FreeMemBlocksList))
		{
			LIST_INSERT_HEAD(&FreeMemBlocksList , blockToInsert);
		}
		else if (blockToInsert->sva < LIST_FIRST(&FreeMemBlocksList)->sva)
		 {
			   if ((svaa + sizee) == LIST_FIRST(&FreeMemBlocksList)->sva )
			     {
			        LIST_FIRST(&FreeMemBlocksList)->sva = svaa;
			    	LIST_FIRST(&FreeMemBlocksList)->size +=sizee;
			        blockToInsert->sva=0;
			    	blockToInsert->size=0;
			    	LIST_INSERT_HEAD(&AvailableMemBlocksList , blockToInsert);

			     }
			    else
			     {
					LIST_INSERT_HEAD(&FreeMemBlocksList , blockToInsert);
			     }
		  }

		//insert at tail if block is bigger than the last block
		else if (blockToInsert->sva > LIST_LAST(&FreeMemBlocksList)->sva)
		{
			if (LIST_LAST(&FreeMemBlocksList)->sva + LIST_LAST(&FreeMemBlocksList)->size == svaa)
			 {
				LIST_LAST(&FreeMemBlocksList)->size +=sizee;
				blockToInsert->sva=0;
				blockToInsert->size=0;
				LIST_INSERT_HEAD(&AvailableMemBlocksList , blockToInsert);

			 }
			 else
			 {
			   LIST_INSERT_TAIL(&FreeMemBlocksList , blockToInsert);
			 }

		}
		else
		{
	    struct MemBlock *blk;
		LIST_FOREACH(blk, &(FreeMemBlocksList))
			{
			   if(blk->sva > blockToInsert->sva && LIST_PREV(blk)!= NULL )
			   {
				   //merge with previous and next
				  	if(LIST_PREV(blk)->size + LIST_PREV(blk)->sva == svaa && svaa+sizee == blk->sva)
				  	{
				  	  LIST_PREV(blk)->size +=(sizee + blk->size);
				  	  blk->sva  =0;
				  	  blockToInsert->sva =0;
				  	  blk->size =0;
				  	  blockToInsert->size =0;

				  	  LIST_REMOVE(&FreeMemBlocksList, blk);
				  	  LIST_INSERT_HEAD(&AvailableMemBlocksList , blockToInsert);
				  	  LIST_INSERT_HEAD(&AvailableMemBlocksList , blk);
				  	  break;
				  	 }
                 //merge with next
				  	else if(svaa + sizee == blk->sva)
		            {
		        	  blk->sva =svaa;
		        	  blk->size += sizee;
		        	  blockToInsert->sva=0;
		        	  blockToInsert->size=0;

		    	     //move blockToInsert to avaliable
		    	     LIST_INSERT_HEAD(&AvailableMemBlocksList , blockToInsert);
		    	     break;
		            }

		          //merge with previous
		          else if(LIST_PREV(blk)->size + LIST_PREV(blk)->sva == svaa)
		          {
		        	  LIST_PREV(blk)->size += sizee;
		        	  blockToInsert->sva =0;
		        	  blockToInsert->size=0;
		        	  LIST_INSERT_HEAD(&AvailableMemBlocksList , blockToInsert);
		        	  break;
		          }


		          else
		          {
		        	 LIST_INSERT_BEFORE(&FreeMemBlocksList, blk, blockToInsert);
		        	 break;
		          }


			  }

		  }

     }


//cprintf("\nAFTER INSERT with MERGE:\n=====================\n");
//print_mem_block_lists();

}

