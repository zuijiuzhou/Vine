#pragma once

#include <vine/appfw/appfw_global.hpp>
#include <vine/String.hpp>

#include <algorithm>
#include <utility>
#include <vector>

V_APPFWGUI_NS_BEGIN

/**
 * @brief Case-insensitive prefix completion over a list of command names.
 */
class CommandCompleter
{
  public:
    /**
     * @brief Replaces the candidate set and sorts it for stable completion.
     *
     * @param names Candidate command names.
     */
    void setCommands(std::vector<String> names)
    {
        names_ = std::move(names);
        std::sort(names_.begin(), names_.end());
    }

    /**
     * @brief Returns all candidates starting with the prefix (case-insensitive).
     *
     * @param prefix Text to match against the start of each candidate.
     * @return Matching candidates, in sorted order.
     */
    std::vector<String> complete(const String& prefix) const
    {
        std::vector<String> result;
        for (const auto& name : names_)
        {
            if (name.startsWith(prefix, true))
            {
                result.push_back(name);
            }
        }
        return result;
    }

  private:
    std::vector<String> names_;
};

V_APPFWGUI_NS_END
