#include <string>
#include <algorithm>
#include <cmath>

#include <inttypes.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <iomanip>
#include <vector>

namespace solution_2_4 {
    enum TradeError {
        ERROR_OK = 0,
        ERROR_INVALID_DATA = 1,
        ERROR_INVALID_MSG_TYPE = 2,
        ERROR_INVALID_MSG_LENGTH = 3,
        ERROR_INVALID_STREAM = 4
    };
    enum TradeMsgType {
        MARKET_OPEN = 1,
        TRADE = 2,
        QUOTE = 3,
        MARKET_CLOSE = 4
    };

    class TradeMsg {
        private:
            uint32_t type;
            uint32_t time;
            uint32_t msg_length;
            char * msg;
        public:
            static const int MAX_MSG_LENHTH = 100000;

            static bool is_debug;
            void static start_debugging() {
                TradeMsg::is_debug = true;
            }
            void static stop_debugging() {
                TradeMsg::is_debug = false;
            }

            TradeMsg() : type(0), time(0), msg_length(0), msg(NULL) {} ;
            TradeMsg(const TradeMsg& msg): type(msg.type), time(msg.time), 
                                           msg_length(msg.msg_length), msg(NULL)
            {
                if (msg_length == 0) 
                    return;
                //some scaffolds
                //std::cerr << "DEBUG: copy constructor\n";
                this->msg = new char[msg_length + 1/*\0*/];
                memcpy(this->msg, msg.msg, msg_length + 1/*\0*/);
            } 

            void dump() const {
                if (!is_debug) 
                    return;

                std::cerr << "DEBUG: ";
                std::cerr << "type: " << type << ", ";
                std::cerr << "time: " << time << ", ";
                std::cerr << "msg_length: " << msg_length << ", ";
                const char *dump_msg = "NULL";
                if (msg) {
                    dump_msg = msg;
                }
                std::cerr << "msg: " << dump_msg << "\n";
            }

            bool is_good_msg_type() const {
                return ( type >= MARKET_OPEN && type <= MARKET_CLOSE );
            }


            TradeError read_from_stream(std::ifstream &input_file) {
                if(!input_file.read((char *)&type, sizeof(type))) {
                    return ERROR_INVALID_STREAM;
                }

                if(!input_file.read((char *)&time, sizeof(time))) {
                    return ERROR_INVALID_STREAM;
                }

                if(!input_file.read((char *)&msg_length, sizeof(msg_length))) {
                    return ERROR_INVALID_STREAM;
                }

                if (msg_length > MAX_MSG_LENHTH) {
                    return ERROR_INVALID_MSG_LENGTH;
                }
                if (msg_length == 0) {
                    //we don't need to read message, all right
                    return ERROR_OK;
                }
                msg = new char[msg_length + 1/*\0*/];

                //valgrind pacifier:)
                //->Conditional jump or move depends on uninitialised value
                //in dump()
                memset(msg, 0, msg_length + 1/*\0*/);

                if(!input_file.read((char *)msg, msg_length)) {
                    return ERROR_INVALID_STREAM;
                }

                return ERROR_OK;
            }

            TradeError write_to_stream(std::ofstream &output_file) {
                if(!output_file.write((char *)&type, sizeof(type))) {
                    return ERROR_INVALID_STREAM;
                }
                if(!output_file.write((char *)&time, sizeof(time))) {
                    return ERROR_INVALID_STREAM;
                }
                if(!output_file.write((char *)&msg_length, sizeof(msg_length))) {
                    return ERROR_INVALID_STREAM;
                }
                if(!output_file.write(msg, msg_length)) {
                    return ERROR_INVALID_STREAM;
                }

                return ERROR_OK;
            }

            ~TradeMsg() {
                if (msg != NULL) {
                    delete [] msg;
                    msg = NULL;
                }
            };

            uint32_t get_time() const {
                return time;
            }
    };
    bool TradeMsg::is_debug = false;
}




int main(int argc, char **argv) {
    std::ifstream input;
    std::ofstream output;

    //some useful scaffolds
    //std::cerr << "DEBUG: BINARY_DIR is " << BINARY_DIR << "\n";
    
    //switch debug mode on/off
    //TradeMsg::start_debugging();


    input.open(BINARY_DIR "/input.txt", std::ios::binary);
    output.open(BINARY_DIR "/output.txt", std::ios::binary);
    if (!input.is_open()) {
        std::cerr << "Unable to open the input file. Reason: " << 
                      strerror(errno) << "\n";
        return 1;
    }
    if (!output.is_open()) {
        std::cerr << "Unable to open the output file. Reason: " << 
                      strerror(errno) << "\n";
        return 1;
    }

    uint32_t max_time = 0;
    while(input) {
        solution_2_4::TradeMsg msg;

        solution_2_4::TradeError error_code = msg.read_from_stream(input);
        if (error_code != solution_2_4::ERROR_OK) {
            if (input.eof()) {
                //all right, all file content has been read
                break;
            }
            std::cerr << "ERROR: Unable to read trade msg from stream. Error: " 
                      << error_code << "\n";
            msg.dump();
            return 1;
        }
        if (!msg.is_good_msg_type()) {
            if (solution_2_4::TradeMsg::is_debug)
                std::cerr << "INFO: skipped msg with invalid type" << "\n";
            msg.dump();
            continue;
        }
        const uint32_t msg_time = msg.get_time();
        if (max_time == 0) {
            max_time = msg_time;
        } else {
            if (msg_time >= max_time) {
                max_time = msg_time;
            } else if (msg_time <= (max_time - 2)) {
                if (solution_2_4::TradeMsg::is_debug)
                    std::cerr << "INFO: skipped msg with invalid time" << "\n";
                msg.dump();
                continue;
            }
        }
        msg.dump();
        error_code = msg.write_to_stream(output);
        if (error_code != solution_2_4::ERROR_OK) {
            std::cerr << "ERROR: Unable to write trade msg from stream. Error: " 
                      << error_code << "\n";
            msg.dump();
            return 1;
        }
    }

    input.close();
    output.close();


    return 0;
}
