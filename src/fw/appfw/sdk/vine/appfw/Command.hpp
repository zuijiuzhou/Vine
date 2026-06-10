#pragma once

#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

V_APPFW_NS_BEGIN

class V_APPFW_API CommandExecutingContext {

  public:
    String arguments() const;

  private:
    struct Data;
    Data* const d;
};

class V_APPFW_API Command : public Object {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(Command);

  public:
    virtual String name() const = 0;

    virtual String group() const = 0;

    virtual void Execute(CommandExecutingContext* context) = 0;
};

V_APPFW_NS_END
