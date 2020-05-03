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

#include "PidginClient.h"
#include <QCoreApplication>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    PidginClient p;

    if (argc > 1) {
        QString param = QCoreApplication::arguments().at(1);

        if (!param.isEmpty()) {
            if (param.contains("--search")) {
                QString pattern = QCoreApplication::arguments().at(2);
                for (auto &buddy : p.search(pattern)) {
                    qDebug() << buddy->alias;
                }
                QCoreApplication::exit(0);
            }
            if (param.contains("--startChat")) {
                // first test invalid buddyId
                p.startChat(9999);

                // now find the first one with the pattern supplied
                QString pattern = QCoreApplication::arguments().at(2);
                auto buddy = p.search(pattern).first();
                p.startChat(buddy->id);
            }

        }
        else {
            QCoreApplication::exit(1);
        }
    }
    else {
        return QCoreApplication::exec();
    }
}


