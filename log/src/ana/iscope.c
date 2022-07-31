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

/* Output from p2c, the Pascal-to-C translator */
/* From input file "tools.text" */



#include <p2c/p2c.h>





#define ISCOPE_G
#include "iscope.h"


#define N_Present       1
#define N_Format        2
#define N_Prec          3

#define Gatename        "Current meter"


typedef struct ICIscope {
  Char InitVsymbol[33], InitIsymbol[33];
  long Initprec;
} ICIscope;




typedef struct Inoderec {
  log_grec *Igate;   /*Gate connected to meter*/
  long Ipin;   /*Pin connected to meter*/
  struct Inoderec *Next;
} Inoderec;

typedef struct Imeterrec {
  Inoderec *Ilist;
  double NewValue;
  boolean NewBlank;
  Char NewOutStr[256], OldOutStr[256];
  boolean Valid, OldValid;
} Imeterrec;



/******************************************************************************/
/*              Procedure to correctly position labels                        */


Local Void Iscope_Locate(rot, Tx, Ty, Left)
long rot;
short *Tx, *Ty;
boolean *Left;
{

  /******************************************************************************/
  switch (rot) {

  case 0:
  case 4:
    *Tx = 7;
    *Ty = -3;
    break;

  case 2:
  case 6:
    *Tx = 7;
    *Ty = 3;
    break;

  case 3:
  case 5:
    *Tx = -3;
    *Ty = -7;
    break;

  case 1:
  case 7:
    *Tx = 3;
    *Ty = -7;
    break;
  }
  if ((unsigned long)rot < 32 && ((1L << rot) & 0xc9) != 0)
    *Left = true;
  else
    *Left = false;
}


Local double Itot(act)
Analog_32_action *act;
{
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  Inoderec *Count;
  Analog_32_action Iinact;
  AnaCell_rec *A_Kind;

  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  Count = Intptr->Ilist;
  if (Count == NULL) {
    act->ok = false;
    return 0.0;
  } else {
    act->ok = true;
    Iinact.Iin = 0.0;
    while (Count != NULL) {
      Iinact.pin = Count->Ipin;
      Iinact.inst = Count->Igate;
      Iinact.action = Analog_act_Iin;
      A_Kind = (AnaCell_rec *)Count->Igate->kind->info;
      (*A_Kind->proc_cell)(&Iinact);
      Count = Count->Next;
    }
    return (Iinact.Iin);
  }
}


/*******************************************************************************/
/*                Procedure to update cell in algorithm                        */
/*******************************************************************************/

Local Void PreUpdate_Iscope(act)
Analog_32_action *act;
{
  AnaExt_rec *A_Node;
  AnaCell_rec *Cellptr;
  ICIscope *ICptr;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  AnaExt_rec *Inode;
  log_grec *WITH;
  Imeterrec *WITH1;
  ICIscope *WITH2;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIscope *)Cellptr->Ainfo;
  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  WITH = act->inst;
  WITH1 = Intptr;
  WITH2 = ICptr;
  WITH1->OldValid = WITH1->Valid;
  WITH1->Valid = false;
  A_Node = (AnaExt_rec *)WITH->pin[0]->info;
  if (A_Node->refcnt == 0) {
    WITH1->NewValue = 0.0;
    strcpy(WITH1->NewOutStr, WITH2->InitIsymbol);
    WITH1->NewBlank = true;
    return;
  }
  WITH1->NewValue = Itot(act);
  if (act->ok) {
    WITH1->NewBlank = false;
    if (fabs(WITH1->NewValue) < 1e-15)
      (*AnaLogglobals->hook.realunit)(0.0,
	(int)WITH->attr[N_Prec - 1].UU.U73.i1, "A", WITH1->NewOutStr);
    else
      (*AnaLogglobals->hook.realunit)(WITH1->NewValue,
	(int)WITH->attr[N_Prec - 1].UU.U73.i1, "A", WITH1->NewOutStr);
    Inode = (AnaExt_rec *)act->inst->pin[1]->info;
    Inode->now += WITH1->NewValue;
    return;
  }
  WITH1->NewBlank = true;
  strcpy(WITH1->NewOutStr, WITH2->InitIsymbol);
  WITH1->NewValue = 0.0;
}  /*PreUpdate_Iscope*/

/*******************************************************************************/
/*                Procedure to update cell in algorithm                        */
/*******************************************************************************/

Local Void Update_Iscope(act)
Analog_32_action *act;
{
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  log_grec *WITH;
  log_gattrrec *WITH2;

  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  WITH = act->inst;
  Intptr->Valid = true;
  WITH2 = &WITH->attr[N_Present - 1];
  WITH2->changed = true;
  if (Intptr->NewBlank)
    WITH2->blnk = true;
  else {
    WITH2->UU.r = Intptr->NewValue;
    WITH2->blnk = false;
  }
  WITH2 = &WITH->attr[N_Format - 1];
  if (!Intptr->OldValid)
    strcpy(Intptr->OldOutStr, WITH2->UU.c);
  strcpy(WITH2->UU.c, Intptr->NewOutStr);
  WITH2->changed = true;
}  /*Update_Iscope*/

/*******************************************************************************/
/*                Procedure to update cell in algorithm                        */
/*******************************************************************************/

Local Void Display_Iscope(act)
Analog_32_action *act;
{
  short tx, ty;
  boolean Left;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  log_grec *WITH;
  Imeterrec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  WITH = act->inst;
  WITH1 = Intptr;
  WITH1->Valid = false;
  if (!strcmp(WITH1->OldOutStr, WITH1->NewOutStr) ||
      act->page != AnaLogglobals->showpage)
    return;
  (*AnaLogglobals->hook.hidecursor)();
  Iscope_Locate((long)WITH->rot, &tx, &ty, &Left);
  (*AnaLogglobals->hook.xform)(act->inst, &tx, &ty);
  m_color((long)AnaLogglobals->color.backgr);
  if (Left)
    m_drawstr((long)tx, (long)ty, NULL, WITH1->OldOutStr);
  else
    m_rightstr((long)tx, (long)ty, NULL, WITH1->OldOutStr);
  m_color((long)AnaColormap.C_Imeter);
  if (Left)
    m_drawstr((long)tx, (long)ty, NULL, WITH1->NewOutStr);
  else
    m_rightstr((long)tx, (long)ty, NULL, WITH1->NewOutStr);
  (*AnaLogglobals->hook.unhidecursor)();
}  /*Update_Iscope*/



Local Void Pass1_Iscope(act)
Analog_32_action *act;
{
  long Mx, My;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  Inoderec *Count, *Newnode;
  log_grec *Gcount;
  long Index;
  AnaExt_rec *Inode;
  log_grec *WITH;
  long FORLIM;

  Inode = (AnaExt_rec *)act->inst->pin[1]->info;
  Inode->now = 0.0;
  WITH = act->inst;
  Mx = WITH->pinpos[0].x;
  My = WITH->pinpos[0].y;
  A_Gate = (Anainstlist *)WITH->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  Count = Intptr->Ilist;
  while (Count != NULL) {   /*Dispose any previous list*/
    Newnode = Count;
    Count = Count->Next;
    Free(Newnode);
  }
  Intptr->Ilist = NULL;
  Gcount = AnaLogglobals->gbase[act->page - 1];
  while (Gcount != NULL) {   /*Make new list*/
    WITH = Gcount;
    if (WITH->kind->simtype->simtype == 32 && Gcount != act->inst) {
      FORLIM = WITH->kind->numpins;
      for (Index = 1; Index <= FORLIM; Index++) {
	if (WITH->pinpos[Index - 1].x == Mx && WITH->pinpos[Index - 1].y == My) {
	  Newnode = (Inoderec *)Malloc(sizeof(Inoderec));
	  Newnode->Igate = Gcount;
	  Newnode->Ipin = Index;
	  Newnode->Next = Intptr->Ilist;
	  Intptr->Ilist = Newnode;
	}
      }
    }
    Gcount = Gcount->next;
  }
}

/*******************************************************************************/
/*                Procedure to update cell in algorithm                        */
/*******************************************************************************/

Local Void Refresh_Iscope(act)
Analog_32_action *act;
{
  short tx, ty;
  boolean Left;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  log_grec *WITH;
  Imeterrec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  WITH = act->inst;
  WITH1 = Intptr;
  (*AnaLogglobals->hook.hidecursor)();
  Iscope_Locate((long)WITH->rot, &tx, &ty, &Left);
  (*AnaLogglobals->hook.xform)(act->inst, &tx, &ty);
  if (WITH1->Valid) {
    WITH1->Valid = false;
    if (strcmp(WITH1->OldOutStr, WITH->attr[N_Format - 1].UU.c)) {
      m_color((long)AnaLogglobals->color.backgr);
      if (Left)
	m_drawstr((long)tx, (long)ty, NULL, WITH1->OldOutStr);
      else
	m_rightstr((long)tx, (long)ty, NULL, WITH1->OldOutStr);
    }
  }
  m_color((long)AnaColormap.C_Imeter);
  if (Left)
    m_drawstr((long)tx, (long)ty, NULL, WITH->attr[N_Format - 1].UU.c);
  else
    m_rightstr((long)tx, (long)ty, NULL, WITH->attr[N_Format - 1].UU.c);
  (*AnaLogglobals->hook.unhidecursor)();
}  /*Refresh_Iscope*/

/*******************************************************************************/
/*                Procedure to give current into cell                          */
/*******************************************************************************/

Local Void Iin_Iscope(act)
Analog_32_action *act;
{
  log_grec *WITH;

  WITH = act->inst;
}

/*******************************************************************************/
/*                Procedure to update meter display                            */
/*******************************************************************************/

Local Void Erase_Iscope(act)
Analog_32_action *act;
{
  short tx, ty;
  boolean Left;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  log_grec *WITH;
  Imeterrec *WITH1;

  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  WITH = act->inst;
  WITH1 = Intptr;
  (*AnaLogglobals->hook.hidecursor)();
  Iscope_Locate((long)WITH->rot, &tx, &ty, &Left);
  (*AnaLogglobals->hook.xform)(act->inst, &tx, &ty);
  m_color((long)AnaLogglobals->color.backgr);
  if (WITH1->Valid) {
    WITH1->Valid = false;
    if (Left)
      m_drawstr((long)tx, (long)ty, NULL, WITH1->OldOutStr);
    else
      m_rightstr((long)tx, (long)ty, NULL, WITH1->OldOutStr);
  } else {
    if (Left)
      m_drawstr((long)tx, (long)ty, NULL, WITH->attr[N_Format - 1].UU.c);
    else
      m_rightstr((long)tx, (long)ty, NULL, WITH->attr[N_Format - 1].UU.c);
  }
  (*AnaLogglobals->hook.unhidecursor)();
}  /*Erase_Iscope*/


Local Void GetCnf_Iscope(NewIC)
ICIscope *NewIC;
{
  boolean Found;
  Char Arg[256], Keyword[256];
  long Dummy;
  ICIscope *WITH;
  Char STR1[256];
  Char STR2[256];
  Char *STR3;

  WITH = NewIC;
  do {
    AnaGetcommand("ISCOPE", Arg, &Found);
    if (Found) {
      (*AnaLogglobals->hook.getword)(Arg, Keyword);
      if (!strcmp(Keyword, "VNULL")) {
	strcpy(Keyword, Arg);
	strcpy(STR1, strltrim(strrtrim(strcpy(STR2, Keyword))));
	strcpy(Keyword, STR1);
	/* Strread(Arg,1,Dummy,KeyWord); */
	if (strlen(Keyword) > 32)
	  Keyword[32] = '\0';
	strcpy(WITH->InitVsymbol, Keyword);
	strcpy(Keyword, "  ");
      }
      if (!strcmp(Keyword, "INULL")) {
	strcpy(Keyword, Arg);
	strcpy(STR1, strltrim(strrtrim(strcpy(STR2, Keyword))));
	strcpy(Keyword, STR1);
	/* Strread(Arg,1,Dummy,KeyWord); */
	if (strlen(Keyword) > 32)
	  Keyword[32] = '\0';
	strcpy(WITH->InitIsymbol, Keyword);
	strcpy(Keyword, "  ");
      }
      if (!strcmp(Keyword, "PREC")) {
	TRY(try1);
	  WITH->Initprec = strtol(Arg, &STR3, 10);
	  Dummy = STR3 - Arg + 1;
	RECOVER(try1);
	  WITH->Initprec = 3;
	ENDTRY(try1);
      }
    }
  } while (Found);
}


/*******************************************************************************/
/*              Procedure to initialize cell in library                        */
/*******************************************************************************/


Local Void INITLIB_ISCOPE(act)
Analog_32_action *act;
{
  AnaCell_rec *NewCell;
  ICIscope *NewIC;

  NewCell = NULL;
  NewCell = (AnaCell_rec *)Malloc(sizeof(AnaCell_rec));
  NewCell->simulatable = true;
  NewCell->intr_cell = 0;
  NewCell->phase = Anacomplex;
  NewCell->plist_cell = (Anapinrec *)Malloc(sizeof(Anapinrec) * act->kind->numpins);
  NewCell->plist_cell[0].standalone = false;
  NewCell->plist_cell[0].active = false;
  NewIC = (ICIscope *)Malloc(sizeof(ICIscope));
  strcpy(NewIC->InitVsymbol, "~");
  strcpy(NewIC->InitIsymbol, "Not on a pin");
  NewIC->Initprec = 3;
  GetCnf_Iscope(NewIC);
  NewCell->Ainfo = (Anyptr)NewIC;
  act->kind->info = (Anyptr)NewCell;
}  /*Iscope_initlib*/


Local Void Attr_Iscope(act)
Analog_32_action *act;
{
  long Attrnum;
  Char Oldout[256], Newout[256];
  double Lastval;
  short tx, ty;
  boolean Left;
  AnaCell_rec *Cellptr;
  ICIscope *ICptr;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  Analog_32_action *WITH;
  ICIscope *WITH1;
  log_grec *WITH2;
  log_gattrrec *WITH3;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIscope *)Cellptr->Ainfo;
  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  WITH = act;
  WITH1 = ICptr;
  Attrnum = WITH->pin;
  if ((unsigned long)Attrnum < 32 &&
      ((1L << Attrnum) & ((1L << N_Present) | (1L << N_Format))) != 0) {
    WITH->ok = false;
    return;
  }
  if (Attrnum != N_Prec)
    return;
  WITH->ok = AnaDisplayrange(WITH->inst, Attrnum);
  if (!WITH->ok || WITH->inst->attr[N_Present - 1].blnk)
    return;
  WITH2 = WITH->inst;
  Lastval = WITH2->attr[N_Present - 1].UU.r;
  if (fabs(Lastval) < 1e-15)
    Lastval = 0.0;
  (*AnaLogglobals->hook.realunit)(Lastval,
    (int)WITH2->attr[N_Prec - 1].UU.U73.i1, "A", Newout);
  if (Intptr->Valid) {
    Intptr->Valid = false;
    strcpy(Oldout, Intptr->OldOutStr);
  } else
    strcpy(Oldout, WITH2->attr[N_Format - 1].UU.c);
  WITH3 = &WITH2->attr[N_Format - 1];
  strcpy(WITH3->UU.c, Newout);
  WITH3->changed = true;
  if (!AnaGate_On_Display(WITH->inst))
    return;
  (*AnaLogglobals->hook.hidecursor)();
  Iscope_Locate((long)WITH2->rot, &tx, &ty, &Left);
  (*AnaLogglobals->hook.xform)(WITH->inst, &tx, &ty);
  m_color((long)AnaLogglobals->color.backgr);
  if (Left)
    m_drawstr((long)tx, (long)ty, NULL, Oldout);
  else
    m_rightstr((long)tx, (long)ty, NULL, Oldout);
  m_color((long)AnaColormap.C_Imeter);
  if (Left)
    m_drawstr((long)tx, (long)ty, NULL, Newout);
  else
    m_rightstr((long)tx, (long)ty, NULL, Newout);
  (*AnaLogglobals->hook.unhidecursor)();
}

/*******************************************************************************/
/*                Procedure to reset cell in algorithm                       */
/*******************************************************************************/

Local Void Reset_Iscope(act)
Analog_32_action *act;
{
  short tx, ty;
  Char Oldout[256];
  boolean Left;
  AnaCell_rec *Cellptr;
  ICIscope *ICptr;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  log_grec *WITH;
  ICIscope *WITH1;
  log_gattrrec *WITH2;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIscope *)Cellptr->Ainfo;
  WITH = act->inst;
  WITH1 = ICptr;
  if (act->page == AnaLogglobals->showpage) {
    A_Gate = (Anainstlist *)act->inst->info;
    Intptr = (Imeterrec *)A_Gate->InstVar;
    if (Intptr->Valid) {
      Intptr->Valid = false;
      strcpy(Oldout, Intptr->OldOutStr);
    } else
      strcpy(Oldout, WITH->attr[N_Format - 1].UU.c);
    (*AnaLogglobals->hook.hidecursor)();
    Iscope_Locate((long)WITH->rot, &tx, &ty, &Left);
    (*AnaLogglobals->hook.xform)(act->inst, &tx, &ty);
    m_color((long)AnaLogglobals->color.backgr);
    if (Left)
      m_drawstr((long)tx, (long)ty, NULL, Oldout);
    else
      m_rightstr((long)tx, (long)ty, NULL, Oldout);
  }
  WITH2 = &WITH->attr[N_Format - 1];
  strcpy(WITH2->UU.c, WITH1->InitVsymbol);
  WITH2->changed = true;
  WITH2 = &WITH->attr[N_Present - 1];
  WITH2->blnk = true;
  WITH2->changed = true;
  if (act->page != AnaLogglobals->showpage)
    return;
  strcpy(Oldout, WITH->attr[N_Format - 1].UU.c);
  m_color((long)AnaColormap.C_Imeter);
  if (Left)
    m_drawstr((long)tx, (long)ty, NULL, Oldout);
  else
    m_rightstr((long)tx, (long)ty, NULL, Oldout);
  (*AnaLogglobals->hook.unhidecursor)();
}  /*Reset_Iscope*/

/*******************************************************************************/
/*                Procedure to disconnect a Iscope                             */
/*******************************************************************************/

Local Void Disconnect_Iscope(act)
Analog_32_action *act;
{
  short tx, ty;
  Char Oldout[256];
  boolean Left;
  AnaCell_rec *Cellptr;
  ICIscope *ICptr;
  Imeterrec *Intptr;
  Anainstlist *A_Gate;
  log_grec *WITH;
  ICIscope *WITH1;
  log_gattrrec *WITH2;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIscope *)Cellptr->Ainfo;
  WITH = act->inst;
  WITH1 = ICptr;
  A_Gate = (Anainstlist *)act->inst->info;
  Intptr = (Imeterrec *)A_Gate->InstVar;
  if (Intptr->Valid) {
    Intptr->Valid = false;
    strcpy(Oldout, Intptr->OldOutStr);
  } else
    strcpy(Oldout, WITH->attr[N_Format - 1].UU.c);
  (*AnaLogglobals->hook.hidecursor)();
  Iscope_Locate((long)WITH->rot, &tx, &ty, &Left);
  (*AnaLogglobals->hook.xform)(act->inst, &tx, &ty);
  m_color((long)AnaLogglobals->color.backgr);
  if (Left)
    m_drawstr((long)tx, (long)ty, NULL, Oldout);
  else
    m_rightstr((long)tx, (long)ty, NULL, Oldout);
  (*AnaLogglobals->hook.unhidecursor)();
  WITH2 = &WITH->attr[N_Format - 1];
  strcpy(WITH2->UU.c, WITH1->InitVsymbol);
  WITH2->changed = true;
  WITH2 = &WITH->attr[N_Present - 1];
  WITH2->blnk = true;
  WITH2->changed = true;
}  /*Disconnect_Iscope*/



Local Void Newgate_Iscope(act)
Analog_32_action *act;
{
  AnaCell_rec *Cellptr;
  ICIscope *ICptr;
  Imeterrec *NewIV;
  Anainstlist *A_Gate;
  log_grec *WITH;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIscope *)Cellptr->Ainfo;
  WITH = act->inst;
  strcpy(WITH->attr[N_Format - 1].UU.c, ICptr->InitVsymbol);
  WITH->attr[N_Format - 1].blnk = false;
  WITH->attr[N_Format - 1].changed = true;
  WITH->attr[N_Prec - 1].UU.U73.i1 = ICptr->Initprec;
  WITH->attr[N_Prec - 1].blnk = false;
  WITH->attr[N_Prec - 1].changed = true;
  NewIV = (Imeterrec *)Malloc(sizeof(Imeterrec));
  NewIV->Ilist = NULL;
  NewIV->NewValue = 0.0;
  NewIV->NewBlank = true;
  strcpy(NewIV->NewOutStr, ICptr->InitVsymbol);
  strcpy(NewIV->OldOutStr, ICptr->InitVsymbol);
  NewIV->Valid = false;
  NewIV->OldValid = false;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NewIV;
}

Local Void Copygate_Iscope(act)
Analog_32_action *act;
{
  AnaCell_rec *Cellptr;
  ICIscope *ICptr;
  Imeterrec *NewIV;
  Anainstlist *A_Gate;

  Cellptr = (AnaCell_rec *)act->inst->kind->info;
  ICptr = (ICIscope *)Cellptr->Ainfo;
  NewIV = (Imeterrec *)Malloc(sizeof(Imeterrec));
  NewIV->Ilist = NULL;
  NewIV->NewValue = 0.0;
  NewIV->NewBlank = true;
  strcpy(NewIV->NewOutStr, ICptr->InitVsymbol);
  strcpy(NewIV->OldOutStr, ICptr->InitVsymbol);
  NewIV->Valid = false;
  NewIV->OldValid = false;
  A_Gate = (Anainstlist *)act->inst->info;
  A_Gate->InstVar = (Anyptr)NewIV;
}


Local Void Probe_Iscope(act)
Analog_32_action *act;
{
  AnaScoreboard(Gatename, (long)AnaMessGate1);
}



/*=============================================================================*/
/*                  Cell Library Entry   Iscope                                */
/*=============================================================================*/

Void Log_iscope_initlib_32(act)
Analog_32_action *act;
{
  /*Body of procedure*/


  switch (act->action) {

  case Analog_act_newkind:
    INITLIB_ISCOPE(act);
    break;

  case Analog_act_preupdate:
    PreUpdate_Iscope(act);
    break;

  case Analog_act_update:
    Update_Iscope(act);
    break;

  case Analog_act_display:
    Display_Iscope(act);
    break;

  case Analog_act_erase:
    Erase_Iscope(act);
    break;

  case Analog_act_refresh:
    Refresh_Iscope(act);
    break;

  case Analog_act_attrchange:
    Attr_Iscope(act);
    break;

  case Analog_act_reset:
    Reset_Iscope(act);
    break;

  case Analog_act_disconnect:
    Disconnect_Iscope(act);
    break;

  case Analog_act_newgate:
    Newgate_Iscope(act);
    break;

  case Analog_act_copygate:
    Copygate_Iscope(act);
    break;

  case Analog_act_Iin:
    Iin_Iscope(act);
    break;

  case Analog_act_pass1:
    Pass1_Iscope(act);
    break;

  case Analog_act_probe:
    Probe_Iscope(act);
    break;

  default:
    break;
  }
}

#undef N_Present
#undef N_Format
#undef N_Prec
#undef Gatename








/* End. */
