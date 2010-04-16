/*
 * SettingsDialog class
 *
 * @author     Olavi Pulkkinen <olavi.pulkkinen@fudeco.com>
 * @copyright  (c) 2010 Speed Freak team
 * @license    http://opensource.org/licenses/gpl-license.php GNU Public License
 */

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "usersettings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");
    this->ui->regEMailLineEdit->setText("@");

    if (loginSaved())
    {
        QString uName, pWord;

        getLoginInfo( &uName, &pWord);
        this->username = uName;
        this->password = pWord;

        ui->setUserPasswordLineEdit->setText(this->password);
        ui->setUserUsernameLineEdit->setText(this->username);

        // Already someone as user - change button text to "Change"
        ui->setUserPushButton->setText("Change user");
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
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

//
// Registrate
//
void SettingsDialog::on_registratePushButton_clicked()
{
    // Send username, password and email to SpeedFreak server
    this->regUsername = ui->regUserNameLineEdit->text();
    this->regPassword = ui->regPasswordLineEdit->text();
    this->regEmail = ui->regEMailLineEdit->text();

    emit sendregistration();

    //close();      //using close() hides popup-window which reports error from server
}

// Next 6 functions can be removed if Settingsdialog is implemented without
// own copy of username, password & email
void SettingsDialog::setRegUserName(QString username)
{
    this->regUsername = username;
}

void SettingsDialog::setRegPassword(QString password)
{
    this->regPassword = password;
}

void SettingsDialog::setRegEmail(QString email)
{
    this->regEmail = email;
}

QString SettingsDialog::getRegUserName()
{
    return this->regUsername;
}

QString SettingsDialog::getRegPassword()
{
    return this->regPassword;
}

QString SettingsDialog::getRegEmail()
{
    return this->regEmail;
}

//
// Set / Change user
//
void SettingsDialog::on_setUserPushButton_clicked()
{
    this->username = ui->setUserUsernameLineEdit->text();
    this->password = ui->setUserPasswordLineEdit->text();

    // Save these also to usersettings
    saveLogin( this->username, this->password);

    // Set "Set/Change User" button text
    if (this->username.length() > 0)
    {
        ui->setUserPushButton->setText("Change user");
    }
    else
    {   // Username "cleared"
        ui->setUserPushButton->setText("Set user");
    }

    emit userNameChanged();

    //close();  //using close() hides popup-window which reports error from server
}

// Next 4 functions can be removed if Settingsdialog is implemented without
// own copy of username & password
void SettingsDialog::setUserName(QString username)
{
    this->username = username;
}

void SettingsDialog::setPassword(QString password)
{
    this->password = password;
}

QString SettingsDialog::getUserName()
{
    return this->username;
}

QString SettingsDialog::getPassword()
{
    return this->password;
}

void SettingsDialog::setLabelInfoToUser(QString infoText)
{
    this->ui->labelInfoToUser->setText(infoText);
}
