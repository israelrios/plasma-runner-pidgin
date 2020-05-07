/*
 *   Copyright (C) 2020 Israel Rios
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PIDGINCLIENT_H
#define PIDGINCLIENT_H

#include <QVariant>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QMutex>
#include <memory>

#include "Buddy.h"

class PidginClient: public QObject
{
Q_OBJECT;
private:
    QDBusInterface pidginInterface;
    QMutex mutex;
    QMap<int, std::shared_ptr<Buddy>> buddies;
    void updateStatus(int);
    void addBuddy(int buddyId, int i);
    void addListener(const char *signalName, const char *slot);
    void updateAlias(int buddyId);
    void addBuddies(const int &accountId);

    //start template
    template<typename T>
    T call(const QString &methodName,
           const QVariant &arg1 = QVariant(),
           const QVariant &arg2 = QVariant(),
           const QVariant &arg3 = QVariant())
    {
        T ret = T();
        if (pidginInterface.isValid()) {
            QDBusReply<T> reply = pidginInterface.call(methodName, arg1, arg2, arg3);
            if (reply.isValid()) {
                ret = reply.value();
            }
            else {
                qWarning() << "Metod name:" << methodName << "reply not valid " << reply.error().message();
            }
        }
        else {
            qWarning() << " IFACE not valid";
        }
        return ret;
    }
    //end template
public:
    PidginClient();
    ~PidginClient() override;
    void startChat(int buddyId);
    QList<std::shared_ptr<Buddy>> search(const QString &alias);
    void showBuddiesList();

public slots:
    void buddyStatusChanged(int buddyId, int old_status, int status);
    void buddySignedOnOff(int buddyId);
    void buddyAdded(int buddyId);
    void buddyRemoved(int buddyId);
    void nodeAliasChanged(int node, const QString &old_alias);
    void accountSignedOn(int accountId);
    void accountSignedOff(int accountId);
};

#endif
