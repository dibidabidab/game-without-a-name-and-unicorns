
#ifndef GAME_SOCKET_H
#define GAME_SOCKET_H

#include <string>
#include <functional>

class Socket
{
  public:

    Socket(std::string url) : url(url) {}

    /**
     * URL this socket is connected to.
     */
    const std::string url;

    std::function<void()>
            onOpen              = [](){},
            onConnectionFailed  = [](){},
            onClose             = [](){};
    std::function<void(const char *data, int size)>
            onMessage           = [](auto, auto){};


    /**
     * Try to open the connection
     */
    virtual void open() = 0;

    /**
     * send data over the socket
     */
    virtual void send(const char *data, unsigned int length) = 0;

    /**
     * close the socket
     */
    virtual void close() = 0;

    /**
     * Returns true if the socket is ready to use
     */
    virtual bool isOpened() const = 0;
};


#endif
