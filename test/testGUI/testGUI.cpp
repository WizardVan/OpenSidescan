#include <iostream>

#include <vector>
#include <utility> // for std::pair

#include <QtTest>

#include <QObject>

#include <QWidget>

#include <QToolBar>

#include <QLineEdit>
#include <QPushButton>


#include <QDialogButtonBox>



#include <QDebug>

#include <QMetaObject>





#include "../../src/OpenSidescan/mainwindow.h"



// https://doc.qt.io/qt-5/qtest-overview.html

class testGUI : public QObject
{
    Q_OBJECT

    // https://www.qtcentre.org/threads/23541-Displaying-GUI-events-with-QtTest
    // If want to show the GUI
    void eventLoop(const int msec);


public slots:
    void InteractWithModalWindowActionImport();

    void InteractWithModalWindowAlreadyAnActiveProject();

    void InteractWithModalWindowToSelectProjectToOpen();

//    void InteractWithContextMenu();

//    void InteractWithModalWindowDialogPlatform();

    void selectFileAndVerify( int fileToSelect, std::string & filename,
                                       std::vector<std::string> & tabNames,
                                       std::vector< std::pair< std::string,std::string > > & properties );

    void timeOutOccured();

private slots:

    // The following private slots that are not treated as test functions.


    void initTestCase();        // initTestCase() will be called before the first test function is executed.

//    void initTestCase_data();   // initTestCase_data() will be called to create a global test data table.

    void cleanupTestCase();     // cleanupTestCase() will be called after the last test function was executed.

//    void init();                // init() will be called before each test function is executed.

//    void cleanup();             // cleanup() will be called after every test function.





    // Test functions

    void useToolBarActionImportToLoadSidescanFile();
    void verifyResultOfUseToolBarActionImportToLoadSidescanFile();

    void useToolBarActionOpenProject();
    void verifyResultOfUseToolBarActionOpenProject();

//    void afterContextMenu();

//    void finish();


private:


    MainWindow * mainWindow;

    bool InteractWithModalWindowActionImportReachedTheEnd;
    bool InteractWithModalWindowAlreadyAnActiveProjectReachedTheEnd;
    bool InteractWithModalWindowToSelectProjectToOpenReachedTheEnd;

    bool selectFileAndVerifyReachTheEnd;

    QTimer *timerTimeOut;

};

void testGUI::eventLoop(const int msec)
{
    QEventLoop loop;
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(msec);
    loop.exec();
}


void testGUI::timeOutOccured()
{
    // Just in case, stop the timer
    timerTimeOut->stop();

    qDebug() << tr( "'testGUI::timeOutOccured()'" );

    std::cout << "\n\n" << std::endl;


    // If there is a modal widget, it must be closed to be able to continue through the test functions
    // and delete the mainWindow
    // (Deleting the mainWindow cannot be done here,
    // it must be done in the thread which created the mainWindow.)

    QWidget * modalWidget = QApplication::activeModalWidget();

    if ( modalWidget )
    {
        qDebug() << tr( "QApplication::activeModalWidget():" );

        std::cout << "\n\nmodalWidget: " << modalWidget << "\n" << std::endl;

        qDebug() << tr( "modalWidget->objectName(): " ) << modalWidget->objectName();

        qDebug() << tr( "modalWidget->windowTitle(): " ) << modalWidget->windowTitle();

        modalWidget->close();
    }

}


void testGUI::initTestCase()
{
    mainWindow = nullptr;

    InteractWithModalWindowActionImportReachedTheEnd = false;
    InteractWithModalWindowAlreadyAnActiveProjectReachedTheEnd = false;
    InteractWithModalWindowToSelectProjectToOpenReachedTheEnd = false;

    timerTimeOut = new QTimer( this );
    timerTimeOut->setSingleShot( true );
    connect(timerTimeOut, &QTimer::timeout, this, &testGUI::timeOutOccured );
}

void testGUI::cleanupTestCase()
{

    std::cout << "\n\n\nBeginning of 'testGUI::cleanupTestCase()'" << std::endl;

    if ( mainWindow ) {
        delete mainWindow;
        mainWindow = nullptr;
    }

}


// Test functions


void testGUI::useToolBarActionImportToLoadSidescanFile()
{
//    QSKIP( "Skip the first test" );

    qDebug() << tr( "Beginning of 'useToolBarActionImportToLoadSidescanFile()'" );


    if ( mainWindow ) {
        delete mainWindow;
        mainWindow = nullptr;
    }

    mainWindow = new MainWindow;

    QVERIFY2( mainWindow, "useToolBarActionImportToLoadSidescanFile: mainWindow tests false");


    // Get action for importing a sidescan file

    QAction * actionImport = mainWindow->findChild< QAction * >( "actionImport" );
    QVERIFY2( actionImport, "useToolBarActionImportToLoadSidescanFile: actionImport tests false");


    mainWindow->show();
    eventLoop(1200);


    QToolBar * mainToolBar = mainWindow->findChild< QToolBar * >( "mainToolBar" );
    QVERIFY2( mainToolBar, "useToolBarActionImportToLoadSidescanFile: mainToolBar tests false");


    QWidget *widgetForActionImport = mainToolBar->widgetForAction( actionImport );
    QVERIFY2( widgetForActionImport, "useToolBarActionImportToLoadSidescanFile: widgetForActionImport tests false");


    // Time out timer in case there is a failure while interacting with the modal window
    timerTimeOut->start( 30 * 1000 ); // Time large enough to include the time it takes to load the files

    // Single shot timer for function that will interact with the modal window
    QTimer::singleShot(500, this, SLOT(InteractWithModalWindowActionImport() ) );

    // Click the button to open the modal dialog
    QTest::mouseClick(widgetForActionImport, Qt::LeftButton);

}


void testGUI::selectFileAndVerify( int fileToSelect, std::string & filename,
                                   std::vector<std::string> & tabNames,
                                   std::vector< std::pair< std::string,std::string > > & properties )
{

    selectFileAndVerifyReachTheEnd = false;


    // Select the file to be sure it is displayed

    QModelIndex indexFileToSelect = mainWindow->projectWindow->model->getModelIndexFileIndex( fileToSelect );

    // Scroll until it is visible
    mainWindow->projectWindow->tree->scrollTo( indexFileToSelect );

    QRect rectFileToSelect = mainWindow->projectWindow->tree->visualRect( indexFileToSelect );

    // Verify that the rectangle position corresponds to the same index in the model
    QModelIndex indexForPosition = mainWindow->projectWindow->tree->indexAt(
                                    rectFileToSelect.center() );

    QVERIFY2( indexFileToSelect == indexForPosition,
              qPrintable( "testGUI::selectFileAndVerify: fileToSelect " + QString::number( fileToSelect )
                          + ":, indexFileToSelect is different from indexForPosition" ) );



    // Select the file
    QTest::mouseClick(mainWindow->projectWindow->tree->viewport(), Qt::LeftButton,
                      Qt::NoModifier,
                      rectFileToSelect.center() );


    QModelIndex currentIndex = mainWindow->projectWindow->tree->currentIndex();
    QVERIFY2( currentIndex.row() == fileToSelect,
              qPrintable( "testGUI::selectFileAndVerify: fileToSelect " + QString::number( fileToSelect )
                          + ":, currentIndex.row() is different from fileToSelect" ) );

    std::string modelFilename = mainWindow->projectWindow->model->data(currentIndex, Qt::DisplayRole).toString().toStdString();

    QVERIFY2( modelFilename == filename,
              qPrintable( "testGUI::selectFileAndVerify: filename for fileToSelect "
            + QString::number( fileToSelect ) + " has wrong text of '" +  tr( modelFilename.c_str() )
            + "' instead of the expected '" + tr( filename.c_str() ) + "'" ) );



//    std::cout << "\n\ncurrentIndex.row(): " << currentIndex.row() << std::endl;

    // Give a bit of time to be sure the tabs are settled
    mainWindow->show();
    eventLoop(100);

    // Verify tabs

    QVERIFY2( mainWindow->tabs,
              qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
            + QString::number( fileToSelect ) + ", mainWindow->tabs tests false" ) );

    QVERIFY2( mainWindow->tabs->count() == tabNames.size(),
              qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
            + QString::number( fileToSelect ) + ", the number of tabs is "
            + QString::number( mainWindow->tabs->count() ) + " instead of the expected "
            + QString::number( tabNames.size() ) ) );


    for ( int count = 0; count < tabNames.size(); count++ )
        QVERIFY2( mainWindow->tabs->tabText( count ).toStdString() == tabNames[ count ],
                    qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
                                + QString::number( fileToSelect ) + ", tab with index "
                  + QString::number( count ) + " has wrong tabText of '" +  mainWindow->tabs->tabText( count )
                  + "' instead of the expected '" + tr( tabNames[ count ].c_str() ) + "'" ) );


    // Verify that the first tab is selected

    QVERIFY2( mainWindow->tabs->currentIndex() == 0,
                qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
              + QString::number( fileToSelect ) + ", The current tab index is different from zero") );


    QTabBar *tabBar = mainWindow->tabs->tabBar();

    QVERIFY2( tabBar,
              qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
            + QString::number( fileToSelect ) + ", tabBar tests false" ) );


    QVERIFY2( tabBar->count() == tabNames.size(),
              qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
            + QString::number( fileToSelect ) + ", tabBar->count() is "
            + QString::number( tabBar->count() ) + " instead of the expected "
            + QString::number( tabNames.size() ) ) );


    // Click on all the tabs in reverse order
    for ( int count = tabNames.size() - 1; count >= 0 ; count-- ) {

        const QPoint tabPos = tabBar->tabRect( count ).center();
        QTest::mouseClick( tabBar, Qt::LeftButton, {}, tabPos);

        mainWindow->show();
        eventLoop(500);

        QVERIFY2( mainWindow->tabs->currentIndex() == count,
                  qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
                + QString::number( fileToSelect ) + ", the current tab index of "
                + QString::number( mainWindow->tabs->currentIndex() ) + " is different from the target of "
                + QString::number( count ) ) );
    }


    // Verifying file properties (which uses QTableWidget class)

    for ( int count = 0; count < properties.size(); count++ ) {

        QList<QTableWidgetItem *> items = mainWindow->fileInfo->propertiesTable->findItems(
                                                    QString( properties[count].first.c_str() ), Qt::MatchExactly);

        // It should find the item only once
        QVERIFY2( items.size() == 1,
                  qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
                + QString::number( fileToSelect ) + ", findItems of '"
                + QString( properties[count].first.c_str() ) + "' found " + QString::number( items.size() )
                + " match(es), it should be 1" ) );


        QVERIFY2( items[ 0 ]->column() == 0,
                qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
              + QString::number( fileToSelect ) + ", item of '"
              + QString( properties[count].first.c_str() ) + "' was found in column "
              + QString::number( items[ 0 ]->column() )
              + ". It should be in column 0" ) );


        std::string propertyText = mainWindow->fileInfo->propertiesTable->item( items[ 0 ]->row(), 1 )->data( Qt::DisplayRole ).toString().toStdString();
        QVERIFY2( propertyText == properties[count].second,
                  qPrintable( "testGUI::selectFileAndVerify: fileToSelect "
                + QString::number( fileToSelect ) + ", property '"
                + QString( properties[count].first.c_str() )
                + "', corresponding property value text is '"
                + QString( propertyText.c_str() ) + "' while it should be '"
                + QString( properties[count].second.c_str() ) + "'" ) );

    }


    selectFileAndVerifyReachTheEnd = true;

}



void testGUI::verifyResultOfUseToolBarActionImportToLoadSidescanFile()
{
//    QSKIP( "Skip the first test" );


    timerTimeOut->stop();

    std::cout << "\n" << std::endl;

    qDebug() << tr( "Beginning of 'testGUI::verifyResultOfUseToolBarActionImportToLoadSidescanFile'" );


    if ( InteractWithModalWindowActionImportReachedTheEnd == false )
    {
        // Give time for the window to be closed after the instruction to close is sent
        // in the function responding to the timer
        QTest::qWait( 200 );

        if ( mainWindow ) {
            delete mainWindow;
            mainWindow = nullptr;
        }

        QVERIFY2( InteractWithModalWindowActionImportReachedTheEnd,
                    "verifyResultOfUseToolBarActionImportToLoadSidescanFile: InteractWithModalWindowActionImportReachedTheEnd is false");
    }


    QVERIFY2( mainWindow, "verifyResultOfUseToolBarActionImportToLoadSidescanFile: mainWindow tests false");


    // There should be one file in the tree model

    QVERIFY2( mainWindow->projectWindow->model->getNbFiles() ==  2,
                qPrintable( "verifyResultOfUseToolBarActionImportToLoadSidescanFile: the number of files in the projectWindow is "
                + QString::number( mainWindow->projectWindow->model->getNbFiles() )
                + " instead of 2") );



    // Select the first file to be sure it is displayed and do a verification


    std::string filename;
    std::vector<std::string> tabNames;
    std::vector< std::pair< std::string,std::string > > propertiesToVerify;

    int indexFileToSelect = 0;

    filename = "plane1.xtf";

    tabNames.clear();
    tabNames.push_back( "Channel 0" );
    tabNames.push_back( "Channel 1" );
    tabNames.push_back( "Channel 2" );

    propertiesToVerify.clear();
    propertiesToVerify.push_back( std::make_pair( "Channels (Sonar)", "3" ) );
    propertiesToVerify.push_back( std::make_pair( "Recording Program Name", "DAT2XTF" ) );

    selectFileAndVerify( indexFileToSelect, filename, tabNames, propertiesToVerify );


    mainWindow->show();
    eventLoop(3000);

//    QVERIFY2( false, "verifyResultOfUseToolBarActionImportToLoadSidescanFile: false on purpose");


    if ( mainWindow ) {
        delete mainWindow;
        mainWindow = nullptr;
    }


}


void testGUI::useToolBarActionOpenProject()
{

//    QSKIP( "Skip the second test" );

    qDebug() << tr( "Beginning of 'useToolBarActionOpenProject()'" );

    // In case the previous test did not complete to the end
    if ( mainWindow ) {
        delete mainWindow;
        mainWindow = nullptr;
    }

    mainWindow = new MainWindow;

    QVERIFY2( mainWindow, "useToolBarActionOpenProject: mainWindow tests false");


    // Get action for importing a sidescan file

    QAction * actionOpenProject = mainWindow->findChild< QAction * >( "actionOpenProject" );
    QVERIFY2( actionOpenProject, "useToolBarActionOpenProject: actionOpenProject tests false");


    QToolBar * mainToolBar = mainWindow->findChild< QToolBar * >( "mainToolBar" );
    QVERIFY2( mainToolBar, "useToolBarActionOpenProject: mainToolBar tests false");



    QWidget *widgetForActionOpenProject = mainToolBar->widgetForAction( actionOpenProject );
    QVERIFY2( widgetForActionOpenProject, "useToolBarActionOpenProject: widgetForActionOpenProject tests false");

    // Show the mainWindow before opening the modal window
    mainWindow->show();
    eventLoop(200);


    // Time out timer in case there is a failure while interacting with the modal window
    timerTimeOut->start( 10 * 1000 );

    QTimer::singleShot(500, this, SLOT(InteractWithModalWindowAlreadyAnActiveProject() ) );

    // Click the button to open the modal dialog
    QTest::mouseClick(widgetForActionOpenProject, Qt::LeftButton);

}

void testGUI::verifyResultOfUseToolBarActionOpenProject()
{
//    QSKIP( "Skip the second test" );

    timerTimeOut->stop();

    std::cout << "\n" << std::endl;

    std::cout << "\n\nBeginning of testGUI::verifyResultOfUseToolBarActionOpenProject()\n" << std::endl;

    if ( InteractWithModalWindowToSelectProjectToOpenReachedTheEnd == false )
    {
        // Give time for the window to be closed after the instruction to close is sent
        // in the function responding to the timer
        QTest::qWait( 200 );

        if ( mainWindow ) {
            delete mainWindow;
            mainWindow = nullptr;
        }

        QVERIFY2( InteractWithModalWindowToSelectProjectToOpenReachedTheEnd,
                    "verifyResultOfUseToolBarActionOpenProject: InteractWithModalWindowActionImportReachedTheEnd is false");
    }

    QVERIFY2( mainWindow, "verifyResultOfUseToolBarActionOpenProject: mainWindow tests false");

    // There should be five files in the tree model
    QVERIFY2( mainWindow->projectWindow->model->getNbFiles() ==  5,
                "verifyResultOfUseToolBarActionOpenProject: the number of files in the projectWindow is different from 5");

    mainWindow->show();
    eventLoop(1000);


    int indexFileToSelect;
    std::string filename;
    std::vector<std::string> tabNames;
    std::vector< std::pair< std::string,std::string > > propertiesToVerify;

    indexFileToSelect = 4;

    filename = "scotsman3.xtf";

    tabNames.clear();
    tabNames.push_back( "port" );
    tabNames.push_back( "starboard" );

    propertiesToVerify.clear();
    propertiesToVerify.push_back( std::make_pair( "Channels (Sonar)", "2" ) );
    propertiesToVerify.push_back( std::make_pair( "System Type", "1" ) );

    selectFileAndVerify( indexFileToSelect, filename, tabNames, propertiesToVerify );

    mainWindow->show();
    eventLoop(2000);


    indexFileToSelect = 0;

    filename = "plane1.xtf";

    tabNames.clear();
    tabNames.push_back( "Channel 0" );
    tabNames.push_back( "Channel 1" );
    tabNames.push_back( "Channel 2" );

    propertiesToVerify.clear();
    propertiesToVerify.push_back( std::make_pair( "Channels (Sonar)", "3" ) );
    propertiesToVerify.push_back( std::make_pair( "Recording Program Name", "DAT2XTF" ) );

    selectFileAndVerify( indexFileToSelect, filename, tabNames, propertiesToVerify );

    mainWindow->show();
    eventLoop(2000);




    if ( mainWindow )
    {
        delete mainWindow;
        mainWindow = nullptr;
    }


}


//void testGUI::afterContextMenu()
//{
//    qDebug() << tr( "Beginning of 'testGUI::afterContextMenu()'" );

//    mainWindow->show();
//    eventLoop(10000);

//    if ( mainWindow )
//    {
//        delete mainWindow;
//        mainWindow = nullptr;
//    }


//}


//void testGUI::InteractWithContextMenu()
//{

//    qDebug() << tr( "Beginning of 'testGUI::InteractWithContextMenu'" );


//    mainWindow->show();
//    eventLoop(5000);

//}



void testGUI::InteractWithModalWindowActionImport()
{
    qDebug() << tr( "Beginning of InteractWithModalWindowActionImport()" );

    mainWindow->show();
    eventLoop(500);

    QWidget * modalWidget = QApplication::activeModalWidget();    
    QVERIFY2( modalWidget, "InteractWithModalWindowActionImport: modalWidget tests false");

    QVERIFY2( modalWidget->windowTitle() == tr( "Import Sidescan Files" ),
              "InteractWithModalWindowActionImport: modalWidget->windowTitle() is not 'Import Sidescan Files'" );


    QLineEdit * lineEdit = modalWidget->findChild<QLineEdit*>("fileNameEdit");
    QVERIFY2( lineEdit, "InteractWithModalWindowActionImport: lineEdit tests false");


    // Number of characters currently present in the QLineEdit
    int nbBackspaces = lineEdit->text().length();

    // Use backspaces to clear the current content
    for ( int count = 0; count < nbBackspaces; count++ )
        QTest::keyClick(lineEdit, Qt::Key_Backspace, Qt::NoModifier, 10 );


    mainWindow->show();
    eventLoop(100);


    // Path with respect to the application executable
    // There may be issues, see https://doc.qt.io/qt-5/qcoreapplication.html#applicationDirPath

    QString filename = "\"" + QCoreApplication::applicationDirPath() + "/../"
                                + tr( "data/wrecks/plane1.xtf" ) + "\" "
                       "\"" + QCoreApplication::applicationDirPath() + "/../"
                                + tr( "data/wrecks/scotsman3.xtf" ) + "\" "             ;


    QTest::keyClicks(lineEdit, filename );

    QVERIFY2( lineEdit->text() == filename, "InteractWithModalWindowActionImport: filename is not the same in the QLineEdit");


    // Find the button to accept and close the modal window

    // The buttons are within a QDialogButtonBox

    QDialogButtonBox *buttonBox = modalWidget->findChild<QDialogButtonBox*>("buttonBox");
    QVERIFY2( buttonBox, "InteractWithModalWindowActionImport: buttonBox tests false");


    // The buttons don't have object names,
    // I have to go through the list of buttons and find the button with
    // the desired text

    QList<QAbstractButton *> listButtonBox = buttonBox->buttons();

    QString acceptButtonText = tr( "&Open" );
    QPushButton * acceptButton = nullptr;

    for (QAbstractButton *button : listButtonBox) {

        if ( button->text() == acceptButtonText )
            acceptButton = static_cast<QPushButton * >( button );
    }

    QVERIFY2( acceptButton, "InteractWithModalWindowActionImport: acceptButton tests false");
    QVERIFY2( acceptButton->isEnabled(), "InteractWithModalWindowActionImport: acceptButton is not enabled");



//    std::cout << "\n\n" << std::endl;


    mainWindow->show();
    eventLoop(500);

    // Click button to close the modal dialog
    QTest::mouseClick(acceptButton, Qt::LeftButton);

    mainWindow->show();
    eventLoop(500);


    InteractWithModalWindowActionImportReachedTheEnd = true;

}


void testGUI::InteractWithModalWindowAlreadyAnActiveProject()
{
    qDebug() << tr( "Beginning of InteractWithModalWindowAlreadyAnActiveProject()" );

    mainWindow->show();
    eventLoop(500);

    qDebug() << tr( "After starting AlreadyAnActiveProject modal window" );

//    QVERIFY2( false, "InteractWithModalWindowAlreadyAnActiveProject: false on purpose");


    QWidget * modalWidget = QApplication::activeModalWidget();
    QVERIFY2( modalWidget, "InteractWithModalWindowAlreadyAnActiveProject: modalWidget tests false");

//    std::cout << "\n\nmodalWidget: " << modalWidget << "\n" << std::endl;

//    qDebug() << tr( "modalWidget->objectName(): " ) << modalWidget->objectName();

//    qDebug() << tr( "modalWidget->windowTitle(): " ) << modalWidget->windowTitle();



    QDialogButtonBox *buttonBox = modalWidget->findChild<QDialogButtonBox*>("qt_msgbox_buttonbox");
    QVERIFY2( buttonBox, "InteractWithModalWindowAlreadyAnActiveProject: buttonBox tests false");

    QList<QAbstractButton *> listButtonBox = buttonBox->buttons();

    QString OKButtonText = tr( "&OK" );
    QPushButton * buttonOK = nullptr;

    for (QAbstractButton *button : listButtonBox) {
        if ( button->text() == OKButtonText )
            buttonOK = static_cast<QPushButton * >( button );
    }

    QVERIFY2( buttonOK, "InteractWithModalWindowAlreadyAnActiveProject: buttonOK tests false");

    // Time out timer in case there is a failure while interacting with the modal window
    timerTimeOut->start( 60 * 1000 ); // Time large enough to include the time it takes to load the files

    QTimer::singleShot(500, this, SLOT(InteractWithModalWindowToSelectProjectToOpen() ) );


    // Click the button to open the modal dialog
    QTest::mouseClick(buttonOK, Qt::LeftButton);

    InteractWithModalWindowAlreadyAnActiveProjectReachedTheEnd = true;


}


void testGUI::InteractWithModalWindowToSelectProjectToOpen()
{

    std::cout << "\n" << std::endl;

    qDebug() << tr( "Beginning of 'testGUI::InteractWithModalWindowToSelectProjectToOpen'" );


    if ( InteractWithModalWindowAlreadyAnActiveProjectReachedTheEnd == false )
    {
        // Just in case, stop the timer
        timerTimeOut->stop();

        // Give time for the window to be closed after the instruction to close is sent
        // in the function responding to the timer
        QTest::qWait( 200 );

        if ( mainWindow ) {
            delete mainWindow;
            mainWindow = nullptr;
        }

        QVERIFY2( InteractWithModalWindowAlreadyAnActiveProjectReachedTheEnd,
                    "InteractWithModalWindowToSelectProjectToOpen: InteractWithModalWindowAlreadyAnActiveProjectReachedTheEnd is false");
    }

//    QVERIFY2( mainWindow, "InteractWithModalWindowToSelectProjectToOpen: mainWindow tests false");



    mainWindow->show();
    eventLoop(1200);

//    QVERIFY2( false, "InteractWithModalWindowToSelectProjectToOpen: false on purpose");


    QWidget * modalWidget = QApplication::activeModalWidget();
    QVERIFY2( modalWidget, "InteractWithModalWindowToSelectProjectToOpen: modalWidget tests false");

    QVERIFY2( modalWidget->windowTitle() == tr( "Sidescan Project Files" ),
              "InteractWithModalWindowToSelectProjectToOpen: modalWidget->windowTitle() is not 'Sidescan Project Files'" );


    QLineEdit *lineEdit = modalWidget->findChild<QLineEdit*>("fileNameEdit");
    QVERIFY2( lineEdit, "InteractWithModalWindowToSelectProjectToOpen: lineEdit tests false");


    // Number of characters currently present in the QLineEdit
    int nbBackspaces = lineEdit->text().length();

    // Use backspaces to clear the current content
    for ( int count = 0; count < nbBackspaces; count++ )
        QTest::keyClick(lineEdit, Qt::Key_Backspace, Qt::NoModifier, 10 );


    mainWindow->show();
    eventLoop(100);


    // Path with respect to the application executable
    // There may be issues, see https://doc.qt.io/qt-5/qcoreapplication.html#applicationDirPath
    QString filename = QCoreApplication::applicationDirPath() + "/../"
                                        + tr( "testProject/TestProject5Files.ssp" );

    QTest::keyClicks(lineEdit, filename );

    QVERIFY2( lineEdit->text() == filename, "InteractWithModalWindowToSelectProjectToOpen: filename is not the same in the QLineEdit");


    // Find the button to accept and close the modal window

    // The buttons are within a QDialogButtonBox

    QDialogButtonBox *buttonBox = modalWidget->findChild<QDialogButtonBox*>("buttonBox");
    QVERIFY2( buttonBox, "InteractWithModalWindowToSelectProjectToOpen: buttonBox tests false");

    QList<QAbstractButton *> listButtonBox = buttonBox->buttons();

    QString acceptButtonText = tr( "&Open" );
    QPushButton * acceptButton = nullptr;

    for (QAbstractButton *button : listButtonBox) {

        if ( button->text() == acceptButtonText )
            acceptButton = static_cast<QPushButton * >( button );
    }

    QVERIFY2( acceptButton, "InteractWithModalWindowToSelectProjectToOpen: acceptButton tests false");
    QVERIFY2( acceptButton->isEnabled(), "InteractWithModalWindowToSelectProjectToOpen: acceptButton is not enabled");


    mainWindow->show();
    eventLoop(500);

    // Click button to close the modal dialog
    QTest::mouseClick(acceptButton, Qt::LeftButton);

    mainWindow->show();
    eventLoop(500);

    InteractWithModalWindowToSelectProjectToOpenReachedTheEnd = true;

}


QTEST_MAIN(testGUI)

#include "testGUI.moc"
