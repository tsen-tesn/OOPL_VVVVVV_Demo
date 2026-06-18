#include "App.hpp"

#include "Core/Context.hpp"

#include <filesystem>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
std::filesystem::path GetExecutablePath(const char* executablePath) {
#ifdef _WIN32
    char buffer[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    if (length > 0)
        return std::filesystem::path(buffer);
#endif
    return std::filesystem::path(executablePath);
}

void UseExecutableDirectoryAsWorkingDirectory(const char* executablePath) {
    const std::filesystem::path path = GetExecutablePath(executablePath);
    if (!path.has_parent_path())
        return;

    std::error_code error;
    std::filesystem::current_path(path.parent_path(), error);
}
}

int main(int, char** argv) {
    UseExecutableDirectoryAsWorkingDirectory(argv[0]);

    auto context = Core::Context::GetInstance();
    App app;

    while (!context->GetExit()) {
        switch (app.GetCurrentState()) {
            case App::State::START:
                app.Start();
                break;

            case App::State::UPDATE:
                app.Update();
                break;

            case App::State::END:
                app.End();
                context->SetExit(true);
                break;
        }
        context->Update();
    }
    return 0;
}
