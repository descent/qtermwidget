/*
 * $Revision$
 * $Author$
 * $Date$
 * $Id$
 * function : design pattern observer example
 */

#include "subject.h"

#include <algorithm>



void DS::Subject::attach(Observer * o)
{
 _observers.push_back(o);
}

void DS::Subject::detach(Observer *o)
{
 // 使用的演算法則會將所有符合 o 的值全部刪除
 _observers.erase(std::remove(_observers.begin(),_observers.end(),o),_observers.end());	

}

void DS::Subject::notify()
{
 std::vector<Observer *> :: iterator it;

 for (it=_observers.begin() ; it != _observers.end() ; ++it)
  (*it)->update(this);

}

