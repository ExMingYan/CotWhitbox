-- Fatal Fury COTW Sig Bypass -ExMingYan

local ModCheckPatch = function()
    local address = AOBScanModule("CotW-Win64-Shipping.exe", ".text", "03 E9 ? ? ? ? CC CC CC CC 48 8D 0D")
    if (address == nil) then
        return
    else
        address = address + 0x06
    end
    local targetAddress = address + address:readInt32(-4)
    targetAddress:writeBytes(asm("x64", "ret"))
end

return {
    main = ModCheckPatch
}

--[[
You can also use this format for patches:

local function applyPatch(address)
-- Compiled assembly:
    address:writeBytes(asm("x64", "ret"))
-- Or directly:
    address[0] = 0xC3
-- And so on...
end

local function ModCheckPatch(context)
    local address = context:address()
    address = address + 0x06
    local targetAddress = address + address:readInt32(-4)
    applyPatch(targetAddress)
end

return {
    {
        pattern = "03 E9 ? ? ? ? CC CC CC CC 48 8D 0D",
        match = ModCheckPatch
    }
}

--]]
