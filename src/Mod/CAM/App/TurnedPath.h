/****************************************************************************
 *   Copyright (c) 2024 Daniel Wood <s.d.wood.82@googlemail.com>            *
 *                                                                          *
 *   This file is part of the FreeCAD CAx development system.               *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Library General Public            *
 *   License as published by the Free Software Foundation; either           *
 *   version 2 of the License, or (at your option) any later version.       *
 *                                                                          *
 *   This library  is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Library General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU Library General Public      *
 *   License along with this library; see the file COPYING.LIB. If not,     *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,          *
 *   Suite 330, Boston, MA  02111-1307, USA                                 *
 *                                                                          *
 ****************************************************************************/

#ifndef TURNED_PATH_H
#define TURNED_PATH_H

#include <TopoDS.hxx>

#include <Mod/Part/App/PartPyCXX.h>
#include <Base/BaseClass.h>

namespace Path
{
class PathExport TurnedPath: public Base::BaseClass
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    /// Constructor
    TurnedPath();
    ~TurnedPath() override;
    TopoDS_Shape makeTurnedArea(const TopoDS_Shape& shape);

private:
    void showShape(const TopoDS_Shape& shape, const char* name);
};

}  // namespace Path

#endif  // TURNED_PATH_H