#pragma once

#include "modelio_global.hpp"

#include <filesystem>

#include <vine/IntrusivePtr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/geometry/BrepShape.hpp>

V_MODELIO_NS_BEGIN

/**
 * @brief Abstract loader that reads a B-rep solid from a file.
 *
 * Concrete B-rep loaders (STEP, IGES, ...) derive from this class and
 * produce a BrepShape. The returned solid is null when the file cannot be
 * read or parsed.
 */
class V_MODELIO_API BrepLoader : public vine::Object, public vine::RefCounted<BrepLoader> {
    V_OBJECT_META_DECL;

  public:
    BrepLoader();
    ~BrepLoader() override;

  public:
    /**
     * @brief Loads a B-rep solid from a file.
     *
     * @param path File to load.
     * @return Loaded solid, or null on failure.
     */
    virtual vine::IntrusivePtr<vine::geometry::BrepShape> load(const std::filesystem::path& path) = 0;

    /**
     * @brief Returns whether this loader supports the given file.
     *
     * @param path File to check.
     * @return true when the file is supported.
     */
    virtual bool canLoad(const std::filesystem::path& path) const = 0;
};

V_MODELIO_NS_END
