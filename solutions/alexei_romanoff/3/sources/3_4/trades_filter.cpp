#include <queue>
#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include "market_message.h"

namespace cppcraft_task_3_4 {
    struct queue_elem {
        std::string input_file_name;
        std::string output_file_name;
    };

    enum TradeMsgType {
        MARKET_OPEN = 1,
        TRADE = 2,
        QUOTE = 3,
        MARKET_CLOSE = 4
    };

    enum TRADE_ERROR {
        ERROR_OK = 1,
        ERROR_UNABLE_TO_OPEN_INPUT_FILE = 2,
        ERROR_UNABLE_TO_OPEN_OUTPUT_FILE = 3
    };
    class trades_filter_mt {
    public:
        explicit trades_filter_mt(const size_t thread_count) :
            stop_flag(false)      
        {
            for(size_t i=0; i < thread_count; ++i) {
                thread_pool_.create_thread(boost::bind( &trades_filter_mt::do_filtering,
                                           this, i));
            }
        }
        bool is_good_msg_type(const boost::uint32_t type) const {
            return ( type >= MARKET_OPEN && type <= MARKET_CLOSE );
        }

        void wait_for_results() {
            stop_flag = true;
            thread_pool_.join_all();
        }

        void do_filtering(const int thread_id) {
            queue_elem next_elem;
            while(1) {
                bool is_next_elem = false;
                {
                    boost::mutex::scoped_lock lock(queue_lock_);
                    if (stop_flag and file_queue_.empty()) {
                        break;
                    }
                    if(!file_queue_.empty()) {
                        next_elem = file_queue_.front();
                        file_queue_.pop();
                        is_next_elem = true;
                    }
                }
                if (!is_next_elem) {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                    continue;
                }
                TRADE_ERROR err = filter_one_file(next_elem.input_file_name, next_elem.output_file_name);
                if (err != ERROR_OK) {
                    std::cerr << "ERROR: error " << err << " occured when processed file " 
                              << next_elem.input_file_name << "\n";
                    continue;
                }
            }
        }

        TRADE_ERROR filter_one_file(std::string &input_file_name, 
                                    std::string &output_file_name) {
            std::ifstream input;
            std::ofstream output;
            input.open(input_file_name.c_str(), std::ios::binary);
            if (!input.is_open()) {
                return ERROR_UNABLE_TO_OPEN_INPUT_FILE;
            }
            output.open(output_file_name.c_str(), std::ios::binary);
            if (!output.is_open()) {
                return ERROR_UNABLE_TO_OPEN_OUTPUT_FILE;
            }

            boost::uint32_t max_time = 0;
            while(input) {
                binary_reader::market_message msg(input);
                if (!is_good_msg_type( msg.type() )) {
                    continue;
                }
                const boost::uint32_t msg_time = msg.time();
                if (max_time == 0) {
                    max_time = msg_time;
                } else {
                    if (msg_time >= max_time) {
                        max_time = msg_time;
                    } else if (msg_time <= (max_time - 2)) {
                        continue;
                    }
                }
                msg.write(output);
            }
            input.close();
            output.close();
            return ERROR_OK;
        }

        void add_file(const char *input_file_name, const char *output_file_name) {
            boost::mutex::scoped_lock lock(queue_lock_);
            queue_elem new_elem;
            new_elem.input_file_name = input_file_name;
            new_elem.output_file_name = output_file_name;
            file_queue_.push(new_elem);
        }

    private:
        boost::thread_group thread_pool_;
        boost::condition on_item_in_queue_;
        mutable boost::mutex queue_lock_;
        bool stop_flag;
        std::queue<queue_elem> file_queue_;

    };
}

const int MAX_INPUT_FILE_COUNT = 1000;
const int MAX_FILE_PATH_SIZE = 256;

int main()
{
    if (sizeof(BINARY_DIR) > MAX_FILE_PATH_SIZE - 13) {
        std::cerr << "ERROR: too long BINARY_DIR: " << BINARY_DIR 
                  << "\n";
        return 1;
    }
    cppcraft_task_3_4::trades_filter_mt trades_filter(2);
    char *input_file_name = new char[MAX_FILE_PATH_SIZE + 1/*\0*/];
    char *output_file_name = new char[MAX_FILE_PATH_SIZE + 1/*\0*/];

    for(int i = 1; i < MAX_INPUT_FILE_COUNT; ++i) {
        memset(input_file_name, 0, (MAX_FILE_PATH_SIZE + 1)*sizeof(char));
        memset(output_file_name, 0, (MAX_FILE_PATH_SIZE + 1)*sizeof(char));
        snprintf(input_file_name, MAX_FILE_PATH_SIZE, BINARY_DIR "/input_%03d.txt", i);
        snprintf(output_file_name, MAX_FILE_PATH_SIZE, BINARY_DIR "/output_%03d.txt", i);
        trades_filter.add_file(input_file_name, output_file_name);
    }
    delete [] input_file_name;
    delete [] output_file_name;
    trades_filter.wait_for_results();

	//binary_reader::stock_data( "", "", 1, 1, 1,1,1,1,1,1 );
}

