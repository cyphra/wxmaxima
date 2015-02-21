//
//  Copyright (C) 2004-2014 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//            (C) 2008-2009 Ziga Lenarcic <zigalenarcic@users.sourceforge.net>
//            (C) 2012-2013 Doug Ilijev <doug.ilijev@gmail.com>
//            (C) 2015      Gunter Königsmann <wxMaxima@physikbuch.de>
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

#include "ToolBar.h"
#include <wx/artprov.h>

#if defined (__WXMSW__) || defined (__WXMAC__)

#define IMAGE(img) wxImage(dirstructure.ConfigToolbarDir()+ wxT(img))


ToolBar::~ToolBar()
{
  m_plotSlider = NULL;
}

ToolBar::ToolBar(wxWindow* parent, int id):wxToolBar(parent,id)
{
  Dirstructure dirstructure;
  
  SetToolBitmapSize(wxSize(24, 24));

#if defined __WXMSW__
  AddTool(tb_new, _("New"),
	  IMAGE("new.png"),
	  _("New document"));
#endif
  AddTool(tb_open, _("Open"),
	  IMAGE("open.png"),
	  _("Open document"));
  AddTool(tb_save, _("Save"),
	  IMAGE("save.png"),
	  _("Save document"));
  AddSeparator();
  AddTool(tb_print, _("Print"),
	  IMAGE("print.png"),
	  _("Print document"));
  AddTool(tb_pref, _("Options"),
	  IMAGE("configure.png"),
	  _("Configure wxMaxima"));
  AddSeparator();
  AddTool(tb_cut, _("Cut"),
	  IMAGE("cut.png"),
	  _("Cut selection"));
  AddTool(tb_copy, _("Copy"),
	  IMAGE("copy.png"),
	  _("Copy selection"));
  AddTool(tb_paste, _("Paste"),
	  IMAGE("paste.png"),
	  _("Paste from clipboard"));
  AddTool(tb_select_all, _("Select all"),
	  IMAGE("edit-select-all.png"),
	  _("Select all"));
  AddSeparator();
  AddTool(tb_find, _("Find"),
	  IMAGE("find.png"),
	  _("Find and replace"));
  AddSeparator();
  AddTool(tb_interrupt, _("Interrupt"),
	  IMAGE("stop.png"),
	  _("Interrupt current computation"));
  m_followIcon=IMAGE("stock_weather-sunny");
  m_needsInformationIcon=IMAGE("dialog-information");
  AddTool(tb_follow, _("Follow"),m_followIcon,
	  _("Return to the cell that is currently being evaluated"));
  AddSeparator();
  AddTool(tb_animation_start, _("Start animation"),
	  IMAGE("playback-start.png"),
	  _("Start animation"));
  AddTool(tb_animation_stop, _("Stop animation"),
	  IMAGE("playback-stop.png"),
	  _("Stop animation"));
  m_plotSlider = new wxSlider(m_console->m_mainToolbar, plot_slider_id, 0, 0, 10,
			      wxDefaultPosition, wxDefaultSize,
			      wxSL_HORIZONTAL | !wxSL_AUTOTICKS);
  AddControl(m_plotSlider);
  AddSeparator();
  AddTool(tb_help, _("Help"),
	  IMAGE("help.png"),
	  _("Show Maxima help"));
  Realize();

  SetToolBar(m_console->m_mainToolbar);
}

#elif defined (__WXGTK20__)

ToolBar::ToolBar(wxWindow* parent, int id):wxToolBar(parent,id)
{
  AddTool(tb_new, _("New"),
	  wxArtProvider::GetBitmap(wxT("gtk-new"),
				   wxART_TOOLBAR),
	  _("New document"));

  AddTool(tb_open, _("Open"),
	  wxArtProvider::GetBitmap(wxT("gtk-open"),
				   wxART_TOOLBAR),
	  _("Open document"));
  AddTool(tb_save, _("Save"),
	  wxArtProvider::GetBitmap(wxT("gtk-save"),
				   wxART_TOOLBAR),
	  _("Save document"));
  AddSeparator();
  AddTool(tb_print, _("Print"),
	  wxArtProvider::GetBitmap(wxT("gtk-print"),
				   wxART_TOOLBAR),
	  _("Print document"));
  AddTool(tb_pref, _("Options"),
	  wxArtProvider::GetBitmap(wxT("gtk-preferences"),
				   wxART_TOOLBAR),
	  _("Configure wxMaxima"));
  AddSeparator();
  AddTool(tb_cut, _("Cut"),
	  wxArtProvider::GetBitmap(wxT("gtk-cut"),
				   wxART_TOOLBAR),
	  _("Cut selection"));
  AddTool(tb_copy, _("Copy"),
	  wxArtProvider::GetBitmap(wxT("gtk-copy"),
				   wxART_TOOLBAR),
	  _("Copy selection"));
  AddTool(tb_paste, _("Paste"),
	  wxArtProvider::GetBitmap(wxT("gtk-paste"),
				   wxART_TOOLBAR),
	  _("Paste from clipboard"));
  AddTool(tb_select_all, _("Paste"),
	  wxArtProvider::GetBitmap(wxT("gtk-select-all"),
				   wxART_TOOLBAR),
	  _("Select all"));
  
  AddSeparator();
  AddTool(tb_find, _("Find..."),
	  wxArtProvider::GetBitmap(wxT("gtk-find"),
				   wxART_TOOLBAR),
	  _("Find and replace"));
  AddSeparator();
  AddTool(tb_interrupt, _("Interrupt"),
	  wxArtProvider::GetBitmap(wxT("gtk-stop"),
				   wxART_TOOLBAR),
	  _("Interrupt current computation"));

  m_followIcon=wxArtProvider::GetBitmap(wxT("stock_weather-sunny"),wxART_TOOLBAR);
  m_needsInformationIcon=wxArtProvider::GetBitmap(wxT("dialog-information"),wxART_TOOLBAR);
  AddTool(tb_follow, _("Follow"),m_followIcon,
  _("Return to the cell that is currently being evaluated"));
  EnableTool(tb_interrupt,false);
  AddSeparator();
  AddTool(tb_animation_start, _("Animation"),
	  wxArtProvider::GetBitmap(wxT("media-playback-start"),
				   wxART_TOOLBAR));
  AddTool(tb_animation_stop, _("Stop animation"),
	  wxArtProvider::GetBitmap(wxT("media-playback-stop"),
				   wxART_TOOLBAR));
  m_plotSlider = new wxSlider(this, plot_slider_id, 0, 0, 10,
			      wxDefaultPosition, wxSize(200, -1),
			      wxSL_HORIZONTAL | !wxSL_AUTOTICKS);
  AddControl(m_plotSlider);
  AddSeparator();
  AddTool(tb_help, _("Help"),
	  wxArtProvider::GetBitmap(wxT("gtk-help"),
				   wxART_TOOLBAR),
	  _("Show Maxima help"));
  Realize();
}

#endif