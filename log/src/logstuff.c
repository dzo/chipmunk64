/* "LOG", the circuit editing and simulation system,
   "DigLOG", the digital simulator for LOG.
   Copyright (C) 1985, 1990 David Gillespie.
   Author's address: daveg@csvax.caltech.edu; 256-80 Caltech/Pasadena CA 91125.

   "AnaLOG", the analog simulator for LOG.
   Copyright (C) 1985, 1990 John Lazzaro.
   Author's address: lazzaro@csvax.caltech.edu; 256-80 Caltech.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (any version).

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */



#include <p2c/p2c.h>

#include <p2c/mylib.h>

#include "logstuff.h"

#include "logdef.h"


#include "logcurs_arr.h"
#include "logcurs_cpy.h"
#include "logcurs_del.h"
#include "logcurs_prb.h"
#include "logcurs_box.h"

void setup_log_cursors()
{
 
}


static int cursor_color = -1;

void recolor_log_cursors(int color, int force)

{

}


static int cursor_shape = -1;

void choose_log_cursor(int curs)

{
  m_choosecursor(curs);

}




void init_X_screen()
{
  setup_log_cursors();
  choose_log_cursor(0);
}





void m_bunny(int x, int y)
{
  m_colormode(m_xor);
  m_color(m_white);
  m_drawstr(x, y, NULL, "Boink");
}


int save_clip_x1, save_clip_y1, save_clip_x2, save_clip_y2;
extern int m_clip_x1, m_clip_y1, m_clip_x2, m_clip_y2;

void m_saveclip()
{
  save_clip_x1 = m_clip_x1;
  save_clip_x2 = m_clip_x2;
  save_clip_y1 = m_clip_y1;
  save_clip_y2 = m_clip_y2;
}

void m_unclip()
{
  m_clip(save_clip_x1, save_clip_y1, save_clip_x2, save_clip_y2);
}


void m_setfont()
{
}

void m_seefont()
{
}

void m_disposepicture()
{
}

void m_getcpicture()
{
}

void m_putcpicture()
{
}


void m_drawarrow(long x1, long y1, long x2, long y2, long a, long b)
{
  m_drawline(x1, y1, x2, y2);
}


void BEEPER(int x, int y)
{
 // XBell(m_display, 0);
}


boolean nk_setcapslock(boolean newval)
{
  return false;
}


void nc_cursXY(int x, int y)
{
}

void nc_scrollXY(int x, int y)
{
}



char *my_strdup(char *s)
{
  char *buf = Malloc(strlen(s) + 1);
  strcpy(buf, s);
  return buf;
}



extern struct ext_proc ext_proc_table[];

boolean findprocedure(char *name, Void (**proc)())
{
  int i;

  if (*name) {
    for (i = 0; ext_proc_table[i].name; i++) {
      if (strciends(name, ext_proc_table[i].name) ||
	  strciends(ext_proc_table[i].name, name)) {
	*proc = ext_proc_table[i].proc;
	return true;
      }
    }
  }
  return false;
}



void newci_inputmap()
{
}

void newci_inputunmap()
{
}

void nc_insLine(x, dx)
int x, dx;
{
  printf("nc_insLine not implemented\n");
}


