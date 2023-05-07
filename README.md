# TSL-Client-Server-Weather

A C++ program for handling SSL connections. The main function of the program creates a socket, listens for connections, and performs communication. When a connection is established, the program reads a message from the client, parses it into JSON format, retrieves the weather for the city received from the client, sends JSON with weather data to the client, and closes the connection.

## Requirements
 To run the program, you need to have the OpenSSL and nlohmann/json libraries installed.

## Installation
 To install the OpenSSL and nlohmann/json libraries, execute the following commands in the terminal:

```bash
sudo apt-get update
sudo apt-get install libssl-dev
sudo apt-get install nlohmann-json3-dev
```

## Compilation
 The program can be compiled using the following command:

```bash
g++ -o program program.cpp -lcurl -lssl -lcrypto -lstdc++fs
```
## Usage

To run the program, execute the following command:

```bash
./program
```

The program will listen on port 2137 on all IP addresses. You can connect to the program using the SSL protocol, e.g., using the openssl s_client program, by executing the following command:

```bash
openssl s_client -connect localhost:2137
```
Once the connection is established, you can send JSON containing the "city" field with the name of the city for which you want to retrieve weather data. The program will send JSON with weather data for the specified city.
