//
// $Source$
// $Date$
// $Revision$
//
// DESCRIPTION:
// Declaration of mixed strategy profile classes
//
// This file is part of Gambit
// Copyright (c) 2002, The Gambit Project
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#ifndef MIXED_H
#define MIXED_H

#include "game.h"
#include "nfgsupport.h"

template <class T> class gbtRectArray;
template <class T> class gbtBehavProfile;

template <class T>
class gbtMixedProfileRep : public gbtConstNfgRep {
friend class gbtMixedProfile<T>;
public:
  // Some non-virtual generic implementations of operations
  void SetCentroid(void);
  void GetRegret(gbtPVector<T> &value) const;
  T GetLiapValue(void) const;

  virtual ~gbtMixedProfileRep() { }

  // The abstract interface begins here
  virtual gbtMixedProfileRep<T> *Copy(void) const = 0;
   
  virtual bool operator==(const gbtMixedProfileRep<T> &) const = 0;

  // Access to individual strategy probabilities
  virtual T &operator()(const gbtGameStrategy &p_strategy) = 0;
  virtual const T &operator()(const gbtGameStrategy &p_strategy) const = 0;

  // The following implement the necessary gPVector-style operations
  // traditionally permitted on mixed profiles.
  virtual const T &operator()(int, int) const = 0;
  virtual T &operator()(int, int) = 0;

  virtual const T &operator[](int) const = 0;
  virtual T &operator[](int) = 0;

  // Computations of payoffs and other values
  virtual T GetPayoff(const gbtGamePlayer &) const = 0;
  virtual T GetPayoff(const gbtGamePlayer &,
		      const gbtGameStrategy &) const = 0;
  virtual T GetPayoff(const gbtGamePlayer &, 
		      const gbtGameStrategy &,
		      const gbtGameStrategy &) const = 0;

  virtual T GetStrategyValue(const gbtGameStrategy &p_strategy) const = 0;

  virtual operator gbtBehavProfile<T>(void) const = 0;

  virtual gbtVector<T> GetStrategy(const gbtGamePlayer &) const = 0;
  virtual void SetStrategy(const gbtGamePlayer &, const gbtVector<T> &) = 0;
  virtual void CopyStrategy(const gbtGamePlayer &, const gbtPVector<T> &) = 0;
  virtual void CopyStrategy(const gbtGamePlayer &,
			    const gbtMixedProfile<T> &) = 0;
};


template <class T> class gbtMixedProfile {
private:
  gbtMixedProfileRep<T> *m_rep;

public:
  gbtMixedProfile(void) : m_rep(0) { }
  gbtMixedProfile(gbtMixedProfileRep<T> *p_rep)
    : m_rep(p_rep) { if (m_rep) m_rep->Reference(); }
  gbtMixedProfile(const gbtMixedProfile<T> &p_profile)
    : m_rep(p_profile.m_rep->Copy()) { if (m_rep) m_rep->Reference(); }
  ~gbtMixedProfile() { if (m_rep && m_rep->Dereference()) delete m_rep; }
  
  gbtMixedProfile &operator=(const gbtMixedProfile<T> &p_profile) {
    if (this != &p_profile) {
      if (m_rep && m_rep->Dereference()) delete m_rep;
      m_rep = p_profile.m_rep->Copy();
      if (m_rep) m_rep->Reference();
    }
    return *this;
  }

  // Equality semantics are defined as having the same profile, not
  // the same underlying object.
  bool operator==(const gbtMixedProfile<T> &p_profile) const
  { return (*m_rep == *p_profile.m_rep); }
  bool operator!=(const gbtMixedProfile<T> &p_profile) const
  { return !(*m_rep == *p_profile.m_rep); }

  // Direct access so that the traditional operators work without dereferencing
  const T &operator()(int pl, int st) const { return (*m_rep)(pl, st); }
  T &operator()(int pl, int st) { return (*m_rep)(pl, st); }

  const T &operator()(const gbtGameStrategy &p_strategy) const 
  { return (*m_rep)(p_strategy); }
  T &operator()(const gbtGameStrategy &p_strategy)
  { return (*m_rep)(p_strategy); }

  // These almost certainly should be obsolete
  const T &operator[](int i) const { return (*m_rep)[i]; }
  T &operator[](int i) { return (*m_rep)[i]; }

  gbtMixedProfileRep<T> *operator->(void) 
  { if (!m_rep) throw gbtGameNullObject(); return m_rep; }
  const gbtMixedProfileRep<T> *operator->(void) const 
  { if (!m_rep) throw gbtGameNullObject(); return m_rep; }

  gbtMixedProfileRep<T> *Get(void) const { return m_rep; }

  operator gbtNfgGame(void) const { return m_rep; }
  operator gbtBehavProfile<T>(void) const 
  { return (gbtBehavProfile<T>) *m_rep; }

  // Questionable whether this should be provided
  bool IsNull(void) const { return (m_rep == 0); }
};

template <class T>
gbtOutput &operator<<(gbtOutput &, const gbtMixedProfile<T> &);

#endif    // MIXED_H
