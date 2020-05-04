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

typedef std::pair<QChar, const char *> SPair;

const QMap<QChar, const char *> DIACRITIC_MAP(
    {SPair(u'Š', "S"), SPair(u'Œ', "OE"), SPair(u'Ž', "Z"), SPair(u'š', "s"),
     SPair(u'œ', "oe"), SPair(u'ž', "z"), SPair(u'Ÿ', "Y"), SPair(u'¥', "Y"), SPair(u'µ', "u"),
     SPair(u'À', "A"), SPair(u'Á', "A"), SPair(u'Â', "A"), SPair(u'Ã', "A"), SPair(u'Ä', "A"),
     SPair(u'Å', "A"), SPair(u'Æ', "AE"), SPair(u'Ç', "C"), SPair(u'È', "E"), SPair(u'É', "E"),
     SPair(u'Ê', "E"), SPair(u'Ë', "E"), SPair(u'Ì', "I"), SPair(u'Í', "I"), SPair(u'Î', "I"),
     SPair(u'Ï', "I"), SPair(u'Ð', "D"), SPair(u'Ñ', "N"), SPair(u'Ò', "O"), SPair(u'Ó', "O"),
     SPair(u'Ô', "O"), SPair(u'Õ', "O"), SPair(u'Ö', "O"), SPair(u'Ø', "O"), SPair(u'Ù', "U"),
     SPair(u'Ú', "U"), SPair(u'Û', "U"), SPair(u'Ü', "U"), SPair(u'Ý', "Y"), SPair(u'ß', "s"),
     SPair(u'à', "a"), SPair(u'á', "a"), SPair(u'â', "a"), SPair(u'ã', "a"), SPair(u'ä', "a"),
     SPair(u'å', "a"), SPair(u'æ', "ae"), SPair(u'ç', "c"), SPair(u'è', "e"), SPair(u'é', "e"),
     SPair(u'ê', "e"), SPair(u'ë', "e"), SPair(u'ì', "i"), SPair(u'í', "i"), SPair(u'î', "i"),
     SPair(u'ï', "i"), SPair(u'ð', "o"), SPair(u'ñ', "n"), SPair(u'ò', "o"), SPair(u'ó', "o"),
     SPair(u'ô', "o"), SPair(u'õ', "o"), SPair(u'ö', "o"), SPair(u'ø', "o"), SPair(u'ù', "u"),
     SPair(u'ú', "u"), SPair(u'û', "u"), SPair(u'ü', "u"), SPair(u'ý', "y"), SPair(u'ÿ', "y")}
);
}

PidginClient::PidginClient()
    :
    pidginInterface(PIDGIN_SERVICE, PIDGIN_PATH, PIDGIN_INTERFACE)
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
    pidginInterface.connection().connect(PIDGIN_SERVICE, PIDGIN_PATH, PIDGIN_INTERFACE, signalName, this, slot);
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

QString removeAccents(const QString &s)
{
    QString output;
    output.reserve(s.size());
    for (auto c : s) {
        const char *replacement = DIACRITIC_MAP.value(c, nullptr);
        if (replacement == nullptr) {
            output.append(c);
        }
        else {
            output.append(replacement);
        }
    }

    return output;
}

void PidginClient::updateAlias(int buddyId)
{
    QString alias = call<QString>("PurpleBuddyGetAlias", buddyId);
    auto buddy = buddies.value(buddyId);
    if (alias.isEmpty() || !buddy) {
        return;
    }
    buddy->alias = alias;
    buddy->normalAlias = removeAccents(alias).toLower();
}

PidginClient::~PidginClient() = default;

void PidginClient::updateStatus(int buddyId)
{
    auto buddy = buddies.value(buddyId);

    if (!buddy) {
        return;
    }

    int presence = call<int>("PurpleBuddyGetPresence", buddyId);
    int isOnline = call<int>("PurplePresenceIsOnline", presence);

    if (isOnline) {
        int isAvailable = call<int>("PurplePresenceIsAvailable", presence);
        if (isAvailable) {
            buddy->status = "Available";
            buddy->icon = "user-available";
        }
        else {
            buddy->status = "Away";
            buddy->icon = "user-away";
        }
    }
    else {
        buddy->status = "Offline";
        buddy->icon = "user-offline";
    }
}

void PidginClient::startChat(int buddyId)
{
    QMutexLocker ml(&mutex);

    auto buddy = buddies.value(buddyId);

    if (!buddy) {
        return;
    }

    int convId = call<int>("PurpleConversationNew",
                           1,
                           buddy->accountId,
                           buddy->name);

    pidginInterface.call("PurpleConversationPresent", convId);
}

QList<std::shared_ptr<Buddy>> PidginClient::search(const QString &alias)
{
    QMutexLocker ml(&mutex);

    const QList<std::shared_ptr<Buddy>> &blist = buddies.values();

    if (alias.isEmpty()) {
        return blist;
    }

    QList<std::shared_ptr<Buddy>> filtered;

    const QString &normalAlias = removeAccents(alias).toLower();

    for (auto &buddy : blist) {
        if (buddy->normalAlias.contains(normalAlias)) {
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
