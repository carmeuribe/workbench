#ifndef __USER_INPUT_MODE_BORDERS_WIDGET__H_
#define __USER_INPUT_MODE_BORDERS_WIDGET__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include <QWidget>

class QActionGroup;
class QComboBox;
class QStackedWidget;

namespace caret {

    class UserInputModeBorders;
    
    class UserInputModeBordersWidget : public QWidget {
        
        Q_OBJECT

    public:
        UserInputModeBordersWidget(UserInputModeBorders* inputModeBorders,
                                   QWidget* parent = 0);
        
        virtual ~UserInputModeBordersWidget();
        
        void updateWidget();
        
    private slots:
        void modeActionTriggered(QAction*);
        void createOperationActionTriggered(QAction*);
        void reviseOperationActionTriggered(QAction*);
        void selectOperationActionTriggered(QAction*);
        
        void modeComboBoxSelection(int);
        
        void createResetButtonClicked();
        void createUndoButtonClicked();
        void createFinishButtonClicked();
        
        void reviseResetButtonClicked();
        void reviseUndoButtonClicked();
        void reviseAcceptButtonClicked();
        
    private:
        UserInputModeBordersWidget(const UserInputModeBordersWidget&);

        UserInputModeBordersWidget& operator=(const UserInputModeBordersWidget&);
        
        void setActionGroupByActionData(QActionGroup* actionGroup, 
                                        const int dataInteger);
        
        QActionGroup* modeActionGroup;
        
        QActionGroup* createOperationActionGroup;
        
        QActionGroup* reviseOperationActionGroup;
        
        QActionGroup* selectOperationActionGroup;
        
        QWidget* createModeWidget();
        
        QWidget* createCreateOperationWidget();
        
        QWidget* createReviseOperationWidget();
        
        QWidget* createSelectOperationWidget();
        
        QComboBox* modeComboBox;
        
        QWidget* widgetMode;
        
        QWidget* widgetCreateOperation;
        
        QWidget* widgetReviseOperation;
        
        QWidget* widgetSelectOperation;
        
        QStackedWidget* operationStackedWidget;
        
        UserInputModeBorders* inputModeBorders;
    };
    
#ifdef __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_BORDERS_WIDGET__H_
