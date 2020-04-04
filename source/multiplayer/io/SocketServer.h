
#ifndef GAME_SOCKETSERVER_H
#define GAME_SOCKETSERVER_H

class Socket;

typedef std::shared_ptr<Socket> SharedSocket;

class SocketServer
{
  public:
    explicit SocketServer(int port) : port(port) {};

    const int port;

    std::function<void(SharedSocket)> onNewSocket;

    virtual void start() = 0;

    virtual void stop() = 0;
};


#endif
