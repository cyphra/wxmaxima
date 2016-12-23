// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2009-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2014-2016 Gunter Königsmann <wxMaxima@physikbuch.de>
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
  This file defines the class Structure

  Structure is the class that serves as the Table-Of-Contents sidebar.
*/

#include "TableOfContents.h"

#include <wx/sizer.h>
#include <wx/regex.h>

TableOfContents::TableOfContents(wxWindow* parent, int id) : wxPanel(parent, id)
{
  m_displayedItems = new wxListCtrl(this, structure_ctrl_id,wxDefaultPosition,wxDefaultSize,wxLC_SINGLE_SEL|wxLC_ALIGN_LEFT|wxLC_REPORT|wxLC_NO_HEADER);
  m_displayedItems->AppendColumn(wxEmptyString);
  m_regex = new wxTextCtrl(this, structure_regex_id);
  wxBoxSizer * box = new wxBoxSizer(wxVERTICAL);
//  box->AddGrowableCol(0);
//  box->AddGrowableRow(0);

  box->Add(m_displayedItems, 0, wxEXPAND | wxALL, 0);
  box->Add(m_regex, 0, wxEXPAND | wxALL, 1);
  m_lastSelection = -1;
  SetSizerAndFit(box);
//  box->SetSizeHints(this);
}

void TableOfContents::OnSize(wxSizeEvent& event)
{
    m_displayedItems->SetColumnWidth(0,event.GetSize().x);
}

TableOfContents::~TableOfContents()
{
  delete m_regex;
  delete m_displayedItems;
}

void TableOfContents::Update(MathCell* tree, GroupCell *cursorPosition)
{
  long selection = m_lastSelection;
  if(IsShown())
    {
      GroupCell* cell=  dynamic_cast<GroupCell*>(tree);
      m_structure.clear();
      
      // Get a new list of tokens.
      while(cell != NULL)
	{
	  int groupType = cell->GetGroupType();
	  if(
	     (groupType == GC_TYPE_TITLE) ||
	     (groupType == GC_TYPE_SECTION) ||
	     (groupType == GC_TYPE_SUBSECTION) ||
	     (groupType == GC_TYPE_SUBSUBSECTION)
            )
	    m_structure.push_back(cell);
          
          if(cell == cursorPosition)
          {
            if(!m_structure.empty())
              selection = m_structure.size()-1;
          }

	  cell = dynamic_cast<GroupCell*>(cell->m_next);
	}
      
      UpdateDisplay();

      long item = -1;
      item = m_displayedItems->GetNextItem(-1,
                                           wxLIST_NEXT_ALL,
                                           wxLIST_STATE_SELECTED);
      
      if((selection >= 0)&&(item != selection))
      {
        if((long)m_displayedItems->GetItemCount() < selection)
          selection = m_displayedItems->GetItemCount();
        if(m_displayedItems->GetItemCount()>0)
          m_displayedItems->SetItemState(selection,wxLIST_STATE_SELECTED,wxLIST_STATE_SELECTED);
        m_lastSelection = selection;
      }
    }
}

void TableOfContents::UpdateDisplay()
{
  wxLogNull disableWarnings;

  wxString regex = m_regex->GetValue();
  wxArrayString items;
  wxRegEx matcher;

  if (regex != wxEmptyString)
    matcher.Compile(regex);

  for (unsigned int i=0; i<m_structure.size(); i++)
  {
    // Indentation further reduces the screen real-estate. So it is to be used
    // sparingly. But we should perhaps add at least a little bit of it to make
    // the list more readable.
    wxString curr;
    switch(dynamic_cast<GroupCell*>(m_structure[i])->GetGroupType())
      {
      case GC_TYPE_TITLE:
	curr = m_structure[i]->GetEditable()->ToString(true);
	break;
      case GC_TYPE_SECTION:
	curr = wxT("  ") + m_structure[i]->GetEditable()->ToString(true);
	break;
      case GC_TYPE_SUBSECTION:
	curr = wxT("    ") + m_structure[i]->GetEditable()->ToString(true);
	break;
      case GC_TYPE_SUBSUBSECTION:
	curr = wxT("      ") + m_structure[i]->GetEditable()->ToString(true);
	break;
      }

    // Respecting linebreaks doesn't make much sense here.
    curr.Replace(wxT("\n"),wxT(" "));
    
    if (regex.Length()>0 && matcher.IsValid())
      {
	if (matcher.Matches(curr))
	  items.Add(curr);
      }
    else
      items.Add(curr);
  }

  // Work around a wxWidgets bug: items==m_items_old if items is empty and m_items_old isn't.
  if((items!=m_items_old)||(items.GetCount()==0))
  {

    // Update the name of all existing items and add new items, if necessary
    for(int i = 0;i<items.GetCount();i++)
    {
      if(i<m_displayedItems->GetItemCount())
      {
        m_displayedItems->SetItemText(i,items[i]);
      }
      else
      {
        m_displayedItems->InsertItem(i,items[i]);
      }
      if(m_structure[i]->GetHiddenTree())
        m_displayedItems->SetItemTextColour(i,wxColor(128,128,128));
      else
        m_displayedItems->SetItemTextColour(i,*wxBLACK);
      }
    // Delete superfluous items
    for(int i = items.GetCount(); i < m_displayedItems->GetItemCount();i++)
    {
      m_displayedItems->DeleteItem(i);
    }
  }

}

MathCell *TableOfContents::GetCell(int index)
{
  wxRegEx matcher;
  int currentIndex = -1;
  wxString regex = m_regex->GetValue();

  if (regex != wxEmptyString)
    matcher.Compile(regex);

  for (unsigned int i=0; i<m_structure.size(); i++)
  {
    
    wxString curr;
    switch((m_structure[i])->GetGroupType())
    {
    case GC_TYPE_TITLE:
      curr = m_structure[i]->GetEditable()->ToString(true);
      break;
    case GC_TYPE_SECTION:
      curr = wxT("  ") + m_structure[i]->GetEditable()->ToString(true);
      break;
    case GC_TYPE_SUBSECTION:
      curr = wxT("    ") + m_structure[i]->GetEditable()->ToString(true);
      break;
    case GC_TYPE_SUBSUBSECTION:
      curr = wxT("      ") + m_structure[i]->GetEditable()->ToString(true);
      break;
    }
    
    // Respecting linebreaks doesn't make much sense here.
    curr.Replace(wxT("\n"),wxT(" "));

    if (regex.Length()>0)
    {
      if((matcher.IsValid())&&((matcher.Matches(curr))))
        currentIndex ++;
    }
    
    else
      currentIndex ++;

    if(currentIndex == index)
      return m_structure[i];
  }
  return NULL;
}

void TableOfContents::OnRegExEvent(wxCommandEvent &ev)
{
  UpdateDisplay();
}

BEGIN_EVENT_TABLE(TableOfContents, wxPanel)
  EVT_TEXT(structure_regex_id, TableOfContents::OnRegExEvent)
  EVT_SIZE(TableOfContents::OnSize)
END_EVENT_TABLE()
