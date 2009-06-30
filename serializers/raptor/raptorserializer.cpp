/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
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

#include "raptorserializer.h"
#include "../../backends/redland/redlandworld.h"
#include "../../backends/redland/redlandstatementiterator.h"

#include "statementiterator.h"
#include "statement.h"

#include <redland.h>

#include <QtCore/QUrl>
#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>


Q_EXPORT_PLUGIN2(soprano_raptorserializer, Soprano::Raptor::Serializer)


Soprano::Raptor::Serializer::Serializer()
    : QObject(),
      Soprano::Serializer( "raptor" )
{
}


Soprano::Raptor::Serializer::~Serializer()
{
}


Soprano::RdfSerializations Soprano::Raptor::Serializer::supportedSerializations() const
{
    return SerializationRdfXml|SerializationNTriples|SerializationTurtle|SerializationUser;
}


QStringList Soprano::Raptor::Serializer::supportedUserSerializations() const
{
    QStringList sl;
    int i = 0;
    const char* name = 0;
    const char* label = 0;
    const char* mimeType = 0;
    const unsigned char* uri = 0;
    while ( !raptor_serializers_enumerate( i,
                                           &name,
                                           &label,
                                           &mimeType,
                                           &uri ) ) {
        sl << QString::fromUtf8( name );
        ++i;
    }
    return sl;
}


int raptorIOStreamWriteByte( void* data, const int byte )
{
    QTextStream* s = reinterpret_cast<QTextStream*>( data );
    // an int is not a byte. Strange raptor API!
    if( s->device() ) {
        s->device()->putChar( (char)byte );
    }
    else {
        ( *s ) << ( char )byte;
    }
    return 0;
}


int raptorIOStreamWriteBytes( void* data, const void* ptr, size_t size, size_t nmemb )
{
    // the raptor API is very weird. But it seems that ATM raptor only uses size == 1
    QTextStream* s = reinterpret_cast<QTextStream*>( data );
    switch( size ) {
    case 1: {
        const char* p = reinterpret_cast<const char*>( ptr );
        if( s->device() ) {
            s->device()->write( p, nmemb );
        }
        else {
            for ( unsigned int i = 0; i < nmemb; ++i ) {
                raptorIOStreamWriteByte( data, p[i] );
            }
        }
        break;
    }
    default:
        qDebug() << "Unsupported data size: " << size;
        return -1;
    }
    return 0;
}


class StreamData {
public:
    Soprano::StatementIterator it;
    Soprano::Redland::World* world;
    bool initialized;
    bool atEnd;
};

// the raptor API is aweful: it seems that first atEnd is called, then get, and then next until next returns false.
// So we have to call it.next() manually if we don't want to get the first statement twice
int streamIsEnd( void* data )
{
    StreamData* it = reinterpret_cast<StreamData*>( data );
    if ( !it->initialized ) {
        it->initialized = true;
        it->atEnd = !it->it.next();
    }
    return it->atEnd;
}


int streamNext( void* data )
{
    StreamData* it = reinterpret_cast<StreamData*>( data );
    it->atEnd = !it->it.next();
    return it->atEnd;
}


void* streamGet( void* data, int what )
{
    StreamData* it = reinterpret_cast<StreamData*>( data );

    if ( what == 0 ) {
        // statement (stupid librdf does not export it)
        return it->world->createStatement( it->it.current() );
    }
    else {
        // context
        return it->world->createNode( it->it.current().context() );
    }
}


void streamFinished( void* )
{}


bool Soprano::Raptor::Serializer::serialize( StatementIterator it,
                                             QTextStream& stream,
                                             RdfSerialization serialization,
                                             const QString& userSerialization ) const
{
    clearError();

    Redland::World world;

    librdf_serializer* serializer = 0;
    if ( serialization == SerializationRdfXml ) {
        serializer = librdf_new_serializer( world.worldPtr(),
                                            "rdfxml-abbrev", // we always want the abbreviated xmlrdf
                                            0,
                                            0 );
    }
    else if ( serialization == SerializationUser ) {
        serializer = librdf_new_serializer( world.worldPtr(),
                                            userSerialization.toLatin1().data(),
                                            0,
                                            0 );
    }
    else {
        serializer = librdf_new_serializer( world.worldPtr(),
                                            0, // all factories
                                            serializationMimeType( serialization, userSerialization ).toLatin1().data(),
                                            0 );
    }

    if ( !serializer ) {
        qDebug() << "(Soprano::Raptor::Serializer) no serializer for mimetype " << serializationMimeType( serialization, userSerialization );
        setError( world.lastError() );
        return false;
    }

    // add prefixes
    QHash<QString, QUrl> namespaces = prefixes();
    for ( QHash<QString, QUrl>::const_iterator pfit = namespaces.constBegin();
          pfit != namespaces.constEnd(); ++pfit ) {
        librdf_serializer_set_namespace( serializer,
                                         librdf_new_uri( world.worldPtr(), reinterpret_cast<unsigned char*>( pfit.value().toEncoded().data() ) ),
                                         pfit.key().toLatin1().data() );
    }

    bool success = true;

#ifdef HAVE_IOSTREAM_HANDLER2
    raptor_iostream_handler2 raptorStreamHandler = {
        2,
        0,
        0,
        raptorIOStreamWriteByte,
        raptorIOStreamWriteBytes,
        0,
        0,
        0
    };
    raptor_iostream* raptorStream = raptor_new_iostream_from_handler2( &stream,
                                                                       &raptorStreamHandler );
#else
    raptor_iostream_handler raptorStreamHandler = {
        0,
        0,
        raptorIOStreamWriteByte,
        raptorIOStreamWriteBytes,
        0
    };
    raptor_iostream* raptorStream = raptor_new_iostream_from_handler( &stream,
                                                                      &raptorStreamHandler );
#endif

    if ( !raptorStream ) {
        qDebug() << "(Soprano::Raptor::Serializer) failed to create Raptor stream.";
        librdf_free_serializer( serializer );
        setError( world.lastError() );
        return false;
    }

    StreamData streamData;
    streamData.it = it;
    streamData.atEnd = false;
    streamData.initialized = false;
    streamData.world = &world;
    librdf_stream* rdfStream = librdf_new_stream( world.worldPtr(),
                                                  &streamData,
                                                  streamIsEnd,
                                                  streamNext,
                                                  streamGet,
                                                  streamFinished );

    if ( !rdfStream ) {
        qDebug() << "(Soprano::Raptor::Serializer) failed to create librdf stream.";
        raptor_free_iostream( raptorStream );
        setError( world.lastError() );
        return false;
    }

    if ( librdf_serializer_serialize_stream_to_iostream( serializer,
                                                         0,
                                                         rdfStream,
                                                         raptorStream ) ) {
        qDebug() << "(Soprano::Raptor::Serializer) serialization failed.";
        setError( world.lastError() );
        success = false;
    }

    librdf_free_stream( rdfStream );
    librdf_free_serializer( serializer );

    return success;
}

#include "raptorserializer.moc"
