#pragma once

#include "modelio_global.hpp"

#include <filesystem>

#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/geometry/Mesh.hpp>

V_MODELIO_NS_BEGIN

/**
 * @brief Abstract exporter that writes a mesh to a file.
 *
 * Concrete mesh exporters (OBJ, STL, PLY, ...) derive from this class and
 * write a Mesh into the target format.
 */
class V_MODELIO_API MeshExporter : public vine::Object, public vine::RefCounted<MeshExporter> {
    V_OBJECT_META_DECL;

  public:
    MeshExporter();
    ~MeshExporter() override;

  public:
    /**
     * @brief Writes a mesh to a file.
     *
     * @param path File to write.
     * @param mesh Mesh to export.
     * @return true on success.
     */
    virtual bool save(const std::filesystem::path& path, const vine::geometry::Mesh& mesh) = 0;

    /**
     * @brief Returns whether this exporter supports the given mesh.
     *
     * @param mesh Mesh to check.
     * @return true when the mesh type is supported.
     */
    virtual bool canExport(const vine::geometry::Mesh& mesh) const = 0;
};

V_MODELIO_NS_END
