#include <string>
#include <sio_client.h>
#include <sio_message.h>

#include <iostream>
#include <vector>
#include <memory>
#include <stdint.h>

class NetworkManager
{
private:
	std::string address;
	sio::client client;

public:
	NetworkManager(const int port)
		: address{ std::string("http://localhost:") + std::to_string(port) }, client{}
	{
		client.connect(address);
		client.set_open_listener([&]() 
		{
			std::cout << "Connected to " << address << std::endl;
		});
	}

	NetworkManager(const std::string address)
		: address{ address }, client{}
	{
		client.connect(address);
		client.set_open_listener([&]() 
		{
			std::cout << "Connected to " << address << std::endl;
		});
	}

	~NetworkManager()
	{
		client.close();
	}

	std::string getAddress() const 
	{
		return address;
	}

	void sendMessage(std::string eventName, std::string message)
	{
		client.socket()->emit(eventName.c_str(), sio::string_message::create(message));
	}

	void sendMessage(const char* eventName, std::string message)
	{
		client.socket()->emit(eventName, sio::string_message::create(message));
	}

	void sendMessage(const char* eventName, const char* message)
	{
		client.socket()->emit(eventName, sio::string_message::create(message));
	}

	void sendMessage(std::string eventName, const char* message)
	{
		client.socket()->emit(eventName.c_str(), sio::string_message::create(message));
	}

	void sendBinary(const std::string& eventName, const std::vector<uint8_t>& binaryData)
	{
		auto binaryString = std::make_shared<std::string>(binaryData.begin(), binaryData.end());

		client.socket()->emit(eventName.c_str(), sio::binary_message::create(binaryString));
	}

	void sendBinary(const std::string& eventName, const char* binaryData, size_t dataSize) {
        auto binaryString = std::make_shared<std::string>(binaryData, dataSize);

        client.socket()->emit(eventName.c_str(), sio::binary_message::create(binaryString));
    }

	void sendBinary(const std::string& eventName, const uint8_t* binaryData, size_t dataSize) {
        auto binaryString = std::make_shared<std::string>(reinterpret_cast<const char*>(binaryData), dataSize);

        client.socket()->emit(eventName.c_str(), sio::binary_message::create(binaryString));
    }


	NetworkManager (const NetworkManager&) = delete;
	NetworkManager& operator= (const NetworkManager&) = delete;
};