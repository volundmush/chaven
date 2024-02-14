#ifndef _INTERFACES_H
#define _INTERFACES_H

#include "MudIo.hpp"

class IStreamable
{
    public:
	virtual ~IStreamable() {}
	virtual bool writeTo(Output &os) = 0;
	virtual bool readFrom(StaticInput &is) = 0;
};

class IEditString
{
    public:
	virtual void editString() = 0;
};

#endif
