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

#ifndef _SOPRANO_NAO_H_
#define _SOPRANO_NAO_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
        namespace NAO {
            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#
             */
            SOPRANO_EXPORT QUrl naoNamespace();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Party 
             * 
             * Represents a single or a group of individuals 
             */
            SOPRANO_EXPORT QUrl Party();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Symbol 
             * 
             * Represents a symbol 
             */
            SOPRANO_EXPORT QUrl Symbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Tag 
             * 
             * Represents a generic tag 
             */
            SOPRANO_EXPORT QUrl Tag();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel 
             * 
             * An alternative label alongside the preferred label for a resource 
             */
            SOPRANO_EXPORT QUrl altLabel();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol 
             * 
             * An alternative symbol representation for a resource 
             */
            SOPRANO_EXPORT QUrl altSymbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#annotation 
             * 
             * Generic annotation for a resource 
             */
            SOPRANO_EXPORT QUrl annotation();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor 
             * 
             * Refers to a single or a group of individuals that contributed 
             * to a resource 
             */
            SOPRANO_EXPORT QUrl contributor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created 
             * 
             * States the creation, or first modification time for a resource 
             */
            SOPRANO_EXPORT QUrl created();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator 
             * 
             * Refers to the single or group of individuals that created the 
             * resource 
             */
            SOPRANO_EXPORT QUrl creator();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description 
             * 
             * A non-technical textual annotation for a resource 
             */
            SOPRANO_EXPORT QUrl description();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#engineeringTool 
             * 
             * Specifies the engineering tool used to generate the graph 
             */
            SOPRANO_EXPORT QUrl engineeringTool();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasDefaultNamespace 
             * 
             * Defines the default static namespace for a graph 
             */
            SOPRANO_EXPORT QUrl hasDefaultNamespace();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasDefaultNamespaceAbbreviation 
             * 
             * Defines the default static namespace abbreviation for a graph 
             */
            SOPRANO_EXPORT QUrl hasDefaultNamespaceAbbreviation();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol 
             * 
             * Annotation for a resource in the form of a symbol representation 
             */
            SOPRANO_EXPORT QUrl hasSymbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag 
             * 
             * Defines an existing tag for a resource 
             */
            SOPRANO_EXPORT QUrl hasTag();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic 
             * 
             * Defines a relationship between two resources, where the object 
             * is a topic of the subject 
             */
            SOPRANO_EXPORT QUrl hasTopic();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#identifier 
             * 
             * Defines a generic identifier for a resource 
             */
            SOPRANO_EXPORT QUrl identifier();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated 
             * 
             * Defines an annotation for a resource in the form of a relationship 
             * between the subject resource and another resource 
             */
            SOPRANO_EXPORT QUrl isRelated();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor 
             * 
             * States which resources a tag is associated with 
             */
            SOPRANO_EXPORT QUrl isTagFor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf 
             * 
             * Defines a relationship between two resources, where the subject 
             * is a topic of the object 
             */
            SOPRANO_EXPORT QUrl isTopicOf();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified 
             * 
             * States the last modification time for a resource 
             */
            SOPRANO_EXPORT QUrl lastModified();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified 
             * 
             * States the modification time for a resource 
             */
            SOPRANO_EXPORT QUrl modified();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating 
             * 
             * Annotation for a resource in the form of a numeric decimal rating 
             */
            SOPRANO_EXPORT QUrl numericRating();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier 
             * 
             * Defines a personal string identifier for a resource 
             */
            SOPRANO_EXPORT QUrl personalIdentifier();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel 
             * 
             * A preferred label for a resource 
             */
            SOPRANO_EXPORT QUrl prefLabel();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol 
             * 
             * A unique preferred symbol representation for a resource 
             */
            SOPRANO_EXPORT QUrl prefSymbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#rating 
             * 
             * Annotation for a resource in the form of an unrestricted rating 
             */
            SOPRANO_EXPORT QUrl rating();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#serializationLanguage 
             * 
             * States the serialization language for a named graph that is 
             * represented within a document 
             */
            SOPRANO_EXPORT QUrl serializationLanguage();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#status 
             * 
             * Specifies the status of a graph, stable, unstable or testing 
             */
            SOPRANO_EXPORT QUrl status();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#version 
             * 
             * Specifies the version of a graph, in numeric format 
             */
            SOPRANO_EXPORT QUrl version();
        }
    }
}

#endif
