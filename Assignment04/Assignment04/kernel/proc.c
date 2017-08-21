
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

void block();
void wakeup();
void next();


/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	for(p = proc_table; p < proc_table+NR_TASKS; p++){
		if (p->ticks > 0) {
			p->ticks--;
		}		
	}	
	next();
	
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}


/*======================================================================*
                           sys_sleep
 *======================================================================*/
PUBLIC void sys_sleep(int milli_sec)
{	

	// 当前进程ticks提高
	p_proc_ready->ticks = milli_sec*HZ/1000;
	//找到下一个ticks为0，且不被阻止的进程
	next();	
}

/*======================================================================*
                           sys_disp_str
 *======================================================================*/
PUBLIC void sys_disp_str(char* str)
{
	disp_str(str);
}

/*======================================================================*
                           sys_disp_color_str
 *======================================================================*/
PUBLIC void sys_disp_color_str(char* str,int color)
{
	disp_color_str(str,color);
}


/*======================================================================*
                           sys_sem_p
 *======================================================================*/
PUBLIC void sys_sem_p(SEMAPHORE* sem,int index)
{	
	sem->value--;
	if(sem->value<0){
		sem->list[sem->in] = index;
		block();
		sem->in = (sem->in+1)%queueSize;	
	}
}


/*======================================================================*
                           sys_sem_v
 *======================================================================*/
PUBLIC void sys_sem_v(SEMAPHORE* sem)
{
	sem->value++;
	//disp_str("V operation");
	if(sem->value<=0){
		int index = sem->list[sem->out];
		wakeup(index);
		sem->out = (sem->out+1)%queueSize;	
	}
}

void next(){
	//找到下一个ticks为0的进程
	while(1){
		p_proc_ready++;
		
		if(p_proc_ready >= proc_table+NR_TASKS){
			p_proc_ready = 	proc_table;
		}

		if(p_proc_ready->ticks<=0&&p_proc_ready->blocked!=1){
			break;	
		}	
		
			
	}
}

void block(){	
	//block操作
	p_proc_ready->blocked = 1;
	next();
}

void wakeup(int index){	
	//wakeup操作
	proc_table[index].blocked = 0;
	p_proc_ready = &proc_table[index];
}
