/*
 * $Revision$
 * $Author$
 * $Date$
 * $Id$
 * function : design pattern observer example
 */

#ifndef OBSERVER_H
#define OBSERVER_H
namespace DS
{
 class Subject;
 class Observer
 {
  public:
   virtual ~Observer(){}
   virtual void update(Subject *the_changed_subject)=0;
  protected:
   Observer(){}
 
 };
}// end namespace DS

#endif
