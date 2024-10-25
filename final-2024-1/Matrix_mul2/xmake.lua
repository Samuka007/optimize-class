add_rules("mode.debug", "mode.release")

-- set_config("fast_io", true)
-- add_defines("PARTIAL_MULT")
-- add_defines("SIMD_MULT")
add_defines("MULTI_THREAD")
set_languages("c99", "c++11")
-- add_requires("eigen")

-- if has_config("fast_io") then
--     add_defines("FAST_IO")
--     add_requires("fast_io")
-- end

target("Matrix_mul2")
    set_kind("binary")
    add_files("src/*.cpp")
    add_cxxflags("-march=native")
    -- if has_config("fast_io") then
    --     add_packages("fast_io")
    -- end
    if is_mode("debug") then
        add_packages("eigen")
        set_optimize("faster")
        -- add_ldflags("-fopenmp")  -- Add this line to link OpenMP library
        -- add_cxxflags("-fopenmp")
        else 
        set_optimize("none")
    end

    on_run(function (target)
        os.execv(target:targetfile(), {"1024", "0.0355"})
        -- Release:
        -- 252384.53
        -- 252384.53
        -- 252384.53
        -- 252384.53
        -- Debug:
        -- Baseline: 252384.55 (O0)
        -- Baseline: 252384.53 (O2)
    end)
    -- add_cxxflags("-fopt-info-vec-optimized")

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

