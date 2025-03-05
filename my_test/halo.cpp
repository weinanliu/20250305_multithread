#include <iostream>
#include <thread>
#include <cmath>
#include <chrono>

#include <cstddef>

#include <sys/mman.h>
#include <unistd.h>

#include <fstream>
#include <set>
#include <string>
#include <sstream>
#include <tuple>



#include "BS_thread_pool.hpp"
#include "thread_pool.hpp"





template<typename T>
T
IP(const T *a, const T *b, size_t n) {
	T ret = 0;
	for (size_t i = 0; i < n; i++) {
		ret += a[i] * b[i];
	}
	return ret;
}


void *
get_pages(size_t nr_page) {
	const size_t page_size = sysconf(_SC_PAGESIZE);
	const size_t total_size = nr_page * page_size;

	void *mapped_memory = mmap(
			nullptr,
			total_size,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1,
			0
			);

	if (mapped_memory == MAP_FAILED) {
		std::cerr << "mmap fail" << std::endl;
		exit(-1);
	}

	return mapped_memory;
}

int
main () {

	//const size_t nr_page = 1024*128;

	//uint32_t *a = static_cast<uint32_t *>(get_pages(nr_page));
	//uint32_t *b = static_cast<uint32_t *>(get_pages(nr_page));

	//const size_t page_size = sysconf(_SC_PAGESIZE);

	//size_t nr_elem = (page_size * nr_page) / sizeof(a[0]);


	size_t nr_para = std::thread::hardware_concurrency();


	size_t nr_submit = 32;

	auto task = [] (size_t i) {
// 微妙级别 10us 50us 100us
// submit 32个，看执行时间
// 
//
	        //std::this_thread::sleep_for(std::chrono::nanoseconds(100000 * 1000)); // 100000us = 100ms
	        //std::this_thread::sleep_for(std::chrono::nanoseconds(10000 * 1000)); // 10000us = 10ms
	        //std::this_thread::sleep_for(std::chrono::nanoseconds(1000 * 1000)); // 1000us = 1ms
	        //std::this_thread::sleep_for(std::chrono::nanoseconds(800 * 1000)); // 800us
	        //std::this_thread::sleep_for(std::chrono::nanoseconds(500 * 1000)); // 500us
	        std::this_thread::sleep_for(std::chrono::nanoseconds(100 * 1000)); // 100us
	        //std::this_thread::sleep_for(std::chrono::nanoseconds(50 * 1000)); // 50us
	        //std::this_thread::sleep_for(std::chrono::nanoseconds(10 * 1000)); // 10us
		return 0;
		//return IP(a, b, nr_elem);
	};

	auto start = std::chrono::high_resolution_clock::now();
	auto ret = task(1);
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

	std::cout << "duration_per_task(ns): " << duration << std::endl;


	double duration__all_submit = duration * (std::ceil((double)nr_submit / (double)nr_para));

	std::cout << "duration__all_submit theoretically(ns): " << duration__all_submit << std::endl;

	start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < nr_submit; i++)
	    auto ret = task(1);
	end = std::chrono::high_resolution_clock::now();
	auto duration_32 = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

	std::cout << "duration 32 task(ns): " << duration_32 << std::endl;



	BS::thread_pool pool;

	start = std::chrono::high_resolution_clock::now();
	pool.submit_loop(0, nr_submit, task).wait();
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

	std::cout << "duration__all_submit  BS thread pool(ns): " << duration << std::endl;
	std::cout << "Efficiency: " << duration__all_submit / duration << std::endl;




	tp::ThreadPool thread_pool1;

	std::promise<int> waiters[nr_para];

	start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < nr_submit; i++) {
		thread_pool1.post([i, &waiters, &task]() {
				waiters[i].set_value(task(i));
				});
	}


	for (size_t i = 0; i < nr_submit; i++) {
		waiters[i].get_future().wait();
	}
	end = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

	std::cout << "duration__all_submit  tp thread pool(ns): " << duration << std::endl;
	std::cout << "Efficiency: " << duration__all_submit / duration << std::endl;

	return 0;
}
