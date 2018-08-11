#ifndef PROCESS_H
#define PROCESS_H

#include "Utils/file_util.h"

#include "libcsys_global.h"

class LIBCSYSSHARED_EXPORT Process {

public:
    pid_t getPid() const;
    void setPid(const pid_t &value);

    quint64 getRss() const;
    void setRss(const quint64 &value);

    double getPmem() const;
    void setPmem(const double &value);

    quint64 getVsize() const;
    void setVsize(const quint64 &value);

    QString getUname() const;
    void setUname(const QString &value);

    double getPcpu() const;
    void setPcpu(const double &value);

    QString getStartTime() const;
    void setStartTime(const QString &value);

    QString getState() const;
    void setState(const QString &value);

    QString getGroup() const;
    void setGroup(const QString &value);

    int getNice() const;
    void setNice(const int &value);

    QString getCpuTime() const;
    void setCpuTime(const QString &value);

    QString getSession() const;
    void setSession(const QString &value);

    QString getCmd() const;
    void setCmd(const QString &value);

private:
    pid_t pid;
    quint64 rss;
    double pmem;
    quint64 vsize;
    QString uname;
    double pcpu;
    QString startTime;
    QString state;
    QString group;
    int nice;
    QString cpuTime;
    QString session;
    QString cmd;
};


#endif // PROCESS_H
