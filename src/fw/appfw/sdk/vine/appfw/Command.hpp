#pragma once

#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

V_APPFW_NS_BEGIN

class V_APPFW_API CommandExecutingContext : public RefObject {
    V_OBJECT_META_DECL;

  public:
    String arguments() const;

  private:
    struct Data;
    Data* const d;
};

using CommandExecutingContextPtr = RefPtr<CommandExecutingContext>;

class V_APPFW_API Command : public RefObject {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(Command);

  public:
    virtual String name() const = 0;

    virtual String group() const = 0;

    virtual void Execute(CommandExecutingContext* context) = 0;
};

using CommandPtr = RefPtr<Command>;

V_APPFW_NS_END
