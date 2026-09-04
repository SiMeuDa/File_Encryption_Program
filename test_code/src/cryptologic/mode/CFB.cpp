#include "CFB.h"
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

std::vector<uint64_t> CFB::encrypt_mode(std::string msg, uint64_t& key, uint64_t& key2)
{
        std::vector<uint64_t> imsg;

        //change string to integer vector
        imsg = to_integer(msg);
        //add random IV value
        imsg.insert(imsg.begin(), random());

        size_t total_blocks = imsg.size();
        //exception handling
        if(total_blocks == 0)
                return imsg;

        auto lastUpdateTime = std::chrono::steady_clock::now();

        //xor with ciphered block
        for(size_t i = 1; i < total_blocks; i++)
        {
                imsg[i] = this->cipher(this->decipher(this->cipher(imsg[i - 1], key), key2), key) ^ imsg[i];

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

        return imsg;
}

std::string CFB::decrypt_mode(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
        std::vector<uint64_t> result;

        size_t total_blocks = msg.size();

        if(total_blocks == 0)
                return " ";

        result.resize(total_blocks - 1, 0);

        auto lastUpdateTime = std::chrono::steady_clock::now();

        //same cipher logic (CFB standard)
        for(size_t i = 1; i < total_blocks; i++)
        {
                result[i - 1] = this->cipher(this->decipher(this->cipher(msg[i - 1], key), key2), key) ^ msg[i];

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
        return from_integer(result);
}
