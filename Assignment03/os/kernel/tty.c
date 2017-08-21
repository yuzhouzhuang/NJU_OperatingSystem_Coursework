
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 tty.c
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 Forrest Yu, 2005
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)
EXTERN void flush(CONSOLE* p_con);

PRIVATE void init_tty(TTY* p_tty);
PRIVATE void tty_do_read(TTY* p_tty);
PRIVATE void tty_do_write(TTY* p_tty);
PRIVATE void put_key(TTY* p_tty, u32 key);

PRIVATE void input_delete(TTY *p_tty);
PRIVATE void input_tab(TTY *p_tty);
PRIVATE void input_enter(TTY* p_tty);

PRIVATE void console_status_change(TTY* p_tty);
/*======================================================================*
 task_tty
 *======================================================================*/
PUBLIC void task_tty()
{
    TTY*	p_tty;
    
    init_keyboard();
    
    for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
        init_tty(p_tty);
    }
    select_console(0);
    while (1) {
        for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
            tty_do_read(p_tty);
            tty_do_write(p_tty);
        }
    }
}

/*======================================================================*
 init_tty
 *======================================================================*/
PRIVATE void init_tty(TTY* p_tty)
{
    p_tty->inbuf_count = 0;
    p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;
    
    init_screen(p_tty);
}

/*======================================================================*
 in_process
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, u32 key)
{
    if(p_tty->p_console->console_status == 2 && key != ESC) {
        return;
    }
    
    char output[2] = {'\0', '\0'};
    
    if (!(key & FLAG_EXT)) {
        if(p_tty->p_console->console_status == 1) {
            p_tty->p_console->keyword_length++;
        }
        put_key(p_tty, key);
    }
    else {
        int raw_code = key & MASK_RAW;
        switch(raw_code) {
            case ESC:
                console_status_change(p_tty);
                break;
            case ENTER:
                input_enter(p_tty);
                break;
            case BACKSPACE:
                if(p_tty->p_console->console_status == 1) {
                    p_tty->p_console->keyword_length--;
                }
                input_delete(p_tty);
                break;
            case TAB:
                input_tab(p_tty);
                break;
            case UP:
                if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
                    scroll_screen(p_tty->p_console, SCR_DN);
                }
                break;
            case DOWN:
                if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {
                    scroll_screen(p_tty->p_console, SCR_UP);
                }
                break;
            case F1:
            case F2:
            case F3:
            case F4:
            case F5:
            case F6:
            case F7:
            case F8:
            case F9:
            case F10:
            case F11:
            case F12:
                /* Alt + F1~F12 */
                if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {
                    select_console(raw_code - F1);
                }
                break;
            default:
                break;
        }
    }
}

PRIVATE void input_delete(TTY *p_tty){
    u8* temp_loaction = (u8*)(V_MEM_BASE + p_tty->p_console->cursor * 2);
    if(p_tty->p_console->cursor == 0) {
        return;
    }
    
    //需不需要换行，或者上一行有没有换行符
    int mark = 1;
    
    if (p_tty->p_console->cursor % SCREEN_WIDTH == 0) {
        temp_loaction -= SCREEN_WIDTH * 2;
        
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            if (*temp_loaction == '\n') {
                p_tty->p_console->cursor -= SCREEN_WIDTH - i;
                *temp_loaction = ' ';
                *(temp_loaction + 1) = DEFAULT_CHAR_COLOR;
                mark = 0;
                break;
            }
            temp_loaction += 2;
        }
    }
    
    if (mark) {
        if (*(temp_loaction-2) == '\0') {//tab
            for (int i = 0; i < 4; i++) {
                temp_loaction -= 2;
                p_tty->p_console->cursor--;
                if (*(temp_loaction - 2) != 0) break;
            }
        }
        else {//正常字符
            *(temp_loaction-2) = ' ';
            *(temp_loaction-1) = DEFAULT_CHAR_COLOR;
            p_tty->p_console->cursor--;
        }
    }
    flush(p_tty->p_console);
}

PRIVATE void input_tab(TTY *p_tty){
    int count = 4 - (p_tty->p_console->cursor - (p_tty->p_console->cursor >> 2 << 2));
    
    for (int i = 0; i < count; i ++) {
        put_key(p_tty, '\0');
    }
}

PRIVATE void searchKeyword(TTY* p_tty){
    u8* keyword_location = (u8*)(V_MEM_BASE + p_tty->p_console->cursor * 2-p_tty->p_console->keyword_length*2);
    
    for(u8* temp_location = (u8*)(V_MEM_BASE); temp_location < keyword_location; temp_location += 2){
        if(*(temp_location) == *(keyword_location)){
            
            int mark=1;
            
            for(int i = 1; i < p_tty->p_console->keyword_length; i++){
                if (*(temp_location + i * 2) == *(keyword_location + i * 2)) {
                }else{
                    mark=0;
                    break;
                }
            }
            
            //变色
            if(mark==1){
                *(temp_location+1)=CHANGE_CHAR_COLOR;
                for(int j = 0; j < p_tty->p_console->keyword_length; j++){
                    *(temp_location+1) = CHANGE_CHAR_COLOR;
                    temp_location += 2;
                }
                temp_location -= 2;
            }
        }
    }
    flush(p_tty->p_console);
}

PRIVATE void input_enter(TTY* p_tty){
    
    switch(p_tty->p_console->console_status) {
        case 0:
            put_key(p_tty, '\n');
            break;
        case 1:
            p_tty->p_console->console_status = 2;
            searchKeyword(p_tty);
            break;
    }
    flush(p_tty->p_console);
}



PRIVATE void console_status_change(TTY* p_tty){
    switch (p_tty->p_console->console_status) {
        case 0:
            disable_irq(CLOCK_IRQ);
            p_tty->p_console->console_status++;
            p_tty->p_console->keyword_length = 0;
            break;
        case 2:
            p_tty->p_console->console_status = 0;
            for (int i = 0; i < p_tty->p_console->keyword_length; i++) {
                put_key(p_tty, '\b');
            }
            p_tty->p_console->keyword_length = 0;
            u8* p_vmem = (u8*)V_MEM_BASE;
            for (int i = 0; i < p_tty->p_console->cursor; i ++) {
                if (*p_vmem == '\n') {
                    *(p_vmem + 1) = 0x00;
                } else {
                    *(p_vmem + 1) = DEFAULT_CHAR_COLOR;
                }
                p_vmem += 2;
            }
            enable_irq(CLOCK_IRQ);
            break;
        default:
            break;
    }
    flush(p_tty->p_console);
}


/*======================================================================*
 put_key
 *======================================================================*/
PRIVATE void put_key(TTY* p_tty, u32 key)
{
    if (p_tty->inbuf_count < TTY_IN_BYTES) {
        *(p_tty->p_inbuf_head) = key;
        p_tty->p_inbuf_head++;
        if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
            p_tty->p_inbuf_head = p_tty->in_buf;
        }
        p_tty->inbuf_count++;
    }
}


/*======================================================================*
 tty_do_read
 *======================================================================*/
PRIVATE void tty_do_read(TTY* p_tty)
{
    if (is_current_console(p_tty->p_console)) {
        keyboard_read(p_tty);
    }
}


/*======================================================================*
 tty_do_write
 *======================================================================*/
PRIVATE void tty_do_write(TTY* p_tty)
{
    if (p_tty->inbuf_count) {
        char ch = *(p_tty->p_inbuf_tail);
        p_tty->p_inbuf_tail++;
        if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
            p_tty->p_inbuf_tail = p_tty->in_buf;
        }
        p_tty->inbuf_count--;
        
        out_char(p_tty->p_console, ch);
    }
}
