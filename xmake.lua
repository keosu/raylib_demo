add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_requires("raylib 5.*")
add_requires("raygui 4.*") 

-- if is_os("windows") then
--     add_ldflags("-subsystem:windows", "-entry:mainCRTStartup", {force = true}) 
-- end

for _, filepath in ipairs(os.files("src/**.cpp")) do
target(path.basename(filepath))
    add_files(filepath)
    set_rundir(path.directory(filepath))
    add_packages("raylib")
    add_packages("raygui")
end
 