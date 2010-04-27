/*
 * Route save dialog class
 *
 * @author     Toni Jussila <toni.jussila@fudeco.com>
 * @copyright  (c) 2010 Speed Freak team
 * @license    http://opensource.org/licenses/gpl-license.php GNU Public License
 */

#include "routesavedialog.h"
#include "ui_routesavedialog.h"
#include <QDebug>

/**
  *Constructor of this class.
  *@param QWidget pointer to parent object. By default the value is NULL.
  */
RouteSaveDialog::RouteSaveDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::RouteSaveDialog){

    qDebug() << "__RouteSaveDialog";
    ui->setupUi(this);
    this->setWindowTitle("Tracking");

    routeDialog = NULL;
    location = NULL;
    gpsData = NULL;
    helpRoutingDialog = NULL;

    //Clear variables
    averageSpeed = 0.0;
    speed = 0.0;
    allSpeeds = 0.0;
    speedCount = 0;

    //Button settings
    buttonStatus = true;
    pixmapRouteStop = new QPixmap("Graphics/route_stop.png");
    pixmapRouteStart = new QPixmap("Graphics/route_start.png");
    iconRouteStop = new QIcon(*pixmapRouteStop);
    iconRouteStart = new QIcon(*pixmapRouteStart);
    QSize iconSize(125, 125);
    ui->buttonRouteStartStop->setIconSize(iconSize);
    ui->buttonRouteStartStop->setIcon(*iconRouteStart);
    ui->buttonRouteStartStop->setAutoFillBackground(true);
    ui->buttonRouteStartStop->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255)");
    ui->pushButtonInfo->setAutoFillBackground(true);
    ui->pushButtonInfo->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255)");

    //Satellite picture and label
    ui->labelRouteSatelliteStatus->setVisible(0);
    ui->labelRouteSatellitePicture->setVisible(0);
    //ui->labelRouteSatellitePicture->setPixmap(QPixmap("Graphics/satellite_vista.png"));
    timerSatellitePicture = new QTimer();
    timerSatellitePicture->setInterval(400);
    connect(timerSatellitePicture, SIGNAL(timeout()),this, SLOT(timerSatellitePictureTimeout()));
    ui->labelUserInfo->setText("Push start button");  //User info label

    //Invisible or clear labels
    ui->labelRouteStatus->setVisible(0);
    ui->labelRoutePicture->setVisible(0);
    ui->labelGpsSpeed->setVisible(0); //GPS speed label
    ui->labelGpsAvgSpeed->setVisible(0); //GPS average speed label
    ui->labelDistance->setVisible(0); //GPS distance label
    ui->labelSignalStrength->setText(""); //GPS signal strength label
    timerRoutePicture = new QTimer();
    timerRoutePicture->setInterval(400);
    connect(timerRoutePicture, SIGNAL(timeout()),this, SLOT(timerRoutePictureTimeout()));

    //GPS
    location = new Maemo5Location(this);
    gpsData = new GPSData(location);
    connect(location,SIGNAL(agnss()),this,SLOT(gpsStatus()));
}

/**
  *Destructor of this class. Deletes all dynamic objects and sets them to NULL.
  */
RouteSaveDialog::~RouteSaveDialog()
{
    qDebug() << "__~RouteSaveDialog";
    if(ui)
        delete ui;
    if(gpsData)
        delete gpsData;
    if(location)
        delete location;
    if(routeDialog)
        delete routeDialog;

    delete timerSatellitePicture;
    delete timerRoutePicture;
    delete pixmapRouteStop;
    delete pixmapRouteStart;
    delete iconRouteStop;
    delete iconRouteStart;
}

/**
  *
  */
void RouteSaveDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  *This slot function is called when route start/stop button clicked.
  */
void RouteSaveDialog::on_buttonRouteStartStop_clicked()
{
    if ( buttonStatus == true )//If start button clicked
    {
        qDebug() << "__start button clicked";

        //Clear variables
        averageSpeed = 0.0;
        speed = 0.0;
        allSpeeds = 0.0;
        speedCount = 1;

        buttonStatus = false;
        ui->buttonRouteStartStop->setIcon(*iconRouteStop);
        location->startPollingGPS();
        gpsStatus();
    }
    else //If stop button clicked
    {
        qDebug() << "__stop button clicked";
        buttonStatus = true;
        ui->buttonRouteStartStop->setIcon(*iconRouteStart);

        //Satellite picture and label
        ui->labelRouteSatelliteStatus->setText("Searching satellite");
        ui->labelRouteSatelliteStatus->setVisible(0);
        ui->labelRouteSatellitePicture->setVisible(0);
        timerSatellitePicture->stop();

        //Route picture and label
        ui->labelRouteStatus->setVisible(0);
        ui->labelRoutePicture->setVisible(0);
        timerRoutePicture->stop();
        location->stopPollingGPS();

        QString routeFile = QString("routetemp.xml");
        if (routeDialog->readRouteFromFile( routeFile ) == true)
        {
            if(!routeDialog)
                routeDialog = new RouteDialog(this);
            connect(routeDialog, SIGNAL(sendroute()), this, SLOT(sendRoute()));
            routeDialog->show();
        }

        //Set GPS speed labels in visible
        ui->labelGpsSpeed->setVisible(0);
        ui->labelGpsAvgSpeed->setVisible(0);

        //GPS distance label
        ui->labelDistance->setVisible(0);

        //Stop route recording
        gpsData->stopRouteRecording();

        //User info label
        ui->labelUserInfo->setText("Push start button");
    }
}

/**
  *This slot function is called when satellite picture timer timeout(400ms).
  */
void RouteSaveDialog::timerSatellitePictureTimeout()
{
    //If satellite picture visible.
    if (ui->labelRouteSatellitePicture->isVisible() == 1)
    {
        ui->labelRouteSatelliteStatus->setVisible(0);
        ui->labelRouteSatellitePicture->setVisible(0);
    }
    else
    {
        ui->labelRouteSatelliteStatus->setVisible(1);
        ui->labelRouteSatellitePicture->setVisible(1);
    }
    timerSatellitePicture->start();
}

/**
  *This slot function is called when route picture timer timeout(400ms).
  */
void RouteSaveDialog::timerRoutePictureTimeout()
{
    //If route picture visible.
    if (ui->labelRoutePicture->isVisible() == 1)
    {
        ui->labelRouteStatus->setVisible(0);
        ui->labelRoutePicture->setVisible(0);
    }
    else
    {
        ui->labelRouteStatus->setVisible(1);
        ui->labelRoutePicture->setVisible(1);
    }
    timerRoutePicture->start();
}

/**
  * This slot function is called when GPS update location.
  */
void RouteSaveDialog::gpsStatus()
{
    //IF GPS start button clicked
    if (buttonStatus == false)
    {
        //ui->labelSignalStrength->setText(QString::number(location->getSignalStrength()));    //Returns average signal strength of satellites which are in use.

        //If GPS find 4 or more satellite and signal stregth is 30 or more.
        if (location->getSatellitesInUse() >= 4 && location->getSignalStrength() >= 30)
        {
            //Satellite picture and label
            ui->labelRouteSatelliteStatus->setText("GPS Ready");
            ui->labelRouteSatelliteStatus->setVisible(1);
            ui->labelRouteSatellitePicture->setVisible(1);
            timerSatellitePicture->stop();

            //Route picture and label
            ui->labelRouteStatus->setText("Recorded " + QString::number(gpsData->roundCounter) + " route point");
            ui->labelUserInfo->setText("Recorded " + QString::number(gpsData->roundCounter) + " route point");
            ui->labelRouteStatus->setVisible(1);
            ui->labelRoutePicture->setVisible(1);
            timerRoutePicture->start();

            //Get GPS speed
            speed = location->getSpeed();

            //Set GPS speed
            gpsSpeed.sprintf("%.0f", speed);
            ui->labelGpsSpeed->setText(gpsSpeed + " km/h");
            ui->labelGpsSpeed->setVisible(1);

            //Set GPS average speed
            allSpeeds += speed;
            averageSpeed = allSpeeds/speedCount;
            gpsSpeed.sprintf("%.0f",averageSpeed);
            ui->labelGpsAvgSpeed->setText("Average: " + gpsSpeed + " km/h");
            ui->labelGpsAvgSpeed->setVisible(1);
            speedCount++;

            //Set distance traveled.
            distanceString.sprintf("%.3f", gpsData->getDistanceTraveled());
            ui->labelDistance->setText(distanceString + " km");
            ui->labelDistance->setVisible(1);

            //Start route recording
            gpsData->startRouteRecording();
        }
        else //If GPS find less than 4 satellite or signal strength is poor.
        {
            //Satellite picture and label
            ui->labelRouteSatelliteStatus->setText("Searching satellite");
            ui->labelUserInfo->setText("Searching satellite");
            ui->labelRouteSatelliteStatus->setVisible(1);
            ui->labelRouteSatellitePicture->setVisible(1);
            timerSatellitePicture->start();

            //Route picture and label
            ui->labelRouteStatus->setVisible(0);
            ui->labelRoutePicture->setVisible(0);
            timerRoutePicture->stop();

            //Set GPS speed labels in visible
            ui->labelGpsSpeed->setVisible(0);
            ui->labelGpsAvgSpeed->setVisible(0);

            //GPS distance label
            ui->labelDistance->setVisible(0);
        }
    }
    else //If stop button clicked
    {
        //Satellite picture and label
        ui->labelRouteSatelliteStatus->setText("Searching satellite");
        ui->labelUserInfo->setText("Push start button");
        ui->labelRouteSatelliteStatus->setVisible(0);
        ui->labelRouteSatellitePicture->setVisible(0);
        timerSatellitePicture->stop();

        //Route picture and label
        ui->labelRouteStatus->setVisible(0);
        ui->labelRoutePicture->setVisible(0);
        timerRoutePicture->stop();

        //Set GPS speed labels in visible
        ui->labelGpsSpeed->setVisible(0);
        ui->labelGpsAvgSpeed->setVisible(0);

        //GPS distance label
        ui->labelDistance->setVisible(0);
    }
}

/**
  * This slot function is called when routeDialog emit sendroute (sendPushButton).
  */
void RouteSaveDialog::sendRoute()
{
    emit sendroute(); //Emit mainwindow clientSendRoute
}

/**
  * This slot function called when ever info button clicked.
  */
void RouteSaveDialog::on_pushButtonInfo_clicked()
{
    if(!helpRoutingDialog)
    {
        helpRoutingDialog = new HelpRoutingDialog;
    }
    connect(helpRoutingDialog, SIGNAL(rejected()), this, SLOT(killHelpDialog()));
    helpRoutingDialog->show();
}

/**
  * This slot function called when ever dialog rejected.
  */
void RouteSaveDialog::killHelpDialog()
{
    if(helpRoutingDialog)
    {
        qDebug() << "__Route save kill: helpRoutingDialog";
        delete helpRoutingDialog;
        helpRoutingDialog = NULL;
    }
}

/**
  * This function return speed average.
  * @return double average speed
  */
double RouteSaveDialog::getAverageSpeed()
{
    return averageSpeed;
}

/**
  * This function return distance traveled in QString.
  * @return QString distance traveled
  */
QString RouteSaveDialog::getDistanceTraveled()
{
    return distanceString;
}
