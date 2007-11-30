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

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtNetwork/QHostAddress>

#include "error.h"
#include "backend.h"
#include "soprano_export.h"

// FIXME: add signals for conection and disconnection, maybe even make it async as Qt's network stuff itself

namespace Soprano {

    class Model;

    namespace Client {
        /**
         * \class TcpClient tcpclient.h Soprano/Client/TcpClient
         *
         * \brief Creates a connection to the %Soprano server.
         *
         * The %Soprano server can be accessed normally through PluginManager::discoverBackendByName()
         * with name "sopranoserver" or by simply creating an instance of TcpClient. The latter solution can have advantages
         * as it uses a new connection to the server.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_CLIENT_EXPORT TcpClient : public QObject, public Error::ErrorCache
        {
            Q_OBJECT

        public:
            /**
             * Create a new Client instance.
             */
            TcpClient( QObject* parent = 0 );

            /**
             * Destructor.
             */
            virtual ~TcpClient();

            /**
             * The default %Soprano server connection port.
             */
            static const quint16 DEFAULT_PORT;

            /**
             * Tries to connect to the %Soprano server.
             *
             * \return \p true on success, \p false if an error occured.
             * Check lastError() for details.
             */
            bool connect( const QHostAddress& address = QHostAddress::LocalHost, int port = DEFAULT_PORT );

            /**
             * Check if the client is connected to a server.
             *
             * \return \p true if this client is connected to a server, \p false
             * otherwise.
             */
            bool isConnected();

            /**
             * Disconnect from the server. The created model instances are not
             * deleted but remain useless; open iterators are closed.
             */
            void disconnect();

            /**
             * Creates a new Model instance that wraps a server model.
             * %Client models are very light wrappers and creating them is
             * very fast.
             *
             * \param name The name of the model to access.
             * \param settings The settings to send to the server for creating a new model.
             * These settings may be ignored by the server if a model with that name has
             * already been created.
             *
             * \return A new Model instance wrapping the requested server
             * model or 0 on error (check lastError() for details.)
             */
            Model* createModel( const QString& name, const QList<BackendSetting>& settings = QList<BackendSetting>() );

            /**
             * Get a list of all models that the server currently holds.
             * This method may return an empty list before any call to 
             * createModel().
             *
             * \return A list of the names of all models that are currently
             * opened by the server.
             */
//        QStringList models() const;

        private Q_SLOTS:
            void slotError( QAbstractSocket::SocketError error );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
