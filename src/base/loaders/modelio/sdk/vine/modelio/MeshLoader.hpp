#pragma once

#include "modelio_global.hpp"

#include <filesystem>

#include <vine/IntrusivePtr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/geometry/Mesh.hpp>

V_MODELIO_NS_BEGIN

/**
 * @brief Abstract loader that reads a mesh from a file.
 *
 * Concrete mesh loaders (OBJ, STL, PLY, ...) derive from this class and
 * produce a Mesh (TriangleMesh or IndexedTriangleMesh). The returned mesh
 * is null when the file cannot be read or parsed.
 */
class V_MODELIO_API MeshLoader : public vine::Object, public vine::RefCounted<MeshLoader> {
    V_OBJECT_META_DECL;

  public:
    MeshLoader();
    ~MeshLoader() override;

  public:
    /**
     * @brief Loads a mesh from a file.
     *
     * @param path File to load.
     * @return Loaded mesh, or null on failure.
     */
    virtual vine::IntrusivePtr<vine::geometry::Mesh> load(const std::filesystem::path& path) = 0;

    /**
     * @brief Returns whether this loader supports the given file.
     *
     * @param path File to check.
     * @return true when the file is supported.
     */
    virtual bool canLoad(const std::filesystem::path& path) const = 0;
};

V_MODELIO_NS_END
