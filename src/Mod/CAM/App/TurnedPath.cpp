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

#include "PreCompiled.h"

#include <App/Document.h>
#include <Base/Tools.h>

#include <Mod/Part/App/CrossSection.h>
#include <Mod/Part/App/FaceMakerBullseye.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/FCBRepAlgoAPI_Fuse.h>
#include <Mod/Part/App/TopoShape.h>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <ShapeFix_Wire.hxx>


#include "TurnedPath.h"

FC_LOG_LEVEL_INIT("Path.TurnedPath", true, true)

using namespace Path;
using namespace Base;

TYPESYSTEM_SOURCE(Path::TurnedPath, Base::BaseClass)

TurnedPath::TurnedPath()
{}

TurnedPath::~TurnedPath()
{}

TopoDS_Shape TurnedPath::makeTurnedArea(const TopoDS_Shape& shape)
{
    // Create a copy of the part shape
    TopoDS_Shape newShape = BRepBuilderAPI_Copy(shape).Shape();

    // turned face is the shape to return
    // it represents a rotated profile of the input shape
    TopoDS_Shape turnedFace;

    // set the position of the slicing plane
    gp_Pnt pos = gp_Pnt(0, 0, 0);
    // set the rotation direction
    gp_Dir zdir = gp_Dir(0, 0, 1);
    // set rotation axis
    gp_Ax1 axis(pos, zdir);
    // create a plane to slice the shape
    gp_Dir slicedir = gp_Dir(0, 1, 0);

    gp_Pln pln(pos, slicedir);
    Standard_Real a, b, c, d;
    pln.Coefficients(a, b, c, d);

    int rotationStep = 15;
    for (int i = 0; i < 360; i = i + rotationStep) {

        double rotation = Base::toRadians<double>(i);

        FC_WARN("Rotation (Deg): " << i);

        // rotate newShape (copy of input shape)
        gp_Trsf mov;
        mov.SetRotation(axis, -rotation);
        TopLoc_Location loc(mov);
        newShape.Move(loc);

        // create a slice through newShape
        Part::FaceMakerBullseye mkFace;
        mkFace.setPlane(pln);
        std::list<TopoDS_Wire> wires;
        Part::CrossSection section(a, b, c, newShape);

        try {
            wires = section.slice(0);
        }
        catch (Base::Exception& e) {
            FC_WARN("CrossSection failed: " << e.what());
        }

        if (wires.empty()) {
            FC_WARN("Section returns no wires");
        }
        else {
            FC_WARN("Section returns some wires");
        }

        for (const TopoDS_Wire& wire : wires) {
            showShape(wire, "section_wire");
            if (BRep_Tool::IsClosed(wire)) {

                // create a new wire with all vertex at Y=0
                BRepBuilderAPI_MakeWire mkWire;

                // ensure all edges are on the y=0 plane
                for (TopExp_Explorer exp(wire, TopAbs_EDGE); exp.More(); exp.Next()) {
                    const TopoDS_Edge& edge = TopoDS::Edge(exp.Current());

                    gp_Pnt p1 = BRep_Tool::Pnt(TopExp::FirstVertex(edge));
                    gp_Pnt p2 = BRep_Tool::Pnt(TopExp::LastVertex(edge));

                    BRepBuilderAPI_MakeEdge e(gp_Pnt(p1.X(), 0.0, p1.Z()),
                                              gp_Pnt(p2.X(), 0.0, p2.Z()));


                    mkWire.Add(e.Edge());
                }

                if (mkWire.IsDone()) {
                    // fix any continuity issues with the wire
                    ShapeFix_Wire aFix;
                    aFix.Load(TopoDS::Wire(mkWire.Wire()));
                    aFix.FixReorder();
                    aFix.FixConnected();
                    aFix.FixClosed();
                    mkFace.addWire(aFix.Wire());
                }
            }
            else {
                FC_WARN("Wire is not closed");
                continue;
            }
        }

        try {
            if (!mkFace.IsDone()) {
                FC_WARN("FaceMakerBullseye failed, mkFace is not done");
                // continue;
            }

            mkFace.Build();
            const TopoDS_Shape& faceShape = mkFace.Shape();
            // showShape(faceShape, "section_face");
            if (faceShape.IsNull()) {
                FC_WARN("FaceMakerBullseye return null shape on section");
            }

            // check if turnedFace is null
            // it will be null on the first iteration
            if (turnedFace.IsNull()) {
                turnedFace = faceShape;
            }
            else {
                // fuse the new face with the previous turnedFace
                FCBRepAlgoAPI_Fuse mkFuse(turnedFace, faceShape);
                Part::TopoShape sh(mkFuse.Shape());
                double tolerance = 1.0e-06;
                sh.removeSplitter();
                sh.sewShape(tolerance);
                Part::TopoShape refined = sh.makeElementRefine();
                turnedFace = refined.getShape();
            }
        }
        catch (Base::Exception& e) {
            FC_WARN("FaceMakerBullseye failed: " << e.what());
        }
    }

    // clean up the final turnedFace
    // Part::TopoShape unrefined(turnedFace);
    // Part::TopoShape nrefined = unrefined.makeElementRefine();
    // showShape(nrefined.getShape(), "refinedShape");
    return turnedFace;
}

void TurnedPath::showShape(const TopoDS_Shape& shape, const char* name)
{
    if (FC_LOG_INSTANCE.level() > FC_LOGLEVEL_TRACE) {
        App::Document* pcDoc = App::GetApplication().getActiveDocument();
        if (!pcDoc) {
            pcDoc = App::GetApplication().newDocument();
        }

        Part::Feature* pcFeature =
            static_cast<Part::Feature*>(pcDoc->addObject("Part::Feature", name));
        pcFeature->Shape.setValue(shape);
    }
}