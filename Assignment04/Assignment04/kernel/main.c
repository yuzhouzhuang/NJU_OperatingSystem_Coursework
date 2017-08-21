
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
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



void showCome(int id);
void showGetHairCut(int id);
void showCome(int id);
void myitoa(char* str,int num);

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[0].ticks = proc_table[0].blocked = 0;
	proc_table[1].ticks = proc_table[0].blocked = 0;
	proc_table[2].ticks = proc_table[0].blocked = 0;
	proc_table[3].ticks = proc_table[0].blocked = 0;
	proc_table[4].ticks = proc_table[0].blocked = 0;
	

	disp_pos = 0;
	for(i=0;i<80*25;i++){
		disp_str(" ");	
	}
	disp_pos = 0;
	
	k_reenter = 0;
	ticks = 0;
	id = 0;	

	waiting = 0;
	customers.value = 0;
	customers.in = 0;
	customers.out = 0;
	mutex.value = 1;
	mutex.in = 0;
	mutex.out = 0;
	barbers.value = 0;
	barbers.in = 0;
	barbers.out = 0;
	

	p_proc_ready	= proc_table;

        /* 初始化 8253 PIT */
        out_byte(TIMER_MODE, RATE_GENERATOR);
        out_byte(TIMER0, (u8) (TIMER_FREQ/HZ) );
        out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */

	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	while (1) {
		milli_delay(100);
	}
}

/*======================================================================*
                               barber
 *======================================================================*/
void barber()
{

	while(1){
		sem_p(&customers,1);
		sem_p(&mutex,1);
		waiting--;
		sem_v(&barbers);
		sem_v(&mutex);

		color_show("barbers cut hair.",MAKE_COLOR(BLACK,BLUE));
		show("\n");
		sleep(2000);
	}
}

/*======================================================================*
                               customerA
 *======================================================================*/
void  customerA()
{

	while(1){
		
		int my_id = id;
		id++;
		showCome(my_id);
		sem_p(&mutex,2);
		if(waiting<CHAIRS){
			waiting++;
			sem_v(&customers);
			sem_v(&mutex);
			sem_p(&barbers,2);	
			showGetHairCut(my_id);
			
		}else{
			sem_v(&mutex);	
			showLeave(my_id);	
		}
		sleep(1000);
	}
}


/*======================================================================*
                               customerB
 *======================================================================*/
void  customerB()
{
	
	while(1){
		int my_id = id;
		id++;
		showCome(my_id);	
		sem_p(&mutex,3);
		if(waiting<CHAIRS){
			waiting++;
			sem_v(&customers);
			sem_v(&mutex);
			sem_p(&barbers,3);	
			showGetHairCut(my_id);
			
		}else{
			sem_v(&mutex);	
			showLeave(my_id);		
		}
		sleep(1000);
	}
}

/*======================================================================*
                               customerC
 *======================================================================*/
void  customerC()
{

	while(1){
		int my_id = id;
		id++;
		showCome(my_id);				
		sem_p(&mutex,4);
		if(waiting<CHAIRS){
			waiting++;
			sem_v(&customers);
			sem_v(&mutex);
			sem_p(&barbers,4);
		
			showGetHairCut(my_id);
			
		}else{
			sem_v(&mutex);	
			showLeave(my_id);	
		}
		sleep(1000);
	}
}



void showCome(int id){
	color_show("cu",MAKE_COLOR(BLACK,RED));
	char output [16];
	myitoa(output,id);
	color_show(output,MAKE_COLOR(BLACK,RED));
	color_show(" come.",MAKE_COLOR(BLACK,RED));
	show("\n");
}

void showGetHairCut(int id){
	color_show("cu" ,MAKE_COLOR(BLACK,RED));
	char output [16];
	myitoa(output,id);
	color_show(output,MAKE_COLOR(BLACK,RED));
	color_show(" get hair cut.",MAKE_COLOR(BLACK,RED));
	show("\n");
}

void showLeave(int id){
	color_show("full and cu",MAKE_COLOR(BLACK,RED));
	char output [16];
	myitoa(output,id);
	color_show(output,MAKE_COLOR(BLACK,RED));
	//show(output);
	color_show(" go.",MAKE_COLOR(BLACK,RED));
	//show(" go.");
	show("\n");
}


void myitoa(char* str,int num){
	char *	p = str;
	char	ch;
	int	i;
	int	flag = FALSE;

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = TRUE;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = '\0';
	
		
	
	return str;
}
