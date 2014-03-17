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
#include <utility>
#include <map>
#include <set>


typedef uint32_t SummaryKey;
typedef std::pair < std::set<uint32_t>, uint32_t> SummaryValue;
typedef std::map <SummaryKey, SummaryValue> TradeMsgSummary;

class TradeMsg {
    private:
        uint32_t type;
        uint32_t time;
        uint32_t msg_length;
        char * msg;
    public:
        enum TradeError {
            ERROR_OK = 0,
            ERROR_INVALID_DATA = 1,
            ERROR_INVALID_MSG_TYPE = 2,
            ERROR_INVALID_MSG_LENGTH = 3,
            ERROR_INVALID_STREAM = 4
        };
        static const int MAX_MSG_LENHTH = 100000;
        static const int MAX_MSG_TYPE = 100000;

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
            if (this->msg_length == 0) 
                return;
            //some scaffolds
            //std::cerr << "DEBUG: copy constructor\n";
            this->msg = new char[this->msg_length + 1/*\0*/];
            memcpy(this->msg, msg.msg, this->msg_length + 1/*\0*/);
        } 
        const TradeMsg& operator=(const TradeMsg&) { 
            //some scaffolds
            //std::cerr << "DEBUG: = operator\n";
            return *this; 
        }

        void dump() const {
            if (!is_debug) 
                return;

            std::cerr << "DEBUG: ";
            std::cerr << "type: " << this->type << ", ";
            std::cerr << "time: " << this->time << ", ";
            std::cerr << "msg_length: " << this->msg_length << ", ";
            const char *dump_msg = "NULL";
            if (this->msg) {
                dump_msg = this->msg;
            }
            std::cerr << "msg: " << dump_msg << "\n";
        }


        TradeMsg::TradeError read_from_stream(std::ifstream &input_file) {
            if(!input_file.read((char *)&type, sizeof(type))) {
                return TradeMsg::ERROR_INVALID_STREAM;
            }

            if(!input_file.read((char *)&time, sizeof(time))) {
                return TradeMsg::ERROR_INVALID_STREAM;
            }

            if(!input_file.read((char *)&msg_length, sizeof(msg_length))) {
                return TradeMsg::ERROR_INVALID_STREAM;
            }

            if (msg_length > MAX_MSG_LENHTH) {
                return TradeMsg::ERROR_INVALID_MSG_LENGTH;
            }
            if (msg_length) {
                msg = new char[msg_length + 1/*\0*/];

                //valgrind pacifier:)
                //->Conditional jump or move depends on uninitialised value
                //in dump()
                memset(msg, 0, msg_length + 1/*\0*/);

                if(!input_file.read((char *)msg, msg_length)) {
                    return TradeMsg::ERROR_INVALID_STREAM;
                }
            }
            if (this->type > MAX_MSG_TYPE) {
                return TradeMsg::ERROR_INVALID_MSG_TYPE;
            }

            return TradeMsg::ERROR_OK;
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
        uint32_t get_type() const {
            return type;
        }
};
bool TradeMsg::is_debug = false;


int main(int argc, char **argv) {
    std::ifstream input;
    std::ofstream output;

    //some useful scaffolds
    //std::cerr << "DEBUG: BINARY_DIR is " << BINARY_DIR << "\n";
    
    //switch debug mode on
    //TradeMsg::start_debugging();


    input.open(BINARY_DIR "/input.txt", std::ios::binary);
    output.open(BINARY_DIR "/output.txt", std::ios::binary);
    if (not input.is_open()) {
        std::cerr << "Unable to open the input file. Reason: " << 
                      strerror(errno) << "\n";
        return 1;
    }
    if (not output.is_open()) {
        std::cerr << "Unable to open the output file. Reason: " << 
                      strerror(errno) << "\n";
        return 1;
    }

    TradeMsgSummary msg_summary;
    while(input) {
        TradeMsg msg;

        TradeMsg::TradeError error_code = msg.read_from_stream(input);
        if (error_code != TradeMsg::ERROR_OK) {
            if (input.eof()) {
                //all right, all file content has been read
                break;
            }
            std::cerr << "ERROR: Unable to read trade msg from stream. Error: " 
                      << error_code << "\n";
            msg.dump();
            return 1;
        }
        SummaryKey key = msg.get_type();
        //, msg.get_time());
        TradeMsgSummary::iterator it = msg_summary.find(key);
        if ( it == msg_summary.end()) {
            //insert new summary value into map
            std::set<uint32_t> uniq_seconds;
            uniq_seconds.insert(msg.get_time());
            SummaryValue value(uniq_seconds, 1);
            std::pair <SummaryKey, SummaryValue> new_record(key, value);
            msg_summary.insert(new_record);
        } else {
            //increment existing summary value
            msg_summary.at(key).first.insert(msg.get_time());
            msg_summary.at(key).second += 1;
        }
    }

    TradeMsgSummary::iterator it;
    for(it=msg_summary.begin(); it != msg_summary.end(); ++it) {
        std::pair <SummaryKey, SummaryValue> summary_record = *it;
        const uint32_t msg_type = summary_record.first;
        const std::set<uint32_t> &uniq_seconds = summary_record.second.first;
        const uint32_t msg_count = summary_record.second.second;
        const double msg_rate = ((double)msg_count)/uniq_seconds.size();
        output.write((char*)&msg_type, sizeof(msg_type));
        output.write((char*)&msg_rate, sizeof(msg_rate));
        //useful debug print
        /*std::cerr << "DEBUG: msg_type is " << msg_type << "\n";
        std::cerr << "DEBUG: msg_rate is " << msg_rate << "\n";*/
        
    }

    return 0;
}

