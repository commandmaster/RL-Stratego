#include <string>
#include <iostream>
#include <emscripten.h>

class NetworkManager
{
private:
	std::string address;

public:
	NetworkManager(const int port)
    : address{ std::string("http://localhost:") + std::to_string(port) }
	{
		EM_ASM(
			{
				var serverAddress = UTF8ToString($0);

				var script = document.createElement('script');
				script.src = "https://cdn.socket.io/4.8.1/socket.io.min.js";

				script.onload = function() {
					console.log('Socket.io loaded!');
					const socket = io(serverAddress);
					console.log('Connected to server at:', serverAddress);
				};
				document.head.appendChild(script);
			},
			address.c_str() 
		);
	}


	NetworkManager(const std::string address)
		: address{ address }
	{
		EM_ASM(
			{
				var serverAddress = UTF8ToString($0);

				var script = document.createElement('script');
				script.src = "https://cdn.socket.io/4.8.1/socket.io.min.js";

				script.onload = function() {
					console.log('Socket.io loaded!');
					const socket = io(serverAddress);
					console.log('Connected to server at:', serverAddress);
				};
				document.head.appendChild(script);
			},
			address.c_str() 
		);
	}

	std::string getAddress() const 
	{
		return address;
	}

	void sendMessage(std::string eventName, std::string message)
	{
	}

	void sendMessage(const char* eventName, std::string message)
	{
	}

	void sendMessage(const char* eventName, const char* message)
	{
	}

	void sendMessage(std::string eventName, const char* message)
	{
	}

	NetworkManager (const NetworkManager&) = delete;
	NetworkManager& operator= (const NetworkManager&) = delete;
};