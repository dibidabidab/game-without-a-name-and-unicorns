
#ifndef GAME_MULTIPLAYERIO_H
#define GAME_MULTIPLAYERIO_H

#include <utils/math_utils.h>
#include <map>
#include <mutex>
#include <utils/hashing.h>
#include <json.hpp>
#include <utils/gu_error.h>
#include <utils/type_name.h>


#include "SocketServer.h"
#include "Socket.h"


struct PacketReceiver
{
    std::function<void*(PacketReceiver*, const char *data, int size)> function;

    template <class PacketType>
    using Func = std::function<PacketType*(const char *data, int size)>;

    void *realFunction; // can be casted to Func<?>*
};

struct PacketHandler
{
    std::function<void(PacketHandler*, void*)> function;

    template <class PacketType>
    using Func = std::function<void(PacketType*)>;

    void *realFunction; // can be casted to Func<?>*
};

struct PacketSender
{
    std::function<void(PacketSender*, void*, std::vector<char> &out)> function;

    template <class PacketType>
    using Func = std::function<void(PacketType*, std::vector<char> &out)>;

    void *realFunction; // can be casted to Func<?>*

};


typedef uint32 PacketTypeHash;

template <class Type>
inline PacketTypeHash typeHashCrossPlatform()
{
    return hashStringCrossPlatform(getTypeName<Type>()) ;
}


class MultiplayerIO
{
  public:

    enum PacketHandlingMode
    {
        IMMEDIATELY_ON_SOCKET_THREAD,
        AFTER_CALLING_handle_packets_ON_ANY_THREAD
    };

  private:

    std::map<PacketTypeHash, std::string> packetTypeNames;
    std::map<PacketTypeHash, PacketReceiver*> packetReceivers;
    std::map<PacketTypeHash, PacketHandler*> packetHandlers;
    std::map<PacketTypeHash, PacketSender*> packetSenders;

    std::mutex unhandledPacketsMutex;
    std::map<PacketTypeHash, std::vector<void *>> unhandledPackets;

    int packetsReceived = 0;

    const PacketHandlingMode handlingMode;

  public:

    const SharedSocket socket;

    MultiplayerIO(SharedSocket, PacketHandlingMode=AFTER_CALLING_handle_packets_ON_ANY_THREAD);

    template <class Type>
    void addPacketReceiver(PacketReceiver::Func<Type> func)
    {
        PacketTypeHash hash = typeHashCrossPlatform<Type>();
        registerName<Type>();

        auto receiver = packetReceivers[hash] = new PacketReceiver;

        receiver->realFunction = (void *) new PacketReceiver::Func<Type>(func);

        receiver->function = [](auto receiver, auto data, auto size)
        {
            auto realFunction = (PacketReceiver::Func<Type> *) receiver->realFunction;

            return (void *) realFunction->operator()(data, size);
        };
    }

    template <class Type>
    void addPacketHandler(PacketHandler::Func<Type> func)
    {
        PacketTypeHash hash = typeHashCrossPlatform<Type>();
        registerName<Type>();

        auto handler = packetHandlers[hash] = new PacketHandler;

        handler->realFunction = (void *) new PacketHandler::Func<Type>(func);

        handler->function = [](auto handler, void *packet)
        {
            auto realFunction = (PacketHandler::Func<Type> *) handler->realFunction;

            realFunction->operator()((Type *) packet);
        };
    }

    template <class Type>
    void addJsonPacketHandler(PacketHandler::Func<Type> func)
    {
        addJsonPacketType<Type>();
        addPacketHandler<Type>(func);
    }

    template <class Type>
    void addPacketSender(PacketSender::Func<Type> func)
    {
        PacketTypeHash hash = typeHashCrossPlatform<Type>();
        registerName<Type>();

        auto sender = packetSenders[hash] = new PacketSender;

        sender->realFunction = (void *) new PacketSender::Func<Type>(func);

        sender->function = [](auto sender, void *packet, auto &out)
        {
            auto realFunction = (PacketSender::Func<Type> *) sender->realFunction;

            realFunction->operator()((Type *) packet, out);
        };
    }

    template <class Type>
    void addJsonPacketType()
    {
        addPacketReceiver<Type>([](auto data, auto size) {

            auto packet = new Type;

            json::from_cbor(data, size).get_to(*packet);

            return packet;
        });
        addPacketSender<Type>([](Type *packet, std::vector<char> &out) {

            json j = *packet;
            json::to_cbor(j, out);
        });
    }

    template <class Type>
    void send(Type &packet)
    {
        PacketTypeHash hash = typeHashCrossPlatform<Type>();

        static int typeHashSize = sizeof(PacketTypeHash);

        std::vector<char> out(typeHashSize);
        memcpy(&out[0], &hash, typeHashSize);

        auto sender = packetSenders[hash];

        if (!sender)
            throw gu_err("tried to send packet, but no sender was registered.");

        sender->function(sender, &packet, out);

        if (socket)
            socket->send(&out[0], out.size());
    }

    void handlePackets();
    
    void printTypes();

    int nrOfPacketsReceived() const { return packetsReceived; }

    ~MultiplayerIO();

  private:

    void handleInput(const char *data, int size);

    template <class Type>
    void registerName()
    {
        PacketTypeHash hash = typeHashCrossPlatform<Type>();
        packetTypeNames[hash] = getTypeName<Type>();
    }

    void handlePacket(PacketTypeHash typeHash, void *packet);

};

#endif
