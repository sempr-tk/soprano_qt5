/* 
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_BACKEND_NEPOMUK_STATEMENT_ITERATOR_H
#define SOPRANO_BACKEND_NEPOMUK_STATEMENT_ITERATOR_H

#include <knep/services/statementlistiterator.h>

#include "StatementIterator.h"

using namespace Nepomuk::Backbone::Services;

namespace Soprano 
{

class Statement;

namespace Backend
{
namespace Nepomuk
{

class NepomukStatementIterator: public Soprano::StatementIterator
{
public:
  explicit NepomukStatementIterator( RDF::StatementListIterator *iter );

  ~NepomukStatementIterator();

  bool hasNext() const;

  const Soprano::Statement next() const;
private:
  class Private;
  Private *d;
};

}
}
}

#endif // SOPRANO_BACKEND_NEPOMUK_STATEMENT_ITERATOR_H
