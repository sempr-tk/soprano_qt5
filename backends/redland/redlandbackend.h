/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _REDLAND_BACKEND_H_
#define _REDLAND_BACKEND_H_

#include "backend.h"

#include <QtCore/QObject>
#include <QtCore/QMutex>

namespace Soprano
{
    namespace Redland
    {
        class BackendPlugin : public QObject, public Soprano::Backend
        {
            Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	    Q_PLUGIN_METADATA(IID "org.soprano.plugins.Backend/2.1")
#endif
            Q_INTERFACES(Soprano::Backend)

        public:
            BackendPlugin();

            /**
             * Supported user options are:
             * \li name - The name of the RDF storage for persistant storage types. Defaults to "soprano".
             * \li storageType - The storage type, can be one of the redland storage types, defaults to "hashes".
             * \li All supported redland options as can be used with librdf_new_storage(), defaults are:
             *     contexts=yes, new=no. Soprano::BackendOptions Soprano::BackendOptionStorageDir and
             *     Soprano::BackendOptionStorageMemory change the values of redland options "dir" and "has-type".
             */
            StorageModel* createModel( const BackendSettings& settings = BackendSettings() ) const;

            bool deleteModelData( const BackendSettings& settings ) const;

            BackendFeatures supportedFeatures() const;

        private:
            mutable QMutex m_mutex;
        };
    }
}

#endif
