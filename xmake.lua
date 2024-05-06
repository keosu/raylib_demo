add_rules("mode.debug", "mode.release")

set_languages("c++17")
add_requires("raylib 5.*")
add_requires("raygui 4.*") 


-- for _, filepath in ipairs(os.files("src/**.cpp")) do
-- target(path.basename(filepath))
--     add_files(filepath)
--     set_rundir(path.directory(filepath))
--     add_packages("raylib")
--     add_packages("raygui")
-- end

target("raindrops")
    add_files("src/raindrops.cpp")
    add_packages("raylib")
    add_packages("raygui")

target("KochCurve")
    add_files("src/KochCurve.cpp")
    add_packages("raylib")
    add_packages("raygui")

target("ApollonianGasket")
    add_files("src/ApollonianGasket.cpp")
    add_packages("raylib")
    add_packages("raygui")

target("ModCircles")
    add_files("src/ModCircles.cpp")
    add_packages("raylib")
    add_packages("raygui")
    