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

#include <memory>

#include "PidginRunner.h"

namespace
{
const QString CATEGORY = "Pidgin";

const QString SHOW_BLIST = "Show Pidgin Buddies";

const QString PIDGIN_ICON = "pidgin";

const int SHOW_BLIST_ID = -1;
}

PidginRunner::PidginRunner(QObject *parent, const QVariantList &args)
    : Plasma::AbstractRunner(parent, args)
{
    Q_UNUSED(args)
    setObjectName(QLatin1String("Pidgin Buddy Finder"));
    setSpeed(AbstractRunner::NormalSpeed);
}

PidginRunner::~PidginRunner() = default;

void PidginRunner::match(Plasma::RunnerContext &context)
{
    QString query = context.query();
    if (!query.isEmpty()) {

        // filters all buddies from pidgin
        QList<std::shared_ptr<Buddy>> buddies = pidginClient.search(query);

        QList<Plasma::QueryMatch> matches;
        matches.reserve(buddies.size());

        for (auto &buddy : buddies) {
            Plasma::QueryMatch match(this);
            match.setText(buddy->alias);
            match.setSubtext(buddy->status);
            match.setData(buddy->id);
            match.setMatchCategory(CATEGORY);
            match.setIconName(buddy->icon);
            defineMatchType(query, match, buddy->alias);
            matches.append(match);
        }

        if (SHOW_BLIST.contains(query, Qt::CaseSensitivity::CaseInsensitive)) {
            Plasma::QueryMatch match(this);
            match.setText(SHOW_BLIST);
            match.setData(SHOW_BLIST_ID);
            match.setMatchCategory(CATEGORY);
            match.setIconName(PIDGIN_ICON);
            defineMatchType(query, match, SHOW_BLIST);
            matches.append(match);
        }

        context.addMatches(matches);
    }
}

void PidginRunner::defineMatchType(const QString &query, Plasma::QueryMatch &match, const QString &text)
{
    if (query.compare(text, Qt::CaseInsensitive) == 0) {
        match.setType(Plasma::QueryMatch::ExactMatch);
    }
    else {
        match.setType(Plasma::QueryMatch::PossibleMatch);
    }
}

void PidginRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(context)

    int id = match.data().toInt();

    if (id == SHOW_BLIST_ID) {
        pidginClient.showBuddiesList();
    }
    else {
        pidginClient.startChat(id);
    }
}

K_EXPORT_PLASMA_RUNNER(pidgin_runner, PidginRunner)

#include "PidginRunner.moc"