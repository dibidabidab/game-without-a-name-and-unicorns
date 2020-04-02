
#ifndef GAME_MULTIPLAYERIO_H
#define GAME_MULTIPLAYERIO_H

#include <utils/math_utils.h>
#include <map>
#include <utils/hashing.h>


#include "SocketServer.h"

struct PacketListener
{

    std::function<void*(PacketListener*, const char *data, int size)> preparer;

    std::function<void(PacketListener*, void*)> handler;

    void *realPreparer, *realHandler;

};

typedef uint32 PacketTypeHash;

class MultiplayerIO
{

    std::map<PacketTypeHash, PacketListener*> packetListeners;

  public:

    template <class PacketType>
    using Preparer = std::function<PacketType*(const char *data, int size)>;

    template <class PacketType>
    using Handler = std::function<void(PacketType*)>;

    template <class PacketType>
    void addPacketListener(

            Preparer<PacketType> preparer,

            Handler<PacketType> handler
    )
    {
        auto listener = new PacketListener;

        listener->realPreparer = (void *) new Preparer<PacketType>(preparer);
        listener->realHandler = (void *) new Handler<PacketType>(handler);

        listener->preparer = [&](auto listener, auto data, auto size)
        {
            auto realPreparer = (Preparer<PacketType> *) listener->realPreparer;

            return (void *) realPreparer->operator()(data, size);
        };

        listener->handler = [&](auto listener, void *packet)
        {
            auto realHandler = (Handler<PacketType> *) listener->realHandler;

            realHandler->operator()((PacketType *) packet);
        };

        packetListeners[typeHashCrossPlatform<PacketType>()] = listener;
    }

    void test(const char *data, int size)
    {
        static int typeHashSize = sizeof(PacketTypeHash);

        assert(size >= typeHashSize);

        PacketTypeHash packetType = ((PacketTypeHash *) data)[0];

        auto listener = packetListeners[packetType];

        void *packet = listener->preparer(listener, data + typeHashSize, size - typeHashSize);
        listener->handler(listener, packet);
    }

};

#endif
