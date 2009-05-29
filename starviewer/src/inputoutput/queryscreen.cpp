/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "queryscreen.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMovie>
#include <QContextMenuEvent>
#include <QShortcut>
#include <QTime>

#include "processimagesingleton.h"
#include "pacsconnection.h"
#include "qstudytreewidget.h"
#include "querypacs.h"
#include "pacsserver.h"
#include "qserieslistwidget.h"
#include "qpacslist.h"
#include "starviewersettings.h"
#include "operation.h"
#include "pacslistdb.h"
#include "logging.h"
#include "status.h"
#include "qcreatedicomdir.h"
#include "dicommask.h"
#include "qoperationstatescreen.h"
#include "localdatabasemanager.h"
#include "patient.h"
#include "starviewerapplication.h"
#include "parsexmlrispierrequest.h"
#include "utils.h"
#include "statswatcher.h"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#endif

namespace udg {

QueryScreen::QueryScreen( QWidget *parent )
 : QDialog(parent )
{
    setupUi( this );
    setWindowFlags( Qt::Widget );
    initialize();//inicialitzem les variables necessàries
    //connectem signals i slots
    createConnections();
    //Fa les comprovacions necessaries per poder executar la QueryScreen de forma correcte   
    checkRequeriments();

    readSettings();
    //fem que per defecte mostri els estudis de la cache
    m_qInputOutputLocalDatabaseWidget->queryStudy(DicomMask());

    // Configuració per Starviewer Lite
#ifdef STARVIEWER_LITE
    m_showPACSNodesToolButton->hide();
    m_operationListToolButton->hide();
    m_createDICOMDIRToolButton->hide();
    m_advancedSearchToolButton->hide();
    m_tab->removeTab(1); // tab de "PACS" fora
#else
    /*L'engeguem després d'haver fet els connects, no es pot fer abans, perquè per exemple en el cas que tinguem un error
     *perquè el port ja està en us, si l'engeguem abans es faria signal indicant error de port en ús i no hi hauria hagut 
     *temps d'haver fet el connect del signal d'error, per tant el signal s'hauria perdut sense poder avisar de l'error
     */
    if (StarviewerSettings().getListenRisRequests()) 
        m_listenRISRequestThread->listen(); 
#endif

    m_statsWatcher = new StatsWatcher("QueryScreen",this);
    m_statsWatcher->addClicksCounter( m_operationListToolButton );
    m_statsWatcher->addClicksCounter( m_showPACSNodesToolButton );
    m_statsWatcher->addClicksCounter( m_createDICOMDIRToolButton );
    m_statsWatcher->addClicksCounter( m_advancedSearchToolButton );
    m_statsWatcher->addClicksCounter( m_viewButtonPACS );
    m_statsWatcher->addClicksCounter( m_retrieveButtonPACS );
    m_statsWatcher->addClicksCounter( m_clearToolButton );
    m_statsWatcher->addClicksCounter( m_createDICOMDIRToolButton );
}

QueryScreen::~QueryScreen()
{
    /*sinó fem un this.close i tenim la finestra queryscreen oberta al tancar l'starviewer, l'starviewer no finalitza
     *desapareixen les finestres, però el procés continua viu
     */
    this->close();

    delete m_listenRISRequestThread;
}

void QueryScreen::initialize()
{
    StarviewerSettings settings;

    //indiquem que la llista de Pacs no es mostra
    m_showPACSNodes = false;
    m_PACSNodes->setVisible(false);

    /* Posem com a pare el pare de la queryscreen, d'aquesta manera quan es tanqui el pare de la queryscreen
     * el QOperationStateScreen també es tancarà
     */
    m_operationStateScreen = new udg::QOperationStateScreen( this );

    m_qcreateDicomdir = new udg::QCreateDicomdir( this );
    m_processImageSingleton = ProcessImageSingleton::getProcessImageSingleton();

    //Indiquem quin és la intefície encara de crear dicomdir per a que es puguin comunicar
    m_qInputOutputLocalDatabaseWidget->setQCreateDicomdir(m_qcreateDicomdir);

    QMovie *operationAnimation = new QMovie(this);
    operationAnimation->setFileName(":/images/loader.gif");
    m_operationAnimation->setMovie(operationAnimation);
    operationAnimation->start();

    m_qadvancedSearchWidget->hide();
    m_operationAnimation->hide();
    m_labelOperation->hide();
    refreshTab( LocalDataBaseTab );

    CreateContextMenuQStudyTreeWidgetPacs();

    setQStudyTreeWidgetColumnsWidth();

    #ifndef STARVIEWER_LITE
    m_listenRISRequestThread = new ListenRISRequestThread(this);
    if (settings.getListenRisRequests()) m_qpopUpRisRequestsScreen = new QPopUpRisRequestsScreen();
    #endif
}

void QueryScreen::CreateContextMenuQStudyTreeWidgetPacs()
{
    QAction *action;

    action = m_contextMenuQStudyTreeWidgetPacs.addAction( QIcon(":/images/view.png") , tr( "&View" ) , this , SLOT( view() ) , tr("Ctrl+V") );
    (void) new QShortcut( action->shortcut() , this , SLOT( view() ) );

    action = m_contextMenuQStudyTreeWidgetPacs.addAction( QIcon(":/images/retrieve.png") , tr("&Retrieve") , this , SLOT( retrieve() ) , tr("Ctrl+R") );
    (void) new QShortcut( action->shortcut() , this , SLOT( retrieve() ) );

    m_studyTreeWidgetPacs->setContextMenu( & m_contextMenuQStudyTreeWidgetPacs ); //Especifiquem que és el menú del PACS
}

void QueryScreen::setQStudyTreeWidgetColumnsWidth()
{
    StarviewerSettings settings;

    for ( int column = 0; column < m_studyTreeWidgetPacs->getNumberOfColumns(); column++)
    {
        m_studyTreeWidgetPacs->setColumnWidth( column , settings.getStudyPacsListColumnWidth(column) );
    }
}

void QueryScreen::checkRequeriments()
{
    //Comprova que la base de dades d'imatges estigui consistent, comprovant que no haguessin quedat estudis a mig descarregar l'última vegada que es va tancar l'starviewer, i si és així esborra les imatges i deixa la base de dades en un estat consistent
    checkDatabaseImageIntegrity();
    //Comprova que el port pel qual es reben les descàrregues d'objectes dicom del PACS no estigui ja en ús
    checkIncomingConnectionsPacsPortNotInUse();
}

void QueryScreen::checkIncomingConnectionsPacsPortNotInUse()
{
    StarviewerSettings settings;

    if (Utils::isPortInUse(settings.getLocalPort().toInt()))
    {
        QString message = tr("Port %1 for incoming connections from PACS is already in use by another application.").arg(settings.getLocalPort());
        message += tr("\n\n%1 couldn't retrieve studies from PACS if the port is in use, please close the application that is using port %2 or change Starviewer port for incoming connections from PACS in the configuration screen.").arg(ApplicationNameString, settings.getLocalPort());
        message += tr("\n\nIf the error has ocurred when openned new %1's windows, close this window. To open new %1 window you have to choose the 'New' option from the File menu.").arg(ApplicationNameString);

        QMessageBox::warning(this, ApplicationNameString, message);
    }
}

void QueryScreen::checkDatabaseImageIntegrity()
{
    LocalDatabaseManager localDatabaseManager;

    localDatabaseManager.checkNoStudiesRetrieving();

    if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok)
    {
        ERROR_LOG("S'ha produït un error esborrant un estudi que no s'havia acabat de descarregar en la última execució");
    }
}

void QueryScreen::updateOperationsInProgressMessage()
{
    if (m_operationStateScreen->getActiveOperationsCount() > 0)
    {
        m_operationAnimation->show();
        m_labelOperation->show();
    }
    else
    {
        m_operationAnimation->hide();
        m_labelOperation->hide();
    }
}

void QueryScreen::createConnections()
{
    //connectem els butons
    connect( m_searchButton, SIGNAL( clicked() ), SLOT( searchStudy() ) );
    connect( m_clearToolButton, SIGNAL( clicked() ), SLOT( clearTexts() ) );
    connect( m_retrieveButtonPACS, SIGNAL( clicked() ), SLOT( retrieve() ) );
    connect( m_operationListToolButton, SIGNAL( clicked() ) , SLOT( showOperationStateScreen() ) );
    connect( m_showPACSNodesToolButton, SIGNAL( toggled(bool) ), m_PACSNodes, SLOT( setVisible(bool) ) );

    connect( m_viewButtonPACS, SIGNAL( clicked() ), SLOT( view() ) );
    connect( m_createDICOMDIRToolButton, SIGNAL( clicked() ), m_qcreateDicomdir, SLOT( show() ) );

    //connectem Slots dels StudyTreeWidget amb la interficie
    connect( m_studyTreeWidgetPacs, SIGNAL( studyExpanded( QString ) ), SLOT( expandStudy( QString ) ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( seriesExpanded( QString , QString ) ), SLOT( expandSeries( QString , QString ) ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( studyDoubleClicked() ), SLOT( retrieve() ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( seriesDoubleClicked() ), SLOT( retrieve() ) );
    connect( m_studyTreeWidgetPacs, SIGNAL( imageDoubleClicked() ), SLOT( retrieve() ) );

    //es canvia de pestanya del TAB
    connect( m_tab , SIGNAL( currentChanged( int ) ), SLOT( refreshTab( int ) ) );

    //connecta el signal que emiteix qexecuteoperationthread, per visualitzar un estudi amb aquesta classe
    connect( &m_qexecuteOperationThread, SIGNAL( viewStudy( QString , QString , QString ) ), SLOT( studyRetrievedView( QString , QString , QString ) ) , Qt::QueuedConnection );

    //connecta els signals el qexecute operation thread amb els de qretrievescreen, per coneixer quant s'ha descarregat una imatge, serie, estudi, si hi ha error, etc..
    connect( &m_qexecuteOperationThread, SIGNAL( setErrorOperation( QString ) ), m_operationStateScreen, SLOT(  setErrorOperation( QString ) ) );
    connect(&m_qexecuteOperationThread, SIGNAL(errorInOperation(QString, QString, QExecuteOperationThread::OperationError)), m_operationStateScreen, SLOT(setErrorOperation(QString)));

    connect( &m_qexecuteOperationThread, SIGNAL( setOperationFinished( QString ) ), m_operationStateScreen, SLOT(  setOperationFinished( QString ) ) );

    connect( &m_qexecuteOperationThread, SIGNAL( setOperating( QString ) ), m_operationStateScreen, SLOT(  setOperating( QString ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( imageCommit( QString , int) ), m_operationStateScreen, SLOT(  imageCommit( QString , int ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( currentProcessingStudyImagesRetrievedChanged(int)), m_operationStateScreen, SLOT( setRetrievedImagesToCurrentProcessingStudy(int) ));
    connect( &m_qexecuteOperationThread, SIGNAL( seriesCommit( QString ) ), m_operationStateScreen, SLOT(  seriesCommit( QString ) ) );
    connect( &m_qexecuteOperationThread, SIGNAL( newOperation( Operation * ) ), m_operationStateScreen, SLOT(  insertNewOperation( Operation *) ) );
    connect(&m_qexecuteOperationThread, SIGNAL(studyWillBeDeleted(QString)), m_qInputOutputLocalDatabaseWidget , SLOT(removeStudyFromQStudyTreeWidget(QString)));
    connect(&m_qexecuteOperationThread, SIGNAL(setCancelledOperation(QString)), m_operationStateScreen, SLOT(setCancelledOperation(QString)));

    // Label d'informació (cutre-xapussa)
    connect(&m_qexecuteOperationThread, SIGNAL(errorInOperation(QString, QString, QExecuteOperationThread::OperationError)), SLOT( updateOperationsInProgressMessage()));
    connect( &m_qexecuteOperationThread, SIGNAL( setErrorOperation(QString) ), SLOT( updateOperationsInProgressMessage() ));
    connect( &m_qexecuteOperationThread, SIGNAL( setOperationFinished(QString) ), SLOT( updateOperationsInProgressMessage() ));
    connect( &m_qexecuteOperationThread, SIGNAL( newOperation(Operation *) ), SLOT( updateOperationsInProgressMessage() ));
    connect( &m_qexecuteOperationThread, SIGNAL( setCancelledOperation(QString) ), SLOT( updateOperationsInProgressMessage() ));

    //connect tracta els errors de connexió al PACS
    connect ( &m_multipleQueryStudy, SIGNAL( errorConnectingPacs( QString ) ), SLOT( errorConnectingPacs( QString ) ) );
    connect ( &m_multipleQueryStudy, SIGNAL( errorQueringStudiesPacs( QString ) ), SLOT( errorQueringStudiesPacs( QString ) ) );

    //connect tracta els errors de connexió al PACS, al descarregar imatges
    connect (&m_qexecuteOperationThread, SIGNAL(errorInOperation(QString, QString, QExecuteOperationThread::OperationError)), SLOT(showQExecuteOperationThreadError(QString, QString, QExecuteOperationThread::OperationError)));
    connect( &m_qexecuteOperationThread, SIGNAL( retrieveFinished( QString ) ), m_qInputOutputLocalDatabaseWidget, SLOT( addStudyToQStudyTreeWidget( QString ) ) );

    //Amaga o ensenya la cerca avançada
    connect( m_advancedSearchToolButton, SIGNAL( toggled( bool ) ), SLOT( setAdvancedSearchVisible( bool ) ) );

    #ifndef STARVIEWER_LITE
    connect(m_listenRISRequestThread, SIGNAL(requestRetrieveStudy(DicomMask)), SLOT(retrieveStudyFromRISRequest(DicomMask)));
    connect(m_listenRISRequestThread, SIGNAL(errorListening(ListenRISRequestThread::ListenRISRequestThreadError)), SLOT(showListenRISRequestThreadError(ListenRISRequestThread::ListenRISRequestThreadError)));
    #endif

    connect(m_qInputOutputDicomdirWidget, SIGNAL(clearSearchTexts()), SLOT(clearTexts()));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(viewPatients(QList<Patient*>)), SLOT(viewPatients(QList<Patient*>)));
    connect(m_qInputOutputDicomdirWidget, SIGNAL(studyRetrieved()), SLOT(refreshLocalDatabaseTab()));

    connect(m_qInputOutputLocalDatabaseWidget, SIGNAL(viewPatients(QList<Patient*>)), SLOT(viewPatients(QList<Patient*>)));
}

void QueryScreen::setAdvancedSearchVisible(bool visible)
{
    m_qadvancedSearchWidget->setVisible(visible);

    if (visible)
    {
        m_advancedSearchToolButton->setText( m_advancedSearchToolButton->text().replace(">>","<<") );
    }
    else
    {
        m_qadvancedSearchWidget->clear();
        m_advancedSearchToolButton->setText( m_advancedSearchToolButton->text().replace("<<",">>") );
    }
}

void QueryScreen::readSettings()
{
    StarviewerSettings settings;
    this->restoreGeometry( settings.getQueryScreenGeometry() );

    //carreguem el processImageSingleton
    m_processImageSingleton->setPath( settings.getCacheImagePath() );
}

void QueryScreen::clearTexts()
{
    m_qbasicSearchWidget->clear();
    m_qadvancedSearchWidget->clear();
}

void QueryScreen::updateConfiguration(const QString &configuration)
{
    if (configuration == "Pacs/ListChanged")
    {
        m_PACSNodes->refresh();
    }
    else if (configuration == "Pacs/CacheCleared")
    {
        m_qInputOutputLocalDatabaseWidget->clear();
    }
}

void QueryScreen::bringToFront()
{
    this->show();
    this->raise();
    this->activateWindow();
}

#ifndef STARVIEWER_LITE
void QueryScreen::showPACSTab()
{
    m_tab->setCurrentIndex( PACSQueryTab );
    bringToFront();
}
#endif

void QueryScreen::showLocalExams()
{
    m_tab->setCurrentIndex( LocalDataBaseTab );
    m_qbasicSearchWidget->clear();
    m_qbasicSearchWidget->setDefaultDate( QBasicSearchWidget::AnyDate );
    m_qadvancedSearchWidget->clear();
    m_qInputOutputLocalDatabaseWidget->queryStudy(DicomMask());
    bringToFront();
}

void QueryScreen::searchStudy()
{
    switch ( m_tab->currentIndex() )
    {
        case LocalDataBaseTab:
            m_qInputOutputLocalDatabaseWidget->queryStudy(buildDicomMask());
            break;

        case PACSQueryTab:
            queryStudyPacs();
        break;

        case DICOMDIRTab:
            m_qInputOutputDicomdirWidget->queryStudy(buildDicomMask());
            break;
    }
}

PacsServer QueryScreen::getPacsServerByPacsID(QString pacsID)
{
    PacsParameters pacsParameters;
    PacsListDB pacsListDB;
    pacsParameters = pacsListDB.queryPacs(pacsID);//cerquem els paràmetres del Pacs 

    StarviewerSettings settings;
    pacsParameters.setAELocal( settings.getAETitleMachine() ); //especifiquem el nostres AE
    pacsParameters.setTimeOut( settings.getTimeout().toInt( NULL , 10 ) ); //li especifiquem el TimeOut

    PacsServer pacsServer;
    pacsServer.setPacs( pacsParameters );

    return pacsServer;
}

void QueryScreen::queryStudyPacs()
{
    QList<PacsParameters> selectedPacsList;
    selectedPacsList = m_PACSNodes->getSelectedPacs(); //Emplemen el pacsList amb les pacs seleccionats al QPacsList

    if (selectedPacsList.isEmpty()) //es comprova que hi hagi pacs seleccionats
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "Please select a PACS to query" ) );
        return;
    }

    DicomMask searchMask = buildDicomMask();
    StatsWatcher::log( "Cerca al PACS amb paràmetres: " + searchMask.getFilledMaskFields() );
    
    bool stopQuery = false;

    if ( searchMask.isAHeavyQuery() )
    {
        QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString,
                                                                     tr("This query can take a long time.\nDo you want continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        stopQuery = (response == QMessageBox::No);
    }

    if( !stopQuery )
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

        Status queryStatus = queryMultiplePacs(searchMask, selectedPacsList, &m_multipleQueryStudy);

        if( !queryStatus.good() )  //no fem la query
        {
            m_studyTreeWidgetPacs->clear();
            QApplication::restoreOverrideCursor();
            QMessageBox::information( this , ApplicationNameString , tr( "An error has produced while querying.\n\nRepeat it, if the problem persist contact with an administrator." ) );
            return;
        }

        if (m_multipleQueryStudy.getPatientStudyList().isEmpty() )
        {
            m_studyTreeWidgetPacs->clear();
            QApplication::restoreOverrideCursor();
            QMessageBox::information( this , ApplicationNameString , tr( "No study match found." ) );
            return;
        }
        m_hashPacsIDOfStudyInstanceUID = m_multipleQueryStudy.getHashTablePacsIDOfStudyInstanceUID();

        m_studyTreeWidgetPacs->insertPatientList( m_multipleQueryStudy.getPatientStudyList() ); //fem que es visualitzi l'studyView seleccionat
        m_studyTreeWidgetPacs->setSortColumn( QStudyTreeWidget::ObjectName );

        QApplication::restoreOverrideCursor();
    }
}

Status QueryScreen::queryMultiplePacs(DicomMask searchMask, QList<PacsParameters> listPacsToQuery, MultipleQueryStudy *multipleQueryStudy)
{
    QList<PacsParameters> filledPacsParameters;
    StarviewerSettings settings;

    //TODO PacsParameters no hauria de contenir el AETitle i el timeout
    //Hem d'afegir a les dades de pacs parameters el nostre aetitle i timeout
    foreach(PacsParameters pacs, listPacsToQuery)
    {
        pacs.setAELocal(settings.getAETitleMachine());
        pacs.setTimeOut(settings.getTimeout().toInt());
        filledPacsParameters.append(pacs);
    }

    multipleQueryStudy->setMask( searchMask ); //assignem la mascara
    multipleQueryStudy->setPacsList(filledPacsParameters);
    return multipleQueryStudy->StartQueries();
}

void QueryScreen::expandStudy( const QString &studyUID )
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    switch ( m_tab->currentIndex() )
    {
        case PACSQueryTab :  //si estem la pestanya del PACS fem query al Pacs
            querySeriesPacs(studyUID);
            break;
    }

    QApplication::restoreOverrideCursor();
}

/* AQUESTA ACCIO ES CRIDADA DES DEL STUDYLISTVIEW*/
void QueryScreen::expandSeries( const QString &studyUID, const QString &seriesUID)
{
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    switch ( m_tab->currentIndex() )
    {
        case PACSQueryTab :  //si estem la pestanya del PACS fem query al Pacs
            queryImagePacs( studyUID , seriesUID );
            break;
    }

    QApplication::restoreOverrideCursor();
}

void QueryScreen::querySeriesPacs(QString studyUID)
{
    Status state;
    QString text, pacsDescription, pacsID = getPacsIDFromQueriedStudies(studyUID);
    QueryPacs querySeriesPacs;

    PacsServer pacsServer = getPacsServerByPacsID(pacsID);
    pacsDescription = pacsServer.getPacs().getAEPacs() + " Institució" + pacsServer.getPacs().getInstitution()  + " IP:" + pacsServer.getPacs().getPacsAddress(); 

    INFO_LOG("Cercant informacio de les sèries de l'estudi" + studyUID + " del PACS " + pacsDescription);

    state = pacsServer.connect(PacsServer::query, PacsServer::seriesLevel);
    if ( !state.good() )
    {
        //Error al connectar
        ERROR_LOG( "Error al connectar al PACS " + pacsDescription + ". PACS ERROR : " + state.text() );
        errorConnectingPacs (pacsID);
        return;
    }

    querySeriesPacs.setConnection(pacsID, pacsServer.getConnection());
    state = querySeriesPacs.query( buildSeriesDicomMask( studyUID ) );
    pacsServer.disconnect();

    if ( !state.good() )
    {
        //Error a la query
        ERROR_LOG( "QueryScreen::QueryPacs : Error cercant les sèries al PACS " + pacsDescription + ". PACS ERROR : " + state.text() );

        text = tr( "Error! Can't query series to PACS named %1" ).arg( pacsDescription );
        QMessageBox::warning( this , ApplicationNameString , text );
        return;
    }

    if ( querySeriesPacs.getQueryResultsAsSeriesList().isEmpty() )
    {
        QMessageBox::information( this , ApplicationNameString , tr( "No series match for this study.\n" ) );
        return;
    }

    m_studyTreeWidgetPacs->insertSeriesList( studyUID, querySeriesPacs.getQueryResultsAsSeriesList() );
}

void QueryScreen::queryImagePacs(QString studyUID, QString seriesUID)
{
    Status state;
    QString text, pacsDescription, pacsID = getPacsIDFromQueriedStudies(studyUID);
    QueryPacs queryImages;
    DicomMask dicomMask;

    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

    PacsServer pacsServer = getPacsServerByPacsID(pacsID);
    pacsDescription = pacsServer.getPacs().getAEPacs() + " Institució" + pacsServer.getPacs().getInstitution()  + " IP:" + pacsServer.getPacs().getPacsAddress(); 

    INFO_LOG("Cercant informacio de les imatges de l'estudi" + studyUID + " serie " + seriesUID + " del PACS " + pacsDescription);

    dicomMask.setStudyUID( studyUID );
    dicomMask.setSeriesUID( seriesUID );
    dicomMask.setImageNumber( "" );
    dicomMask.setSOPInstanceUID( "" );

    state = pacsServer.connect(PacsServer::query,PacsServer::imageLevel);
    if ( !state.good() )
    {   //Error al connectar
        QApplication::restoreOverrideCursor();
        ERROR_LOG( "Error al connectar al PACS " + pacsDescription + ". PACS ERROR : " + state.text() );
        errorConnectingPacs (pacsID);
        return;
    }

    queryImages.setConnection(pacsID, pacsServer.getConnection());

    state = queryImages.query( dicomMask );
    if ( !state.good() )
    {
        //Error a la query
        QApplication::restoreOverrideCursor();
        ERROR_LOG( "QueryScreen::QueryPacs : Error cercant les images al PACS " + pacsDescription + ". PACS ERROR : " + state.text() );

        text = tr( "Error! Can't query images to PACS named %1 " ).arg(pacsDescription);
        QMessageBox::warning( this , ApplicationNameString , text );
        return;
    }

    pacsServer.disconnect();

    if ( queryImages.getQueryResultsAsImageList().isEmpty() )
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::information( this , ApplicationNameString , tr( "No images match for this series.\n" ) );
        return;
    }

    m_studyTreeWidgetPacs->insertImageList( studyUID , seriesUID , queryImages.getQueryResultsAsImageList() );

    QApplication::restoreOverrideCursor();
}

void QueryScreen::retrieve(bool view)
{
    StatsWatcher::log( QString("Cridem slot 'retrieve(view=%1)'. Cridat desde: %2").arg(view).arg(sender()->objectName()) );
    QList<Study*> selectedStudies = m_studyTreeWidgetPacs->getSelectedStudies();

    if( selectedStudies.isEmpty() )
    {
        QApplication::restoreOverrideCursor();
        if( view )
            QMessageBox::warning( this , ApplicationNameString , tr( "Select a study to view " ) );
        else
            QMessageBox::warning( this , ApplicationNameString , tr( "Select a study to download " ) );

        return;
    }

    foreach( Study *studyToRetrieve, selectedStudies)
    {
        DicomMask maskStudyToRetrieve;

        maskStudyToRetrieve.setStudyUID( studyToRetrieve->getInstanceUID() );

        // TODO aquí només tenim en compte l'última sèrie o imatge seleccionada
        // per tant si seleccionem més d'una sèrie/imatge només s'en baixarà una
        // Caldria fer possible que es baixi tants com en seleccionem
        if ( !m_studyTreeWidgetPacs->getCurrentSeriesUID().isEmpty() )
            maskStudyToRetrieve.setSeriesUID( m_studyTreeWidgetPacs->getCurrentSeriesUID() );

        if ( !m_studyTreeWidgetPacs->getCurrentImageUID().isEmpty() )
            maskStudyToRetrieve.setSOPInstanceUID( m_studyTreeWidgetPacs->getCurrentImageUID() );

        retrieveFromPacs(view, getPacsIDFromQueriedStudies( studyToRetrieve->getInstanceUID() ), maskStudyToRetrieve, studyToRetrieve);
    }
}

void QueryScreen::retrieveFromPacs(bool view, QString pacsIdToRetrieve, DicomMask maskStudyToRetrieve, Study *studyToRetrieve)
{
    StarviewerSettings settings;
    QString defaultSeriesUID;
    Operation operation;
    PacsParameters pacs;

    QApplication::setOverrideCursor( QCursor ( Qt::WaitCursor ) );

    //busquem els paràmetres del pacs del qual volem descarregar l'estudi
    PacsListDB pacsListDB;
    pacs = pacsListDB.queryPacs(pacsIdToRetrieve);

    //emplanem els parametres locals per conenctar amb el pacs amb dades del starviewersettings
    pacs.setAELocal( settings.getAETitleMachine() );
    pacs.setTimeOut( settings.getTimeout().toInt( NULL, 10 ) );
    pacs.setLocalPort( settings.getLocalPort() );

    //definim l'operació
    operation.setPacsParameters( pacs );
    operation.setDicomMask(maskStudyToRetrieve);
    if ( view )
    {
        operation.setOperation( Operation::View );
        operation.setPriority( Operation::Medium );//Té priorita mitjà per passar al davant de les operacions de Retrieve
    }
    else
    {
        operation.setOperation( Operation::Retrieve );
        operation.setPriority( Operation::Low );
    }
    //emplenem les dades de l'operació
    operation.setPatientName( studyToRetrieve->getParentPatient()->getFullName() );
    operation.setPatientID( studyToRetrieve->getParentPatient()->getID() );
    operation.setStudyID( studyToRetrieve->getID() );
    operation.setStudyUID( maskStudyToRetrieve.getStudyUID() );

    m_qexecuteOperationThread.queueOperation( operation );

    QApplication::restoreOverrideCursor();
}

void QueryScreen::studyRetrievedView( QString studyUID , QString seriesUID , QString sopInstanceUID )
{
    QStringList studyUIDList;
    studyUIDList << studyUID;

    //Indiquem que volem veure un estudi que està guardat a la base de dades
    m_qInputOutputLocalDatabaseWidget->view(studyUIDList, seriesUID);
}

void QueryScreen::refreshTab( int index )
{
    switch ( index )
    {
        case LocalDataBaseTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(false);
                m_qadvancedSearchWidget->setEnabled(false);
                break;

        case PACSQueryTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(true);
                m_qadvancedSearchWidget->setEnabled( true );
                break;

        case DICOMDIRTab:
                m_qbasicSearchWidget->setEnabledSeriesModality(false);
                m_qadvancedSearchWidget->setEnabled(false);
                break;
    }
}

void QueryScreen::view()
{
    StatsWatcher::log( "Cridem slot 'view()'. Cridat desde: " + sender()->objectName() );
    switch ( m_tab->currentIndex() )
    {
        case PACSQueryTab :
            retrieve(true);
           break;

        default :
            break;
    }
}

void QueryScreen::viewPatients(QList<Patient*> listPatientsToView)
{
    this->close();//s'amaga per poder visualitzar la serie

    if (m_operationStateScreen->isVisible())
    {
        m_operationStateScreen->close();//s'amaga per poder visualitzar la serie
    }

    emit selectedPatients(listPatientsToView);
}

void QueryScreen::refreshLocalDatabaseTab()
{
    m_qInputOutputLocalDatabaseWidget->queryStudy(DicomMask());
}

void QueryScreen::closeEvent( QCloseEvent* event )
{
    writeSettings(); // guardem els settings

    m_operationStateScreen->close(); //Tanquem la QOperationStateScreen al tancar la QueryScreen

    event->accept();
    m_qcreateDicomdir->clearTemporaryDir();
}

void QueryScreen::writeSettings()
{
    /* Només guardem els settings quan la interfície ha estat visible, ja que hi ha settings com el QSplitter que si la primera vegada
     * que executem l'starviewer no obrim la QueryScreen retorna un valors incorrecte per això, el que fem és comprova que la QueryScreen
     * hagi estat visible per guardar el settings
     */
    if (this->isVisible())
    {
        StarviewerSettings settings;

        saveQStudyTreeWidgetColumnsWidth();

        settings.saveQueryScreenGeometry( this->saveGeometry() );
    }
}

void QueryScreen::saveQStudyTreeWidgetColumnsWidth()
{
    StarviewerSettings settings;

    for ( int column = 0; column < m_studyTreeWidgetPacs->getNumberOfColumns(); column++ )
    {
        settings.setStudyPacsListColumnWidth( column , m_studyTreeWidgetPacs->getColumnWidth( column ) );
    }
}

void QueryScreen::showOperationStateScreen()
{
    if ( !m_operationStateScreen->isVisible() )
    {
        m_operationStateScreen->setVisible( true );
    }
    else
    {
        m_operationStateScreen->raise();
        m_operationStateScreen->activateWindow();
    }
}

void QueryScreen::openDicomdir()
{
    m_qInputOutputDicomdirWidget->openDicomdir();

    this->bringToFront();
    m_tab->setCurrentIndex( DICOMDIRTab ); // mostre el tab del dicomdir
}

void QueryScreen::storeStudiesToPacs()
{
    /*QList<PacsParameters> selectedPacsList;
    QStringList studiesUIDList = m_studyTreeWidgetCache->getSelectedStudiesUID();
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    StarviewerSettings settings;

    selectedPacsList = m_PACSNodes->getSelectedPacs(); //Emplemen el pacsList amb les pacs seleccionats al QPacsList

    if(selectedPacsList.size() == 0)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("You have to select a PACS to store the study in") );
    }
    else if(selectedPacsList.size() == 1)
    {
        StarviewerSettings settings;
        foreach( QString studyUID, studiesUIDList )
        {
            PacsListDB pacsListDB;
            PacsParameters pacs;
            Operation storeStudyOperation;
            Study *study;
            LocalDatabaseManager localDatabaseManager;
            QList<Patient*> patientList;

            DicomMask dicomMask;
            dicomMask.setStudyUID(studyUID);
            patientList = localDatabaseManager.queryPatientStudy(dicomMask);
//            if( showDatabaseManagerError( localDatabaseManager.getLastError() ))    return;

            // \TODO Això s'ha de fer perquè queryPatientStudy retorna llista de Patients
            // Nosaltres, en realitat, volem llista d'study amb les dades de Patient omplertes.
            if(patientList.size() != 1 && patientList.first()->getNumberOfStudies() != 1)
            {
//                showDatabaseManagerError(LocalDatabaseManager::DatabaseCorrupted);
                return;
            }

            study = patientList.first()->getStudies().first();
            Patient *patient = study->getParentPatient();

            storeStudyOperation.setPatientName( patient->getFullName() );
            storeStudyOperation.setPatientID( patient->getID() );
            storeStudyOperation.setStudyUID( study->getInstanceUID() );
            storeStudyOperation.setStudyID( study->getID() );
            storeStudyOperation.setPriority( Operation::Low );
            storeStudyOperation.setOperation( Operation::Move );
            storeStudyOperation.setDicomMask( dicomMask );

            delete patient;
            //cerquem els paràmetres del Pacs al qual s'han de cercar les dades
            pacs = pacsListDB.queryPacs(selectedPacsList.value(0).getPacsID());
            pacs.setAELocal(settings.getAETitleMachine());
            pacs.setTimeOut(settings.getTimeout().toInt());
            storeStudyOperation.setPacsParameters( pacs );

            m_qexecuteOperationThread.queueOperation( storeStudyOperation );
        }
    }
    else
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(this, ApplicationNameString, tr("The studies can only be stored to one PACS") );
    }

    QApplication::restoreOverrideCursor();*/
}

void QueryScreen::errorConnectingPacs( QString IDPacs )
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    errorPacs = pacsListDB.queryPacs(IDPacs);

    errorMessage = tr( "Can't connect to PACS %1 from %2.\nBe sure that the IP and AETitle of the PACS are correct." )
        .arg( errorPacs.getAEPacs() )
        .arg( errorPacs.getInstitution()
    );

    QMessageBox::critical( this , ApplicationNameString , errorMessage );
}

void QueryScreen::errorQueringStudiesPacs(QString PacsID)
{
    PacsListDB pacsListDB;
    PacsParameters errorPacs;
    QString errorMessage;

    errorPacs = pacsListDB.queryPacs(PacsID);
    errorMessage = tr( "Can't query PACS %1 from %2\nBe sure that the IP and AETitle of this PACS are correct" )
        .arg( errorPacs.getAEPacs() )
        .arg( errorPacs.getInstitution()
    );

    QMessageBox::critical( this , ApplicationNameString , errorMessage );
}

DicomMask QueryScreen::buildSeriesDicomMask( QString studyUID )
{
    DicomMask mask;

    mask.setStudyUID( studyUID );
    mask.setSeriesDate( "" );
    mask.setSeriesTime( "" );
    mask.setSeriesModality( "" );
    mask.setSeriesNumber( "" );
    mask.setSeriesUID( "" );
    mask.setPPSStartDate( "" );
    mask.setPPStartTime( "" );
    mask.setRequestAttributeSequence( "" , "" );

    return mask;
}

DicomMask QueryScreen::buildDicomMask()
{
    return m_qbasicSearchWidget->buildDicomMask() + m_qadvancedSearchWidget->buildDicomMask();
}

void QueryScreen::retrieveStudyFromRISRequest(DicomMask maskRisRequest)
{
    MultipleQueryStudy multipleQueryStudy;
    DicomMask maskStudyToRetrieve;
    StarviewerSettings settings;

    m_qpopUpRisRequestsScreen->setAccessionNumber(maskRisRequest.getAccessionNumber()); //Mostrem el popUP amb l'accession number

    m_qpopUpRisRequestsScreen->show();

    Status state = queryMultiplePacs(maskRisRequest, PacsListDB().queryDefaultPacs(), &multipleQueryStudy);

    //Fem els connects per tracta els possibles errors que es poden donar
    connect ( &m_multipleQueryStudy, SIGNAL( errorConnectingPacs( QString ) ), SLOT( errorConnectingPacs( QString ) ) );
    connect ( &m_multipleQueryStudy, SIGNAL( errorQueringStudiesPacs( QString ) ), SLOT( errorQueringStudiesPacs( QString ) ) );

    if (!state.good())
    {
        QMessageBox::critical(this , ApplicationNameString , tr("An error ocurred querying default PACS, can't process the RIS request."));
        return;
    }

    if (multipleQueryStudy.getPatientStudyList().isEmpty())
    {
        QString message = tr("%2 can't execute the RIS request, because hasn't found the Study with accession number %1 in the default PACS.").arg(maskRisRequest.getAccessionNumber(), ApplicationNameString);
        QMessageBox::information(this , ApplicationNameString , message);
        return;
    }

    foreach (Patient *patient, multipleQueryStudy.getPatientStudyList())
    {
        foreach(Study *study, patient->getStudies())
        {
            QString pacsID = multipleQueryStudy.getHashTablePacsIDOfStudyInstanceUID()[study->getInstanceUID()];

            maskStudyToRetrieve.setStudyUID(study->getInstanceUID());
            retrieveFromPacs(settings.getViewAutomaticallyAStudyRetrievedFromRisRequest(), pacsID, maskStudyToRetrieve, study);
        }
    }
}

QString QueryScreen::getPacsIDFromQueriedStudies(QString studyInstanceUID)
{
    /*TODO Tenir en compte que podem tenir un studyUID repetit en dos PACS, ara mateix no ho tenim contemplat a la QHash  */
    if (!m_hashPacsIDOfStudyInstanceUID.contains(studyInstanceUID))
    {
        ERROR_LOG(QString("No s'ha trobat a quin PACS pertany l'estudi %1 a la QHash").arg(studyInstanceUID));
        return "";
    }
    else
        return m_hashPacsIDOfStudyInstanceUID[studyInstanceUID];
}

void QueryScreen::showQExecuteOperationThreadError(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::OperationError error)
{
    QString message;
    StarviewerSettings settings;
    PacsParameters pacs = PacsListDB().queryPacs(pacsID);

    switch (error)
    {
        case QExecuteOperationThread::ErrorConnectingPacs :
            message = tr("Please review the operation list screen, ");
            message += tr("%1 can't connect to PACS %2 trying to retrieve or store a study.\n").arg(ApplicationNameString, pacs.getAEPacs());
            message += tr("\nBe sure that your computer is connected on network and the Pacs parameters are correct.");
            message += tr("\nIf the problem persist contact with an administrator.");
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
        case QExecuteOperationThread::ErrorRetrieving :
            message = tr("Please review the operation list screen, ");
            message += tr("an error ocurred retrieving a study.\n");
            message += tr("\nPACS %1 doesn't respond correctly, be sure that your computer is connected on network and the PACS parameters are correct.").arg(pacs.getAEPacs());
            message += tr("\nIf the problem persist contact with an administrator.");
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
        case QExecuteOperationThread::MoveDestinationAETileUnknown:
            message = tr("Please review the operation list screen, ");
            message += tr("PACS %1 doesn't recognize your computer's AETitle %2 and some studies can't be retrieved.").arg(pacs.getAEPacs(), settings.getAETitleMachine());
            message += tr("\n\nContact with an administrador to register your computer to the PACS.");
            QMessageBox::warning( this , ApplicationNameString , message );
            break;
        case QExecuteOperationThread::NoEnoughSpace :
            message = tr("There is not enough space to retrieve studies, please free space.");
            message += tr("\nAll pending retrieve operations will be cancelled.");
            QMessageBox::warning( this , ApplicationNameString , message );
            break;
        case QExecuteOperationThread::ErrorFreeingSpace :
            message = tr("Please review the operation list screen, ");
            message += tr("an error ocurred freeing space and some operations may have failed.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
        case QExecuteOperationThread::DatabaseError :
            message = tr("Please review the operation list screen, ");
            message += tr("a database error ocurred and some operations may have failed.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
       case QExecuteOperationThread::PatientInconsistent :
            message = tr("Please review the operation list screen, ");
            message += tr("an error ocurred and some operations may have failed.");
            message += tr("\n%1 has not be capable of read correctly dicom information of the study.").arg(ApplicationNameString);
            message += tr("\n\nThe study may be corrupted, if It is not corrupted please contact with %1 team.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
	   case QExecuteOperationThread::MoveRefusedOutOfResources :
			message = tr("Please review the operation list screen, ");
            message += tr("PACS %1 is out of resources and can't process the request for retrieving a study.").arg(pacs.getAEPacs());
            message += tr("\n\nTry later to retrieve the study, if the problem persists please contact with PACS administrator to solve the problem.");
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
       case QExecuteOperationThread::IncomingConnectionsPortPacsInUse :
            message = tr("Port %1 for incoming connections from PACS is already in use by another application.").arg(StarviewerSettings().getLocalPort());
            message += tr("\n\n%1 can't retrieve the studies, all pending retrieve operations will be cancelled.").arg(ApplicationNameString);
            message += tr("\n\nIf there is another %1 window retrieving studies from the PACS please wait until those retrieving has finished and try again.").arg(ApplicationNameString);
            QMessageBox::critical( this , ApplicationNameString , message );
            break;
        default:
            message = tr("Please review the operation list screen, an unknown error has ocurred retrieving a study.");
            message += tr("\n\nClose all %1 windows and try again."
                         "\nIf the problem persist contact with an administrator.").arg(ApplicationNameString);
    }
}

void QueryScreen::showListenRISRequestThreadError(ListenRISRequestThread::ListenRISRequestThreadError error)
{
    QString message;
    StarviewerSettings settings;

    switch(error)
    {
        case ListenRISRequestThread::risPortInUse :
            message = tr("Can't listen RIS requests on port %1, the port is in use by another application.").arg(settings.getListenPortRisRequests());
            message += tr("\n\nIf the error has ocurred when openned new %1's windows, close this window. To open new %1 window you have to choose the 'New' option from the File menu.").arg(ApplicationNameString);
            break;
        case ListenRISRequestThread::unknowNetworkError :
            message = tr("Can't listen RIS requests on port %1, an unknown network error has produced.").arg(settings.getListenPortRisRequests());
            message += tr("\nIf the problem persist contact with an administrator.");
            break;
    }
    
    QMessageBox::critical(this, ApplicationNameString, message);
}

};

