# -*- coding: utf-8 -*-

# ***************************************************************************
# *                                                                         *
# *   Copyright (c) 2019 Daniel Wood <s.d.wood.82@gmail.com>                *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

import FreeCAD
import FreeCADGui
import PathScripts.PathLog as PathLog
import PathScripts.PathOpGui as PathOpGui

from PySide import QtCore, QtGui

__title__ = "Path Turn Base Gui"
__author__ = "dubstar-04 (Daniel Wood)"
__url__ = "http://www.freecadweb.org"
__doc__ = "Base class Gui implementation for turning operations."

LOGLEVEL = False

if LOGLEVEL:
    PathLog.setLevel(PathLog.Level.DEBUG, PathLog.thisModule())
    PathLog.trackModule(PathLog.thisModule())
else:
    PathLog.setLevel(PathLog.Level.NOTICE, PathLog.thisModule())

class TaskPanelTurnBase(PathOpGui.TaskPanelPage):
    '''Page controller class for pocket operations, supports:
          FeaturePocket  ... used for pocketing operation
          FeatureFacing  ... used for face milling operation
          FeatureOutline ... used for pocket-shape operation
    '''

    DataFeatureName = QtCore.Qt.ItemDataRole.UserRole
    DataObject      = QtCore.Qt.ItemDataRole.UserRole + 1
    DataObjectSub   = QtCore.Qt.ItemDataRole.UserRole + 2

    def initPage(self, obj):
        self.updating = False # pylint: disable=attribute-defined-outside-init

    def getForm(self):
        '''getForm() ... return UI'''
        return FreeCADGui.PySideUic.loadUi(":/panels/PageOpTurnBaseEdit.ui")

    def getFields(self, obj):
        '''getFields(obj) ... transfers values from UI to obj's proprties'''
        PathLog.track()
        if obj.Direction != str(self.form.direction.currentText()):
            obj.Direction = str(self.form.direction.currentText())

        if obj.MinDia != self.form.minDia.value():
            obj.MinDia = self.form.minDia.value() 
        
        if obj.MaxDia != self.form.maxDia.value():
            obj.MaxDia = self.form.maxDia.value() 

        if obj.StartOffset != self.form.startOffset.value():
            obj.StartOffset = self.form.startOffset.value()

        if obj.EndOffset != self.form.endOffset.value():
            obj.EndOffset = self.form.endOffset.value()

        if obj.AllowGrooving != self.form.allowGrooving.isChecked():
            obj.AllowGrooving = self.form.allowGrooving.isChecked()

        if obj.AllowFacing != self.form.allowFacing.isChecked():
            obj.AllowFacing= self.form.allowFacing.isChecked()
        
        if obj.StepOver != self.form.stepOver.value():
            obj.StepOver = self.form.stepOver.value()   

        self.updateToolController(obj, self.form.toolController)

    def setFields(self, obj):
        '''setFields(obj) ... transfers obj's property values to UI'''
        PathLog.track()

        self.form.stepOver.setValue(obj.StepOver)
        self.selectInComboBox(obj.Direction, self.form.direction)
        #self.selectInComboBox(obj.StartSide, self.form.startSide)

        self.setupToolController(obj, self.form.toolController)

    def getSignalsForUpdate(self, obj):
        '''getSignalsForUpdate(obj) ... return list of signals for updating obj'''
        signals = []

        signals.append(self.form.stepOver.editingFinished)
        signals.append(self.form.direction.currentIndexChanged)

        signals.append(self.form.minDia.editingFinished)
        signals.append(self.form.maxDia.editingFinished)
        signals.append(self.form.startOffset.editingFinished)
        signals.append(self.form.endOffset.editingFinished)
        signals.append(self.form.allowGrooving.stateChanged)
        signals.append(self.form.allowFacing.stateChanged)

        signals.append(self.form.toolController.currentIndexChanged)

        return signals