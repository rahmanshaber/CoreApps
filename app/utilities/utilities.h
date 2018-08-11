#ifndef UTILITIES_H
#define UTILITIES_H


#include "settings/settingsmanage.h"
#include "bookmarks/bookmarks.h"
#include "bookmarks/bookmarkmanage.h"
#include "settings/settings.h"
#include "globalfunctions.h"



    void utilities();
    void appEngine(AppsName i, const QString arg);

    QString checkIsValidDir(const QString str);
    QString checkIsValidFile(const QString str);



#endif // UTILITIES_H
