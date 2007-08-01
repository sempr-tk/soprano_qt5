/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#ifndef THREE_STORE_QUERY_RESULT_H
#define THREE_STORE_QUERY_RESULT_H

#include "queryresult.h"

extern "C" {
#include <rasqal.h>
#include <3store3/datatypes.h>
}


namespace Soprano {

    class Model;
    class Node;
    class Statement;

    namespace ThreeStore {

	class QueryResult : public Soprano::QueryResult
	{
	public:
	    QueryResult( ts_result* );
	    ~QueryResult();

	    bool next();

	    Statement currentStatement() const;

	    Node binding( const QString &name ) const;

	    Node binding( int offset ) const;

	    int bindingCount() const;

	    QStringList bindingNames() const;

	    bool isGraph() const;

	    bool isBinding() const;

	    bool isBool() const;

	    bool boolValue() const;

	    Model* model() const;

	private:
	    class Private;
	    Private* d;
	};

    }
}

#endif
