
#include <iostream>
#include "MultiplayerIO.h"

MultiplayerIO::MultiplayerIO(Socket *socket) : socket(socket)
{
    socket->onMessage = [&](auto data, auto size)
    {
        handleInput(data, size);
    };
}

void MultiplayerIO::handleInput(const char *data, int size)
{
    static int typeHashSize = sizeof(PacketTypeHash);

    if (size < typeHashSize)
    {
        std::cerr << "Received packet of less than " << typeHashSize << "bytes. Could not determine type.\n";
        return;
    }

    PacketTypeHash packetType = ((PacketTypeHash *) data)[0];

    if (!packetReceivers[packetType])
    {
        if (packetTypeNames.count(packetType))
            std::cerr << "Received packet of type " << packetTypeNames[packetType] << ", but no receiver was registered!\n";
        else
            std::cerr << "Received packet of unregistered type " << packetType << "!\n";
        return;
    }

    auto receiver = packetReceivers[packetType];

    try
    {
        void *packet = receiver->function(receiver, data + typeHashSize, size - typeHashSize);

        packetsReceived++;

        auto handler = packetHandlers[packetType];
        handler->function(handler, packet);

    } catch(const std::exception& e) {
        std::cerr << "Caught exception while handling " << packetTypeNames[packetType] << "-packet:\n" << e.what() << "\n";
    }
}

MultiplayerIO::~MultiplayerIO()
{
    for (auto &r : packetReceivers) delete r.second;
    for (auto &h : packetHandlers) delete h.second;
    for (auto &s : packetSenders) delete s.second;

    std::cout << "io ended\n";
}

