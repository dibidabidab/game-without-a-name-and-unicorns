
#ifndef GAME_SOCKETSERVER_H
#define GAME_SOCKETSERVER_H

class Socket;

class SocketServer
{
  public:
    std::function<void(Socket *)> onNewSocket;

    virtual void start() = 0;

    virtual void stop() = 0;
};


#endif
