#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class thread_queue{
private:	
	std::queue<T> t_queue;
	mutable std::mutex t_mutex;
	std::condition_variable cv_read;
	std::condition_variable cv_write;
	size_t max_size;
	bool finished;
public:
	explicit thread_queue(size_t max_size) : max_size(max_size), finished(false) {}

	void push(T value)
	{
		std::unique_lock<std::mutex> lock(t_mutex);
		//if queue is full, wait write
		cv_write.wait(std::lock, [this]() { return t_queue.size() < max_size; });

		t_queue.push(std::move(value));
		cv_read.notify_one();
	}

	bool pop(T& value)
	{
		std::unique_lock<std::mutex> lock(t_mutex);
		//if queue empty & work not done, wait 
		cv_read.wait(std::lock, [this]() { return !t_queue.empty() || finished; });
			
		//there is no value to treat
		if(t_queue.empty() && finished)
			return false;

		value = std::move(t_queue.front());
		t_queue.pop();
		cv_write.notify_one();

		return true;
	}

	void set_finished(void);
};
