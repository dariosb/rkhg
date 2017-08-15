/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2012 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 */

#ifndef _INPUTSTRLIST_H
#define _INPUTSTRLIST_H

#include "input.h"
#include "dtree.h"

#include <QObject>
#include <QStringList>

class QLabel;
class QLineEdit;
class QPushButton;
class QListWidget;
class QStringList;
class QGridLayout;
class QAction;

class InputStrList : public QObject, public Input
{
  Q_OBJECT

  public:
    enum ListMode { ListString  = 0,
                    ListFile    = 1,
                    ListDir     = 2,
                    ListFileDir = ListFile | ListDir
                  };

    InputStrList( QGridLayout *layout,int &row,
                  const QString &id, const QStringList &sl,
                  ListMode v, const QString &docs);
    void setValue(const QStringList &sl);

    QVariant &value();
    void update();
    Kind kind() const { return StrList; }
    QString docs() const { return m_docs; }
    QString id() const { return m_id; }
    void addDependency(Input *) { Q_ASSERT(false); }
    void addDependencyDTree(DTree *) { Q_ASSERT(false); }
    void setEnabled(bool);
    void updateDependencies() {}
    void backToDefault() {}
    void updateDependencies(QVariant) {}
    void writeValue(QTextStream &t,QTextCodec *codec);

  public slots:
    void reset();

  signals:
    void changed();
    void showHelp(Input *);

  private slots:
    void addString(); 
    void delString(); 
    void updateString(); 
    void selectText(const QString &s);
    void browseFiles();
    void browseDir();
    void help();

  private:
    void updateDefault();
    QLabel       *m_lab;
    QLineEdit    *m_le;
    QAction      *m_add;
    QAction      *m_del;
    QAction      *m_upd;
    QAction      *m_brFile;
    QAction      *m_brDir;
    QListWidget  *m_lb;
    QStringList   m_default;
    QStringList   m_strList;
    QVariant      m_value;
    QString       m_docs;
    QString       m_id;

};

#endif
