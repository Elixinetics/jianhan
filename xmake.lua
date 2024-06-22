set_project("jianhan")
set_version("0.0.1")

add_rules("mode.release", "mode.debug")

set_languages("c99", "cxx23")
set_optimize("fastest")
set_fpmodels("fast")

add_requires("openmp", {debug = true})
add_requires("fmt >=10.2", {debug = true})
add_requires("toml11 >=3.8", {debug = true})
add_requires("doctest >=2.4", {debug = true})
add_requires("nanobench >=4.3", {debug = true})

target("jianhan", function ()
    set_kind("shared")
    add_files("src/**.cpp")
    add_packages(
        "openmp", "fmt", "toml11"
    )
end)

target("tests", function ()
    set_kind("binary")
    add_files("src/**.cpp")
    add_files("tests/**.cpp")
    add_packages(
        "openmp", "fmt", "toml11",
        "doctest", "nanobench"
    )
end)
