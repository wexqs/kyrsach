#include "Communicate.h"
#include "Logger.h"
#include "Errors.h"
#include "Calculator.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

#define buff_size 1024

std::unique_ptr<char[]> buff(new char[buff_size]);

std::string Communicate::sha256(std::string input_str) {
    using namespace CryptoPP;
    SHA256 hash;
    std::string new_hash;
    StringSource(input_str, true, new HashFilter(hash, new HexEncoder(new StringSink(new_hash))));
    return new_hash;
}

std::string Communicate::generate_salt() {
    using namespace CryptoPP;
    byte raw_salt[8];
    AutoSeededRandomPool prng;
    prng.GenerateBlock(raw_salt, sizeof(raw_salt));

    std::string salt_str;
    ArraySource(raw_salt, sizeof(raw_salt), true, new HexEncoder(new StringSink(salt_str)));
    return salt_str;
}

int Communicate::connection(int port, std::map<std::string, std::string> database, Logger* l1) {
    try {
        int queue_len = 100;
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_aton("127.0.0.1", &addr.sin_addr);
        
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s <= 0) {
            throw crit_err("Socket creation error");
        } else {
            l1->writelog("Listen socket created");
        }

        auto rc = bind(s, (const sockaddr*)&addr, sizeof(sockaddr_in));
        if (rc == -1) {
            throw crit_err("Socket bind error");
        } else {
            l1->writelog("Bind success");
        }

        rc = listen(s, queue_len);
        if (rc == -1) {
            throw crit_err("Socket listen error");
        }

        for (;;) {
            try {
                sockaddr_in client_addr;
                socklen_t len = sizeof(sockaddr_in);
                int work_sock = accept(s, (sockaddr*)&client_addr, &len);
                if (work_sock <= 0) {
                    throw no_crit_err("[Uncritical] Client socket error");
                }
                l1->writelog("Client socket created");

                rc = recv(work_sock, buff.get(), buff_size, 0);
                if (rc <= 0) {
                    close(work_sock);
                    throw no_crit_err("[Uncritical] ID receive error");
                }
                buff[rc] = 0;
                std::string ID(buff.get(), rc);

                if (database.find(ID) == database.end()) {
                    close(work_sock);
                    throw no_crit_err("[Uncritical] Unknown ID");
                }

                std::string salt_s = generate_salt();
                rc = send(work_sock, salt_s.c_str(), salt_s.size(), 0);
                if (rc <= 0) {
                    close(work_sock);
                    throw no_crit_err("[Uncritical] Send SALT error");
                }

                rc = recv(work_sock, buff.get(), 64, 0);
                if (rc <= 0) {
                    close(work_sock);
                    throw no_crit_err("[Uncritical] HASH received error");
                }
                buff[rc] = 0;
                std::string client_hash(buff.get(), rc);

                std::string expected_hash = sha256(salt_s + database[ID]);
                if (client_hash != expected_hash) {
                    close(work_sock);
                    throw no_crit_err("[Uncritical] Auth error");
                }

                rc = send(work_sock, "OK", 2, 0);
                if (rc <= 0) {
                    close(work_sock);
                    throw no_crit_err("[Uncritical] Send OK error");
                }

                // Обработка векторов
                int count;
                rc = recv(work_sock, &count, sizeof(count), 0);
                if (rc <= 0) {
                    close(work_sock);
                    throw no_crit_err("[Uncritical error] Number of vectors not received");
                }

                l1->writelog("Received number of vectors: " + std::to_string(count));

                for (int i = 0; i < count; ++i) {
                    uint32_t vector_len;
                    rc = recv(work_sock, &vector_len, sizeof(vector_len), 0);
                    if (rc <= 0) {
                        close(work_sock);
                        throw no_crit_err("[Uncritical error] Length of vector not received");
                    }

                    l1->writelog("Received length of vector " + std::to_string(i+1) + ": " + std::to_string(vector_len));

                    std::unique_ptr<uint16_t[]> vector_data(new uint16_t[vector_len]);
                    rc = recv(work_sock, vector_data.get(), vector_len * sizeof(uint16_t), 0);
                    if (rc <= 0) {
                        close(work_sock);
                        throw no_crit_err("[Uncritical error] Vector data not received");
                    }

                    std::vector<uint16_t> v(vector_data.get(), vector_data.get() + vector_len);
                    Calculator calc(v);
                    auto res = calc.send_res();
                    rc = send(work_sock, &res, sizeof(res), 0);
                    if (rc <= 0) {
                        close(work_sock);
                        throw no_crit_err("[Uncritical error] Result not sent");
                    }

                    l1->writelog("Result of calculation for vector " + std::to_string(i+1) + " sent");
                }
            } catch (no_crit_err& e) {
                l1->writelog(e.what());
            }
        }
    } catch (crit_err& e) {
        l1->writelog(e.what());
    }
    return 0;
}
