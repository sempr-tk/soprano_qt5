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

#ifndef _SOPRANO_SERVER_CLIENT_H_
#define _SOPRANO_SERVER_CLIENT_H_

#include <soprano/error.h>
#include <soprano/sopranotypes.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtNetwork/QHostAddress>


namespace Soprano {

    class Node;
    class Statement;
    class BindingSet;
    class BackendSetting;
    class QueryLegacy;

    namespace Server {
	class Client : public QObject, public Error::ErrorCache
	{
	    Q_OBJECT

	public:
	    Client( QObject* parent = 0 );
	    ~Client();

	    // FIXME: put the default port in a header file
	    bool open( const QHostAddress& address = QHostAddress::LocalHost, quint16 port = 5000 );
	    bool isOpen();
	    void close();
	    
	    // Backend methods
	    /**
	     * Create a new Model and return its ID.
	     */
	    int createModel( const QList<BackendSetting>& );
	    Soprano::BackendFeatures supportedFeatures();

	    // Model methods
	    Error::ErrorCode addStatement( int modelId, const Statement &statement );
	    int listContexts( int modelId );
	    int executeQuery( int modelId, const QueryLegacy &query );
	    int listStatements( int modelId, const Statement &partial );
	    Error::ErrorCode removeStatements( int modelId, const Statement &statement );
	    int statementCount( int modelId );
	    bool isEmpty( int modelId );
	    bool containsStatements( int modelId, const Statement &statement );

	    // Iterator methods
	    bool iteratorNext( int id );
	    Node nodeIteratorCurrent( int id );
	    Statement statementIteratorCurrent( int id );
	    BindingSet queryIteratorCurrent( int id );
	    Statement queryIteratorCurrentStatement( int id );
	    int queryIteratorType( int id );
	    bool queryIteratorBoolValue( int id );

	    void iteratorClose( int id );

	private Q_SLOTS:
	    void slotError( QAbstractSocket::SocketError error );

	private:
	    class Private;
	    Private* const d;
	};
    }
}

#endif