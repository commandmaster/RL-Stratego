#pragma once

#include <emscripten.h>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include <string>
#include <memory>
#include <vector>
#include <stdint.h>
#include <functional>
#include <cstdlib>
#include <iostream>

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
    std::string socketID;

    void initializeSocketIO(const std::string& address) {
        EM_ASM(
            {
                var serverAddress = UTF8ToString($0);
                console.log('Socket.io loaded!');
                window.socket.io.uri = serverAddress;
                window.socketID = window.socket.connect();
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

	void onJson(const std::string& eventName, const std::function<void(rapidjson::Document&)>& callback) 
    {
		auto callbackWrapper = new std::function<void(rapidjson::Document&)>(callback);

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
							'handleJsonFromJS', // C++ function name
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

    void onBinary(const std::string& eventName, const std::function<void(std::vector<uint8_t>&)>& callback)
	{
		auto callbackWrapper = new std::function<void(std::vector<uint8_t>&)>(callback);

		EM_ASM(
			{
				var eventName = UTF8ToString($0);
				var callbackId = $1;

				console.log("Setting up binary listener for event:", eventName);

				if (window.socket) {
					console.log("Created binary event listener for:", eventName);

					window.socket.on(eventName, function(message) {
						if (message instanceof ArrayBuffer || message instanceof Uint8Array) {
							// Convert the message to a Uint8Array if it's not already
							var binaryData = new Uint8Array(message);

							// Allocate memory in WASM and copy the data
							var ptr = Module._malloc(binaryData.length);
							Module.HEAPU8.set(binaryData, ptr);

							Module.ccall(
								'handleBinaryFromJS', // C++ function name
								null,                 // return type
								['number', 'number', 'number'], // parameter types
								[callbackId, ptr, binaryData.length]
							);

							// Free the memory after use
							Module._free(ptr);
						} else {
							console.error("Received non-binary data for binary event:", eventName);
						}
					});
				} else {
					console.error("Socket.io is not initialized yet!");
				}
			},
			eventName.c_str(),
			reinterpret_cast<intptr_t>(callbackWrapper)
		);
	}


    std::string getSocketId() const
    {
        const void* jsResult = EM_ASM_PTR({
            var result = window.socketID;
            var length = lengthBytesUTF8(result) + 1; 
            var buffer = _malloc(length); 
			stringToUTF8(result, buffer, length); 
			return buffer; 
        });

        std::string result(reinterpret_cast<const char*>(jsResult));
        free((void*)jsResult);

        return result;
    }

    void sendBinary(const char* eventName, const uint8_t* binaryData, size_t dataSize) {
        sendBinary(eventName, reinterpret_cast<const char*>(binaryData), dataSize);
    }



	NetworkManager (const NetworkManager&) = delete;
	NetworkManager& operator= (const NetworkManager&) = delete;
};

