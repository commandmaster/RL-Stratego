#include <string>
#include <sio_client.h>
#include <sio_message.h>
#include <iostream>

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


	NetworkManager (const NetworkManager&) = delete;
	NetworkManager& operator= (const NetworkManager&) = delete;
};