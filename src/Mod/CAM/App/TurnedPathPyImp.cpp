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

#include <Mod/Part/App/TopoShapePy.h>
#include <Mod/Part/App/OCCError.h>

#include "PathPy.h"
// inclusion of the generated files (generated out of TurnedPathPy.xml)
#include "TurnedPathPy.h"
#include "TurnedPathPy.cpp"


// #include "TurnedPath.h"

using namespace Path;

// returns a string which represents the object e.g. when printed in python
std::string TurnedPathPy::representation() const
{
    std::stringstream ss;
    ss.precision(5);
    ss << "TurnedPath() [python object]";
    return ss.str();
}

PyObject* TurnedPathPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of TurnedPathPy and its twin object
    return new TurnedPathPy(new TurnedPath);
}

// constructor method
int TurnedPathPy::PyInit(PyObject*, PyObject*)
{
    return 0;
}

// custom attributes get/set

PyObject* TurnedPathPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int TurnedPathPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}


PyObject* TurnedPathPy::makeTurnedArea(PyObject* args)
{

    PyObject* pcObj;
    if (!PyArg_ParseTuple(args, "O!", &(Part::TopoShapePy::Type), &pcObj)) {
        PyErr_SetString(PyExc_TypeError, "non-shape object in sequence");
        return nullptr;
    }


    PY_TRY
    {
        // Expand the variable as function call arguments
        TopoDS_Shape resultShape = getTurnedPathPtr()->makeTurnedArea(
            static_cast<Part::TopoShapePy*>(pcObj)->getTopoShapePtr()->getShape());

        return Py::new_reference_to(Part::shape2pyshape(resultShape));
    }
    PY_CATCH_OCC
}
