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

#include "inferencerule.h"
#include "statementpattern.h"
#include "nodepattern.h"
#include "bindingset.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


class Soprano::Inference::Rule::Private : public QSharedData
{
public:
    QList<StatementPattern> preconditions;
    StatementPattern effect;
    Statement bindingStatement;
};


Soprano::Inference::Rule::Rule()
    : d( new Private() )
{
}


Soprano::Inference::Rule::Rule( const Rule& other )
{
    d = other.d;
}


Soprano::Inference::Rule::~Rule()
{
}


Soprano::Inference::Rule& Soprano::Inference::Rule::operator=( const Rule& other )
{
    d = other.d;
    return *this;
}


QList<Soprano::Inference::StatementPattern> Soprano::Inference::Rule::preconditions() const
{
    return d->preconditions;
}


void Soprano::Inference::Rule::addPrecondition( const StatementPattern& sp )
{
    d->preconditions.append( sp );
}


Soprano::Inference::StatementPattern Soprano::Inference::Rule::effect() const
{
    return d->effect;
}


void Soprano::Inference::Rule::setEffect( const StatementPattern& e )
{
    d->effect = e;
}


void Soprano::Inference::Rule::bindToStatement( const Statement& statement )
{
    d->bindingStatement = statement;
}


bool Soprano::Inference::Rule::match( const Statement& statement ) const
{
    for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
          it != d->preconditions.constEnd(); ++it ) {
        if ( it->match( statement ) ) {
            return true;
        }
    }
    return false;
}


QString Soprano::Inference::Rule::createSparqlQuery( bool bindStatement ) const
{
    QString query = "SELECT * WHERE { ";

    if ( !bindStatement || !d->bindingStatement.isValid() ) {
        for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
              it != d->preconditions.constEnd(); ++it ) {
            query += it->createSparqlGraphPattern( BindingSet() ) + " . ";
        }
    }
    else {
        QStringList subQueries;

        // bind the statement to the variables in the query. If multiple bindings are possible use a SPARQL UNION
        for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
              it != d->preconditions.constEnd(); ++it ) {
            const StatementPattern& p = *it;
            if ( p.match( d->bindingStatement ) ) {
                BindingSet bindings;
                if ( p.subjectPattern().isVariable() ) {
                    bindings.insert( p.subjectPattern().variableName(), d->bindingStatement.subject() );
                }
                if ( p.predicatePattern().isVariable() ) {
                    bindings.insert( p.predicatePattern().variableName(), d->bindingStatement.predicate() );
                }
                if ( p.objectPattern().isVariable() ) {
                    bindings.insert( p.objectPattern().variableName(), d->bindingStatement.object() );
                }

                // create a whole new subquery
                QString subQuery;
                for ( QList<StatementPattern>::const_iterator it2 = d->preconditions.constBegin();
                      it2 != d->preconditions.constEnd(); ++it2 ) {
                    // skip the one that is fully bound
                    if ( it2 != it ) {
                        subQuery += it2->createSparqlGraphPattern( bindings ) + " . ";
                    }
                }

                subQueries.append( subQuery );
            }
        }

        if ( subQueries.count() > 1 ) {
            query += "{ " + subQueries.join( " } UNION { " ) + " }";
        }
        else {
            query += subQueries[0];
        }
    }

    query += "}";

    return query;
}


Soprano::Statement Soprano::Inference::Rule::bindStatementPattern( const StatementPattern& pattern, const BindingSet& bindings ) const
{
    // below we make sure that all binding are available (in case of optimized queries the bindingStatement must not have changed)

    Statement s;
    if ( pattern.subjectPattern().isVariable() ) {
        s.setSubject( bindings[pattern.subjectPattern().variableName()] );
    }
    else {
        s.setSubject( pattern.subjectPattern().resource() );
    }

    if ( pattern.predicatePattern().isVariable() ) {
        s.setPredicate( bindings[pattern.predicatePattern().variableName()] );
    }
    else {
        s.setPredicate( pattern.predicatePattern().resource() );
    }

    if ( pattern.objectPattern().isVariable() ) {
        s.setObject( bindings[pattern.objectPattern().variableName()] );
    }
    else {
        s.setObject( pattern.objectPattern().resource() );
    }

    return s;
}


Soprano::BindingSet Soprano::Inference::Rule::mergeBindingStatement( const BindingSet& bindings ) const
{
    //
    // Here we regenerate the information (bindings) we "removed" for optimization purposes in createSparqlGraphPattern
    // This can simply be done by matching the bindingStatement which was used to optimize the query to the precondition
    // that has no binding yet. Because that is the one which was removed from the optmized query
    //
    BindingSet bs( bindings );
    for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
          it != d->preconditions.constEnd(); ++it ) {
        const StatementPattern& pattern = *it;
        if ( pattern.subjectPattern().isVariable() && bindings[pattern.subjectPattern().variableName()].isValid() ) {
            continue;
        }
        if ( pattern.predicatePattern().isVariable() && bindings[pattern.predicatePattern().variableName()].isValid() ) {
            continue;
        }
        if ( pattern.objectPattern().isVariable() && bindings[pattern.objectPattern().variableName()].isValid() ) {
            continue;
        }

        // update bindings
        if ( pattern.subjectPattern().isVariable() ) {
            bs.insert( pattern.subjectPattern().variableName(), d->bindingStatement.subject() );
        }
        if ( pattern.predicatePattern().isVariable() ) {
            bs.insert( pattern.predicatePattern().variableName(), d->bindingStatement.predicate() );
        }
        if ( pattern.objectPattern().isVariable() ) {
            bs.insert( pattern.objectPattern().variableName(), d->bindingStatement.object() );
        }
    }
    return bs;
}


Soprano::Statement Soprano::Inference::Rule::bindEffect( const BindingSet& bindings ) const
{
    return bindStatementPattern( d->effect, mergeBindingStatement( bindings ) );
}


QList<Soprano::Statement> Soprano::Inference::Rule::bindPreconditions( const BindingSet& bindings ) const
{
    // 2 sweeps: 1. update bindings by matching the bindingStatement to the precondition we left out in the
    //              optimized query creation. That gives us the rest of the bindings we need.
    //           2. actually bind all vars

    QList<Statement> sl;
    for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
          it != d->preconditions.constEnd(); ++it ) {
        sl.append( bindStatementPattern( *it, mergeBindingStatement( bindings ) ) );
    }

    return sl;
}


QDebug operator<<( QDebug s, const Soprano::Inference::Rule& rule )
{
    s.nospace() << "[";
    QList<Soprano::Inference::StatementPattern> cl = rule.preconditions();
    QList<Soprano::Inference::StatementPattern>::const_iterator it = cl.constBegin();
    while ( it != cl.constEnd() ) {
        s.nospace() << *it;
        ++it;
        if ( it != cl.constEnd() ) {
            s.nospace() << ", ";
        }
    }
    return s.nospace() << " -> " << rule.effect() << "]";
}