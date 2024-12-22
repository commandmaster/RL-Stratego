#include "networking.h"

extern "C" void handleMessageFromJS(intptr_t callbackId, const char* message)
{
    auto callback = reinterpret_cast<std::function<void(const std::string&)>*>(callbackId);
    if (callback) {
        (*callback)(std::string(message));
    }
}

