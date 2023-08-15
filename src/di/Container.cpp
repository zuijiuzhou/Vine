#include <di/Container.h>
#include <di/Registration.h>

#include <set>


VINE_DI_NS_BEGIN

namespace
{
    bool isValidRegistration(Registration *r)
    {
        return true;
    }
}

struct Container::Data
{
    std::set<Registration *> regs;
};

Container::Container()
    : d(new Data())
{
}

void Container::add(Registration *reg)
{
    if (isValidRegistration(reg))
    {
        if (d->regs.find(reg) == d->regs.end())
        {
            d->regs.insert(reg);
            reg->addRef();
        }
    }
}

VINE_DI_NS_END