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

#ifndef _SOPRANO_INFERENCE_RULE_H_
#define _SOPRANO_INFERENCE_RULE_H_

#include <QtCore/QSharedDataPointer>
#include <QtCore/QList>

#include "statementpattern.h"
#include "statement.h"
#include "soprano_export.h"


namespace Soprano {

    class BindingSet;

    namespace Inference {

        /**
	 * \brief A rule used by the InferenceModel to generate inferenced statements.
	 *
	 * \author Sebastian Trueg <trueg@kde.org>
	 */
	class SOPRANO_EXPORT Rule
	{
	public:
	    Rule();
	    Rule( const Rule& other );
	    ~Rule();

	    Rule& operator=( const Rule& other );

	    /**
	     * The list of preconditions for the rule. Each condition
	     * is a StatementPattern which can contain variables.
	     * These variables are identified by their name and matched
	     * accordingly.
	     */
	    QList<StatementPattern> preconditions() const;

	    void addPrecondition( const StatementPattern& );

	    /**
	     * The effect of a rule is a pattern that repeats variables from
	     * the preconditions and thus, identifies the statements that
	     * are to be infered from the preconditions.
	     */
	    StatementPattern effect() const;

	    void setEffect( const StatementPattern& );

	    /**
	     * Check if a statement matches any of the statement patterns
	     * in this rule.
	     * \return true if statement matches any of the patterns, false otherwise.
	     */
	    bool match( const Statement& statement ) const;

	    /**
	     * Bind this rule to a specific Statement.
	     *
	     * The purpose of this method is to allow retricting the application of
	     * a rule to one statement, i.e. a newly added one.
	     * 
	     * \param statement The Statement to bind this rule to.
	     *
	     * \sa createSparqlQuery, bindEffect, bindPreconditions
	     */
	    void bindToStatement( const Statement& statement );

	    /**
	     * Create a SPARQL query that retrieves all resources matching this rule.
	     * \param bindVariables If true and a valid binding statement is set the query
	     * will be bound to this statement resulting in a UNION query of all possible
	     * bindings.
	     *
	     * \sa bindToStatement
	     */
	    QString createSparqlQuery( bool bindVariables = false ) const;

	    /**
	     * Bind the rule's effect to a set of bindings as reveived from a query.
	     * If the bindings do not contain all variables the bound statement is used as backup.
	     *
	     * \param bindings The bindings to apply to this rule.
	     *
	     * \return The statement infered by this rule under the application of bindings.
	     *
	     * \sa bindToStatement, bindPreconditions
	     */
	    Statement bindEffect( const BindingSet& bindings ) const;

	    /**
	     * Bind the rule's preconditions to a set of bindings as reveived from a query.
	     * If the bindings do not contain all variables the bound statement is used as backup.
	     *
	     * \param bindings The bindings to apply to this rule.
	     *
	     * \return The statements that have to be valid in order for this rule to infer the statement
	     * returned by bindEffect under the same bindings.
	     *
	     * \sa bindToStatement, bindEffect
	     */
	    QList<Statement> bindPreconditions( const BindingSet& bindings ) const;

	private:
	    /**
	     * Merges in binding information from the bindingStatement by matching it to the preconditions.
	     * This is necessary for optimized queries.
	     */
	    BindingSet mergeBindingStatement( const BindingSet& bindings ) const;
	    Statement bindStatementPattern( const StatementPattern& pattern, const BindingSet& bindings ) const;

	    class Private;
	    QSharedDataPointer<Private> d;
	};
    }
}

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Inference::Rule& );

#endif
