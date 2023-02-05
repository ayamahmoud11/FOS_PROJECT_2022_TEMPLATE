/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//Handle the page fault
void page_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//placment case
				 int crev=env_page_ws_get_size(curenv);
				 fault_va=ROUNDDOWN(fault_va,PAGE_SIZE);
				struct FrameInfo *ptr_fr=NULL;


			//-------------------------------------------------------------------------
				//if(crev ==curenv->page_WS_max_size);
		   if(crev ==curenv->page_WS_max_size) {

					  int f=1;
			    while(f){
			    	uint32 *p_p_t=NULL;
			    	 struct FrameInfo *ptr_fr=NULL;

				// if(curenv->page_last_WS_index==curenv->page_WS_max_size){
					// curenv->page_last_WS_index=0;
								//	   }
				  uint32 v_va = env_page_ws_get_virtual_address(curenv,curenv->page_last_WS_index);
				 // uint32 v_va =  curenv->ptr_pageWorkingSet[curenv->page_last_WS_index].virtual_address;

				   uint32 prem=pt_get_page_permissions(curenv->env_page_directory,v_va);
				   get_page_table(curenv->env_page_directory,v_va,&p_p_t);
				   ptr_fr=get_frame_info(curenv->env_page_directory , v_va , &p_p_t);
				   uint32 p_u=prem&PERM_USED;
				   uint32 p_m=prem&PERM_MODIFIED;
				   if(  p_u != PERM_USED ){
						if(p_m == PERM_MODIFIED){
							pf_update_env_page(curenv,v_va,ptr_fr);
							env_page_ws_clear_entry(curenv,curenv->page_last_WS_index);
							unmap_frame(curenv->env_page_directory,v_va);
							//curenv->page_last_WS_index++;
							f=0;
							break;
					  }
						else{
							env_page_ws_clear_entry(curenv,curenv->page_last_WS_index);
							unmap_frame(curenv->env_page_directory,v_va);
							//curenv->page_last_WS_index++;
							f=0;
							break;
					   }


				   }
				   else{
							 pt_set_page_permissions(curenv->env_page_directory,v_va,0,PERM_USED);

								}


				   curenv->page_last_WS_index=  ( (curenv->page_last_WS_index)+1)%(curenv->page_WS_max_size);

			}



				   //cprintf("=========Placement %x =======\n;",fault_va);
				    struct FrameInfo *ptr_fr1=NULL;
				   			allocate_frame(&ptr_fr1);
				   			map_frame(curenv->env_page_directory , ptr_fr1 , fault_va , PERM_USER|PERM_WRITEABLE);
				   			int readP = pf_read_env_page(curenv,(void *)fault_va);

				   			if(readP == E_PAGE_NOT_EXIST_IN_PF){

				   				if( ( fault_va < USTACKTOP  && fault_va>=USTACKBOTTOM)|| (fault_va<USER_HEAP_MAX &&fault_va>=USER_HEAP_START)){

				   				}
				   				else{
				   					unmap_frame(curenv->env_page_directory,fault_va);
				   					panic("ILLEGAL MEMORY ACCESS ");
				   				}
				   			}
					while(1){
					if(curenv->ptr_pageWorkingSet[curenv->page_last_WS_index].empty==1){
					env_page_ws_set_entry(curenv, curenv->page_last_WS_index, fault_va);
					break ;
					}
					else{
						(curenv->page_last_WS_index)++;
					}
					}
					 curenv->page_last_WS_index=  ( (curenv->page_last_WS_index)+1)%(curenv->page_WS_max_size);

			   }
		   else if(crev < curenv->page_WS_max_size){
		   					allocate_frame(&ptr_fr);
		   					map_frame(curenv->env_page_directory , ptr_fr , fault_va , PERM_USER|PERM_WRITEABLE);
		   					int readP = pf_read_env_page(curenv,(void *)fault_va);

		   					if(readP == E_PAGE_NOT_EXIST_IN_PF){
		   						if(( fault_va < USTACKTOP  && fault_va>=USTACKBOTTOM)|| (fault_va<USER_HEAP_MAX &&fault_va>=USER_HEAP_START)){

		   						}
		   						else{
		   							unmap_frame(curenv->env_page_directory,fault_va);
		   							panic("ILLEGAL MEMORY ACCESS ");
		   						}
		   					}
		   		while(1){
		   				if(curenv->ptr_pageWorkingSet[curenv->page_last_WS_index].empty==1){
		   				env_page_ws_set_entry(curenv, curenv->page_last_WS_index, fault_va);
		   				break ;
		   				}
		   				else{
		   					//(curenv->page_last_WS_index)++;
		   					 curenv->page_last_WS_index=  ( (curenv->page_last_WS_index)+1)%(curenv->page_WS_max_size);

		   				}
		   				}

		   				}
}


//refer to the project presentation and documentation for details

void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	// Write your code here, remove the panic and write your code
	panic("__page_fault_handler_with_buffering() is not implemented yet...!!");


}
