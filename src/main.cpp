#include "App.hpp"

#include "Core/Context.hpp"

#include <filesystem>
#include <system_error>

namespace {
void UseExecutableDirectoryAsWorkingDirectory(const char* executablePath) {
    const std::filesystem::path path(executablePath);
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
