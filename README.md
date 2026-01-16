# hitbox

饿狼传说判定框显示

## 使用
- launcher.exe
  1. 启动游戏
  2. 启动launcher.exe
- Proxy.dll
  [LoadDLL](Proxy/README.md#LoadDLL)

- DLLInjector

  ```cmd
  DLLInjector <DLLPath> -pid <ProcessID>
  ```
  or
  ```
  DLLInjector <DLLPath> -pname CotW-Win64-Shipping.exe
  ```

## 判定框

- 攻击框
- 身体框
- 受击框

## 编译

- Virtual Studio 2022

## 关于报毒

- launcher软件本身有注入行为，所以会被各家杀软件拦截

## 感谢

- 感谢[Redundantcy](https://space.bilibili.com/107436250)提供程序支持
- [ImGui](https://github.com/ocornut/imgui)
- [MinHook](https://github.com/TsudaKageyu/minhook)
- [asmjit](https://github.com/asmjit/asmjit)
- [asmtk](https://github.com/asmjit/asmtk)
- [Lua](https://lua.org)
