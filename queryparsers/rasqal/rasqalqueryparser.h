/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SOPRANO_QUERY_PARSER_RASQALQUERYPARSER_H
#define SOPRANO_QUERY_PARSER_RASQALQUERYPARSER_H

#include <QtCore/QObject>

#include "query/queryparser.h"

namespace Soprano {

    namespace Rasqal {

        class QueryParser : public QObject, public Soprano::Query::Parser
    {
        Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_PLUGIN_METADATA(IID "org.soprano.plugins.QueryParser/1.0")
#endif
        Q_INTERFACES(Soprano::Query::Parser)

        public:
            QueryParser();
            ~QueryParser();

            virtual Soprano::Query::Query parseQuery( const QString &query, Soprano::Query::QueryLanguage lang, const QString& userQueryLanguage = QString() ) const;

            virtual Soprano::Query::QueryLanguages supportedQueryLanguages() const;

        private:
            static void raptor_message_handler( void *query_parser, raptor_locator *rl, const char *msg );

            void emitSyntaxError( const Soprano::Error::Locator& locator, const QString& message );
        };

    }
}

#endif // SOPRANO_QUERY_PARSER_RASQALQUERYPARSER_H
