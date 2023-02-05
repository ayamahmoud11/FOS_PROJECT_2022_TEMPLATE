/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"


/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	//panic("cut_paste_pages() is not implemented yet...!!");
	 uint32 size=num_of_pages * PAGE_SIZE;
		   source_va= ROUNDDOWN(source_va, PAGE_SIZE);
		   dest_va= ROUNDDOWN(dest_va, PAGE_SIZE);
		   uint32 end_d=ROUNDUP(size+dest_va,PAGE_SIZE);
		  uint32 end_s=ROUNDUP( size+source_va,PAGE_SIZE);

		   uint32* ptr_page_table_destination= NULL;
		   uint32* ptr_page_table_source= NULL;
		   struct FrameInfo* ptr_frame=NULL;
		   struct FrameInfo* ptr_frame_s=NULL;
		   uint32 d=dest_va;



	                 for( d ; d < end_d; d+=PAGE_SIZE ) {

	                ptr_frame=get_frame_info(page_directory, d,&ptr_page_table_destination);

						  if (ptr_frame!= NULL) {
									 return -1;
						  }



	             }


	 	       // source_va= ROUNDDOWN(source_va, PAGE_SIZE);
	             for(d=dest_va ; d < end_d; d+=PAGE_SIZE) {
	            	 	if (ptr_frame==NULL) {

	            					   int ret =get_page_table( page_directory, d, &ptr_page_table_destination);
	            					     if ( ret == TABLE_NOT_EXIST ) {
	            				        ptr_page_table_destination= create_page_table(page_directory,d) ;
	            					       }

	        // uint32 per=pt_get_page_permissions(page_directory,d);
	         uint32 per_s=pt_get_page_permissions(page_directory,source_va);
	         ptr_frame_s= get_frame_info(page_directory, source_va,&ptr_page_table_source);
	               map_frame(page_directory,ptr_frame_s,d,per_s);
	               unmap_frame(page_directory,source_va);
	                        source_va+=PAGE_SIZE;

		                    }
	             }


	       return 0;
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va
//if the page table at any destination page in the range is not exist, it should create it
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the panic and write your code
	//panic("copy_paste_chunk() is not implemented yet...!!");

	 uint32 s = ROUNDDOWN( source_va , PAGE_SIZE );
	 uint32 d = ROUNDDOWN(dest_va, PAGE_SIZE);
	   uint32 end_d = ROUNDUP(size+dest_va,PAGE_SIZE);
	   uint32 end_s = ROUNDUP(size+source_va,PAGE_SIZE);

	   uint32* ptr_page_table_destination = NULL;
	   uint32* ptr_page_table_source = NULL;

	   struct FrameInfo* ptr_frame_d = NULL;
	   struct FrameInfo* ptr_frame_s = NULL;



	   for(d , s ; d < end_d , s<end_s ; d += PAGE_SIZE, s += PAGE_SIZE ){

		   ptr_frame_d = get_frame_info(page_directory, d,&ptr_page_table_destination);
		   ptr_frame_s = get_frame_info(page_directory, s,&ptr_page_table_source);
		   uint32 per_d = pt_get_page_permissions(page_directory,d);
		   uint32 per_s = pt_get_page_permissions(page_directory,s);

		   int ret = get_page_table( page_directory, d, &ptr_page_table_destination);

			  if (ptr_frame_d != NULL) {

				 if((per_d & PERM_WRITEABLE) != PERM_WRITEABLE){

					 return -1;
				 }
			  }

			  if(ret == TABLE_NOT_EXIST){

				  create_page_table(page_directory,d);
			  }

			  if(ptr_frame_d == NULL && ptr_frame_s != NULL){

				  struct FrameInfo *fr = NULL;
				  int cheakfr = allocate_frame(&fr);

					  if(cheakfr == 0){

						  map_frame(page_directory,fr,d,per_s);
						  pt_set_page_permissions(page_directory,d,1,0);

					   } if(cheakfr != 0 ){

						  return -1;
					  }

			  }
			  if(ptr_frame_d!=NULL&&ptr_frame_s==NULL){

				  pt_set_page_permissions(page_directory,d,per_s,0);
			  }
	   }
	   uint32 size_d=dest_va+size;
	   uint32 size_s=source_va+size;

	   for(uint32 d2 = dest_va , s2 = source_va ; d2 < size_d , s2 < size_s ; d2++ , s2++ ){

		   unsigned char *p_d = (unsigned char*)d2;
		   unsigned char *p_s = (unsigned char*)s2;
		   *p_d = *p_s;

	   }

	   return 0;

}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should share the given size from dest_va with the source_va
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	   uint32 chunck1= ROUNDUP( source_va+size,PAGE_SIZE );
		   source_va= ROUNDDOWN(source_va, PAGE_SIZE);
		   uint32 chunck2 = ROUNDUP( dest_va+size, PAGE_SIZE);
		   dest_va= ROUNDDOWN(dest_va, PAGE_SIZE);
	       uint32* ptr_page_table_destination= NULL;
			           uint32* ptr_page_table_source= NULL;
			           struct FrameInfo* ptr_frame=NULL;
			           struct FrameInfo* ptr_frame_s=NULL;
			           uint32 b;
			           uint32 t=source_va;


		      for( b=dest_va ; b < chunck2; b+=PAGE_SIZE ) {

		        ptr_frame= get_frame_info(page_directory, b,&ptr_page_table_destination);

		                              if (ptr_frame!= NULL) {
		                                    	 return -1;  }


		                               if (ptr_frame==NULL) {

		                    int ret =get_page_table( page_directory, b, &ptr_page_table_destination);
		             	     if ( ret == TABLE_NOT_EXIST ) {
		                   ptr_page_table_destination= create_page_table(page_directory,b) ; }

		                             }
		                               t+=PAGE_SIZE;
		                }


		                     t=source_va;
		          for( b=dest_va ; b < chunck2; b+=PAGE_SIZE ) {

		            ptr_frame_s= get_frame_info(page_directory, t,&ptr_page_table_source);
				        	   map_frame(page_directory,ptr_frame_s,b,perms);
		                       t+=PAGE_SIZE;

			                    }

				      return 0;



}


//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate in RAM the given range [va, va+size)
//Hint: use ROUNDDOWN/ROUNDUP macros to align the addresses
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//TODO: [PROJECT MS2] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the panic and write your code
	//panic("allocate_chunk() is not implemented yet...!!");
	 uint32 chunck= ROUNDUP( va+size,PAGE_SIZE );
	      va= ROUNDDOWN(va, PAGE_SIZE);

	            uint32* ptr_page_table_va= NULL;
	      	    struct FrameInfo* ptr_frame=NULL;
	      	    struct FrameInfo*ptr_alloc=NULL;
	      	    uint32 st;

	                   for( st=va; st< chunck; st+=PAGE_SIZE ) {

	      	          ptr_frame= get_frame_info(page_directory, va,&ptr_page_table_va);

	      	                               if (ptr_frame!= NULL) {
	      	                                     	 return -1;  }


	      	                       if (ptr_frame==NULL) {

	      	              int ret =get_page_table( page_directory, st, &ptr_page_table_va);
	      	                         if ( ret == TABLE_NOT_EXIST ) {
	      	               ptr_page_table_va= create_page_table(page_directory,st) ; }


	      	                                                          }

	            }
	                   uint32 ph_add;
	                                          struct FrameInfo* ptr;
	                                      for( st=va; st< chunck; st+=PAGE_SIZE ) {

	                                   	   int rett= allocate_frame(&ptr_alloc);
	                                   	   if (rett != E_NO_MEM){
	                       		        	   map_frame(page_directory,ptr_alloc,st,perms);
	                       		        	ph_add=virtual_to_physical(page_directory,st);
	                       		            ptr =to_frame_info(ph_add);
	                       		            ptr->va=st;

	                	   }
	                   }


	            return 0;

}

/*BONUS*/
//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	//panic("calculate_allocated_space() is not implemented yet...!!");
	uint32 *p = NULL;
	uint32 *l = NULL;
    int pnu=0;int pny=0;
	uint32 s=ROUNDDOWN(sva,PAGE_SIZE) , e=ROUNDUP(eva,PAGE_SIZE);
	struct FrameInfo*au = NULL;
	//int np=0;
	for(uint32 pag = s ; pag < e ; pag+= PAGE_SIZE)
	{
	    au = get_frame_info(page_directory,pag,&p);
     		if(au!=0)
			{
     			  pny++;
			}
     }
	(*num_pages)=pny;
	uint32 kk= (PAGE_SIZE*1024);
	uint32 ff =ROUNDDOWN(sva,PAGE_SIZE*1024), jj = ROUNDUP(eva,PAGE_SIZE*1024);
	for(uint32 tab = ff ; tab <  jj; tab+=kk)
		{
		   int po= get_page_table(page_directory,tab,&l);
			if (l!=NULL)
			{
			   pnu++;
			}
		}
	 (*num_tables)=pnu;
}

/*BONUS*/
//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
// calculate_required_frames:
// calculates the new allocation size required for given address+size,
// we are not interested in knowing if pages or tables actually exist in memory or the page file,
// we are interested in knowing whether they are allocated or not.
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//TODO: [PROJECT MS2 - BONUS] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	//panic("calculate_required_frames() is not implemented yet...!!");
	    uint32 *p = NULL;
		uint32 *l = NULL;
	    int pnu=0;int pny=0;
		uint32 s=ROUNDDOWN(sva,PAGE_SIZE);
		uint32 e=ROUNDUP((sva+size),PAGE_SIZE);
		struct FrameInfo*au = NULL;
		//int np=0;
		for(uint32 i = s ; i < e ; i+= PAGE_SIZE)
			{
		    au = get_frame_info(page_directory,i,&p);
	     		if(au==0)
				{
	     			  pny++;
				}
	     }
		uint32 kk= (PAGE_SIZE*1024);
		uint32 gg= ROUNDDOWN(sva,PAGE_SIZE*1024),ll=ROUNDUP((sva+size),PAGE_SIZE*1024);
		for(uint32 i = gg; i < ll ; i+=kk)
			{
			   int po= get_page_table(page_directory,i,&l);
				if (l==NULL)
				{
				   pnu++;
				}
			}
		return (pny+pnu);
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	// Write your code here, remove the panic and write your code
	panic("allocate_user_mem() is not implemented yet...!!");
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	 uint32* ptr_page_table=NULL;
     int emptytable=0;
     size= ROUNDUP (size,PAGE_SIZE);
     uint32 end = (virtual_address+size);
     //1. Free ALL pages of the given range from the Page File
     for (uint32 a = virtual_address; a < end  ; a += PAGE_SIZE)
     {
    	 pf_remove_env_page(e,a);
     }
     //2. Free ONLY pages that are resident in the working set from the memory
     uint32 end_set=e->page_WS_max_size;
     for(uint32 a=0;a<end_set;a++)
     {
    	 uint32 v_wor=env_page_ws_get_virtual_address(e,a);
    	 if(v_wor>=virtual_address&& v_wor<end)
    	 {
         unmap_frame(e->env_page_directory,v_wor);
         env_page_ws_clear_entry(e,a);
    	 }
     }

    //3. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
    for (uint32 a = virtual_address; a < end  ; a += PAGE_SIZE)
     {
       get_page_table(e->env_page_directory,a, &ptr_page_table);
       emptytable=0;
       if (ptr_page_table != NULL)
          {
           for (int j = 0; j < 1024; j++)
            {
        	   if(ptr_page_table[j] != 0)
        	   {
        		   emptytable=1;break;

        	   }
            }
           if(emptytable==0)
           {
        	   pd_clear_page_dir_entry(e->env_page_directory, a);
        	   kfree((void*)ptr_page_table);
           }
          }
     }
}

//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");

	//This function should:
	//1. Free ALL pages of the given range from the Page File
	//2. Free ONLY pages that are resident in the working set from the memory
	//3. Free any BUFFERED pages in the given range
	//4. Removes ONLY the empty page tables (i.e. not used) (no pages are mapped in the table)
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//TODO: [PROJECT MS3 - BONUS] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory

	/**/
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

