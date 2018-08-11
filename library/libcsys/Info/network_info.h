#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H

#include <QtNetwork/QNetworkInterface>
#include "Utils/file_util.h"
#include "Utils/command_util.h"

#include "libcsys_global.h"

class LIBCSYSSHARED_EXPORT NetworkInfo
{
public:
    NetworkInfo();

    QString getDefaultNetworkInterface() const;
    QList<QNetworkInterface> getAllInterfaces();

    quint64 getRXbytes() const;
    quint64 getTXbytes() const;

private:
    QString defaultNetworkInterface;

    QString rxPath;
    QString txPath;
};

#endif // NETWORK_INFO_H
