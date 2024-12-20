#include <emscripten.h>
#include <string>
#include <memory>
#include <vector>
#include <stdint.h>

class NetworkManager 
{
private:
    std::string address;

    void initializeSocketIO(const std::string& address) {
        EM_ASM(
            {
                var serverAddress = UTF8ToString($0);

                // Load Socket.IO script
                var script = document.createElement('script');
                script.src = "https://cdn.socket.io/4.8.1/socket.io.min.js";

                script.onload = function() {
                    console.log('Socket.io loaded!');
                    window.socket = io(serverAddress);
                    console.log('Connected to server at:', serverAddress);
                };
                document.head.appendChild(script);
            },
            address.c_str()
        );
    }

    void sendMessageToJS(const char* eventName, const char* message) {
        EM_ASM(
            {
                var eventName = UTF8ToString($0);
                var message = UTF8ToString($1);

                if (window.socket) {
                    window.socket.emit(eventName, message);
                } else {
                    console.error('Socket.io is not initialized yet!');
                }
            },
            eventName, message
        );
    }



public:
    NetworkManager(const int port)
        : address{ std::string("http://localhost:") + std::to_string(port) }
    {
        initializeSocketIO(address);
    }

    NetworkManager(const std::string& address)
        : address{ address }
    {
        initializeSocketIO(address);
    }

    void sendMessage(const std::string& eventName, const std::string& message) {
        sendMessageToJS(eventName.c_str(), message.c_str());
    }

    void sendMessage(const char* eventName, const std::string& message) {
        sendMessageToJS(eventName, message.c_str());
    }

    void sendMessage(const char* eventName, const char* message) {
        sendMessageToJS(eventName, message);
    }

    void sendMessage(const std::string& eventName, const char* message) {
        sendMessageToJS(eventName.c_str(), message);
    }

    void sendBinary(const char* eventName, const std::vector<uint8_t>& binaryData) {
        EM_ASM(
            {
                var eventName = UTF8ToString($0);
                var dataSize = $1;
                var dataPtr = $2;

                // Convert C++ binary data to a JavaScript Uint8Array
                var binaryData = new Uint8Array(Module.HEAPU8.buffer, dataPtr, dataSize);

                if (window.socket) {
                    window.socket.emit(eventName, binaryData);
                } else {
                    console.error('Socket.io is not initialized yet!');
                }
            },
            eventName, binaryData.size(), binaryData.data()
        );
    }

    void sendBinary(const char* eventName, const char* binaryData, size_t dataSize) {
        EM_ASM(
            {
                var eventName = UTF8ToString($0);
                var dataSize = $1;
                var dataPtr = $2;

                var binaryData = new Uint8Array(Module.HEAPU8.buffer, dataPtr, dataSize);

                if (window.socket) {
                    window.socket.emit(eventName, binaryData);
                } else {
                    console.error('Socket.io is not initialized yet!');
                }
            },
            eventName, dataSize, binaryData
        );
    }

    void sendBinary(const char* eventName, const uint8_t* binaryData, size_t dataSize) {
        sendBinary(eventName, reinterpret_cast<const char*>(binaryData), dataSize);
    }

	NetworkManager (const NetworkManager&) = delete;
	NetworkManager& operator= (const NetworkManager&) = delete;
};