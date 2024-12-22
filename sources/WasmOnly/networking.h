#pragma once

#include <emscripten.h>
#include <string>
#include <memory>
#include <vector>
#include <stdint.h>
#include <functional>
#include <cstdlib>

class NetworkManager
{
private:
    struct Listener {
        std::string eventName;
        std::function<void(const std::string&)> callback;

        Listener(const std::string& event, std::function<void(const std::string&)> cb)
            : eventName(event), callback(std::move(cb)) {
        }
    };

    std::string address;
    std::vector<Listener> listeners;

    void initializeSocketIO(const std::string& address) {
        EM_ASM(
            {
                var serverAddress = UTF8ToString($0);
                console.log('Socket.io loaded!');
                window.socket.io.uri = serverAddress;
                window.socket.connect();
                console.log('Connected to server at:', serverAddress);

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
    static NetworkManager& getInstance() {
        static NetworkManager instance;
        return instance;
    }

    NetworkManager()
    { 
        EM_ASM(
            {
                window.socket = io({autoConnect: false, reconnection : false});
            }
        );
    }

    void connect(const std::string& address)
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

    void onMessage(const std::string& eventName, const std::function<void(const std::string&)>& callback) 
    {
		auto callbackWrapper = new std::function<void(const std::string&)>(callback);


        EM_ASM(
            {
				var eventName = UTF8ToString($0);
				var callbackId = $1;

                console.log("tried to setup listener");

				if (window.socket) {
					console.log("created event listener");
					window.socket.on(eventName, function(message) {
						if (typeof message === 'object') {
							message = JSON.stringify(message);
						}
                        
                      	// Pass the message back to C++
						Module.ccall(
							'handleMessageFromJS', // C++ function name
							null,                  // return type
							['number', 'string'],  // parameter types
							[callbackId, message]
						);
					});
				} else {
					console.error('Socket.io is not initialized yet!');
				}
			},
			eventName.c_str(),
			reinterpret_cast<intptr_t>(callbackWrapper)
		);
    }

    void sendBinary(const char* eventName, const uint8_t* binaryData, size_t dataSize) {
        sendBinary(eventName, reinterpret_cast<const char*>(binaryData), dataSize);
    }

	NetworkManager (const NetworkManager&) = delete;
	NetworkManager& operator= (const NetworkManager&) = delete;
};

