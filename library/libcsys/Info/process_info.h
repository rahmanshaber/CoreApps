#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <QObject>

#include <Utils/file_util.h>
#include <Utils/command_util.h>
#include "process.h"

#include "libcsys_global.h"

class LIBCSYSSHARED_EXPORT ProcessInfo : public QObject
{
    Q_OBJECT

public:
    QList<Process> getProcessList() const;

public slots:
    void updateProcesses();

private:
    QList<Process> processList;
};

#endif // PROCESS_INFO_H
