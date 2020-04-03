
#include <iostream>
#include "MultiplayerIO.h"

MultiplayerIO::MultiplayerIO(SharedSocket socket, PacketHandlingMode handlingMode) : socket(socket), handlingMode(handlingMode)
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

    auto receiver = packetReceivers[packetType];

    if (!receiver)
    {
        if (packetTypeNames.count(packetType))
            std::cerr << "Received packet of type " << packetTypeNames[packetType] << ", but no receiver was registered!\n";
        else
            std::cerr << "Received packet of unregistered type " << packetType << "!\n";
        return;
    }
    try
    {
        void *packet = receiver->function(receiver, data + typeHashSize, size - typeHashSize);

        packetsReceived++;

        if (handlingMode == PacketHandlingMode::IMMEDIATELY_ON_SOCKET_THREAD)
        {
           handlePacket(packetType, packet);
        }
        else
        {
            unhandledPacketsMutex.lock();
            unhandledPackets[packetType].push_back(packet);
            unhandledPacketsMutex.unlock();
        }

    } catch(const std::exception& e) {
        std::cerr << "Caught exception while handling " << packetTypeNames[packetType] << "-packet:\n" << e.what() << "\n";
    }
}

MultiplayerIO::~MultiplayerIO()
{
    for (auto &r : packetReceivers) delete r.second;    // TODO: realFunction is not deleted.
    for (auto &h : packetHandlers) delete h.second;
    for (auto &s : packetSenders) delete s.second;

    if (!unhandledPackets.empty())
        std::cerr << "MultiplayerIO deleted before handling remaining packets!\n";

    std::cout << "io ended\n";
}

void MultiplayerIO::printTypes()
{
    for (auto t : packetTypeNames)
        std::cout << t.second << ": " << t.first << '\n';
}

void MultiplayerIO::handlePackets()
{
    unhandledPacketsMutex.lock();
    for (auto &uP : unhandledPackets)
    {
        PacketTypeHash hash = uP.first;

        std::cout << "handle " << uP.second.size() << " " << packetTypeNames[hash] << "-packet(s)\n";

        for (auto packet : uP.second) handlePacket(hash, packet);
        uP.second.clear();
    }
    unhandledPacketsMutex.unlock();
}

void MultiplayerIO::handlePacket(PacketTypeHash typeHash, void *packet)
{
    auto handler = packetHandlers[typeHash];
    if (!handler)
    {
        std::cerr << "No handler found for packet of type " << packetTypeNames[typeHash] << "!\n";
        return;
    }
    handler->function(handler, packet);
}
