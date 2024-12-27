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

extern "C" void handleBinaryFromJS(intptr_t callbackId, uint8_t* data, size_t length)
{
    auto callback = reinterpret_cast<std::function<void(std::vector<uint8_t>&)>*>(callbackId);

    std::vector<uint8_t> binaryData(data, data + length);

    (*callback)(binaryData);
}


