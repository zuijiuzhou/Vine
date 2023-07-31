#pragma once

#include "core/Inherit.h"

#include "appfw_export.h"

ETD_APPFW_NS_BEGIN

class ETD_APPFW_API CommandExecutingContext : public Inherit<Object, CommandExecutingContext>
{
public:
    Str arguments() const;
private:
    struct Data;
    const Data *d_ptr;
};

class ETD_APPFW_API Command : public Inherit<Object, Command>
{

public:
    virtual Str name() const = 0;

    virtual Str group() const = 0;

    virtual void Execute(CommandExecutingContext::Ptr context) = 0;
};
ETD_APPFW_NS_END