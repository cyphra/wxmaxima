// -*- mode: c++; c-file-style: "linux"; c-basic-offset: 2; indent-tabs-mode: nil -*-
//
//  Copyright (C) 2004-2015 Andrej Vodopivec <andrej.vodopivec@gmail.com>
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

#ifndef DIFFCELL_H
#define DIFFCELL_H

#include "MathCell.h"
#include "TextCell.h"

class DiffCell : public MathCell
{
public:
	DiffCell();
	~DiffCell();
  void Destroy();
  MathCell* Copy();
  void SetBase(MathCell *base);
  void SetDiff(MathCell *diff);
  void RecalculateHeight(int fontsize);
  void RecalculateWidths(int fontsize);
  void Draw(wxPoint point, int fontsize);
  void SelectInner(wxRect& rect, MathCell** first, MathCell** last);
  wxString ToString();
  wxString ToTeX();
  wxString ToMathML();
  wxString ToOMML();
  wxString ToXML();
  bool BreakUp();
  void Unbreak();
  void SetParent(MathCell *parent);
protected:
  MathCell *m_baseCell;
  MathCell *m_diffCell;
  MathCell *m_last;
};

#endif // DIFFCELL_H
