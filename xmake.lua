add_rules("mode.debug", "mode.release")
add_repositories("nazara-repo https://github.com/NazaraEngine/xmake-repo.git")
add_requires("nazaraengine", "nazarautils", { configs = { debug = is_mode("debug") } })
add_requires("nzsl", { configs = { debug = is_mode("debug"), shared = true } })

set_project("Bomberman")

set_languages("c++17")
add_includedirs("src")
set_rundir("bin")
set_targetdir("bin/$(plat)_$(arch)_$(mode)")
set_runtimes(is_mode("debug") and "MDd" or "MD")

target("Bomberman")
    set_kind("binary")
    add_headerfiles("src/**.hpp")
    add_files("src/**.cpp")
    add_packages("nazaraengine", "nzsl", "nazarautils")
