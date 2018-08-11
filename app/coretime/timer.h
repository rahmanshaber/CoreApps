/*
CoreBox is combination of some common desktop apps.

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

#ifndef TIMER_H
#define TIMER_H

#include "alarm.h"
#include "schedulecollection.h"

#include <QObject>
#include <QDateTime>
#include <QList>


class Timer : public QObject
{
    Q_OBJECT

public:
    void StartTimer(Alarm*);
    explicit Timer(QObject *parent = 0,ScheduleCollection* Collection=0);

private:
    Alarm *_CurAlarm;
    ScheduleCollection *_Schedules;

private slots:
    void AlarmCheck();

};

#endif // TIMER_H
