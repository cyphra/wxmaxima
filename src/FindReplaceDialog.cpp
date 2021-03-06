// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C)      2016 Gunter Königsmann <wxMaxima@physikbuch.de>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/*! \file
  This file defines the class FindReplaceDialog

  FindReplaceDialog is the find/replace dialog
 */


#include "FindReplaceDialog.h"
#include "EditorCell.h"

FindReplaceDialog::FindReplaceDialog(wxWindow *parent, wxFindReplaceData *data, const wxString &title, int style):
  wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
  m_active = true;
  m_contents = new FindReplacePane(this,data);
  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  vbox->Add(m_contents,wxSizerFlags().Expand());
  SetSizerAndFit(vbox);
}

void FindReplaceDialog::OnKeyDown(wxKeyEvent& event)
{
  if(event.GetKeyCode()==WXK_ESCAPE)
    Close();
  else
    event.Skip();
}
  
void FindReplaceDialog::OnClose(wxCloseEvent& event)
{
  wxFindDialogEvent *findEvent = new wxFindDialogEvent(wxEVT_FIND_CLOSE);
  GetParent()->GetEventHandler()->QueueEvent(findEvent);
}

void FindReplaceDialog::OnActivate(wxActivateEvent& event)
{
  if(event.GetActive())
    SetTransparent(255);
  else
    SetTransparent(180);
  m_active = true;
}


BEGIN_EVENT_TABLE(FindReplaceDialog, wxDialog)
 EVT_ACTIVATE(FindReplaceDialog::OnActivate)
 EVT_CHAR_HOOK(FindReplaceDialog::OnKeyDown)
 EVT_CLOSE(FindReplaceDialog::OnClose)

END_EVENT_TABLE()
