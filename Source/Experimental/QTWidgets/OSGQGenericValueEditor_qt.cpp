/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000-2002 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#include "OSGQGenericValueEditor_qt.h"
#include <OSGQOSGWidgetFactoryHelper.h>
#include <OSGField.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>

OSG_USING_NAMESPACE

ValueEditorRegistrator _regGeneric( ValueEditorRegistrator::DefaultValueEditor,
                                   &QGenericValueEditor   ::create            );

QAbstractValueEditor *
QGenericValueEditor::create(QWidget *pParent, const char *name)
{
    return new QGenericValueEditor(pParent, name);
}

QGenericValueEditor::QGenericValueEditor(QWidget *pParent, const char *name)
    : Inherited (pParent, name),
      _pHBox    (NULL         ),
      _pLabel   (NULL         ),
      _pLineEdit(NULL         )
{
    createChildWidgets();
    layoutChildWidgets();
    initSelf          ();
}

QGenericValueEditor::~QGenericValueEditor(void)
{
}

void
QGenericValueEditor::setLabelsVisible(bool bLabels)
{
    Inherited::setLabelsVisible(bLabels);

    if(bLabels == true)
    {
        _pLabel->show();
    }
    else
    {
        _pLabel->hide();
    }
}

void
QGenericValueEditor::setReadOnly(bool bReadOnly)
{
    Inherited::setReadOnly(bReadOnly);

    _pLineEdit->setReadOnly(bReadOnly);
}

void
QGenericValueEditor::readField(
    FieldContainerPtr pFC,          UInt32 uiFieldId,
    UInt32            uiValueIndex, UInt32 uiAspect  )
{
    std::string strFieldValue;

    if(pFC->getField(uiFieldId)->getCardinality() == FieldType::SINGLE_FIELD)
    {
        pFC.getAspectCPtr(uiAspect)->getField(uiFieldId)->getValueByStr(
            strFieldValue);

        _pLineEdit->blockSignals(true                 );
        _pLineEdit->setText     (strFieldValue.c_str());
        _pLineEdit->blockSignals(false                );
    }
    else
    {
        pFC.getAspectCPtr(uiAspect)->getField(uiFieldId)->getValueByStr(
            strFieldValue, uiValueIndex);

        _pLineEdit->blockSignals(true                 );
        _pLineEdit->setText     (strFieldValue.c_str());
        _pLineEdit->blockSignals(false                );
    }
}

void
QGenericValueEditor::readField(
    FieldContainerPtr pFC,          UInt32 uiFieldId,
    UInt32            uiValueIndex                   )
{
    readField(pFC, uiFieldId, uiValueIndex, Thread::getCurrent()->getAspect());
}

void
QGenericValueEditor::writeField(
    FieldContainerPtr pFC,          UInt32 uiFieldId,
    UInt32            uiValueIndex                   )
{
    if(pFC->getField(uiFieldId)->getCardinality() == FieldType::SINGLE_FIELD)
    {
        pFC->getField(uiFieldId)->pushValueByStr(_pLineEdit->text());
    }
    else
    {
        SWARNING << "QGenericValueEditor::writeField: "
                 << "writing to MField is not supported. data not written."
                 << endLog;
    }
}

void
QGenericValueEditor::slotLineEditValueChanged(void)
{
    emit valueChanged();
}

void
QGenericValueEditor::createChildWidgets(void)
{
    _pHBox     = new QHBoxLayout(this, 0, 1, "QGenericValueEditor::_pHBox"    );
    _pLabel    = new QLabel     (this,       "QGenericValueEditor::_pLabel"   );
    _pLineEdit = new QLineEdit  (this,       "QGenericValueEditor::_pLineEdit");
}

void
QGenericValueEditor::layoutChildWidgets(void)
{
    _pHBox->addWidget(_pLabel,                  0);
    _pHBox->addWidget(_pLineEdit,              10);

    _pHBox->addWidget(this->getActionButton(), 10);
}

void
QGenericValueEditor::initSelf(void)
{
    Inherited::setReadOnly (true);
    _pLineEdit->setReadOnly(true);

    _pLabel->setText("String value");

    connect(_pLineEdit, SIGNAL(textChanged             (const QString &)),
            this,       SLOT  (slotLineEditValueChanged(void           )) );
}

// include generated file
#include "OSGQGenericValueEditor_qt_moc.cpp"

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGQGenericValueEditor_qt.cpp,v 1.1 2004/07/30 15:31:57 neumannc Exp $";
    static Char8 cvsid_hpp       [] = OSGQGENERICVALUEEDITORQT_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGQGENERICVALUEEDITORQT_INLINE_CVSID;
}

#ifdef OSG_LINUX_ICC
#pragma warning(enable : 177)
#endif

#ifdef OSG_SGI_CC
#pragma reset woff 1174
#endif