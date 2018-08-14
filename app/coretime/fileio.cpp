/*
CoreBox is combination of some common desktop apps

CoreBox is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see {http://www.gnu.org/licenses/}. */

#include "fileio.h"
#include "schedule.h"
#include "schedulecollection.h"

#include <QFile>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QDateTime>
#include <QTime>
#include <QList>


FileIO::FileIO(QObject *parent) :
    QObject(parent)
{

}

bool FileIO::ExtractAudio()
{
    if(QFile::copy(":/sound/other/sound.ogg", QDir::tempPath()+"/sound.ogg"))
    {
        return true;
    }
    return false;
}

bool FileIO::DelExtracted()
{
    return QFile::remove(QDir::tempPath()+"/sound.ogg");
}

QList<Schedule*> FileIO::LoadConfig()
{
    //Legacy save file check
    if(this->_Settings.value("AlarmCount").isNull())
    {
        return LegacyRead();
    }

    QList<Schedule*> scheduleList;
    QString indexStr;

    for(int index=0;index<this->_Settings.value("AlarmCount").toInt();index++)
    {
        Schedule *sched=new Schedule(this);

        indexStr.setNum(index);

        sched->SetTime(this->_Settings.value(indexStr+"Time").toTime());
        if(sched->GetTime().isNull())
        {
            QTime reset;
            reset.setHMS(0,0,0,0);
            sched->SetTime(reset);
        }

        sched->setIsMonEnabled(this->_Settings.value(indexStr+"MonEnabled").toBool());
        sched->setIsTueEnabled(this->_Settings.value(indexStr+"TueEnabled").toBool());
        sched->setIsWedEnabled(this->_Settings.value(indexStr+"WedEnabled").toBool());
        sched->setIsThurEnabled(this->_Settings.value(indexStr+"ThurEnabled").toBool());
        sched->setIsFriEnabled(this->_Settings.value(indexStr+"FriEnabled").toBool());
        sched->setIsSatEnabled(this->_Settings.value(indexStr+"SatEnabled").toBool());
        sched->setIsSunEnabled(this->_Settings.value(indexStr+"SunEnabled").toBool());

        sched->SetCustEnabled(this->_Settings.value(indexStr+"CustEnabled").toBool());
        sched->SetCust(this->_Settings.value(indexStr+"CustDate").toDate());

        sched->SetCustomSoundEnabled(this->_Settings.value(indexStr+"CustomSoundEnabled").toBool());
        sched->SetCustomSound(this->_Settings.value(indexStr+"CustomSound").toString());

        scheduleList.append(sched);
    }
    return scheduleList;
}

bool FileIO::Save(ScheduleCollection *Collection)
{
    try
    {
        QList<Schedule*> SchedList=Collection->GetScheduleList();
        Schedule *currentSche;
        int index=0;

        this->_Settings.setValue("AlarmCount",SchedList.count());
        foreach(currentSche,SchedList)
        {
            QString IndexStr;
            IndexStr.setNum(index);
            this->_Settings.setValue(IndexStr+"MonEnabled",currentSche->isMonEnabled());
            this->_Settings.setValue(IndexStr+"TueEnabled",currentSche->isTueEnabled());
            this->_Settings.setValue(IndexStr+"WedEnabled",currentSche->isWedEnabled());
            this->_Settings.setValue(IndexStr+"ThurEnabled",currentSche->isThurEnabled());
            this->_Settings.setValue(IndexStr+"FriEnabled",currentSche->isFriEnabled());
            this->_Settings.setValue(IndexStr+"SatEnabled",currentSche->isSatEnabled());
            this->_Settings.setValue(IndexStr+"SunEnabled",currentSche->isSunEnabled());
            this->_Settings.setValue(IndexStr+"Time",currentSche->GetTime());
            this->_Settings.setValue(IndexStr+"CustEnabled",currentSche->GetCustomEnabled());
            this->_Settings.setValue(IndexStr+"CustDate",currentSche->GetCustomDate());
            this->_Settings.setValue(IndexStr+"CustomSoundEnabled",currentSche->GetCustomSoundEnabled());
            this->_Settings.setValue(IndexStr+"CustomSound",currentSche->GetCustomSound());
            this->_Settings.sync();
            index++;
        }
    }
    catch(...)
    {
        return false;
    }

    return true;
}


int FileIO::LoadVolume()
{
    QSettings settings;
    return settings.value("Volume").toInt();
}

void FileIO::SaveVolume(int vol)
{
    QSettings settings;
    settings.setValue("Volume",vol);
}

bool FileIO::LoadWindowShow()
{
    QSettings settings;
    return settings.value("ShowWindowDefault").toBool();
}


void FileIO::SaveWindowShow(bool showWindow)
{
    QSettings settings;
    settings.setValue("ShowWindowDefault",showWindow);
}


bool FileIO::isMilTime()
{
    QSettings settings;
    return settings.value("isMilTime").toBool();
}

void FileIO::SaveTimeMode(bool isMilTime)
{
    QSettings settings;
    settings.setValue("isMilTime",isMilTime);
}


bool FileIO::LoadWarnOnPm()
{
    QSettings settings;
    return settings.value("WarnOnPm").toBool();
}

void FileIO::SaveWarnOnPm(bool warn)
{
    QSettings settings;
    settings.setValue("WarnOnPm",warn);
}

//to be removed in future versions
QList<Schedule*> FileIO::LegacyRead()
{
    QList<Schedule*> convertedSche;

    for(int index=0;index<5;index++)
    {

        QString Index;
        Index.setNum(index);
        if(this->_Settings.value(Index+"WDEnabled").toBool())
        {
            Schedule *newSche=new Schedule;
            newSche->setIsMonEnabled(true);
            newSche->setIsTueEnabled(true);
            newSche->setIsWedEnabled(true);
            newSche->setIsThurEnabled(true);
            newSche->setIsFriEnabled(true);

            if(this->_Settings.value(Index+"WDTime").toTime().isNull())
            {
                QTime emptyTime;
                emptyTime.setHMS(0,0,0,0);
                newSche->SetTime(emptyTime);
            }
            else
            {
                newSche->SetTime(this->_Settings.value(Index+"WDTime").toTime());
            }

            if(this->_Settings.value((Index+"CustomSoundEnabled")).isNull()==false)
            {
                newSche->SetCustomSoundEnabled(this->_Settings.value(Index+"CustomSoundEnabled").toBool());
                newSche->SetCustomSound(this->_Settings.value(Index+"CustomSound").toString());
            }
            convertedSche.append(newSche);
        }

        if(this->_Settings.value(Index+"WEEnabled").toBool())
        {
            Schedule *newSche=new Schedule;
            newSche->setIsSatEnabled(true);
            newSche->setIsSunEnabled(true);

            if(this->_Settings.value(Index+"WETime").toTime().isNull())
            {
                QTime emptyTime;
                emptyTime.setHMS(0,0,0,0);
                newSche->SetTime(emptyTime);
            }
            else
            {
                newSche->SetTime(this->_Settings.value(Index+"WETime").toTime());
            }

            if(this->_Settings.value((Index+"CustomSoundEnabled")).isNull()==false)
            {
                newSche->SetCustomSoundEnabled(this->_Settings.value(Index+"CustomSoundEnabled").toBool());
                newSche->SetCustomSound(this->_Settings.value(Index+"CustomSound").toString());
            }
            convertedSche.append(newSche);
        }

        if(this->_Settings.value(Index+"CustEnabled").toBool())
        {
            Schedule *newSche=new Schedule;
            newSche->setIsCustomEnabled(true);
            if(this->_Settings.value(Index+"CustTime").toDateTime().isNull())
            {
                QTime emptyTime;
                emptyTime.setHMS(0,0,0,0);
                newSche->SetTime(emptyTime);
            }else
            {
                QDateTime val=this->_Settings.value(Index+"CustTime").toDateTime();
                newSche->SetTime(val.time());
                newSche->SetCust(val.date());
            }
            if(this->_Settings.value((Index+"CustomSoundEnabled")).isNull()==false)
            {
                newSche->SetCustomSound(this->_Settings.value(Index+"CustomSound").toString());
                newSche->SetCustomSoundEnabled(this->_Settings.value(Index+"CustomSoundEnabled").toBool());
            }
            convertedSche.append(newSche);
        }

    }
    return convertedSche;

}
