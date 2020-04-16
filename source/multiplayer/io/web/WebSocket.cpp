
#include "WebSocket.h"

#ifdef EMSCRIPTEN

struct EmscriptenCallbackFunctions
{
    static EM_BOOL onOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData)
    {
        std::cout << ((WebSocket *) userData)->url << "\n";

        WebSocket *sock = (WebSocket *) userData;
        sock->opened = true;

//        printf("open(eventType=%d)\n", eventType);

        sock->onOpen();
        return 0;
    }

    static EM_BOOL onClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData)
    {
//        printf("close(eventType=%d, wasClean=%d, code=%d, reason=%s)\n", eventType, e->wasClean, e->code, e->reason);

        WebSocket *sock = (WebSocket *) userData;

        if (sock->opened)
        {
            sock->closed = true;
            sock->onClose();
        }
        else
            sock->onConnectionFailed();

        return 0;
    }

    static EM_BOOL onError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
    {
        printf("error(eventType=%d, userData=%d)\n", eventType, (int)userData);
        return 0;
    }

    static EM_BOOL onMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
    {
//        printf("message(eventType=%d, data=%p, numBytes=%d, isText=%d)\n", eventType, e->data, e->numBytes, e->isText);

        WebSocket *sock = (WebSocket *) userData;

        if (e->isText)
            printf("IGNORED text data: \"%s\"\n", e->data);
        else
        {
//            printf("binary data:");
//            for(int i = 0; i < e->numBytes; ++i)
//                printf(" %02X", e->data[i]);
//            printf("\n");

            sock->onMessage((char *) e->data, e->numBytes);
        }
        return 0;
    }
};
#else


WebSocket::WebSocket(websockcon connection)
    :   connectionCreatedByServer(connection),
        opened(true),
        Socket("[" + connection->get_raw_socket().remote_endpoint().address().to_string() + "]:"
            + std::to_string(connection->get_raw_socket().remote_endpoint().port()))
{
}

WebSocket::~WebSocket()
{
    if (client && !client->stopped())
        std::cerr << "WebSocket-object to " << url << " destroyed before actual socket was closed. Did the object go out of scope?";
    delete client;
    std::cout << "WebSocket ended\n";
}

#endif


WebSocket::WebSocket(const std::string &url) : Socket(url)
{
}


void WebSocket::open()
{
    if (opened)
        throw gu_err("WebSocket to " + url + " is already open");

    std::cout << "Opening websocket to " << url << '\n';
    #ifdef EMSCRIPTEN
    // code to setup a WebSocket in the browser:

    if (!emscripten_websocket_is_supported())
        throw gu_err("Websockets are not supported in this browser");

    EmscriptenWebSocketCreateAttributes attr;
    emscripten_websocket_init_create_attributes(&attr);
    attr.url = url.c_str();

    emSockId = emscripten_websocket_new(&attr);
    if (emSockId <= 0)
    {
        printf("WebSocket creation failed, error code %d!\n", (EMSCRIPTEN_RESULT) emSockId);
        onConnectionFailed();
        return;
    }

    emscripten_websocket_set_onopen_callback(emSockId, (void*) this, EmscriptenCallbackFunctions::onOpen);
    emscripten_websocket_set_onclose_callback(emSockId, (void*) this, EmscriptenCallbackFunctions::onClose);
    emscripten_websocket_set_onerror_callback(emSockId, (void*) this, EmscriptenCallbackFunctions::onError);
    emscripten_websocket_set_onmessage_callback(emSockId, (void*) this, EmscriptenCallbackFunctions::onMessage);

    #else
    // code to setup a WebSocket in native desktop version:
    client = new websockclient;

    client->set_access_channels(websocketpp::log::alevel::all);
    client->clear_access_channels(websocketpp::log::alevel::all);
    client->set_error_channels(websocketpp::log::elevel::all);

    client->set_open_handler([&](websocketpp::connection_hdl hdl) {

        opened = true;
        onOpen();
    });
    client->set_message_handler([&](websocketpp::connection_hdl hdl, websockserver::message_ptr msg) {

        onMessage(msg->get_payload().data(), msg->get_payload().size());
    });
    client->set_close_handler([&](websocketpp::connection_hdl hdl) {

        closed = true;
        onClose();
    });
    client->set_fail_handler([&](websocketpp::connection_hdl hdl) {

        onConnectionFailed();
    });
    client->init_asio();

    websocketpp::lib::error_code ec;
    clientConnection = client->get_connection(url, ec);
    client->connect(clientConnection);

    std::thread([&]{
        client->run();
    }).detach();
    #endif
}


void WebSocket::send(const char *data, unsigned int length)
{
    if (closed)
    {
        std::cerr << "tried to send data on already closed WebSocket\n";
        return;
    }
    #ifdef EMSCRIPTEN

    emscripten_websocket_send_binary(emSockId, (void *) data, length);

    #else

    if (connectionCreatedByServer)
        connectionCreatedByServer->send(data, length);
    else
    {
        assert(client != NULL);

        clientConnection->send(data, length);
    }
    #endif
}

void WebSocket::close()
{
    if (closeWasCalled) return;
    closeWasCalled = true;

    #ifdef EMSCRIPTEN

    emscripten_websocket_close(emSockId, 0, 0);

    #else

    if (connectionCreatedByServer)
        connectionCreatedByServer->close(websocketpp::close::status::normal, "yolo");
    else
    {
        assert(client != NULL);

        clientConnection->close(websocketpp::close::status::normal, "yolo");
    }

    #endif
}
