-- Fatal Fury COTW Space Jump by ExMingYan

local ESCPatch = function(addr, offset)
    addr[offset] = 0x06
end

local MainProgramSearch = function(pattern)
    return AOBScanModule("CotW-Win64-Shipping.exe", ".text", pattern)
end

local ArcadeESCMenu = function() -- 街机模式/Arcade
    local ESCAddress = MainProgramSearch("EB ?? 41 80 ?? 0F 75 ?? 48 8B CB")
    ESCPatch(ESCAddress, 5)
end

local PracticeESCMenu = function() -- 练习模式/Training
    local ESCAddress = MainProgramSearch("EB ?? 41 80 ?? 0F 75 ?? 41 8B")
    ESCPatch(ESCAddress, 5)
end

local SurvivalESCMenu = function() -- 生存模式/Survival | 竞速模式/Time Attack | EOST
    local ESCAddress = MainProgramSearch("E9 ?? ?? ?? ?? 41 80 ?? 0F 75 ?? 48 8B")
    ESCPatch(ESCAddress, 8)
    local ESCAddress2 = MainProgramSearch("E9 ?? ?? ?? ?? 41 80 ?? 0F 75 ?? 48 8B")
    ESCPatch(ESCAddress2, 8)
end

local TrialESCMenu = function() -- 考验任务/Trial
    local ESCAddress = MainProgramSearch("0F 85 ?? ?? ?? ?? 41 80 ?? 0F 75 ??")
    ESCPatch(ESCAddress, 9)
end

local VersusESCMenu = function() -- 对战/Versus
    local ESCAddress = MainProgramSearch("84 C0 75 ? 41 80 ? 0F 75 ? 4?")
    ESCPatch(ESCAddress, 7)
end

local SetSpace = function() -- 按键设置可用Space键/Button Config can use Space Key
    local SpaceAddress = MainProgramSearch("48 B9 FE 38 C0 FF CF 1E")
    SpaceAddress[7] = 0x1A
end

return {
    {
        main = ArcadeESCMenu
    },
    {
        main = PracticeESCMenu
    },
    {
        main = SurvivalESCMenu
    },
    {
        main = TrialESCMenu
    },
    {
        main = VersusESCMenu
    },
    {
        main = SetSpace
    }
}
