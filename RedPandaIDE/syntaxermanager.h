/*
 * Copyright (C) 2020-2022 Roy Qu (royqh1979@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef SYNTAXERMANAGER_H
#define SYNTAXERMANAGER_H
#include "qsynedit/syntaxer/syntaxer.h"

class SyntaxerManager
{
public:
    SyntaxerManager();

    QSynedit::PSyntaxer getSyntaxer(QSynedit::ProgrammingLanguage language);
    QSynedit::PSyntaxer getSyntaxer(const QString& filename);
    QSynedit::PSyntaxer copy(QSynedit::PSyntaxer syntaxer);
    void applyColorScheme(QSynedit::PSyntaxer syntaxer, const QString& schemeName);
};

extern SyntaxerManager syntaxerManager;

#endif // SYNTAXERMANAGER_H
