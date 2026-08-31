#ifndef NX_FOUNDATION_OBSERVABLE
#define NX_FOUNDATION_OBSERVABLE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Allocateable.h"
#include "Array.h"
/**
 This is a simple implementation of the observer / observable patter.  Because most
 observers are also observables, we unite the two classes into one.  This also
 avoids multiple inheritance.

  This is an abstract class.
*/
namespace NxFoundation
	{

class Observable;
typedef Observable Observer;
typedef NxArraySDK<Observer *> ObserverList;
class Observable 
	{
	public:
	enum Event						//these are some standard events, between 0 and 255. The derived classes may define others > 255.
		{
		OE_RELEASED = 0, 
		OE_CHANGED = 1,
		OE_LAST_OBSERVER_REMOVED = 2,	//the observer count of an observable went from nonzero to zero
		};

	//observable
	void addObserver(Observer &);
	void removeObserver(Observer &);
	void notifyObservers(NxU32 event);
	unsigned getNumObservers();

	//observer
	virtual void event(NxU32 event, Observable &);

	private:
	ObserverList observers;
	};
	}
#endif
