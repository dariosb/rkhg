/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "variantdelegate.h"

static QRect CheckBoxRect(const QStyleOptionViewItem &view_item_style_options) {
  QStyleOptionButton check_box_style_option;
  QRect check_box_rect = QApplication::style()->subElementRect(
      QStyle::SE_CheckBoxIndicator,
      &check_box_style_option);
  QPoint check_box_point(view_item_style_options.rect.x() +
                         view_item_style_options.rect.width() / 2 -
                         check_box_rect.width() / 2,
                         view_item_style_options.rect.y() +
                         view_item_style_options.rect.height() / 2 -
                         check_box_rect.height() / 2);
  return QRect(check_box_point, check_box_rect.size());
}


VariantDelegate::VariantDelegate(QObject *parent,
                                 int min, int max,int dec)
    : QItemDelegate(parent),m_min(min),m_max(max),m_dec(dec)
{
    boolExp.setPattern("true|false");

    boolExp.setCaseSensitivity(Qt::CaseInsensitive);

    byteArrayExp.setPattern("[\\x00-\\xff]*");
    charExp.setPattern(".");
    colorExp.setPattern("\\(([0-9]*),([0-9]*),([0-9]*),([0-9]*)\\)");
    doubleExp.setPattern("");
    pointExp.setPattern("\\((-?[0-9]*),(-?[0-9]*)\\)");
    rectExp.setPattern("\\((-?[0-9]*),(-?[0-9]*),(-?[0-9]*),(-?[0-9]*)\\)");
    signedIntegerExp.setPattern("-?[0-9]*");
    sizeExp = pointExp;
    unsignedIntegerExp.setPattern("[0-9]*");

    dateExp.setPattern("([0-9]{,4})-([0-9]{,2})-([0-9]{,2})");
    timeExp.setPattern("([0-9]{,2}):([0-9]{,2}):([0-9]{,2})");
    dateTimeExp.setPattern(dateExp.pattern() + "T" + timeExp.pattern());

    QString Byte = "(?!0[0-9])(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    ipType.setPattern("^" + Byte + "\\." + Byte + "\\." + Byte + "\\." +
                      Byte + "$");
    intType.setPattern("^\\d+$");

    isTrue.setPattern("true");
    isTrue.setCaseSensitivity(Qt::CaseInsensitive);
    isFalse.setPattern("false");
    isFalse.setCaseSensitivity(Qt::CaseInsensitive);
}

void VariantDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    if (index.column() == 2) {
        QVariant value = index.model()->data(index, Qt::UserRole);
        if (!isSupportedType(value.type())) {
            QStyleOptionViewItem myOption = option;
            myOption.state &= ~QStyle::State_Enabled;
            QItemDelegate::paint(painter, myOption, index);
            return;
        }
    }
    QVariant key = index.model()->data(index, Qt::UserRole);
    if(boolExp.exactMatch(key.toString()))
    {
        bool checked = index.model()->data(index, Qt::DisplayRole).toBool();

        QStyleOptionButton check_box_style_option;
        check_box_style_option.state |= QStyle::State_Enabled;
        if (checked) {
            check_box_style_option.state |= QStyle::State_On;
        } else {
            check_box_style_option.state |= QStyle::State_Off;
        }
        check_box_style_option.rect = CheckBoxRect(option);

        QApplication::style()->drawControl(QStyle::CE_CheckBox,
                                           &check_box_style_option,
                                           painter);
        return;
    }


    QItemDelegate::paint(painter, option, index);
}

bool VariantDelegate::editorEvent(QEvent *event,
                                   QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) {
    QVariant key = index.model()->data(index, Qt::UserRole);
    if(boolExp.exactMatch(key.toString())){
        if ((event->type() == QEvent::MouseButtonRelease) ||
                (event->type() == QEvent::MouseButtonDblClick)) {
            QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
            if (mouse_event->button() != Qt::LeftButton ||
                    !CheckBoxRect(option).contains(mouse_event->pos())) {
                return false;
            }
            if (event->type() == QEvent::MouseButtonDblClick) {
                return true;
            }
        } else if (event->type() == QEvent::KeyPress) {
            if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
                    static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select) {
                return false;
            }
        } else {
            return false;
        }

        bool checked = index.model()->data(index, Qt::DisplayRole).toBool();
        model->setData(index, !checked, Qt::EditRole);
        return model->setData(index, !checked, Qt::UserRole);
    }

    return QItemDelegate::editorEvent(event,model, option, index);
}


QWidget *VariantDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem & /* option */,
        const QModelIndex &index) const
{
    if (index.column() != 1)
        return 0;

    QVariant key = index.model()->data(index, Qt::UserRole);
    if(intType.exactMatch(key.toString()))
    {
        //QSpinBox *editor = new QSpinBox(parent);
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setMinimum(m_min);
        editor->setMaximum(m_max);
        editor->setDecimals(m_dec);
        if(m_dec != 0)
        {
            double step = 1/(10*(double)m_dec);
            editor->setSingleStep(step);
        }

        return editor;
    }else if(ipType.exactMatch(key.toString()))
    {
        QLineEdit *editor = new QLineEdit(parent)   ;
        QRegExpValidator* ValIPAddr;
        QString Byte = "(?!0[0-9])(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
        QRegExp rxip;
        rxip.setPattern("^" + Byte + "\\." + Byte + "\\." + Byte + "\\." +
        Byte + "$");
        ValIPAddr = new QRegExpValidator(rxip, 0);
        editor->setValidator(ValIPAddr);


        //Input format
        editor->setInputMask("000.000.000.000;_");

        editor->setCursorPosition(0);
        editor->setMinimumWidth(100);
        return editor;

    }else if(boolExp.exactMatch(key.toString()))
    {
        QCheckBox *editor = new QCheckBox(parent);

        return editor;
    }

    QVariant originalValue = index.model()->data(index, Qt::UserRole);
    if (!isSupportedType(originalValue.type()))
        return 0;

    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(false);

    QRegExp regExp;

    switch (originalValue.type()) {
    case QVariant::Bool:
        regExp = boolExp;
        break;
    case QVariant::ByteArray:
        regExp = byteArrayExp;
        break;
    case QVariant::Char:
        regExp = charExp;
        break;
    case QVariant::Color:
        regExp = colorExp;
        break;
    case QVariant::Date:
        regExp = dateExp;
        break;
    case QVariant::DateTime:
        regExp = dateTimeExp;
        break;
    case QVariant::Double:
        regExp = doubleExp;
        break;
    case QVariant::Int:
    case QVariant::LongLong:
        regExp = signedIntegerExp;
        break;
    case QVariant::Point:
        regExp = pointExp;
        break;
    case QVariant::Rect:
        regExp = rectExp;
        break;
    case QVariant::Size:
        regExp = sizeExp;
        break;
    case QVariant::Time:
        regExp = timeExp;
        break;
    case QVariant::UInt:
    case QVariant::ULongLong:
        regExp = unsignedIntegerExp;
        break;
    default:
        ;
    }

    if (!regExp.isEmpty()) {
        QValidator *validator = new QRegExpValidator(regExp, lineEdit);
        lineEdit->setValidator(validator);
    }

    return lineEdit;
}

void VariantDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant key = index.model()->data(index, Qt::UserRole);
    if(intType.exactMatch(key.toString()))
    {
        double value = index.model()->data(index, Qt::UserRole).toDouble();
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(value);
        return;
    }else if(ipType.exactMatch(key.toString()))
    {
        QString value = index.model()->data(index, Qt::UserRole).toString();

        QLineEdit *ipBox = static_cast<QLineEdit*>(editor);
        ipBox->setText(value);
        return;
    }else if(boolExp.exactMatch(key.toString()))
    {
        QString value = index.model()->data(index, Qt::UserRole).toString();
        QCheckBox *chkBox = static_cast<QCheckBox*>(editor);
        chkBox->setCheckState((isTrue.exactMatch(value))?Qt::Checked:Qt::Unchecked);

        return;
    }

    QVariant value = index.model()->data(index, Qt::UserRole);
    if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor))
        lineEdit->setText(displayText(value));
}

void VariantDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QVariant key = index.model()->data(index, Qt::UserRole);
    if(intType.exactMatch(key.toString()))
    {
        //QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        QVariant val;

        model->setData(index, spinBox->textFromValue(value), Qt::DisplayRole);
        if(m_dec)
            val = value;
        else
            val = (int) value;

        model->setData(index, val, Qt::UserRole);
        return;
    }else if(ipType.exactMatch(key.toString()))
    {
        int pos = 0;
        QVariant val;
        QLineEdit *ipBox = static_cast<QLineEdit*>(editor);
        QString value = ipBox->text();
        val = value;
        if(ipBox->validator()->validate(value,pos) == QValidator::Acceptable)
        {
            model->setData(index, value, Qt::DisplayRole);
            model->setData(index, val, Qt::UserRole);
        }
        else
            QMessageBox::critical(editor, tr("Invalid IP value"),
                               tr("Invalid IP value"),tr("Accept"),0);

        return;
    }else if(boolExp.exactMatch(key.toString()))
    {
        //Captured in event
        return;
    }

    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (!lineEdit->isModified())
        return;


    QString text = lineEdit->text();
    const QValidator *validator = lineEdit->validator();
    if (validator) {
        int pos;
        if (validator->validate(text, pos) != QValidator::Acceptable)
            return;
    }

    QVariant originalValue = index.model()->data(index, Qt::UserRole);
    QVariant value;

    switch (originalValue.type()) {
    case QVariant::Char:
        value = text.at(0);
        break;
    case QVariant::Color:
        colorExp.exactMatch(text);
        value = QColor(qMin(colorExp.cap(1).toInt(), 255),
                       qMin(colorExp.cap(2).toInt(), 255),
                       qMin(colorExp.cap(3).toInt(), 255),
                       qMin(colorExp.cap(4).toInt(), 255));
        break;
    case QVariant::Date:
        {
            QDate date = QDate::fromString(text, Qt::ISODate);
            if (!date.isValid())
                return;
            value = date;
        }
        break;
    case QVariant::DateTime:
        {
            QDateTime dateTime = QDateTime::fromString(text, Qt::ISODate);
            if (!dateTime.isValid())
                return;
            value = dateTime;
        }
        break;
    case QVariant::Point:
        pointExp.exactMatch(text);
        value = QPoint(pointExp.cap(1).toInt(), pointExp.cap(2).toInt());
        break;
    case QVariant::Rect:
        rectExp.exactMatch(text);
        value = QRect(rectExp.cap(1).toInt(), rectExp.cap(2).toInt(),
                      rectExp.cap(3).toInt(), rectExp.cap(4).toInt());
        break;
    case QVariant::Size:
        sizeExp.exactMatch(text);
        value = QSize(sizeExp.cap(1).toInt(), sizeExp.cap(2).toInt());
        break;
    case QVariant::StringList:
        value = text.split(",");
        break;
    case QVariant::Time:
        {
            QTime time = QTime::fromString(text, Qt::ISODate);
            if (!time.isValid())
                return;
            value = time;
        }
        break;
    default:
        value = text;
        value.convert(originalValue.type());
    }

    model->setData(index, displayText(value), Qt::DisplayRole);
    model->setData(index, value, Qt::UserRole);
}

bool VariantDelegate::isSupportedType(QVariant::Type type)
{
    switch (type) {
    case QVariant::Bool:
    case QVariant::ByteArray:
    case QVariant::Char:
    case QVariant::Color:
    case QVariant::Date:
    case QVariant::DateTime:
    case QVariant::Double:
    case QVariant::Int:
    case QVariant::LongLong:
    case QVariant::Point:
    case QVariant::Rect:
    case QVariant::Size:
    case QVariant::String:
    case QVariant::StringList:
    case QVariant::Time:
    case QVariant::UInt:
    case QVariant::ULongLong:
        return true;
    default:
        return false;
    }
}

QString VariantDelegate::displayText(const QVariant &value)
{
    switch (value.type()) {
    case QVariant::Bool:
    case QVariant::ByteArray:
    case QVariant::Char:
    case QVariant::Double:
    case QVariant::Int:
    case QVariant::LongLong:
    case QVariant::String:
    case QVariant::UInt:
    case QVariant::ULongLong:
        return value.toString();
    case QVariant::Color:
        {
            QColor color = qvariant_cast<QColor>(value);
            return QString("(%1,%2,%3,%4)")
                   .arg(color.red()).arg(color.green())
                   .arg(color.blue()).arg(color.alpha());
        }
    case QVariant::Date:
        return value.toDate().toString(Qt::ISODate);
    case QVariant::DateTime:
        return value.toDateTime().toString(Qt::ISODate);
    case QVariant::Invalid:
        return "<Invalid>";
    case QVariant::Point:
        {
            QPoint point = value.toPoint();
            return QString("(%1,%2)").arg(point.x()).arg(point.y());
        }
    case QVariant::Rect:
        {
            QRect rect = value.toRect();
            return QString("(%1,%2,%3,%4)")
                   .arg(rect.x()).arg(rect.y())
                   .arg(rect.width()).arg(rect.height());
        }
    case QVariant::Size:
        {
            QSize size = value.toSize();
            return QString("(%1,%2)").arg(size.width()).arg(size.height());
        }
    case QVariant::StringList:
        return value.toStringList().join(",");
    case QVariant::Time:
        return value.toTime().toString(Qt::ISODate);
    default:
        break;
    }
    return QString("<%1>").arg(value.typeName());
}