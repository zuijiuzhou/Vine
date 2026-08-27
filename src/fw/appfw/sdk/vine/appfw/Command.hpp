#pragma once

#include "appfw_global.hpp"

#include <vine/RefObject.hpp>

V_APPFW_NS_BEGIN

class CommandManager;

class V_APPFW_API CommandExecutingContext {

    friend class CommandManager;

  private:
    CommandExecutingContext();

  public:
    String arguments() const;

  private:
};

enum class CommandFlags : uint32_t
{
    None = 0,

    HasUserInteraction = 1 << 0,

    CanNested = 1 << 1,

    Exclusive = 1 << 2,

    Undoable = 1 << 3,

    Transactional = 1 << 4,

    Parallelizable = 1 << 5,

    Async = 1 << 6
};

class V_APPFW_API Command : public Object {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(Command);


  public:
    virtual String name() const = 0;

    virtual String group() const = 0;

    virtual void execute(CommandExecutingContext* context) = 0;
};

V_APPFW_NS_END
