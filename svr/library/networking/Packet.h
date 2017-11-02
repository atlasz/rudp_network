#pragma once
#include "Object.h"

class Packet : public Object
{
    int type;

public:
    
    Packet( int _type ) : type(_type) {}

    virtual ~Packet() {}

    int GetType() const { return type; }

private:

    Packet( const Packet & other );
    Packet & operator = ( const Packet & other );
};