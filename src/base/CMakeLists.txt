add_subdirectory(global)
add_subdirectory(core)
add_subdirectory(async)
add_subdirectory(math)
add_subdirectory(geometry)
# iobase must be configured before loaders: loaders (assimp) consumes the
# zlibstatic that iobase builds (assimp's bundled zlib is disabled in
# loaders/modelio), so its target must already exist by the time assimp
# runs find_package(ZLIB).
add_subdirectory(iobase)
add_subdirectory(loaders)
add_subdirectory(runtime)
add_subdirectory(system)
add_subdirectory(crypto)
add_subdirectory(progress)
add_subdirectory(logging)