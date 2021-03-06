// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
//  Copyright (C) 2015-2016 Gunter Königsmann <wxMaxima@physikbuch.de>
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
  This file defines the class BitMap that renders math as bitmap.
 */

#include "Bitmap.h"
#include "Configuration.h"
#include "GroupCell.h"

#include <wx/config.h>
#include <wx/clipbrd.h>

#define BM_FULL_WIDTH 1000
Bitmap::Bitmap(int scale)
{
  m_tree = NULL;
  m_scale=scale;
  // The depth 24 hinders wxWidgets from creating rgb0 bitmaps that some
  // windows applications will interpret as rgba if they appear on
  // the clipboards and therefore render them all-transparent.
  m_bmp.CreateScaled(10,10,24,1.0/((double)scale));
  m_width = 10;
  m_height = 10;
}

Bitmap::~Bitmap()
{
  if (m_tree != NULL)
    DestroyTree();
}

bool Bitmap::SetData(MathCell* tree,long int maxSize)
{
  if (m_tree != NULL)
    delete m_tree;
  m_tree = tree;
  return Layout(maxSize);
}

bool Bitmap::Layout(long int maxSize)
{
  if (m_tree->GetType() != MC_TYPE_GROUP)
  {
    RecalculateWidths();
    BreakUpCells();
    BreakLines();
    RecalculateHeight();
  }
  else {
    GroupCell* tmp = (GroupCell *)m_tree;

    wxMemoryDC dc;
    dc.SelectObject(m_bmp);
    dc.SetUserScale(m_scale,m_scale);
    Configuration configuration(dc);
    configuration.SetClientWidth(BM_FULL_WIDTH);

    while (tmp != NULL)
    {
      tmp->Recalculate();
      tmp = (GroupCell *)tmp->m_next;
    }
  }

  int width, height;
  GetMaxPoint(&width, &height);

  // Too big bitmaps or bitmaps that are too wide or high can crash windows
  // or the X server.
  if((maxSize < 0) ||
     (
       (width*height*m_scale*m_scale < maxSize) &&
       (width*m_scale < 20000) &&
       (height*m_scale < 20000)
       )
    )
  {
    // The depth 24 hinders wxWidgets from creating rgb0 bitmaps that some
    // windows applications will interpret as rgba if they appear on
    // the clipboards and therefore render them all-transparent.
    m_bmp.Create(m_width=width * m_scale, m_height=height * m_scale,24);
    Draw();
    return true;
  }
  else
  {
    m_bmp = wxNullBitmap;
    return false;
  }
}

double Bitmap::GetRealWidth()
{
  return m_width/m_scale;
}

double Bitmap::GetRealHeight()
{
  return m_height/m_scale;
}

void Bitmap::RecalculateHeight()
{
  int fontsize = 12;
  wxConfig::Get()->Read(wxT("fontSize"), &fontsize);
  int mfontsize = fontsize;
  wxConfig::Get()->Read(wxT("mathfontsize"), &mfontsize);
  MathCell* tmp = m_tree;

  wxMemoryDC dc;
  dc.SelectObject(m_bmp);
  dc.SetUserScale(m_scale,m_scale);
  Configuration configuration(dc);

  while (tmp != NULL)
  {
    tmp->RecalculateHeight(tmp->IsMath() ? mfontsize : fontsize);
    tmp = tmp->m_next;
  }
}

void Bitmap::RecalculateWidths()
{
  int fontsize = 12;
  wxConfig::Get()->Read(wxT("fontSize"), &fontsize);
  int mfontsize = fontsize;
  wxConfig::Get()->Read(wxT("mathfontsize"), &mfontsize);

  MathCell* tmp = m_tree;

  wxMemoryDC dc;
  dc.SelectObject(m_bmp);
  dc.SetUserScale(m_scale,m_scale);
  Configuration configuration(dc);
  configuration.SetClientWidth(BM_FULL_WIDTH);
  configuration.SetClientHeight(BM_FULL_WIDTH);

  while (tmp != NULL)
  {
    tmp->RecalculateWidths(tmp->IsMath() ? mfontsize : fontsize);
    tmp = tmp->m_next;
  }
}

void Bitmap::BreakLines()
{
  int fullWidth = BM_FULL_WIDTH;
  int currentWidth = 0;

  MathCell* tmp = m_tree;

  while (tmp != NULL)
  {
    if (!tmp->m_isBroken)
    {
      tmp->BreakLine(false);
      tmp->ResetData();
      if (tmp->BreakLineHere() ||
              (currentWidth + tmp->GetWidth() >= fullWidth))
      {
        currentWidth = tmp->GetWidth();
        tmp->BreakLine(true);
      }
      else
        currentWidth += (tmp->GetWidth() + MC_CELL_SKIP);
    }
    tmp = tmp->m_nextToDraw;
  }
}

void Bitmap::GetMaxPoint(int* width, int* height)
{
  MathCell* tmp = m_tree;
  int currentHeight = 0;
  int currentWidth = 0;
  *width = 0;
  *height = 0;
  bool bigSkip = false;
  bool firstCell = true;
  while (tmp != NULL)
  {
    if (!tmp->m_isBroken)
    {
      if (tmp->BreakLineHere() || firstCell)
      {
        firstCell = false;
        currentHeight += tmp->GetMaxHeight();
        if (bigSkip)
          currentHeight += MC_LINE_SKIP;
        *height = currentHeight;
        currentWidth = tmp->GetWidth();
        *width = MAX(currentWidth, *width);
      }
      else
      {
        currentWidth += (tmp->GetWidth() + MC_CELL_SKIP);
        *width = MAX(currentWidth - MC_CELL_SKIP, *width);
      }
      bigSkip = tmp->m_bigSkip;
    }
    tmp = tmp->m_nextToDraw;
  }
}

void Bitmap::Draw()
{
  MathCell::ClipToDrawRegion(false);
  MathCell* tmp = m_tree;
  wxMemoryDC dc;
  dc.SelectObject(m_bmp);
  dc.SetUserScale(m_scale,m_scale);

  wxString bgColStr = wxT("white");
  wxConfig::Get()->Read(wxT("Style/Background/color"), &bgColStr);
  dc.SetBackground(*(wxTheBrushList->FindOrCreateBrush(bgColStr, wxBRUSHSTYLE_SOLID)));
  dc.Clear();

  if (tmp != NULL)
  {
    wxPoint point;
    point.x = 0;
    point.y = tmp->GetMaxCenter();
    int fontsize = 12;
    int drop = tmp->GetMaxDrop();

    wxConfig::Get()->Read(wxT("fontSize"), &fontsize);
    int mfontsize = fontsize;
    wxConfig::Get()->Read(wxT("mathfontsize"), &mfontsize);

    Configuration configuration(dc);

    while (tmp != NULL)
    {
      if (!tmp->m_isBroken)
      {
        tmp->Draw(point, tmp->IsMath() ? mfontsize : fontsize);
        if ((tmp->m_next != NULL) && (tmp->m_next->BreakLineHere()))
        {
          point.x = 0;
          point.y += drop + tmp->m_next->GetMaxCenter();
          if (tmp->m_bigSkip)
            point.y += MC_LINE_SKIP;
          drop = tmp->m_next->GetMaxDrop();
        }
        else
          point.x += (tmp->GetWidth() + MC_CELL_SKIP);
      }
      else
      {
        if ((tmp->m_next != NULL) && (tmp->m_next->BreakLineHere()))
        {
          point.x = 0;
          point.y += drop + tmp->m_next->GetMaxCenter();
          if (tmp->m_bigSkip)
            point.y += MC_LINE_SKIP;
          drop = tmp->m_next->GetMaxDrop();
        }
      }
      tmp = tmp->m_nextToDraw;
    }
  }
  dc.SelectObject(wxNullBitmap);
  // Update the bitmap's size information.
  m_ppi = dc.GetPPI();
  m_ppi.x *= m_scale;
  m_ppi.y *= m_scale;
  MathCell::ClipToDrawRegion(true);
}

wxSize Bitmap::ToFile(wxString file)
{
  // Assign an resolution to the bitmap.
  wxImage img=m_bmp.ConvertToImage();
  int resolution = img.GetOptionInt(wxIMAGE_OPTION_RESOLUTION);
  if(resolution <= 0)
    resolution = 75;
  img.SetOption(wxIMAGE_OPTION_RESOLUTION,resolution * m_scale);

  bool success = false;
  if (file.Right(4) == wxT(".bmp"))
    success = img.SaveFile(file, wxBITMAP_TYPE_BMP);
  else if (file.Right(4) == wxT(".xpm"))
    success = img.SaveFile(file, wxBITMAP_TYPE_XPM);
  else if (file.Right(4) == wxT(".jpg"))
    success = img.SaveFile(file, wxBITMAP_TYPE_JPEG);
  else
  {
    if (file.Right(4) != wxT(".png"))
      file = file + wxT(".png");
    success = img.SaveFile(file, wxBITMAP_TYPE_PNG);
  }

  wxSize retval;
  if( success )
  {
    retval.x=GetRealWidth();
    retval.y=GetRealHeight();
    return retval;
  }
  else
  {
    retval.x=-1;
    retval.y=-1;
    return retval;
  };
}

bool Bitmap::ToClipboard()
{
  if (wxTheClipboard->Open())
  {
    bool res = wxTheClipboard->SetData(new wxBitmapDataObject(m_bmp));
    wxTheClipboard->Close();
    return res;
  }
  return false;
}

void Bitmap::DestroyTree()
{
  if (m_tree != NULL)
  {
    MathCell *tmp1, *tmp = m_tree;
    while (tmp != NULL)
    {
      tmp1 = tmp;
      tmp = tmp->m_next;
      tmp1->Destroy();
      delete tmp1;
    }
  }
  m_tree = NULL;
}

void Bitmap::BreakUpCells()
{
  MathCell *tmp = m_tree;
  int fontsize = 12;
  wxConfig::Get()->Read(wxT("fontSize"), &fontsize);
  int mfontsize = fontsize;
  wxConfig::Get()->Read(wxT("mathfontsize"), &mfontsize);
  wxMemoryDC dc;
  dc.SelectObject(m_bmp);
  dc.SetUserScale(m_scale,m_scale);
  Configuration configuration(dc);

  while (tmp != NULL)
  {
    if (tmp->GetWidth() > BM_FULL_WIDTH)
    {
      if (tmp->BreakUp())
      {
        tmp->RecalculateWidths(tmp->IsMath() ? mfontsize : fontsize);
        tmp->RecalculateHeight(tmp->IsMath() ? mfontsize : fontsize);
      }
    }
    tmp = tmp->m_nextToDraw;
  }
  dc.SelectObject(wxNullBitmap);
}
