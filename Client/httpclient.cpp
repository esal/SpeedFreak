/*
 * Http client Connects application to server.
 *
 * @author      Tiina Kivilinna-Korhola <tiina.kivilinna-korhola@fudeco.com>
 * @author      Olavi Pulkkinen <olavi.pulkkinen@fudeco.com>
 * @author      Toni Jussila 	<toni.jussila@fudeco.com>
 * @copyright   (c) 2010 Speed Freak team
 * @license     http://opensource.org/licenses/gpl-license.php GNU Public License
 */

#include <QString>
#include <QMessageBox>
#include "httpclient.h"
#include "mainwindow.h"

/**
  *@brief Constructor, connects object to GUI
  *@param Pointer to carmainwindow, which is temporarily used during development
  */
HttpClient::HttpClient(MainWindow *myCarw)
{
    qDebug() << "__HttpClient";
    myMainw = myCarw;
    netManager = new QNetworkAccessManager();
    myXmlwriter = new XmlWriter();
    myXmlreader = new XmlReader();
}

/**
  *@brief Destructor
  */
HttpClient::~HttpClient()
{
    qDebug() << "__~HttpClient" ;

    if(myXmlwriter)
        delete myXmlwriter;
    if(myXmlreader)
        delete myXmlreader;
}

/**
  *@brief Sends registration information to the server in xml format.
  *Reads user name, password and emaol address from resuldialogs internal variables.
  */
void HttpClient::requestRegistration()
{
    qDebug() << "_requestRegistration" ;
    qDebug() <<  myMainw->settingsDialog->getRegUserName() << "+" <<  myMainw->settingsDialog->getRegPassword() << "+" <<  myMainw->settingsDialog->getRegEmail();

    QBuffer *regbuffer = new QBuffer();
    QUrl qurl("http://api.speedfreak-app.com/api/register");
    QNetworkRequest request(qurl);
    qDebug() << qurl.toString();
    QNetworkReply *currentDownload;

    regbuffer->open(QBuffer::ReadWrite);
    myXmlwriter->writeRegistering(regbuffer,
                       myMainw->settingsDialog->getRegUserName(),
                       myMainw->settingsDialog->getRegPassword(),
                       myMainw->settingsDialog->getRegEmail());
    qDebug() << "carmainwindow: regbuffer->data(): " << regbuffer->data();

    currentDownload = netManager->post(request, ("xml=" + regbuffer->data()));
    connect(currentDownload,SIGNAL(finished()),this,SLOT(ackOfRegistration()));
    //connect(currentDownload,SIGNAL(error(QNetworkReply::NetworkError)),myMainw,SLOT(errorFromServer(QNetworkReply::NetworkError)));

    //Indicating user
    if(myMainw->settingsDialog)
        myMainw->settingsDialog->setLabelInfoToUser("Reguesting registration from server");

    regbuffer->close();
}

/**
  *@brief Sends result(s) to the server in xml format.
  *Send authentication information in the header.
  */
void HttpClient::sendResultXml(QString category, double result)
{
    qDebug() << "_sendResultXml";

    QBuffer *xmlbuffer = new QBuffer();

    QUrl qurl("http://api.speedfreak-app.com/api/update/" + category);
    qDebug() << qurl.toString();
    QNetworkRequest request(qurl);
    QNetworkReply *currentDownload;

    xmlbuffer->open(QBuffer::ReadWrite);
    myXmlwriter->writeResult(xmlbuffer, result);
    qDebug() << "carmainwindow: xmlbuffer->data(): " << xmlbuffer->data();

    QString credentials = myMainw->settingsDialog->getUserName() + ":" + myMainw->settingsDialog->getPassword();
    credentials = "Basic " + credentials.toAscii().toBase64();
    request.setRawHeader(QByteArray("Authorization"),credentials.toAscii());

    currentDownload = netManager->post(request, ("xml=" + xmlbuffer->data()));
    connect(currentDownload,SIGNAL(finished()),this,SLOT(ackOfResult()));
    //connect(currentDownload,SIGNAL(error(QNetworkReply::NetworkError)),myMainw,SLOT(errorFromServer(QNetworkReply::NetworkError)));

    //Indicating user
    if(myMainw->accstart->accRealTimeDialog->resultDialog)
        myMainw->accstart->accRealTimeDialog->resultDialog->setLabelInfoToUser("Sending result to server");

    xmlbuffer->close();
}

/**
  *@brief Sends route to the server in xml format.
  *Send authentication information in the header.
  *@todo Check destination URL.
  */
void HttpClient::sendRouteXml()
{
    qDebug() << "_sendRouteXml";

    QString filename = "route.xml";
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "_sendRouteXml file.open() fail";
        return;
    }

    QUrl qurl("http://api.speedfreak-app.com/api/update/route");
    qDebug() << qurl.toString();
    QNetworkRequest request(qurl);
    QNetworkReply *currentDownload;

    QString credentials = myMainw->settingsDialog->getUserName() + ":" + myMainw->settingsDialog->getPassword();
    credentials = "Basic " + credentials.toAscii().toBase64();
    request.setRawHeader(QByteArray("Authorization"),credentials.toAscii());

    currentDownload = netManager->post(request, ("xml=" + file.readAll()));
    connect(currentDownload,SIGNAL(finished()),this,SLOT(ackOfRoute()));
    //connect(currentDownload,SIGNAL(error(QNetworkReply::NetworkError)),myMainw,SLOT(errorFromServer(QNetworkReply::NetworkError)));

    //Indicating user
    if(myMainw->routeSaveDialog->routeDialog)
        myMainw->routeSaveDialog->routeDialog->setLabelInfoToUser("Sending route to server");

    file.close();
}

/**
  *@brief Request the Top10List of certain category from the server.
  *Send authentication information in the header.
  *@param Category of results.
  *@param Limit, the number of results.
  */
void HttpClient::requestTopList(QString category, QString limit)
{
    qDebug() << "_requestTopList";
    qDebug() << category;

    QString urlBase = "http://api.speedfreak-app.com/api/results/";
    QUrl qurl(urlBase + category + "/" + limit);
    qDebug() << qurl.toString();
    QNetworkRequest request(qurl);
    QNetworkReply *currentDownload;

    QString credentials = myMainw->settingsDialog->getUserName() + ":" + myMainw->settingsDialog->getPassword();
    credentials = "Basic " + credentials.toAscii().toBase64();
    request.setRawHeader(QByteArray("Authorization"),credentials.toAscii());

    currentDownload = netManager->post(request, ("data=" ));
    connect(currentDownload,SIGNAL(finished()),this,SLOT(ackOfToplist()));
    //connect(currentDownload,SIGNAL(error(QNetworkReply::NetworkError)),myMainw,SLOT(errorFromServer(QNetworkReply::NetworkError)));

    //Indicating user
    if(myMainw->topResultDialog)
        myMainw->topResultDialog->setLabelInfoToUser("Reguesting top10 list from server");
}

/**
  *@brief Request categories list from the server.
  *Send authentication information in the header.
  */
void HttpClient::requestCategories()
{
    qDebug() << "_requestCategories" ;

    QUrl qurl("http://api.speedfreak-app.com/api/categories/");
    qDebug() << qurl.toString();
    QNetworkRequest request(qurl);
    QNetworkReply *currentDownload;

    QString credentials = myMainw->settingsDialog->getUserName() + ":" + myMainw->settingsDialog->getPassword();
    credentials = "Basic " + credentials.toAscii().toBase64();
    request.setRawHeader(QByteArray("Authorization"),credentials.toAscii());

    currentDownload = netManager->post(request, ("data=" ));
    connect(currentDownload,SIGNAL(finished()),this,SLOT(ackOfCategories()));
    //connect(currentDownload,SIGNAL(error(QNetworkReply::NetworkError)),myMainw,SLOT(errorFromServer(QNetworkReply::NetworkError)));

    //Indicating user
    if(myMainw->topResultDialog)
        myMainw->topResultDialog->setLabelInfoToUser("Reguesting categories from server");
}

/**
  *@brief Check that username and password exist on the server.
  *Send authentication information in the header.
  */
void HttpClient::checkLogin()
{
    qDebug() << "_checkLogin";

    QUrl qurl("http://api.speedfreak-app.com/api/login/");
    qDebug() << qurl.toString();
    QNetworkRequest request(qurl);
    QNetworkReply *currentDownload;

    QString credentials = myMainw->settingsDialog->getUserName() + ":" + myMainw->settingsDialog->getPassword();
    credentials = "Basic " + credentials.toAscii().toBase64();
    request.setRawHeader(QByteArray("Authorization"),credentials.toAscii());

    currentDownload = netManager->post(request, ("data=" ));
    connect(currentDownload,SIGNAL(finished()),this,SLOT(ackOfLogin()));
    //connect(currentDownload,SIGNAL(error(QNetworkReply::NetworkError)),myMainw,SLOT(errorFromServer(QNetworkReply::NetworkError)));

    //Indicating user
    if(myMainw->settingsDialog)
        myMainw->settingsDialog->setLabelInfoToUser("Checking login validity from server");
}

/**
  *@brief React to servers responce after result has been sent.
  */
void HttpClient::ackOfResult()
{
    qDebug() << "_ackOfResult";

    //Indicating user
    if(myMainw->accstart->accRealTimeDialog->resultDialog)
        myMainw->accstart->accRealTimeDialog->resultDialog->setLabelInfoToUser("");

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();

        //Indicating user
        if(myMainw->accstart->accRealTimeDialog->resultDialog)
            QMessageBox::about(myMainw->accstart->accRealTimeDialog->resultDialog, "Server reply to result sending ",reply->errorString());
        if(myMainw->accstart->accRealTimeDialog->resultDialog)
            myMainw->accstart->accRealTimeDialog->resultDialog->setLabelInfoToUser("Error");
        if(myMainw->accstart->accRealTimeDialog->resultDialog)
            myMainw->accstart->accRealTimeDialog->resultDialog->setSendServerButtonEnabled();
    }
    else {
        qDebug() << "errorcode:" << errorcode << reply->errorString();

        //Indicating user
        if(myMainw->accstart->accRealTimeDialog->resultDialog)
            QMessageBox::about(myMainw->accstart->accRealTimeDialog->resultDialog, "Server reply to result sending", "Result received " + reply->readAll());
        if(myMainw->accstart->accRealTimeDialog->resultDialog)
            myMainw->accstart->accRealTimeDialog->resultDialog->setLabelInfoToUser("Result received");
    }
}

/**
  *@brief React to servers responce after route has been sent.
  */
void HttpClient::ackOfRoute()
{
    qDebug() << "_ackOfRoute";

    if(myMainw->routeSaveDialog->routeDialog)
        myMainw->routeSaveDialog->routeDialog->setLabelInfoToUser("");

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        if(myMainw->routeSaveDialog->routeDialog)
            QMessageBox::about(myMainw->routeSaveDialog->routeDialog, "Server reply to route sending ",reply->errorString());
        if(myMainw->routeSaveDialog->routeDialog)
            myMainw->routeSaveDialog->routeDialog->setSendServerButtonEnabled();
    }
    else {
        qDebug() << "errorcode:" << errorcode << reply->errorString();
        if(myMainw->routeSaveDialog->routeDialog)
            QMessageBox::about(myMainw->routeSaveDialog->routeDialog, "Server reply to route sending", "Route received " + reply->readAll());
    }
}

/**
  *@brief React to servers responce after registration has been sent.
  *@todo Implement consequencies of reply.
  */
void HttpClient::ackOfRegistration()
{
    qDebug() << "_ackOfRegistration";

    if(myMainw->settingsDialog)
        myMainw->settingsDialog->setLabelInfoToUser("");

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        if(myMainw->settingsDialog)
            QMessageBox::about(myMainw->settingsDialog, "Server reply to registration",reply->readAll());
    }
    else {
        qDebug() << "errorcode=0" << errorcode << reply->errorString();
        if(myMainw->settingsDialog)
        {
            QMessageBox::about(myMainw->settingsDialog, "Server reply to registration", "User registration " + reply->readAll());
            myMainw->settingsDialog->clearRegisterLineEdits();
        }
    }
}

/**
  *@brief React to servers responce after request for categories has been sent.
  */
void HttpClient::ackOfCategories()
{
    qDebug() << "_ackOfCategories";

    if(myMainw->topResultDialog)
        myMainw->topResultDialog->setLabelInfoToUser("");

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    myXmlreader->xmlReadCategories(reply);

    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //QMessageBox::about(myMainw->topResultDialog, "Server reply to requesting categories",reply->errorString());
        if(myMainw->topResultDialog)
            myMainw->topResultDialog->setLabelInfoToUser("You're not logged! Please register or log in.");
    }
    else {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //QMessageBox::about(myMainw->topResultDialog, "Server reply to requesting categories ", "OK");
        if(myMainw->topResultDialog)
            myMainw->topResultDialog->setLabelInfoToUser("");
    }
}

/**
  *@brief React to servers responce after request of TopList in certain category has been sent.
  */
void HttpClient::ackOfLogin()
{
    qDebug() << "_ackOffLogin";

    if(myMainw->settingsDialog)
        myMainw->settingsDialog->setLabelInfoToUser("");

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) 
    {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
	if(myMainw->settingsDialog)
	{
        	QMessageBox::about(myMainw->settingsDialog, "Server does not recognize your username. Please registrate.",reply->errorString());
        	myMainw->settingsDialog->usernameOk(false);
	}
    }
    else 
    {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
	if(myMainw->settingsDialog)
        	QMessageBox::about(myMainw->settingsDialog, "Server reply to login", "User login " + reply->readAll());
        // here signal emit to mainwindow for username setting to main panel
        emit loginOK();
	if( myMainw->settingsDialog)
	{
        	myMainw->settingsDialog->usernameOk(true);
        	myMainw->settingsDialog->close();    
    	}
    }
}

/**
  *@brief Reports errors, when server has sent error signal.
  */
void HttpClient::errorFromServer(QNetworkReply::NetworkError errorcode)
{
    qDebug() << "_errorFromServer";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode;
        //Note that errors are already reported on other each functions for server communication
        //QMessageBox::about(myMainw, "Server reported an error", reply->errorString());
    }
    else {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        qDebug() << reply->readAll();
    }
}

/**
  *@brief React to servers responce after request of TopList in certain category has been sent.
  */
void HttpClient::ackOfToplist()
{
    qDebug() << "_ackOfToplist";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    myXmlreader->xmlReadTop10Results(reply,myMainw->settingsDialog->getUserName());

    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //Indicating user
        if(myMainw->topResultDialog)
        {
            //QMessageBox::about(myMainw->topResultDialog, "Server reply to requesting top 10 list",reply->errorString());
            myMainw->topResultDialog->setLabelInfoToUser("No results ;(");
        }
    }
    else {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //Indicating user
        if(myMainw->topResultDialog)
        {
            //QMessageBox::about(myMainw->topResultDialog, "Server reply to requesting top 10 list", "OK " + reply->readAll());
            myMainw->topResultDialog->setLabelInfoToUser("");
        }
    }
}

/**
  * This function sends profile to the server in xml format.
  * Send authentication information in the header.
  */
void HttpClient::sendProfileXml()
{
    qDebug() << "_sendProfileXml";

    QString userName = myMainw->settingsDialog->getUserName();
    QString filename = userName + "_profile.xml";
    QFile file(filename);
    if (!file.open(QFile::ReadWrite | QFile::Text))
    {
        qDebug() << "_xmlWrite fail";
        return;
    }
    myXmlwriter->writeProfileXmlFile(&file, userName,
            myMainw->settingsDialog->profileDialog->getManufacturer(),
            myMainw->settingsDialog->profileDialog->getType(),
            myMainw->settingsDialog->profileDialog->getModel(),
            myMainw->settingsDialog->profileDialog->getDescription(),
            myMainw->settingsDialog->profileDialog->getPicture());

    //Indicating user
    if(myMainw->settingsDialog->profileDialog)
        myMainw->settingsDialog->profileDialog->setLabelInfoToUser("Profile saved to phone");

    // Send xml to server
    /*QUrl qurl("http://api.speedfreak-app.com/api/profile");
    QNetworkRequest request(qurl);
    qDebug() << qurl.toString();
    QNetworkReply *currentDownload;

    QString credentials = myMainw->settingsDialog->getUserName() + ":" + myMainw->settingsDialog->getPassword();
    credentials = "Basic " + credentials.toAscii().toBase64();
    request.setRawHeader(QByteArray("Authorization"),credentials.toAscii());

    currentDownload = netManager->post(request, ("xml=" + file.readAll()));
    bool error = connect(currentDownload, SIGNAL(finished()), this, SLOT(ackOfProfile()));*/

    file.close();

    // Send picture to server
    /*if(myMainw->settingsDialog->profileDialog->getPicture() != "" && error == false)
    {
        QFile pictureFile( myMainw->settingsDialog->profileDialog->getPicture() );
        if (!pictureFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "__picture read fail";
            return;
        }
        currentDownload = netManager->post(request, pictureFile.readAll());
        connect(currentDownload, SIGNAL(finished()), this, SLOT(ackOfSendingPicture()));
        pictureFile.close();
    }*/
}

/**
  * This slot function react to servers responce after request of profile has been sent.
  */
bool HttpClient::ackOfProfile()
{
    qDebug() << "__ackOfProfile";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //Indicating user
        if(myMainw->settingsDialog->profileDialog)
        {
            //QMessageBox::about(myMainw->settingsDialog->profileDialog, "Server reply to requesting profile",reply->errorString());
            myMainw->settingsDialog->profileDialog->setLabelInfoToUser("Profile save to server - fail");
            return true;
        }
    }
    else {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //Indicating user
        if(myMainw->settingsDialog->profileDialog)
        {
            //QMessageBox::about(myMainw->settingsDialog->profileDialog, "Server reply to requesting profile", "OK " + reply->readAll());
            myMainw->settingsDialog->profileDialog->setLabelInfoToUser("Profile saved to server");
            return false;
        }
    }
}
/**
  * This slot function react to servers responce after request of picture has been sent.
  */
void HttpClient::ackOfSendingPicture()
{
    qDebug() << "__ackOfSendingPicture";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QNetworkReply::NetworkError errorcode;
    errorcode = reply->error();
    if(errorcode != 0) {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //Indicating user
        if(myMainw->settingsDialog->profileDialog)
        {
            //QMessageBox::about(myMainw->settingsDialog->profileDialog, "Server reply to requesting picture",reply->errorString());
            myMainw->settingsDialog->profileDialog->setLabelInfoToUser("Picture save to server - fail");
        }
    }
    else {
        qDebug() <<  "errorcode:" << errorcode << reply->errorString();
        //Indicating user
        if(myMainw->settingsDialog->profileDialog)
        {
            //QMessageBox::about(myMainw->settingsDialog->profileDialog, "Server reply to requesting picture", "OK " + reply->readAll());
            myMainw->settingsDialog->profileDialog->setLabelInfoToUser("Picture saved to server");
        }
    }
}
