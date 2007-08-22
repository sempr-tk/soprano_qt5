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

#include "inferencemodel.h"
#include "rdf.h"
#include "sil.h"
#include "statement.h"
#include "inferencerule.h"
#include "querylegacy.h"
#include "statementpattern.h"
#include "nodepattern.h"
#include "queryresultiterator.h"
#include "literalvalue.h"
#include "bindingset.h"
#include "nodeiterator.h"

#include <QtCore/QString>
#include <QtCore/QUuid>
#include <QtCore/QDebug>

// FIXME: add error handling!


static Soprano::Node compressStatement( const Soprano::Statement& statement )
{
    // There should be some method Statement::toXXXString for this
    QString s = QString( "<%1> <%2> " ).arg( statement.subject().toString() ).arg( statement.predicate().toString() );
    if ( statement.object().isLiteral() ) {
        s.append( QString( "\"%1\"^^<%2>" ).arg( statement.object().toString() ).arg( statement.object().dataType().toString() ) );
    }
    else {
        s.append( '<' + statement.object().toString() + '>' );
    }
    return Soprano::LiteralValue( s );
}


static QUrl createRandomUri()
{
    // FIXME: check if the uri already exists
    QString uid = QUuid::createUuid().toString();
    uid = uid.mid( 1, uid.length()-2 );
    return QUrl( "inference://localhost#" + uid );
}


// this is stuff we only need for the temp implementation that is used due to the lack of proper SPARQL support in redland
// -----------------------------------------------------------------------------------------------------------------------
#include <QSet>
#include "statementiterator.h"
uint qHash( const Soprano::Node& node )
{
    return qHash( node.toString() );
}
// -----------------------------------------------------------------------------------------------------------------------



class Soprano::Inference::InferenceModel::Private
{
public:
    QList<Rule> rules;
    bool compressedStatements;
    bool optimizedQueries;
};


Soprano::Inference::InferenceModel::InferenceModel( Model* parent )
    : FilterModel( parent ),
      d( new Private() )
{
    d->compressedStatements = true;
    d->optimizedQueries = false;
}


Soprano::Inference::InferenceModel::~InferenceModel()
{
    delete d;
}


void Soprano::Inference::InferenceModel::setCompressedSourceStatements( bool b )
{
    d->compressedStatements = b;
}


void Soprano::Inference::InferenceModel::setOptimizedQueriesEnabled( bool b )
{
    d->optimizedQueries = b;
}


void Soprano::Inference::InferenceModel::addRule( const Rule& rule )
{
    d->rules.append( rule );
}


void Soprano::Inference::InferenceModel::setRules( const QList<Rule>& rules )
{
    d->rules = rules;
}


Soprano::ErrorCode Soprano::Inference::InferenceModel::addStatement( const Statement& statement )
{
    ErrorCode error = parentModel()->addStatement( statement );
    if ( error == ERROR_NONE ) {
        // FIXME: error handling for the inference itself
        if( inferStatement( statement, true ) ) {
            emit statementsAdded();
        }
    }
    return error;
}


Soprano::ErrorCode Soprano::Inference::InferenceModel::removeStatements( const Statement& statement )
{
    // FIXME: should we check if the statement could match some rule at all and if not do nothing?

    // are there any rules that handle objects? Probably not.
    if ( !statement.object().isLiteral() ) {
        // we need to list statements first and then iterate over all that will be removed
        // since we change the model we also have to cache the statements
        QList<Statement> removedStatements = parentModel()->listStatements( statement ).allStatements();
        for ( QList<Statement>::const_iterator it2 = removedStatements.constBegin();
              it2 != removedStatements.constEnd(); ++it2 ) {
            removeStatement( *it2 );
        }
    }

    return FilterModel::removeStatements( statement );
}


void Soprano::Inference::InferenceModel::removeStatement( const Statement& statement )
{
    QList<Node> graphs = inferedGraphsForStatement( statement );
    for ( QList<Node>::const_iterator it = graphs.constBegin(); it != graphs.constEnd(); ++it ) {
        Node graph = *it;

        // Step 1: remove the source statements of the removed graph
        if ( !d->compressedStatements ) {
            // (redland probem again: iteraotors are invalidated by remove methods)
            QList<Node> graphSources = parentModel()->listStatements( Statement( graph,
                                                                                 Vocabulary::SIL::SOURCE_STATEMENT(),
                                                                                 Node(),
                                                                                 Vocabulary::SIL::INFERENCE_METADATA() ) ).iterateObjects().allNodes();
            for( QList<Node>::const_iterator it = graphSources.constBegin();
                 it != graphSources.constEnd(); ++it ) {
                parentModel()->removeStatements( Statement( *it, Node(), Node(), Vocabulary::SIL::INFERENCE_METADATA() ) );
            }
        }


        // Step 2: remove the graph metadata
        parentModel()->removeStatements( Statement( graph, Node(), Node(), Vocabulary::SIL::INFERENCE_METADATA() ) );

        // Step 3 remove the infered metadata (and trigger recursive removal) - can be slow
        removeContext( graph );
    }
}


QList<Soprano::Node> Soprano::Inference::InferenceModel::inferedGraphsForStatement( const Statement& statement ) const
{
    if ( d->compressedStatements ) {
        // get the graphs our statement was the source for
        StatementIterator it = parentModel()->listStatements( Statement( Node(),
                                                                         Vocabulary::SIL::SOURCE_STATEMENT(),
                                                                         compressStatement( statement ),
                                                                         Vocabulary::SIL::INFERENCE_METADATA() ) );
        return it.iterateSubjects().allNodes();
    }
    else {
        QList<Soprano::Node> graphs;

        // sadly redland does not seem to support even the most simple queries :(
#if 0
        // check if our statement is source statement for any infered graph
        QString query = QString( "PREFIX rdf: <%1> "
                                 "SELECT ?s WHERE { "
                                 "?s rdf:type rdf:Statement . "
                                 "?s rdf:subject <%2> . "
                                 "?s rdf:predicate <%3> . "
                                 "?s rdf:object <%4> ." )
                        .arg( Vocabulary::RDF::NAMESPACE().toString() )
                        .arg( statement.subject().toString() )
                        .arg( statement.predicate().toString() )
                        .arg( statement.object().toString() );

        if ( statement.context().isValid() ) {
            query += QString( "?s <%1> <%2> ." )
                     .arg( Vocabulary::SIL::CONTEXT().toString() )
                     .arg( statement.context().toString() );
        }

        query += " }";

        QueryResultIterator it = parentModel()->executeQuery( QueryLegacy( query, QueryLegacy::SPARQL ) );
        while ( it.next() ) {
            // Step 2: Check for which graph it is source statement
            query = QString( "SELECT ?g WHERE { "
                             "GRAPH <%1> { "
                             "?g <%2> <%3> . "
                             "?g <%4> <%5> . } }" )
                    .arg( Vocabulary::SIL::INFERENCE_METADATA().toString() )
                    .arg( Vocabulary::SIL::SOURCE_STATEMENT().toString() )
                    .arg( it.binding( 0 ).toString() )
                    .arg( Vocabulary::RDF::TYPE().toString() )
                    .arg( Vocabulary::SIL::INFERENCE_GRAPH().toString() );

            QueryResultIterator it2 = parentModel()->executeQuery( QueryLegacy( query, QueryLegacy::SPARQL ) );
            while ( it2.next() ) {
                // Step 3: remove the whole infered graph and its metadata
                graphs += it2.binding( 0 );
            }
        }
#endif

        // since redland cannot handle our query we have to do it the ugly way
        QSet<Node> sourceStatements;
        StatementIterator it = parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::SUBJECT(), statement.subject() ) );
        sourceStatements = it.iterateSubjects().allNodes().toSet();
        it = parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::PREDICATE(), statement.predicate() ) );
        sourceStatements &= it.iterateSubjects().allNodes().toSet();
        it = parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::OBJECT(), statement.object() ) );
        sourceStatements &= it.iterateSubjects().allNodes().toSet();

        // now sourceStatements should contain what our nice first query above returns
        // and we use a siplyfied version of the query above to redland won't get confused :(
        Q_FOREACH( Node node, sourceStatements ) {
            // Step 2: Check for which graph it is source statement
            QString query = QString( "SELECT ?g WHERE { "
                                     "?g <%1> <%2> . }" )
                            .arg( Vocabulary::SIL::SOURCE_STATEMENT().toString() )
                            .arg( node.toString() );

            QueryResultIterator it2 = parentModel()->executeQuery( QueryLegacy( query, QueryLegacy::SPARQL ) );
            while ( it2.next() ) {
                // Step 3: remove the whole infered graph and its metadata
                graphs += it2.binding( 0 );
            }
        }

        return graphs;
    }
}


void Soprano::Inference::InferenceModel::performInference()
{
    for ( QList<Rule>::iterator it = d->rules.begin();
          it != d->rules.end(); ++it ) {
        // reset the binding statement, we want to infer it all
        Rule& rule = *it;
        rule.bindToStatement( Statement() );
        inferRule( rule, true );
    }
}


void Soprano::Inference::InferenceModel::clearInference()
{
    // remove all infered statements
    QueryLegacy query( QString( "select ?g where { ?g <%1> <%2> . }" )
                       .arg( Vocabulary::RDF::TYPE().toString() )
                       .arg( Vocabulary::SIL::INFERENCE_GRAPH().toString() ),
                       QueryLegacy::SPARQL );

    QList<BindingSet> bindings = parentModel()->executeQuery( query ).allBindings();
    for ( QList<BindingSet>::const_iterator it = bindings.constBegin(); it != bindings.constEnd(); ++it ) {
        parentModel()->removeContext( it->value( 0 ) );
    }

    // remove infered graph metadata
    parentModel()->removeContext( Vocabulary::SIL::INFERENCE_METADATA() );
}


int Soprano::Inference::InferenceModel::inferStatement( const Statement& statement, bool recurse )
{
    for ( QList<Rule>::iterator it = d->rules.begin();
          it != d->rules.end(); ++it ) {
        Rule& rule = *it;
        if( rule.match( statement) ) {
            rule.bindToStatement( statement );
            inferRule( rule, recurse );
        }
    }
}


int Soprano::Inference::InferenceModel::inferRule( const Rule& rule, bool recurse )
{
    QueryLegacy q( rule.createSparqlQuery( d->optimizedQueries ), QueryLegacy::SPARQL );

//    qDebug() << "Rule query: " << q.query();

    int inferedStatementsCount = 0;
    // cache the bindings since we work recursively
    QList<BindingSet> bindings = parentModel()->executeQuery( q ).allBindings();
    for ( QList<BindingSet>::const_iterator it = bindings.constBegin(); it != bindings.constEnd(); ++it ) {

//         qDebug() << "rule bindings:";
//         for ( int i = 0; i < it.bindingCount(); ++i ) {
//             qDebug() << "   " << it.bindingNames()[i] << " - " << it.binding( i );
//         }

        Statement inferedStatement = rule.bindEffect( *it );

        // we only add infered statements if they are not already present (in any named graph, aka. context)
        if ( !parentModel()->containsStatements( inferedStatement ) ) {
            ++inferedStatementsCount;

            QUrl inferenceGraphUrl = createRandomUri();

            // write the actual infered statement
            inferedStatement.setContext( inferenceGraphUrl );
            parentModel()->addStatement( inferedStatement );

            // write the metadata about the new inference graph into the inference metadata graph
            // type of the new graph is sil:InferenceGraph
            parentModel()->addStatement( Statement( inferenceGraphUrl,
                                                    Vocabulary::RDF::TYPE(),
                                                    Vocabulary::SIL::INFERENCE_GRAPH(),
                                                    Vocabulary::SIL::INFERENCE_METADATA() ) );

            // add sourceStatements
            QList<Statement> sourceStatements = rule.bindPreconditions( *it );
            for ( QList<Statement>::const_iterator it = sourceStatements.constBegin();
                  it != sourceStatements.constEnd(); ++it ) {
                const Statement& sourceStatement = *it;

                if ( d->compressedStatements ) {
                    // remember the statement through a checksum (well, not really a checksum for now ;)
                    parentModel()->addStatement( Statement( inferenceGraphUrl,
                                                            Vocabulary::SIL::SOURCE_STATEMENT(),
                                                            compressStatement( sourceStatement ),
                                                            Vocabulary::SIL::INFERENCE_METADATA() ) );
                }
                else {
                    // remember the source statement as a source for our graph
                    parentModel()->addStatement( Statement( inferenceGraphUrl,
                                                            Vocabulary::SIL::SOURCE_STATEMENT(),
                                                            storeUncompressedSourceStatement( sourceStatement ),
                                                            Vocabulary::SIL::INFERENCE_METADATA() ) );
                }
            }

            if ( recurse ) {
                inferedStatementsCount += inferStatement( inferedStatement, true );
            }
        }
    }

    return inferedStatementsCount;
}


QUrl Soprano::Inference::InferenceModel::storeUncompressedSourceStatement( const Statement& sourceStatement )
{
    QUrl sourceStatementUri = createRandomUri();
    parentModel()->addStatement( Statement( sourceStatementUri,
                                            Vocabulary::RDF::TYPE(),
                                            Vocabulary::RDF::STATEMENT(),
                                            Vocabulary::SIL::INFERENCE_METADATA() ) );

    parentModel()->addStatement( Statement( sourceStatementUri,
                                            Vocabulary::RDF::SUBJECT(),
                                            sourceStatement.subject(),
                                            Vocabulary::SIL::INFERENCE_METADATA() ) );
    parentModel()->addStatement( Statement( sourceStatementUri,
                                            Vocabulary::RDF::PREDICATE(),
                                            sourceStatement.predicate(),
                                            Vocabulary::SIL::INFERENCE_METADATA() ) );
    parentModel()->addStatement( Statement( sourceStatementUri,
                                            Vocabulary::RDF::OBJECT(),
                                            sourceStatement.object(),
                                            Vocabulary::SIL::INFERENCE_METADATA() ) );
    if ( sourceStatement.context().isValid() ) {
        parentModel()->addStatement( Statement( sourceStatementUri,
                                                Vocabulary::SIL::CONTEXT(),
                                                sourceStatement.context(),
                                                Vocabulary::SIL::INFERENCE_METADATA() ) );
    }
    return sourceStatementUri;
}

#include "inferencemodel.moc"