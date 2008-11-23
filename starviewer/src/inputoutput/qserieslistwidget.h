/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSERIESLISTWIDGET_H
#define UDGSERIESLISTWIDGET_H

#include "ui_qserieslistwidgetbase.h"
#include <QWidget>
#include <QHash>
#include <QStringList>

namespace udg {

/** Aquesta classe és un widget millorat del ListWidget de QT, fet  modificat per poder mostrar les sèries d'un estudi.
@author marc
*/

class Series;

const int scaledSeriesSizeX = 100;///< Mida de la imatge escalada a l'eix x
const int scaledSeriesSizeY = 100;///< Mida de la imatge escalada a l'eix y

class QSeriesListWidget : public QWidget , private Ui::QSeriesListWidgetBase{
Q_OBJECT
public:
    ///constructor de la classe
    QSeriesListWidget( QWidget *parent = 0 );

    /// Destructor de la classe
    ~QSeriesListWidget();

    QString getCurrentSeriesUID();

    ///Retorna l'estudi uid de la sèrie seeleccionada
    QString getCurrentStudyUID();

public slots:
    ///Estableix quina és la série seleccionada
    void setCurrentSeries( const QString &seriesUID );

    ///Neteja el ListWidget de sèries
    void clear();

    ///Insereix l'informació d'una sèrie
    void insertSeries(QString studyInstanceUID, Series *series);

    /** slot que s'activa quant es selecciona una serie, emiteix signal a QStudyTreeWidget, perquè selecciona la mateixa serie que el QSeriesListWidget
     *  @param serie Seleccionada
     */
    void clicked( QListWidgetItem *item );

    /** slot que s'activa quant es fa doblec
     *  @param item de la serie Seleccionada
     */
    void view( QListWidgetItem *item );

signals:
    ///quan seleccionem una sèrie emeiteix una signal per a que el QStudyTreeWidget, tingui seleccionada la mateixa sèrie
    void selectedSeriesIcon( QString );

    /// Quan es fa doble click emiteix un signal, perquè la sèrie sigui visualitzada
    void viewSeriesIcon();

private:

    QHash<QString, QString> m_HashSeriesStudy;///Guardem per cada sèrie a quin estudi pertany
    QStringList m_nonDicomImageSeriesList;//modalitats de sèries que no són images, com (KO, PR, SR) 
    int m_lastInsertedImageRow; //indica a quina ha estat la última fila que hem inseritat una sèrie que era una imatge


    /// crea les connexions dels signals i slots
    void createConnections();

};

};

#endif
