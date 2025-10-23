#pragma once

#include "appfw_global.hpp"

#include <vine/core/RefObject.hpp>

VI_APPFW_NS_BEGIN

class VI_APPFW_API CommandExecutingContext : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(CommandExecutingContext);

  public:
    String arguments() const;

  private:
    VI_OBJECT_DATA
};
using CommandExecutingContextPtr = RefPtr<CommandExecutingContext>;

class VI_APPFW_API Command : public RefObject {
    VI_OBJECT_META;
    VI_DISABLE_COPY_MOVE(Command);

  public:
    virtual String name() const = 0;

    virtual String group() const = 0;

    virtual void Execute(CommandExecutingContext* context) = 0;
};
using CommandPtr = RefPtr<Command>;

VI_APPFW_NS_END