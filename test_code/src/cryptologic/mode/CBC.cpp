#include "CBC.h"
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

std::vector<uint64_t> CBC::encrypt_mode(std::string msg, uint64_t& key, uint64_t& key2)
{
        std::vector<uint64_t> imsg;

        //change string to integer vector
        imsg = to_integer(msg);

        //save IV for deciphering
        imsg.insert(imsg.begin(), random());

        //take counter of block
        size_t total_blocks = imsg.size();

        //exception handling
        if(total_blocks == 0)
                return imsg;

        auto lastUpdateTime = std::chrono::steady_clock::now();

        for(size_t i = 1; i < total_blocks; i++)
        {
                imsg[i] = this->cipher(this->decipher(this->cipher((imsg[i] ^ imsg[i - 1]), key), key2), key);
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

        //explicitly print 100%
        m_callback->update(1.0);

        return imsg;
}

std::string CBC::decrypt_mode(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
        std::vector<uint64_t> result;
        size_t total_blocks = msg.size();
        if(total_blocks == 0)
                return " ";
        //save IV for variable
        uint64_t IV = msg[0];

        //temp
        uint64_t deciphered;

        //erase IV in vector
        msg.erase(msg.begin());
        //cause erase caculation
        result.resize(total_blocks - 1, 0);

        auto lastUpdateTime = std::chrono::steady_clock::now();
        for(size_t i = 0; i < total_blocks - 1; i++)
        {
                //save deciphered value
                //-> CBC use ciphered block
                deciphered = this->decipher(this->cipher(this->decipher(msg[i], key), key2), key);

                if(i)        //decipher block ^ ciphered block
                        result[i] = (deciphered ^ msg[i - 1]);
                else        //first xor with IV
                        result[i] = deciphered ^ IV;

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

        //change integer vector to string
        return from_integer(result);
}
