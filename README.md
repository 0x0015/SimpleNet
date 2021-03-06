# SimpleNet
An oversimplified TCP networking library.

Depends on SDL_net

Usage:
* Create server or client class<br>
* Create a service(with either `service::createService<T>(std::string name, [optional] std::function<void(T)>)` for a service that will call a function, or `service::createForwardService(std::stringname )` for a server that will just forward those messages to the other clients connected.)<br>
* add the service to the client or server with `client::addService(servicePtr)` or `server::addService(servicePtr)`<br>
* connect/start with `client::connect(std::string ip, unsigned int port)` or `server::start(unsigned int port)` respectively.<br>
* at intervals(eg. every frame or 10 in a video game) throughout the connection make sure to run `client::update()` or `server::update()` to process messages.  Alternatively if you don't need it to be done on the main thread you can also set `server::autoUpdate` or `client::autoUpdate` to true to have it automatically update at a set interval changeable with `server::autoUpdateTime` or `client::autoUpdateTime` before starting/connectiong the server/client.<br>
<br>
see the example programs testServer.cpp and testClient.cpp<br>
