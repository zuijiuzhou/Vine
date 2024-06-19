#pragma once

#include "appfw_global.h"

#include "core/Object.h"

VI_APPFW_NS_BEGIN

class VI_APPFW_API CommandExecutingContext : public Object
{
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(CommandExecutingContext);

public:
    String arguments() const;

private:
    VI_OBJECT_DATA
};
using CommandExecutingContextPtr = RefPtr<CommandExecutingContext>;

class VI_APPFW_API Command : public Object
{
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(Command);

public:
    virtual String name() const = 0;

    virtual String group() const = 0;

    virtual void Execute(CommandExecutingContext *context) = 0;
};
using CommandPtr = RefPtr<Command>;

VI_APPFW_NS_END