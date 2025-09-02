add_rules("mode.debug", "mode.release")

add_repositories("levimc-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("groupmountain-repo https://github.com/GroupMountain/xmake-repo.git")

-- 版本管理 - 仅从环境变量获取，用于GitHub Actions
local function get_version()
    local version = os.getenv("MOD_VERSION")
    if version and version ~= "" then
        return version
    end
    -- 如果没有环境变量，使用默认版本
    return "dev"
end

local mod_version = get_version()
print("Building with version: " .. mod_version)

if is_config("target_type", "server") then
    add_requires("levilamina 1.4.0", {configs = {target_type = "server"}})
else
    add_requires("levilamina1.4.0", {configs = {target_type = "client"}})
end

add_requires("levibuildscript")

add_requires("gmlib")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

target("Carpet-Mod-For-LL")
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    add_cxflags( "/EHa", "/utf-8", "/W4", "/w44265", "/w44289", "/w44296", "/w45263", "/w44738", "/w45204")
    add_defines("NOMINMAX", "UNICODE")
    add_defines("MOD_VERSION=\"" .. mod_version .. "\"")
    add_packages("levilamina")
    add_packages("gmlib")
    set_exceptions("none") -- To avoid conflicts with /EHa.
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")
    add_headerfiles("src/**.h")
    add_files("src/**.cpp")
    add_includedirs("src")
    
    -- 构建后复制assets文件夹到bin目录
    after_build(function (target)
        -- 复制到target目录（build目录）
        local targetdir = target:targetdir()
        local mod_name = target:name()
        local assets_src = path.join(os.scriptdir(), "assets")
        local build_dest = path.join(targetdir, mod_name)
        
        if os.isdir(assets_src) then
            print("Copying assets folder to build directory: " .. build_dest)
            os.cp(assets_src, build_dest)
        end
        
        -- 复制到bin目录（最终输出）
        local bin_dest = path.join(os.scriptdir(), "bin", mod_name)
        if os.isdir(assets_src) then
            print("Copying assets folder to bin directory: " .. bin_dest)
            os.cp(assets_src, bin_dest)
        else
            print("Assets folder not found: " .. assets_src)
        end
    end)
    -- if is_config("target_type", "server") then
    --     add_includedirs("src-server")
    --     add_files("src-server/**.cpp")
    -- else
    --     add_includedirs("src-client")
    --     add_files("src-client/**.cpp")
    -- end
