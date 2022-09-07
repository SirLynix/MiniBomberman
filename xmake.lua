add_rules("mode.debug", "mode.release")
add_repositories("nazara-repo https://github.com/NazaraEngine/xmake-repo.git")
add_requires("nazaraengine~client", { debug = is_mode("debug"), configs = { physics2d = false } })
add_requires("nazaraengine~server", { debug = is_mode("debug"), configs = { 
    audio = false, 
    graphics = false, 
    physics2d = false,
    renderer = false,
    widgets = false,
} })

set_project("Bomberman")

set_languages("c++17")
add_includedirs("src")
set_rundir("bin")
set_targetdir("bin/$(plat)_$(arch)_$(mode)")
set_runtimes(is_mode("debug") and "MDd" or "MD")

target("Bomberman")
    set_kind("binary")
    add_headerfiles("src/Client/**.hpp", "src/Client/**.inl", "src/Shared/**.hpp", "src/Shared/**.inl")
    add_files("src/Client/**.cpp", "src/Shared/**.cpp")
    add_packages("nazaraengine~client")

target("BombermanServer")
    set_kind("binary")
    add_headerfiles("src/Server/**.hpp", "src/Server/**.inl", "src/Shared/**.hpp", "src/Shared/**.inl")
    add_files("src/Server/**.cpp", "src/Shared/**.cpp")
    add_packages("nazaraengine~server")
