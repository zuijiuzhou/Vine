#pragma once

#include "core/Inherit.h"

#include "appfw_global.h"

VINE_APPFW_NS_BEGIN

class VINE_APPFW_API CommandExecutingContext : public Inherit<Object, CommandExecutingContext>
{
public:
    String arguments() const;
private:
    struct Data;
    const Data *d;
};
using CommandExecutingContextPtr = RefPtr<CommandExecutingContext>;

class VINE_APPFW_API Command : public Inherit<Object, Command>
{

public:
    virtual String name() const = 0;

    virtual String group() const = 0;

    virtual void Execute(CommandExecutingContext* context) = 0;
};
using CommandPtr = RefPtr<Command>;

VINE_APPFW_NS_END