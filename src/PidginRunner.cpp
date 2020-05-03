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

PidginRunner::PidginRunner(QObject *parent, const QVariantList &args)
    : Plasma::AbstractRunner(parent, args)
{
    Q_UNUSED(args)
    setObjectName("Pidgin Buddy Finder");
    setSpeed(AbstractRunner::NormalSpeed);
}

PidginRunner::~PidginRunner() = default;

static const QString CATEGORY = "Pidgin";

void
PidginRunner::match(Plasma::RunnerContext &context)
{
    QString query = context.query();
    if (!query.isEmpty()) {

        // filters all buddies from pidgin
        QList<std::shared_ptr<Buddy>> buddies = pidgin_d.search(query);

        QList<Plasma::QueryMatch> matches;
        matches.reserve(buddies.size());

        for (auto &buddy : buddies) {
            Plasma::QueryMatch match(this);
            match.setText(buddy->alias);
            match.setSubtext(buddy->status);
            match.setData(buddy->id);
            match.setMatchCategory(CATEGORY);
            match.setIconName(buddy->icon);
            matches.append(match);
            if (query.compare(buddy->alias, Qt::CaseSensitivity::CaseInsensitive) == 0) {
                match.setType(Plasma::QueryMatch::ExactMatch);
            }
            else {
                match.setType(Plasma::QueryMatch::PossibleMatch);
            }
        }
        context.addMatches(matches);
    }
}

void
PidginRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(context)
    pidgin_d.startChat(match.data().toInt());
}

K_EXPORT_PLASMA_RUNNER(pidgin_runner, PidginRunner)

#include "PidginRunner.moc"