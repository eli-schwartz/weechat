/*
 * Copyright (c) 2003-2007 by FlashCode <flashcode@flashtux.org>
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

/* gui-history.c: memorize commands or text for buffers */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "../core/weechat.h"
#include "../core/wee-config.h"
#include "../core/wee-string.h"
#include "gui-history.h"
#include "gui-buffer.h"


struct t_gui_history *history_global = NULL;
struct t_gui_history *history_global_last = NULL;
struct t_gui_history *history_global_ptr = NULL;
int num_history_global = 0;


/*
 * gui_history_buffer_add: add a text/command to buffer's history
 */

void
gui_history_buffer_add (void *buffer, char *string)
{
    struct t_gui_buffer *ptr_buffer;
    struct t_gui_history *new_history, *ptr_history;

    ptr_buffer = (struct t_gui_buffer *)buffer;
    
    if (!string)
        return;
    
    if (!ptr_buffer->history
        || (ptr_buffer->history
            && (string_strcasecmp (ptr_buffer->history->text,
                                   string) != 0)))
    {	
	new_history = (struct t_gui_history *)malloc (sizeof (struct t_gui_history));
	if (new_history)
	{
	    new_history->text = strdup (string);
	    /*if (config_log_hide_nickserv_pwd)
              irc_display_hide_password (new_history->text, 1);*/
	    
	    if (ptr_buffer->history)
		ptr_buffer->history->prev_history = new_history;
	    else
		ptr_buffer->last_history = new_history;
	    new_history->next_history = ptr_buffer->history;
	    new_history->prev_history = NULL;
	    ptr_buffer->history = new_history;
	    ptr_buffer->num_history++;
	    
	    /* remove one command if necessary */
	    if ((CONFIG_INTEGER(config_history_max_commands) > 0)
		&& (ptr_buffer->num_history > CONFIG_INTEGER(config_history_max_commands)))
	    {
		ptr_history = ptr_buffer->last_history->prev_history;
                if (ptr_buffer->ptr_history == ptr_buffer->last_history)
                    ptr_buffer->ptr_history = ptr_history;
		ptr_buffer->last_history->prev_history->next_history = NULL;
		if (ptr_buffer->last_history->text)
		    free (ptr_buffer->last_history->text);
		free (ptr_buffer->last_history);
		ptr_buffer->last_history = ptr_history;
		ptr_buffer->num_history++;
	    }
	}
    }
}

/*
 * history_global_add: add a text/command to buffer's history
 */

void
gui_history_global_add (char *string)
{
    struct t_gui_history *new_history, *ptr_history;

    if (!string)
        return;
    
    if (!history_global
        || (history_global
            && (string_strcasecmp (history_global->text, string) != 0)))
    {    
	new_history = (struct t_gui_history *)malloc (sizeof (struct t_gui_history));
	if (new_history)
	{
	    new_history->text = strdup (string);
	    /*if (config_log_hide_nickserv_pwd)
              irc_display_hide_password (new_history->text, 1);*/
	    
	    if (history_global)
		history_global->prev_history = new_history;
	    else
		history_global_last = new_history;
	    new_history->next_history = history_global;
	    new_history->prev_history = NULL;
	    history_global = new_history;
	    num_history_global++;
	    
	    /* remove one command if necessary */
	    if ((CONFIG_INTEGER(config_history_max_commands) > 0)
		&& (num_history_global > CONFIG_INTEGER(config_history_max_commands)))
	    {
		ptr_history = history_global_last->prev_history;
                if (history_global_ptr == history_global_last)
                    history_global_ptr = ptr_history;
		history_global_last->prev_history->next_history = NULL;
		if (history_global_last->text)
		    free (history_global_last->text);
		free (history_global_last);
		history_global_last = ptr_history;
		num_history_global--;
	    }
	}
    }
}

/*
 * gui_history_global_free: free global history
 */

void
gui_history_global_free ()
{
    struct t_gui_history *ptr_history;
    
    while (history_global)
    {
        ptr_history = history_global->next_history;
        if (history_global->text)
            free (history_global->text);
        free (history_global);
        history_global = ptr_history;
    }
    history_global = NULL;
    history_global_last = NULL;
    history_global_ptr = NULL;
    num_history_global = 0;
}


/*
 * gui_history_buffer_free: free history for a buffer
 */

void
gui_history_buffer_free (void *buffer)
{
    struct t_gui_buffer *ptr_buffer;
    struct t_gui_history *ptr_history;
    
    ptr_buffer = (struct t_gui_buffer *)buffer;
    
    while (ptr_buffer->history)
    {
        ptr_history = ptr_buffer->history->next_history;
        if (ptr_buffer->history->text)
            free (ptr_buffer->history->text);
        free (ptr_buffer->history);
        ptr_buffer->history = ptr_history;
    }
    ptr_buffer->history = NULL;
    ptr_buffer->last_history = NULL;
    ptr_buffer->ptr_history = NULL;
    ptr_buffer->num_history = 0;
}
