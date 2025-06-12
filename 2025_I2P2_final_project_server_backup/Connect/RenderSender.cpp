// RenderSender.cpp
#include "RenderSender.hpp"
#include "Enemy/Enemy.hpp"
#include <iostream>
#include <chrono>
using namespace std::chrono;
std::string compress_string(const std::string& str);
std::string decompress_string(const std::string& str, uLong estimated_size = 10 * 1024 * 1024);
void sendCompressedJson(SOCKET sock, const nlohmann::json& j);

RenderSender::RenderSender(int port) {
#ifdef _WIN32
    WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSock, NUM);
    std::cout << "Listening on port " << port << "...\n";
}

RenderSender::~RenderSender() {
    closesocket(serverSock);
#ifdef _WIN32
    WSACleanup();
#endif

    for (auto& ctx : clients) {
        ctx->active = false;
        if (ctx->recvThread.joinable()) ctx->recvThread.join();
        if (ctx->sendThread.joinable()) ctx->sendThread.join();
    }
}


void RenderSender::start() {
    
    // accept
    std::thread([this]() {
        
        std::cout << "Client accept thread started.\n";
        while (true) {
            SOCKET clientSock = accept(serverSock, nullptr, nullptr);
            u_long mode = 1;
            ioctlsocket(clientSock, FIONBIO, &mode);
            if (clientSock == INVALID_SOCKET) continue;

            auto context = std::make_shared<ClientContext>();
            context->socket = clientSock;

            // give a client id
            context->id = nextClientId++;
            std::cout << "Client connected with ID: " << context->id << "\n";

            {
                std::lock_guard<std::mutex> lock(clientMutex);
                clients.push_back(context);
            }
        }
    }).detach();

   
    using namespace std::chrono;
    const auto interval = milliseconds(1000 / TARGET_FPS);
    auto last_time = steady_clock::now();
    float deltaTime;
    auto now = steady_clock::now();
    deltaTime = duration_cast<duration<float>>(now - last_time).count();
    last_time = now;
    while (true) {
        auto now = steady_clock::now();
        deltaTime = duration_cast<duration<float>>(now - last_time).count();
        last_time = now;
        auto start_time = steady_clock::now();
        if(clients.size() == 0){
            storedMapState.reset();
            // std::cerr<<"Clear Map!"<<'\n';
        }
        // 1. recv from all client
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            for (auto& ctx : clients) {
                if (ctx->active)
                    recvOnce(ctx);
            }

            // 2. collect all data
            frame.clear();
            for (auto& ctx : clients) {
                if (ctx->active) {
                    frame[std::to_string(ctx->id)] = ctx->lastInput;
                }
            }
            //update enemy
            for (auto& enemy : enemies) {
                // std::cerr<<"hello enemy "<<enemy.id<<' '<<"alive? "<<enemy.alive<<'\n';
                if (enemy.alive) {
                    
                    UpdateEnemyInstance(enemy, deltaTime, *this);
                    nlohmann::json obj;
                    obj["type"] = "-1";
                    obj["id"] = enemy.id;
                    obj["enemyType"] = enemy.type;
                    obj["x"] = enemy.x;
                    obj["y"] = enemy.y;
                    obj["rotation"] = enemy.rotation;
                    obj["alive"] = true;
                    obj["damage"] = enemy.damage;
                    AddToFrame(obj);
                }
            }
            // 3. send to all clients
            for (auto& ctx : clients) {
                if (ctx->active){
                    frame["my_id"] = ctx->id;
                    if(ctx->sendMap){ 
                        std::cerr<<"send Map to "<<ctx->id<<'\n';
                        frame["map"] = storedMapState;
                    }else{
                        frame.erase("map");
                    } 
                    ctx->sendMap = false;
                    sendOnce(ctx);
                    
                }
                    
            }
            
            
            cleanupInactiveClients();
        }

        
        auto elapsed = steady_clock::now() - start_time;
        if (elapsed < interval)
            std::this_thread::sleep_for(interval - elapsed);
    }
}

char buffer[1000000];
void RenderSender::recvOnce(std::shared_ptr<ClientContext> ctx) {
    
    int len = recv(ctx->socket, buffer, sizeof(buffer), 0);
    
    if (len > 0) {
        std::string raw(buffer, len);
        size_t start = 0;
        ctx->lastInput = nullptr; // Clear previous messages

        while (start < raw.size()) {
            size_t end = raw.find('\n', start);
            if (end == std::string::npos) break;
            std::string line = raw.substr(start, end - start);
            start = end + 1;
            try {
                ctx->lastInput = nlohmann::json::parse(line);
            } catch (...) {
                std::cerr << "Invalid JSON chunk from client.\n";
            }
        }
        // std::cerr<<"recv: "<<ctx->lastInput<<'\n';

        if(ctx->lastInput.contains("map")){
            if(clients.size() == 1 || !storedMapState.has_value()){
                storedMapState = ctx->lastInput;

                        // 重建 EnemyInstance 清單
                enemies.clear(); // 清空原本的怪物清單

                const auto& map = storedMapState.value()["map"]["MapState"];
                int h = map.size();
                
                int w = map[0].size();
                int idCounter = 0;
                std::cerr<<"h "<<h<<" w "<< w<<'\n';
                for (int y = 0; y < h; ++y) {
                    for (int x = 0; x < w; ++x) {
                        const auto& cell = map[y][x];
                        if (!cell.contains("SpawnPoint")) continue;
                    
                        // 讀取 type，若沒指定則設為 0
                        int type = 0;
                        if (cell.contains("Type") && cell["Type"].is_number_integer())
                            type = cell["Type"];
                    
                        EnemyInstance enemy;
                        enemy.id = idCounter++;
                        enemy.type = type;
                        enemy.x = x * blockSize + blockSize / 2;
                        enemy.y = y * blockSize + blockSize / 2;
                        enemy.spawnX = enemy.x;
                        enemy.spawnY = enemy.y;
                        enemy.alive = true;
                        enemy.cooldownTimer = 0;
                        // enemy.path = ... ← 可加上自動導向起點的 path
                        enemies.push_back(enemy);
                        std::cerr<<"enemy id: "<<enemy.id<<' '<<enemy.x<<' '<<enemy.y<<'\n';
                    }
                }
                std::cerr << "Spawned " << enemies.size() << " enemies.\n";
            }

            
            ctx->sendMap = true;
        }
    } else if (len == 0) {
        // client Close the connection normally
        ctx->active = false;
        closesocket(ctx->socket);
    } else {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            // Close only if it is a real error
            std::cerr << "recv error: " << err << "\n";
            ctx->active = false;
            closesocket(ctx->socket);
        }
        // Otherwise, there is no information and no work.
    }
}
void RenderSender::sendOnce(std::shared_ptr<ClientContext> ctx) {
    if (!ctx->active) return;
    // if(frame.contains("map"))
    std::cerr<<"frame  "<<frame.dump()<<'\n';
    std::string raw = frame.dump(); // 原始 JSON 字串
    // std::cerr << "[send raw size]: " << raw.size() << " bytes\n";
    // std::cerr << "[send raw content]: " << raw << "\n";

    try {
        std::string compressed = compress_string(raw);
        uint32_t len = htonl(compressed.size());

        int header_sent = send(ctx->socket, (char*)&len, sizeof(len), 0);
        int body_sent = send(ctx->socket, compressed.data(), compressed.size(), 0);

        if (header_sent == SOCKET_ERROR || body_sent == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) {
                std::cerr << "send error: " << err << "\n";
                ctx->active = false;
                closesocket(ctx->socket);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Compression or send failed: " << e.what() << "\n";
    }
}

void RenderSender::cleanupInactiveClients() {
    std::lock_guard<std::mutex> lock(clientMutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [](const std::shared_ptr<ClientContext>& ctx) {
            return !ctx->active;
        }), clients.end());
}

std::string compress_string(const std::string& str) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
        throw std::runtime_error("deflateInit failed");

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (ret == Z_STREAM_ERROR) {
            deflateEnd(&zs);
            throw std::runtime_error("deflate failed");
        }

        outstring.append(outbuffer, sizeof(outbuffer) - zs.avail_out);
    } while (ret != Z_STREAM_END);

    deflateEnd(&zs);
    return outstring;
}

std::string decompress_string(const std::string& str, uLong estimated_size) {
    std::string out(estimated_size, 0);
    uLong destLen = out.size();
    if (uncompress((Bytef*)&out[0], &destLen, (const Bytef*)str.data(), str.size()) != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }
    out.resize(destLen);
    return out;
}

void sendCompressedJson(SOCKET sock, const nlohmann::json& j) {
    std::string raw = j.dump();
    std::string compressed = compress_string(raw);
    uint32_t len = htonl(compressed.size());
    send(sock, (char*)&len, sizeof(len), 0);
    send(sock, compressed.data(), compressed.size(), 0);
}

