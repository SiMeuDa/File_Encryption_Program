#include "OFB.h"
#include "../DES/DES.h"
#include "mode.h"
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>
#include <random>

std::vector<uint64_t> OFB::encrypt_mode(std::string msg, uint64_t& key, uint64_t& key2)
{
        std::vector<uint64_t> imsg;

        //change string to integer vector
        imsg = to_integer(msg);
        //insert IV
        uint64_t IV = random();
        imsg.insert(imsg.begin(), IV);

        size_t total_blocks = imsg.size();
        //exception handling
        if(total_blocks == 0)
                return imsg;

        auto lastUpdateTime = std::chrono::steady_clock::now();

        for(size_t i = 1; i < total_blocks; i++)
        {
                IV = this->cipher(this->decipher(this->cipher(IV, key), key2), key);
                imsg[i] = imsg[i] ^ IV;
                //check current time
                auto currentTime = std::chrono::steady_clock::now();

                std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
                //10 fps loading & block count loading
                if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
                {
                        double fraction = static_cast<double>(i) / (total_blocks - 1);
                        m_callback->update(fraction);
                        //update time
                        lastUpdateTime = currentTime;
                }
        }

        m_callback->update(1.0);

        secure_wipe(&key, sizeof(key));
        secure_wipe(&key2, sizeof(key2));

        return imsg;
}

std::string OFB::decrypt_mode(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
        size_t total_blocks = msg.size();
        if(total_blocks == 0)
                return " ";

        auto lastUpdateTime = std::chrono::steady_clock::now();

        for(size_t i = 1; i < total_blocks; i++)
        {
                msg[0] = this->cipher(this->decipher(this->cipher(msg[0], key), key2), key);
                msg[i] = msg[i] ^ msg[0];

                //check current time
                auto currentTime = std::chrono::steady_clock::now();

                std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
                //10 fps loading & block count loading
                if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
                {
                        double fraction = static_cast<double>(i) / (total_blocks - 1);
                        m_callback->update(fraction);
                        //update time
                        lastUpdateTime = currentTime;
                }
        }

        m_callback->update(static_cast<double>(1));
        explicit_bzero(&key, sizeof(uint64_t));
        explicit_bzero(&key2, sizeof(uint64_t));

        //before change to string, erase IV
        msg.erase(msg.begin());

        std::string result = from_integer(msg);

        return result;
}
