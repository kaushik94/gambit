//
// FILE: rational.h
//
// $Id$
//

// This may look like C code, but it is really -*- C++ -*-

/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _Rational_h
#ifdef __GNUG__
#pragma interface
#endif
#define _Rational_h 1

#include "math/integer.h"
#include <math.h>

class gInput;
class gOutput;

class gRational
{
protected:
  gInteger          num;
  gInteger          den;

  void             normalize();

public:
                   gRational();
                   gRational(double);
                   gRational(int n);
                   gRational(long n);
                   gRational(int n, int d);
                   gRational(long n, long d);
                   gRational(long n, unsigned long d);
                   gRational(unsigned long n, long d);
                   gRational(unsigned long n, unsigned long d);
                   gRational(const gInteger& n);
                   gRational(const gInteger& n, const gInteger& d);
                   gRational(const gRational&);

                  ~gRational();

  gRational &operator =  (const gRational& y);

  friend int       operator == (const gRational& x, const gRational& y);
  friend int       operator != (const gRational& x, const gRational& y);
  friend int       operator <  (const gRational& x, const gRational& y);
  friend int       operator <= (const gRational& x, const gRational& y);
  friend int       operator >  (const gRational& x, const gRational& y);
  friend int       operator >= (const gRational& x, const gRational& y);

  friend gRational  operator +  (const gRational& x, const gRational& y);
  friend gRational  operator -  (const gRational& x, const gRational& y);
  friend gRational  operator *  (const gRational& x, const gRational& y);
  friend gRational  operator /  (const gRational& x, const gRational& y);

  void             operator += (const gRational& y);
  void             operator -= (const gRational& y);
  void             operator *= (const gRational& y);
  void             operator /= (const gRational& y);

#ifdef __GNUG__
  friend gRational  operator <? (const gRational& x, const gRational& y); // min
  friend gRational  operator >? (const gRational& x, const gRational& y); // max
#endif

  friend gRational  operator - (const gRational& x);


// builtin gRational functions


  void             negate();                      // x = -x
  void             invert();                      // x = 1/x

  friend int       sign(const gRational& x);             // -1, 0, or +1
  friend gRational  abs(const gRational& x);              // absolute value
  friend gRational  sqr(const gRational& x);              // square
  friend gRational  pow(const gRational& x, long y);
  friend gRational  pow(const gRational& x, const gInteger& y);
  const gInteger&   numerator() const;
  const gInteger&   denominator() const;

// coercion & conversion

                   operator double() const;
  friend gInteger   floor(const gRational& x);
  friend gInteger   ceil(const gRational& x);
  friend gInteger   trunc(const gRational& x);
  friend gInteger   round(const gRational& x);

  friend gInput&    operator >> (gInput& s, gRational& y);
  friend gOutput&   operator << (gOutput& s, const gRational& y);

  int		   fits_in_float() const;
  int		   fits_in_double() const;

// procedural versions of operators

  friend int       compare(const gRational& x, const gRational& y);
  friend void      add(const gRational& x, const gRational& y, gRational& dest);
  friend void      sub(const gRational& x, const gRational& y, gRational& dest);
  friend void      mul(const gRational& x, const gRational& y, gRational& dest);
  friend void      div(const gRational& x, const gRational& y, gRational& dest);

// error detection

  void    error(const char* msg) const;
  int              OK() const;

};

gText ToText(const gRational &);
gRational FromText(const gText &, gRational &);
void gEpsilon(gRational &v, int i = 8);

#endif
