class Selector
	: CompositeProcessor
{
%TypeHeaderCode
	#include <BALL/PYTHON/EXTENSIONS/pyUnaryProcessors.h>
	#include <BALL/KERNEL/selector.h>
%End
	public:
	Selector();
	Selector(const Selector&);
	Selector(const String&);
	~Selector();
	Processor::Result operator () (Composite&);
%MethodCode
	sipRes = sipCpp->operator () (*a0);
%End

	virtual bool start();
	Size getNumberOfSelectedAtoms() const;
	list<Atom*> getSelectedAtoms();
};
