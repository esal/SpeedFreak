/*
 * Calculate class to process accelerometer data
 *
 * @author      Kai Rasilainen
 * @author      Jukka Kurttila <jukka.kurttila@fudeco.com>
 * @copyright   (c) 2010 Speed Freak team
 * @license     http://opensource.org/licenses/gpl-license.php GNU Public License
 */

#ifndef CALCULATE_H
#define CALCULATE_H

#include <QObject>
#include <QTime>
#include <QList>
#include <QMap>

class Calculate : public QObject
{
    Q_OBJECT

public:
    Calculate();
    ~Calculate();

    void reset();
    void calculateParameters(double currentAcceleration, double seconds);
    void accelStoppedCheck(double currentAcceleration);
    QMap<int,double> getValuesMap();

    double getAverageSpeed();
    void setAverageSpeed(double value);

    double getCurrentSpeed();
    void setCurrentSpeed(double value);

    double getDistanceTraveled();
    void setDistanceTraveled(double value);

    double getLastAcceleration();
    void setLastAcceleration(double value);

    double getLastCheckpoint();
    void setLastCheckpoint(double value);

    double getLastDistance();
    void setLastDistance(double value);

    double getLastSpeed();
    void setLastSpeed(double value);

    long getNumOfIterations();
    void setNumOfIterations(long value);

    double getTotalTime();
    void setTotalTime(double value);

    double getCurrentPower();
    void setCurrentPower(double value);

    double getPeakPower();
    void setPeakPower(double value);

    double getAveragePower();
    void setAveragePower(double value);

    double getMaxSpeed();
    void setMaxSpeed(double value);

private:
    double averageSpeed;
    double currentSpeed;
    double maxSpeed;
    double distanceTraveled;
    double lastAcceleration;
    double lastDistance;
    double lastSpeed;
    double checkPoint;
    long numOfIterations;
    double totalTime;
    int count;
    int checkPointCounter;
    double peakPower;
    double currentPower;
    double averagePower;
    QList<int> speedCheckPoints;

    QMap<int,double> valuesMap;

signals:
    void checkPointReached(double totalTime, double currentSpeed);

};

#endif // CALCULATE_H
