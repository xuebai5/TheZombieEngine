package.name = "ode"
package.language = "c++"
package.objdir = "obj/ode"

-- Differentiate between Single and Double precision

  for k,v in ipairs(project.configs) do
    if (string.find(v, "Single") ~= nil) then
      package.config[v].target="ode_single"
    end
    if (string.find(v, "Double") ~= nil) then
      package.config[v].target="ode_double"
    end
  end

-- Append a "d" to the debug version of the libraries

  for k,v in ipairs(project.configs) do
    if (string.find(v, "Debug") ~= nil) then
      package.config[v].target=package.config[v].target.."d"
    end
  end


-- Define _DEBUG/NDEBUG depending on build kind

  for k,v in ipairs(project.configs) do
    if (string.find(v, "Debug") ~= nil) then
      table.insert(package.config[v].defines, "_DEBUG")
    else
-- Allow asserts to be included in release build by default
--      table.insert(package.config[v].defines, "NDEBUG")
    end
  end


-- Output is placed in a directory named for the target toolset.
  package.path = options["target"]


-- Write a custom <config.h> to src/ode, based on the specified flags

  io.input("config-default.h")
  local text = io.read("*a")

  if (options["no-trimesh"]) then

    text = string.gsub(text, "#define dTRIMESH_ENABLED 1", "/* #define dTRIMESH_ENABLED 1 */")
    text = string.gsub(text, "#define dTRIMESH_OPCODE 1", "/* #define dTRIMESH_OPCODE 1 */")

  elseif (options["with-gimpact"]) then

    text = string.gsub(text, "#define dTRIMESH_OPCODE 1", "#define dTRIMESH_GIMPACT 1")

  end

  if (options["no-alloca"]) then
    text = string.gsub(text, "/%* #define dUSE_MALLOC_FOR_ALLOCA %*/", "#define dUSE_MALLOC_FOR_ALLOCA")
  end

  if (options["enable-ou"]) then
    text = string.gsub(text, "/%* #define dOU_ENABLED 1 %*/", "#define dOU_ENABLED 1")
    text = string.gsub(text, "/%* #define dATOMICS_ENABLED 1 %*/", "#define dATOMICS_ENABLED 1")
    text = string.gsub(text, "/%* #define dTLS_ENABLED 1 %*/", "#define dTLS_ENABLED 1")
  end

  io.output("../ode/src/config.h")
  io.write(text)
  io.close()


-- Package Build Settings

  if (not options["enable-shared-only"]) then

    package.config["DebugSingleLib"].kind = "lib"
    package.config["ReleaseSingleLib"].kind = "lib"
    table.insert(package.config["DebugSingleLib"].defines, "ODE_LIB")
    table.insert(package.config["ReleaseSingleLib"].defines, "ODE_LIB")
    table.insert(package.config["DebugSingleLib"].defines, "dSINGLE")
    table.insert(package.config["ReleaseSingleLib"].defines, "dSINGLE")

    package.config["DebugDoubleLib"].kind = "lib"
    package.config["ReleaseDoubleLib"].kind = "lib"
    table.insert(package.config["DebugDoubleLib"].defines, "ODE_LIB")
    table.insert(package.config["ReleaseDoubleLib"].defines, "ODE_LIB")
    table.insert(package.config["DebugDoubleLib"].defines, "dDOUBLE")
    table.insert(package.config["ReleaseDoubleLib"].defines, "dDOUBLE")

  end

  if (not options["enable-static-only"]) then

    package.config["DebugSingleDLL"].kind = "dll"
    package.config["ReleaseSingleDLL"].kind = "dll"
    table.insert(package.config["DebugSingleDLL"].defines, "ODE_DLL")
    table.insert(package.config["ReleaseSingleDLL"].defines, "ODE_DLL")
    table.insert(package.config["DebugSingleDLL"].defines, "dSINGLE")
    table.insert(package.config["ReleaseSingleDLL"].defines, "dSINGLE")

    package.config["DebugDoubleDLL"].kind = "dll"
    package.config["ReleaseDoubleDLL"].kind = "dll"
    table.insert(package.config["DebugDoubleDLL"].defines, "ODE_DLL")
    table.insert(package.config["ReleaseDoubleDLL"].defines, "ODE_DLL")
    table.insert(package.config["DebugDoubleDLL"].defines, "dDOUBLE")
    table.insert(package.config["ReleaseDoubleDLL"].defines, "dDOUBLE")

  end


  package.includepaths =
  {
    "../../ode/src",
    "../../include",
    "../../OPCODE",
    "../../GIMPACT/include",
    "../../ou/include"
  }

  if (windows) then
    table.insert(package.defines, "WIN32")
  end

  -- disable VS2005 CRT security warnings
  if (options["target"] == "vs2005") then
    table.insert(package.defines, "_CRT_SECURE_NO_DEPRECATE")
  end


-- Build Flags

	package.config["DebugSingleLib"].buildflags   = { 'static-runtime' }
	package.config["DebugSingleDLL"].buildflags   = { 'static-runtime' }

	package.config["ReleaseSingleDLL"].buildflags = { 'static-runtime', 'optimize-speed', 'no-symbols', 'no-frame-pointer' }
	package.config["ReleaseSingleLib"].buildflags = { 'static-runtime', 'optimize-speed', 'no-symbols', 'no-frame-pointer' }

	package.config["DebugDoubleLib"].buildflags   = { 'static-runtime' }
	package.config["DebugDoubleDLL"].buildflags   = { 'static-runtime' }

	package.config["ReleaseDoubleDLL"].buildflags = { 'static-runtime', 'optimize-speed', 'no-symbols', 'no-frame-pointer' }
	package.config["ReleaseDoubleLib"].buildflags = { 'static-runtime', 'optimize-speed', 'no-symbols', 'no-frame-pointer' }

	if (options.target == "vs6" or options.target == "vs2002" or options.target == "vs2003") then
      for k,v in ipairs(project.configs) do
        if (string.find(v, "Lib") ~= nil) then
          table.insert(package.config[v].buildflags, "static-runtime")
        end
      end
	end


-- Libraries

  if (windows) then
    table.insert(package.links, "user32")
  end


-- Files

  core_files =
  {
    matchfiles("../../include/ode/*.h"),
    matchfiles ("../../ode/src/*.h", "../../ode/src/*.c", "../../ode/src/*.cpp")
  }

  excluded_files =
  {
--  "../../ode/src/collision_std.cpp",
    "../../ode/src/scrapbook.cpp_deprecated",
    "../../ode/src/stack.cpp_deprecated",
    "../../ode/src/stack.h_deprecated",
  }

  trimesh_files =
  {
    "../../ode/src/collision_trimesh_colliders.h",
    "../../ode/src/collision_trimesh_internal.h",
    "../../ode/src/collision_trimesh_opcode.cpp",
    "../../ode/src/collision_trimesh_gimpact.cpp",
    "../../ode/src/collision_trimesh_box.cpp",
    "../../ode/src/collision_trimesh_ccylinder.cpp",
    "../../ode/src/collision_cylinder_trimesh.cpp",
    "../../ode/src/collision_trimesh_distance.cpp",
    "../../ode/src/collision_trimesh_ray.cpp",
    "../../ode/src/collision_trimesh_sphere.cpp",
    "../../ode/src/collision_trimesh_trimesh.cpp",
    "../../ode/src/collision_trimesh_plane.cpp"
  }

  opcode_files =
  {
    matchrecursive("../../OPCODE/*.h", "../../OPCODE/*.cpp")
  }

  gimpact_files =
  {
    matchrecursive("../../GIMPACT/*.h", "../../GIMPACT/*.cpp")
  }

  ou_files =
  {
    matchrecursive("../../ou/*.h", "../../ou/*.cpp")
  }

  dif_files =
  {
    "../../ode/src/export-dif.cpp"
  }

  package.files = { core_files }
  package.excludes = { excluded_files }

  if (options["no-dif"]) then
    table.insert(package.excludes, dif_files)
  end

  if (options["no-trimesh"]) then
    table.insert(package.excludes, trimesh_files)
  else
    table.insert(package.files, gimpact_files)
    table.insert(package.files, opcode_files)
  end

  if (options["enable-ou"]) then
    table.insert(package.files, ou_files)
-- Insert namespace redefinition in project options instead of config.h
-- because OU library does not include that file itself.
    table.insert(package.defines, "_OU_NAMESPACE=odeou")
  end
