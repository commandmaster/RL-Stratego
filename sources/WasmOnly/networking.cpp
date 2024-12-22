#include "networking.h"

extern "C" void handleMessageFromJS(intptr_t callbackId, const char* message)
{
    auto callback = reinterpret_cast<std::function<void(const std::string&)>*>(callbackId);
    if (callback) {
        (*callback)(std::string(message));
    }
}

extern "C" void handleJsonFromJS(intptr_t callbackId, const char* message)
{
    auto callback = reinterpret_cast<std::function<void(rapidjson::Document&)>*>(callbackId);
    if (callback) {
		rapidjson::Document document;
			
		if (document.Parse(message).HasParseError()) {
            std::cerr << "Failed to parse JSON" << std::endl;;
			return;
		}

        (*callback)(document);
    }
}

