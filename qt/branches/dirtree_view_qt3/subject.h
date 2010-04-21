/*
 * $Revision$
 * $Author$
 * $Date$
 * $Id$
 * function : design pattern observer example
 */

#ifndef SUBJECT_H
#define SUBJECT_H

#include "observer.h"

#include <vector>


namespace DS
{
 class Subject
 {
  public:
   virtual ~Subject(){}
   virtual void attach(Observer *o);
   virtual void detach(Observer *o);
   virtual void notify();
  protected:
   Subject(){}
  private:
   std::vector<Observer *> _observers;
 };

} // end namespace DS

#endif

