/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "q2dviewerwidget.h"

#include <QAction>
#include "volume.h"
#include "logging.h"

namespace udg {

Q2DViewerWidget::Q2DViewerWidget(QWidget *parent)
 : QFrame(parent)
{
    setupUi( this );
    createConnections();
}

Q2DViewerWidget::~Q2DViewerWidget()
{
}

void Q2DViewerWidget::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged(int) ) , m_spinBox , SLOT( setValue(int) ) );
    connect( m_spinBox , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );
    connect( m_2DView, SIGNAL (sliceChanged( int )), this, SLOT ( viewerEvent ( int )));
}

void Q2DViewerWidget::setInput( Volume *input )
{
    m_mainVolume = input;
    m_2DView->setInput( input );
    changeViewToAxial();
    m_2DView->render();
}

void Q2DViewerWidget::mousePressEvent ( QMouseEvent * event )
{
        emit selected( this );
} 

void Q2DViewerWidget::viewerEvent ( int )
{
//     switch( event )
//     {
//     case vtkCommand::LeftButtonPressEvent:
//     case vtkCommand::MiddleButtonPressEvent:
//     case vtkCommand::RightButtonPressEvent:
        emit selected( this );
//         break;
//     default:
//     break;
//     }
}

void Q2DViewerWidget::changeViewToAxial()
{
    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[4] );
    m_spinBox->setMaximum( extent[5] );
    m_slider->setMaximum( extent[5] );
    m_viewText->setText( tr("XY : Axial") );
    m_2DView->setViewToAxial();
    m_2DView->render();

    INFO_LOG("Visor per defecte: Canviem a vista axial");
}

void Q2DViewerWidget::changeViewToSagital()
{
    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[0] );
    m_spinBox->setMaximum( extent[1] );
    m_slider->setMaximum( extent[1] );
    m_viewText->setText( tr("XY : Sagital") );
    m_2DView->setViewToSagittal();
    m_2DView->render();

    INFO_LOG("Visor per defecte: Canviem a vista sagital");
}

void Q2DViewerWidget::changeViewToCoronal()
{
    int extent[6];
    m_mainVolume->getWholeExtent( extent );

    m_spinBox->setMinimum( extent[2] );
    m_spinBox->setMaximum( extent[3] );
    m_slider->setMaximum( extent[3] );
    m_viewText->setText( tr("XY : Coronal") );
    m_2DView->setViewToCoronal();
    m_2DView->render();

    INFO_LOG("Visor per defecte: Canviem a vista coronal");
}

}
