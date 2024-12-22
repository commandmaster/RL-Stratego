#pragma once

#include <string>
#include <sio_client.h>
#include <sio_message.h>

#include <iostream>
#include <vector>
#include <memory>
#include <stdint.h>
#include <mutex>






class NetworkManager
{
private:
	std::string address;
	sio::client client;
	std::mutex event_mutex;

	struct Listener {
		std::string eventName;
		std::function<void(const std::string&)> callback;

		Listener(const std::string& event, std::function<void(const std::string&)> cb)
			: eventName(event), callback(std::move(cb)) {}
	};

public:
	static NetworkManager& getInstance() 
	{
		static NetworkManager instance;
        return instance;
    }

	NetworkManager()
		: client{}
	{
		client.set_open_listener([&]() 
		{
			std::cout << "Connected to " << this->address << std::endl;
		});
	}

	~NetworkManager()
	{
		client.close();
	}

	void connect(const std::string& address)
	{
		client.connect(address);
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

	void onMessage(const std::string& eventName, const std::function<void(const std::string&)>& callback) {
        auto& socket = client.socket();
		socket->on(eventName, [callback](sio::event ev) {
			callback(ev.get_message()->get_string());
		});
     
	}

	NetworkManager (const NetworkManager&) = delete;
	NetworkManager& operator= (const NetworkManager&) = delete;
};