-- Fatal Fury COTW hitbox loader -ExMingYan

local function Loadhitbox()
    if CheckGameVersion("108052") then
        LoadDLL("hitbox.dll", true, "UnrealWindow")
    end
end

return {
    main = Loadhitbox
}

--[[
LoadDLL(dllPath, waitForWindowLoad, windowClassName)

dllPath is based on Proxy.dll's directory.
if waitForWindowLoad is true, the DLL will be loaded when a window with the specified class name is created.
If windowClassName is nil or an empty string, it will wait for any same processID window to be created.
--]]
