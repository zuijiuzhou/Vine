#pragma once

#include <atomic>
#include <memory>
#include <type_traits>

ETD_CORE_NS_BEGIN
template <typename T>
using SharedPtr = std::shared_ptr<T>;

class Object;
template <typename T>
    requires std::is_base_of<Object, T>::value
class RefPtr
{
public:
    RefPtr() : RefPtr(nullptr)
    {
    }
    RefPtr(T *obj) : obj_(obj)
    {
        if(obj_){
            obj_->addRef();
        }
    }
    ~RefPtr()
    {
        if(obj_){
            obj_->removeRef();
        }
    }

private:
    T *obj_;
};

ETD_CORE_NS_END
