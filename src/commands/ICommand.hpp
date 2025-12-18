#pragma once

namespace Command {
    class ICommand {
    public:
        virtual ~ICommand() = default;
        virtual void Execute() = 0;
    };
}