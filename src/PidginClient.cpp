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

#include <QtDBus>

#include "PidginClient.h"

namespace
{
const char *const PIDGIN_SERVICE = "im.pidgin.purple.PurpleService";

const char *const PIDGIN_PATH = "/im/pidgin/purple/PurpleObject";

const char *const PIDGIN_INTERFACE = "im.pidgin.purple.PurpleInterface";

typedef QList<int> IntList;
}

PidginClient::PidginClient()
    :
    pidgin_d(PIDGIN_SERVICE, PIDGIN_PATH, PIDGIN_INTERFACE)
{
    QMutexLocker ml(&mutex);

    IntList accounts = call<IntList>("PurpleAccountsGetAllActive");

    for (auto &accountId : accounts) {
        addBuddies(accountId);
    }

    addListener("BuddyStatusChanged", SLOT(buddyStatusChanged(int, int, int)));
    addListener("BuddySignedOn", SLOT(buddySignedOnOff(int)));
    addListener("BuddySignedOff", SLOT(buddySignedOnOff(int)));
    addListener("BuddyAdded", SLOT(buddyAdded(int)));
    addListener("BuddyRemoved", SLOT(buddyRemoved(int)));
    addListener("BlistNodeAliased", SLOT(nodeAliasChanged(int, const QString &)));
    addListener("AccountSignedOn", SLOT(accountSignedOn(int)));
    addListener("AccountSignedOff", SLOT(accountSignedOff(int)));
}

void PidginClient::addBuddies(const int &accountId)
{
    IntList buddyIds = call<IntList>("PurpleFindBuddies", accountId, "");
    for (auto &buddyId : buddyIds) {
        addBuddy(buddyId, accountId);
    }
}

void PidginClient::addListener(const char *signalName, const char *slot)
{
    pidgin_d.connection().connect(PIDGIN_SERVICE, PIDGIN_PATH, PIDGIN_INTERFACE, signalName, this, slot);
}

void PidginClient::addBuddy(int buddyId, int accountId)
{
    auto buddy = std::make_shared<Buddy>(buddyId);
    buddy->name = call<QString>("PurpleBuddyGetName", buddyId);
    buddy->accountId = accountId;
    buddies[buddyId] = buddy;
    updateAlias(buddyId);
    updateStatus(buddyId);
}

void PidginClient::updateAlias(int buddyId)
{
    QString alias = call<QString>("PurpleBuddyGetAlias", buddyId);
    if (alias.isEmpty()) {
        return;
    }
    auto buddy = this->buddies[buddyId];
    buddy->alias = alias;
}

PidginClient::~PidginClient() = default;

void PidginClient::updateStatus(int buddyId)
{
    int presence = call<int>("PurpleBuddyGetPresence", buddyId);
    int isOnline = call<int>("PurplePresenceIsOnline", presence);
    int isAvailable = call<int>("PurplePresenceIsAvailable", presence);
    int isIdle = call<int>("PurplePresenceIsIdle", presence);

    auto buddy = buddies[buddyId];

    if (isOnline) {
        if (isIdle || !isAvailable) {
            buddy->status = "Away";
            buddy->icon = "user-away";
        }
        else {
            buddy->status = "Available";
            buddy->icon = "user-available";
        }
    }
    else {
        buddy->status = "Offline";
        buddy->icon = "user-offline";
    }
}

void PidginClient::startChat(int buddyId)
{
    auto buddy = buddies.value(buddyId, nullptr);

    if (buddy == nullptr) {
        return;
    }

    int convId = call<int>("PurpleConversationNew",
                           1,
                           buddy->accountId,
                           buddy->name);

    pidgin_d.call("PurpleConversationPresent", convId);
}

QList<std::shared_ptr<Buddy>> PidginClient::search(const QString &alias)
{
    QMutexLocker ml(&mutex);

    const QList<std::shared_ptr<Buddy>> &blist = buddies.values();

    if (alias.isEmpty()) {
        return blist;
    }

    QList<std::shared_ptr<Buddy>> filtered;

    for (auto &buddy : blist) {
        if (buddy->alias.contains(alias, Qt::CaseInsensitive)) {
            filtered.append(buddy);
        }
    }

    return filtered;
}

void PidginClient::buddyStatusChanged(int buddyId, int old_status, int status)
{
    Q_UNUSED(old_status)
    Q_UNUSED(status)

    QMutexLocker ml(&mutex);

    updateStatus(buddyId);
}

void PidginClient::buddySignedOnOff(int buddyId)
{
    QMutexLocker ml(&mutex);

    updateStatus(buddyId);
}

void PidginClient::buddyAdded(int buddyId)
{
    QMutexLocker ml(&mutex);
    int accountId = call<int>("PurpleBuddyGetAccount", buddyId);
    addBuddy(buddyId, accountId);
}

void PidginClient::buddyRemoved(int buddyId)
{
    QMutexLocker ml(&mutex);
    buddies.remove(buddyId);
}

void PidginClient::nodeAliasChanged(int node, const QString &old_alias)
{
    Q_UNUSED(old_alias)

    QMutexLocker ml(&mutex);

    updateAlias(node);
}

void PidginClient::accountSignedOn(int accountId)
{
    QMutexLocker ml(&mutex);

    addBuddies(accountId);
}

void PidginClient::accountSignedOff(int accountId)
{
    QMutexLocker ml(&mutex);

    for (auto it = buddies.begin(); it != buddies.end();) {
        if (it.value()->accountId == accountId) {
            it = buddies.erase(it);
        }
        else {
            it++;
        }
    }
}
