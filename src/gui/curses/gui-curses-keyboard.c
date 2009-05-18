/*
 * Copyright (c) 2003-2009 by FlashCode <flashcode@flashtux.org>
 * See README for License detail, AUTHORS for developers list.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* gui-curses-keyboard.c: keyboard functions for Curses GUI */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../core/weechat.h"
#include "../../core/wee-config.h"
#include "../../core/wee-hook.h"
#include "../../core/wee-log.h"
#include "../../core/wee-utf8.h"
#include "../../core/wee-string.h"
#include "../../plugins/plugin.h"
#include "../gui-keyboard.h"
#include "../gui-buffer.h"
#include "../gui-color.h"
#include "../gui-input.h"
#include "../gui-completion.h"
#include "../gui-window.h"
#include "gui-curses.h"


/*
 * gui_keyboard_default_bind: create key bind, only if it does not exist yet
 */

void
gui_keyboard_default_bind (const char *key, const char *command)
{
    struct t_gui_key *ptr_key;
    char *internal_code;
    
    internal_code = gui_keyboard_get_internal_code (key);
    
    ptr_key = gui_keyboard_search (NULL, (internal_code) ? internal_code : key);
    if (!ptr_key)
    {
        gui_keyboard_new (NULL, key, command);
    }
    
    if (internal_code)
        free (internal_code);
}

/*
 * gui_keyboard_default_bindings: create default key bindings
 */

void
gui_keyboard_default_bindings ()
{
    int i;
    char key_str[32], command[32];
    
    gui_keyboard_default_bind (/* RC            */ "ctrl-M",             "/input return");
    gui_keyboard_default_bind (/* RC            */ "ctrl-J",             "/input return");
    gui_keyboard_default_bind (/* tab           */ "ctrl-I",             "/input complete_next");
    gui_keyboard_default_bind (/* s-tab         */ "meta2-Z",            "/input complete_previous");
    gui_keyboard_default_bind (/* ^R            */ "ctrl-R",             "/input search_text");
    gui_keyboard_default_bind (/* basckpace     */ "ctrl-H",             "/input delete_previous_char");
    gui_keyboard_default_bind (/* basckpace     */ "ctrl-?",             "/input delete_previous_char");
    gui_keyboard_default_bind (/* del           */ "meta2-3~",           "/input delete_next_char");
    gui_keyboard_default_bind (/* ^D            */ "ctrl-D",             "/input delete_next_char");
    gui_keyboard_default_bind (/* ^W            */ "ctrl-W",             "/input delete_previous_word");
    gui_keyboard_default_bind (/* m-d           */ "meta-d",             "/input delete_next_word");
    gui_keyboard_default_bind (/* ^K            */ "ctrl-K",             "/input delete_end_of_line");
    gui_keyboard_default_bind (/* m-r           */ "meta-r",             "/input delete_line");
    gui_keyboard_default_bind (/* ^T            */ "ctrl-T",             "/input transpose_chars");
    gui_keyboard_default_bind (/* ^U            */ "ctrl-U",             "/input delete_beginning_of_line");
    gui_keyboard_default_bind (/* ^Y            */ "ctrl-Y",             "/input clipboard_paste");
    gui_keyboard_default_bind (/* home          */ "meta2-1~",           "/input move_beginning_of_line");
    gui_keyboard_default_bind (/* home          */ "meta2-H",            "/input move_beginning_of_line");
    gui_keyboard_default_bind (/* home          */ "meta2-7~",           "/input move_beginning_of_line");
    gui_keyboard_default_bind (/* ^A            */ "ctrl-A",             "/input move_beginning_of_line");
    gui_keyboard_default_bind (/* end           */ "meta2-4~",           "/input move_end_of_line");
    gui_keyboard_default_bind (/* end           */ "meta2-F",            "/input move_end_of_line");
    gui_keyboard_default_bind (/* end           */ "meta2-8~",           "/input move_end_of_line");
    gui_keyboard_default_bind (/* ^E            */ "ctrl-E",             "/input move_end_of_line");
    gui_keyboard_default_bind (/* left          */ "meta2-D",            "/input move_previous_char");
    gui_keyboard_default_bind (/* ^B            */ "ctrl-B",             "/input move_previous_char");
    gui_keyboard_default_bind (/* right         */ "meta2-C",            "/input move_next_char");
    gui_keyboard_default_bind (/* ^F            */ "ctrl-F",             "/input move_next_char");
    gui_keyboard_default_bind (/* m-b           */ "meta-b",             "/input move_previous_word");
    gui_keyboard_default_bind (/* ^left         */ "meta-Od",            "/input move_previous_word");
    gui_keyboard_default_bind (/* m-f           */ "meta-f",             "/input move_next_word");
    gui_keyboard_default_bind (/* ^right        */ "meta-Oc",            "/input move_next_word");
    gui_keyboard_default_bind (/* up            */ "meta2-A",            "/input history_previous");
    gui_keyboard_default_bind (/* down          */ "meta2-B",            "/input history_next");
    gui_keyboard_default_bind (/* ^up           */ "meta-Oa",            "/input history_global_previous");
    gui_keyboard_default_bind (/* ^down         */ "meta-Ob",            "/input history_global_next");
    gui_keyboard_default_bind (/* m-a           */ "meta-a",             "/input jump_smart");
    gui_keyboard_default_bind (/* m-j,m-l       */ "meta-jmeta-l",       "/input jump_last_buffer");
    gui_keyboard_default_bind (/* m-j,m-p       */ "meta-jmeta-p",       "/input jump_previous_buffer");
    gui_keyboard_default_bind (/* m-j,m-r       */ "meta-jmeta-r",       "/server raw");
    gui_keyboard_default_bind (/* m-h           */ "meta-h",             "/input hotlist_clear");
    gui_keyboard_default_bind (/* m-k           */ "meta-k",             "/input grab_key");
    gui_keyboard_default_bind (/* m-s           */ "meta-s",             "/server switch");
    gui_keyboard_default_bind (/* m-u           */ "meta-u",             "/input scroll_unread");
    gui_keyboard_default_bind (/* ^S^U          */ "ctrl-Sctrl-U",       "/input set_unread");
    gui_keyboard_default_bind (/* ^Cb           */ "ctrl-Cb",            "/input insert \\x02");
    gui_keyboard_default_bind (/* ^Cc           */ "ctrl-Cc",            "/input insert \\x03");
    gui_keyboard_default_bind (/* ^Cc           */ "ctrl-Ci",            "/input insert \\x1D");
    gui_keyboard_default_bind (/* ^Co           */ "ctrl-Co",            "/input insert \\x0F");
    gui_keyboard_default_bind (/* ^Cr           */ "ctrl-Cr",            "/input insert \\x12");
    gui_keyboard_default_bind (/* ^Cu           */ "ctrl-Cu",            "/input insert \\x15");
    gui_keyboard_default_bind (/* m-left        */ "meta-meta2-D",       "/buffer -1");
    gui_keyboard_default_bind (/* m-left (kde)  */ "meta2-1;3D",         "/buffer -1");
    gui_keyboard_default_bind (/* m-up          */ "meta-meta2-A",       "/buffer -1");
    gui_keyboard_default_bind (/* F5            */ "meta2-15~",          "/buffer -1");
    gui_keyboard_default_bind (/* ^P            */ "ctrl-P",             "/buffer -1");
    gui_keyboard_default_bind (/* m-right       */ "meta-meta2-C",       "/buffer +1");
    gui_keyboard_default_bind (/* m-right (kde) */ "meta2-1;3C",         "/buffer +1");
    gui_keyboard_default_bind (/* m-down        */ "meta-meta2-B",       "/buffer +1");
    gui_keyboard_default_bind (/* F6            */ "meta2-17~",          "/buffer +1");
    gui_keyboard_default_bind (/* ^N            */ "ctrl-N",             "/buffer +1");
    gui_keyboard_default_bind (/* pgup          */ "meta2-5~",           "/window page_up");
    gui_keyboard_default_bind (/* pgup          */ "meta2-I",            "/window page_up");
    gui_keyboard_default_bind (/* pgdn          */ "meta2-6~",           "/window page_down");
    gui_keyboard_default_bind (/* pgdn          */ "meta2-G",            "/window page_down");
    gui_keyboard_default_bind (/* m-pgup        */ "meta-meta2-5~",      "/window scroll_up");
    gui_keyboard_default_bind (/* m-pgdn        */ "meta-meta2-6~",      "/window scroll_down");
    gui_keyboard_default_bind (/* m-home        */ "meta-meta2-1~",      "/window scroll_top");
    gui_keyboard_default_bind (/* m-home        */ "meta-meta2-7~",      "/window scroll_top");
    gui_keyboard_default_bind (/* m-end         */ "meta-meta2-4~",      "/window scroll_bottom");
    gui_keyboard_default_bind (/* m-end         */ "meta-meta2-8~",      "/window scroll_bottom");
    gui_keyboard_default_bind (/* m-n           */ "meta-n",             "/window scroll_next_highlight");
    gui_keyboard_default_bind (/* m-p           */ "meta-p",             "/window scroll_previous_highlight");
    gui_keyboard_default_bind (/* F9            */ "meta2-20~",          "/bar scroll title * x-50%");
    gui_keyboard_default_bind (/* F10           */ "meta2-21~",          "/bar scroll title * x+50%");
    gui_keyboard_default_bind (/* F11           */ "meta2-23~",          "/bar scroll nicklist * y-100%");
    gui_keyboard_default_bind (/* F12           */ "meta2-24~",          "/bar scroll nicklist * y+100%");
    gui_keyboard_default_bind (/* m-F11         */ "meta-meta2-23~",     "/bar scroll nicklist * yb");
    gui_keyboard_default_bind (/* m-F12         */ "meta-meta2-24~",     "/bar scroll nicklist * ye");
    gui_keyboard_default_bind (/* ^L            */ "ctrl-L",             "/window refresh");
    gui_keyboard_default_bind (/* F7            */ "meta2-18~",          "/window -1");
    gui_keyboard_default_bind (/* F8            */ "meta2-19~",          "/window +1");
    gui_keyboard_default_bind (/* m-w,m-up      */ "meta-wmeta-meta2-A", "/window up");
    gui_keyboard_default_bind (/* m-w,m-down    */ "meta-wmeta-meta2-B", "/window down");
    gui_keyboard_default_bind (/* m-w,m-left    */ "meta-wmeta-meta2-D", "/window left");
    gui_keyboard_default_bind (/* m-w,m-right   */ "meta-wmeta-meta2-C", "/window right");
    gui_keyboard_default_bind (/* m-z           */ "meta-z",             "/window zoom");
    gui_keyboard_default_bind (/* m-=           */ "meta-=",             "/filter toggle");
    gui_keyboard_default_bind (/* m-0           */ "meta-0",             "/buffer *10");
    gui_keyboard_default_bind (/* m-1           */ "meta-1",             "/buffer *1");
    gui_keyboard_default_bind (/* m-2           */ "meta-2",             "/buffer *2");
    gui_keyboard_default_bind (/* m-3           */ "meta-3",             "/buffer *3");
    gui_keyboard_default_bind (/* m-4           */ "meta-4",             "/buffer *4");
    gui_keyboard_default_bind (/* m-5           */ "meta-5",             "/buffer *5");
    gui_keyboard_default_bind (/* m-6           */ "meta-6",             "/buffer *6");
    gui_keyboard_default_bind (/* m-7           */ "meta-7",             "/buffer *7");
    gui_keyboard_default_bind (/* m-8           */ "meta-8",             "/buffer *8");
    gui_keyboard_default_bind (/* m-9           */ "meta-9",             "/buffer *9");
    gui_keyboard_default_bind (/* m-<           */ "meta-<",             "/input jump_previously_visited_buffer");
    gui_keyboard_default_bind (/* m->           */ "meta->",             "/input jump_next_visited_buffer");
    
    /* bind meta-j + {01..99} to switch to buffers # > 10 */
    for (i = 1; i < 100; i++)
    {
        sprintf (key_str, "meta-j%02d", i);
        sprintf (command, "/buffer %d", i);
        gui_keyboard_default_bind (key_str, command);
    }
}

/*
 * gui_keyboard_flush: flush keyboard buffer
 */

void
gui_keyboard_flush ()
{
    int i, key, insert_ok;
    char key_str[32], *key_utf, *input_old;
    
    /* if there's no paste pending, then we use buffer and do actions
       according to keys */
    if (!gui_keyboard_paste_pending)
    {
        if (gui_keyboard_buffer_size > 0)
            gui_keyboard_last_activity_time = time (NULL);
        
        for (i = 0; i < gui_keyboard_buffer_size; i++)
        {
            key = gui_keyboard_buffer[i];
            
            insert_ok = 1;
            
            if (key < 32)
            {
                insert_ok = 0;
                key_str[0] = '^';
                key_str[1] = (char) key + '@';
                key_str[2] = '\0';
            }
            else if (key == 127)
            {
                key_str[0] = '^';
                key_str[1] = '?';
                key_str[2] = '\0';
            }
            else
            {
                if (local_utf8)
                {
                    /* 1 char: 0vvvvvvv */
                    if (key < 0x80)
                    {
                        key_str[0] = (char) key;
                        key_str[1] = '\0';
                    }
                    /* 2 chars: 110vvvvv 10vvvvvv */
                    else if ((key & 0xE0) == 0xC0)
                    {
                        key_str[0] = (char) key;
                        if (i < gui_keyboard_buffer_size - 1)
                        {
                            key_str[1] = (char) (gui_keyboard_buffer[++i]);
                            key_str[2] = '\0';
                        }
                        else
                            key_str[1] = '\0';
                    }
                    /* 3 chars: 1110vvvv 10vvvvvv 10vvvvvv */
                    else if ((key & 0xF0) == 0xE0)
                    {
                        key_str[0] = (char) key;
                        if (i < gui_keyboard_buffer_size - 1)
                        {
                            key_str[1] = (char) (gui_keyboard_buffer[++i]);
                            if (i < gui_keyboard_buffer_size - 1)
                            {
                                key_str[2] = (char) (gui_keyboard_buffer[++i]);
                                key_str[3] = '\0';
                            }
                            else
                                key_str[2] = '\0';
                        }
                        else
                            key_str[1] = '\0';
                    }
                    /* 4 chars: 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv */
                    else if ((key & 0xF8) == 0xF0)
                    {
                        key_str[0] = (char) key;
                        if (i < gui_keyboard_buffer_size - 1)
                        {
                            key_str[1] = (char) (gui_keyboard_buffer[++i]);
                            if (i < gui_keyboard_buffer_size - 1)
                            {
                                key_str[2] = (char) (gui_keyboard_buffer[++i]);
                                if (i < gui_keyboard_buffer_size - 1)
                                {
                                    key_str[3] = (char) (gui_keyboard_buffer[++i]);
                                    key_str[4] = '\0';
                                }
                                else
                                    key_str[3] = '\0';
                            }
                            else
                                key_str[2] = '\0';
                        }
                        else
                            key_str[1] = '\0';
                    }
                }
                else
                {
                    key_str[0] = (char) key;
                    key_str[1] = '\0';
                    
                    /* convert input to UTF-8 is user is not using UTF-8 as locale */
                    if (!local_utf8)
                    {
                        key_utf = string_iconv_to_internal (NULL, key_str);
                        strncpy (key_str, key_utf, sizeof (key_str));
                        key_str[sizeof (key_str) - 1] = '\0';
                    }
                }
            }
            
            if (strcmp (key_str, "^") == 0)
            {
                key_str[1] = '^';
                key_str[2] = '\0';
            }
            
            /*gui_printf (gui_current_window->buffer,
                          "gui_input_read: key = %s (%d)\n", key_str, key);*/
            
            hook_signal_send ("key_pressed",
                              WEECHAT_HOOK_SIGNAL_STRING, key_str);
            
            if (gui_current_window->buffer->text_search != GUI_TEXT_SEARCH_DISABLED)
                input_old = (gui_current_window->buffer->input_buffer) ?
                    strdup (gui_current_window->buffer->input_buffer) : strdup ("");
            else
                input_old = NULL;
            
            if ((gui_keyboard_pressed (key_str) != 0) && (insert_ok))
            {
                if (strcmp (key_str, "^^") == 0)
                    key_str[1] = '\0';
                
                gui_input_insert_string (gui_current_window->buffer,
                                         key_str, -1);
                if (gui_current_window->buffer->completion)
                    gui_completion_stop (gui_current_window->buffer->completion, 0);
                gui_input_text_changed_modifier_and_signal (gui_current_window->buffer);
            }
            
            /* incremental text search in buffer */
            if ((gui_current_window->buffer->text_search != GUI_TEXT_SEARCH_DISABLED)
                && ((input_old == NULL) || (gui_current_window->buffer->input_buffer == NULL)
                    || (strcmp (input_old, gui_current_window->buffer->input_buffer) != 0)))
            {
                gui_window_search_restart (gui_current_window);
            }
            
            if (input_old)
                free (input_old);
        }
        
        if (gui_key_grab && (gui_key_grab_count > 0))
            gui_keyboard_grab_end ();
        
        gui_keyboard_buffer_reset ();
    }
}

/*
 * gui_keyboard_read_cb: read keyboard chars
 */

int
gui_keyboard_read_cb (void *data, int fd)
{
    int ret, i, accept_paste, cancel_paste, text_added_to_buffer, paste_lines;
    unsigned char buffer[4096];
    
    /* make C compiler happy */
    (void) data;
    (void) fd;
    
    accept_paste = 0;
    cancel_paste = 0;
    text_added_to_buffer = 0;

    if (gui_keyboard_paste_pending)
    {
        ret = read (STDIN_FILENO, buffer, 1);
        if (ret == 0)
        {
            /* no data on stdin, terminal lost */
            log_printf (_("Terminal lost, exiting WeeChat..."));
            hook_signal_send ("quit", WEECHAT_HOOK_SIGNAL_STRING, NULL);
            weechat_quit = 1;
            return WEECHAT_RC_OK;
        }
        if (ret <= 0)
            return WEECHAT_RC_OK;
        
        /* ctrl-Y: accept paste */
        if (buffer[0] == 25)
            accept_paste = 1;
        
        /* ctrl-N: cancel paste */
        if (buffer[0] == 14)
            cancel_paste = 1;
    }
    else
    {
        ret = read (STDIN_FILENO, buffer, sizeof (buffer));
        if (ret == 0)
        {
            /* no data on stdin, terminal lost */
            log_printf (_("Terminal lost, exiting WeeChat..."));
            hook_signal_send ("quit", WEECHAT_HOOK_SIGNAL_STRING, NULL);
            weechat_quit = 1;
            return WEECHAT_RC_OK;
        }
        if (ret < 0)
            return WEECHAT_RC_OK;
        
        for (i = 0; i < ret; i++)
        {
            gui_keyboard_buffer_add (buffer[i]);
        }
        
        text_added_to_buffer = 1;
    }
    
    if (gui_keyboard_paste_pending)
    {
        /* user is ok for pasting text, let's paste! */
        if (accept_paste)
            gui_keyboard_paste_accept ();
        /* user doesn't want to paste text: clear whole buffer! */
        else if (cancel_paste)
            gui_keyboard_paste_cancel ();
        else if (text_added_to_buffer)
            gui_input_text_changed_modifier_and_signal (gui_current_window->buffer);
    }
    else
    {
        /* detect user paste or large amount of text
           if so, ask user what to do */
        if (CONFIG_INTEGER(config_look_paste_max_lines) > 0)
        {
            paste_lines = gui_keyboard_get_paste_lines ();
            if (paste_lines > CONFIG_INTEGER(config_look_paste_max_lines))
            {
                gui_keyboard_paste_pending = 1;
                gui_input_paste_pending_signal ();
            }
        }
    }
    
    gui_keyboard_flush ();
    
    return WEECHAT_RC_OK;
}
