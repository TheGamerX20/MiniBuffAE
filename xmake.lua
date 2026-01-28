-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibf4")

-- set project
set_project("Buffout4AE")
set_license("GPL-3.0")

-- project version
local version = "1.7.0"
local major, minor, patch = version:match("^(%d+)%.(%d+)%.(%d+)$")
set_version(version)

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.release", "mode.releasedbg", "mode.debug")
add_rules("plugin.vsxmake.autoupdate")

-- add options
set_config("commonlib_ini", true)
set_config("commonlib_xbyak", true)

-- add requires
add_requires("unordered_dense")

-- targets
target("Buffout4AE")
    -- add packages
    add_packages("unordered_dense")

    -- static tbb from vcpkg
    add_includedirs(os.getenv("VCPKG_ROOT") .. "/packages/tbb_x64-windows-static-md/include")
    add_linkdirs(os.getenv("VCPKG_ROOT") .. "/packages/tbb_x64-windows-static-md/lib")
    add_links("tbb12", "tbbmalloc")

    -- add dependencies to target
    add_deps("commonlibf4")

    -- add commonlibsse plugin
    add_rules("commonlibf4.plugin", {
        name = "Buffout4AE",
        author = "TheGamerX20",
        description = "Buffout 4 NG ported to AE"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

    -- pass major version
    add_defines(
        "PLUGIN_VERSION_MAJOR=" .. major,
        "PLUGIN_VERSION_MINOR=" .. minor,
        "PLUGIN_VERSION_PATCH=" .. patch
    )
