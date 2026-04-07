#include "CTR.h"
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

std::vector<uint64_t> CTR::encrypt_mode(std::string msg, uint64_t& key, uint64_t& key2)
{
        std::vector<uint64_t> imsg;

        //change string to integer vector & padding
        imsg = to_integer(msg);

    size_t total_blocks = imsg.size();
        //exception handling
    if (total_blocks == 0) 
                return imsg;

    std::vector<uint64_t> result(total_blocks);

        //take uint64_t random value
        uint64_t counter = random();

        //for counting in multi thread env
        //loading menu
        std::atomic<size_t> completed_count(0);

        //check hardware's thread count
    unsigned int hw_threads = std::thread::hardware_concurrency();

    int num_threads = (hw_threads >= 4) ? 4 : 2;

        //if block is smaller than thread, overhead exist
    if (total_blocks < static_cast<size_t>(num_threads))
        num_threads = static_cast<int>(total_blocks);
        //vector for thread
    std::vector<std::thread> t;
    size_t chunk_size = total_blocks / num_threads;
    size_t remainder = total_blocks % num_threads; 

    size_t current_start = 0;

    for (int i = 0; i < num_threads; ++i) 
    {
        size_t current_chunk = chunk_size + (i < remainder ? 1 : 0);
        size_t start = current_start;
        size_t end = start + current_chunk;
        //start threading
        t.emplace_back([this, &imsg, &result, start, end, key, key2, &counter, &completed_count]() {
                                for(size_t j = start; j < end; ++j)
                                {
                                        result[j] = imsg[j] ^ this->cipher(this->decipher(this->cipher(counter++, key), key2), key);
                                        //increase count
                                        completed_count.fetch_add(1, std::memory_order_relaxed);
                                }
        });

                //save end index
        current_start = end; 
    }
        //loading logic
        if(m_callback)
        {
                while(completed_count < total_blocks)
                {
                        double fraction = static_cast<double>(completed_count.load()) / total_blocks;

                        m_callback->update(fraction);
                        //10 fps limitation
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                m_callback->update(1.0);
        }
        //if it allow to join, join thread
    for (auto& it : t)
        if (it.joinable()) 
                        it.join();

        counter -= result.size();

        result.insert(result.begin(), counter);

    return result;
}



std::string CTR::decrypt_mode(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
        //take counter
        uint64_t counter = msg.front();
        msg.erase(msg.begin());

        std::vector<uint64_t> result;
    size_t total_blocks = msg.size();
        //exception handling
    if (total_blocks == 0) 
                return " ";

        result.resize(total_blocks, 0);

        //for counting in multi thread env
        //loading menu
        std::atomic<size_t> completed_count(0);

        //check hardware's thread count
    unsigned int hw_threads = std::thread::hardware_concurrency();

    int num_threads = (hw_threads >= 4) ? 4 : 2;

        //if block is smaller than thread, overhead exist
    if (total_blocks < static_cast<size_t>(num_threads))
        num_threads = static_cast<int>(total_blocks);
        //vector for thread
    std::vector<std::thread> t;
    size_t chunk_size = total_blocks / num_threads;
    size_t remainder = total_blocks % num_threads; 

    size_t current_start = 0;

        for (int i = 0; i < num_threads; ++i) 
    {
        size_t current_chunk = chunk_size + (i < remainder ? 1 : 0);
        size_t start = current_start;
        size_t end = start + current_chunk;
        //start threading
        t.emplace_back([this, &msg, &result, start, end, key, key2, &counter, &completed_count]() {
                                for(size_t j = start; j < end; ++j)
                                {
                                        result[j] = msg[j] ^ this->cipher(this->decipher(this->cipher(counter++, key), key2), key);
                                        //increase count
                                        completed_count.fetch_add(1, std::memory_order_relaxed);
                                }
        });

                //save end index
        current_start = end; 
    }
        //loading logic
        if(m_callback)
        {
                while(completed_count < total_blocks)
                {
                        double fraction = static_cast<double>(completed_count.load()) / total_blocks;

                        m_callback->update(fraction);
                        //10 fps limitation
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                m_callback->update(1.0);
        }
        //if it allow to join, join thread
    for (auto& it : t)
        if (it.joinable()) 
                        it.join();

        return from_integer(result);
}
