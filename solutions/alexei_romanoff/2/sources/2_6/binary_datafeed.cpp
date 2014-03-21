#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <cerrno>
#include <cstring>

#include "boost/date_time/gregorian/gregorian.hpp"


class StockMsg {
    private:
        static const size_t date_time_size = 8;
        static const size_t default_stock_name_size = 8;
        size_t stock_name_size;

        char *stock_name;
        char *date_time;
        uint32_t date_time_as_uint;
        double price;
        double vwap;
        uint32_t volume;
        double f1;
        double t1;
        double f2;
        double f4;
        double f3;
    public:
        StockMsg() : stock_name_size(StockMsg::default_stock_name_size), 
                     stock_name(NULL), date_time(NULL), date_time_as_uint(0),
                     price(0.0), vwap(0.0), volume(0), f1(0.0), t1(0.0),
                     f2(0.0), f4(0.0), f3(0.0) { } ;
        StockMsg(const StockMsg& msg) : 
                     stock_name_size(StockMsg::default_stock_name_size), 
                     stock_name(NULL), date_time(NULL), 
                     date_time_as_uint(msg.date_time_as_uint),
                     price(msg.price), vwap(msg.vwap), volume(msg.volume), 
                     f1(msg.f1), t1(msg.t1),
                     f2(msg.f2), f4(msg.f4), f3(msg.f3) { 
            if (msg.stock_name) {
                stock_name = new char[stock_name_size + 1/*\0*/];
                memcpy(stock_name, msg.stock_name, stock_name_size + 1/*\0*/);
            }
            if (msg.date_time) {
                date_time = new char[date_time_size + 1/*\0*/];
                memcpy(date_time, msg.date_time, date_time_size + 1/*\0*/);
            }
        };


        enum StockError {
            ERROR_OK = 0,
            ERROR_INVALID_DATA = 1,
            ERROR_INVALID_STREAM = 2,
            ERROR_INVALID_DATETIME = 3
        };

        StockMsg::StockError read_from_stream(std::ifstream &input_file) {
            stock_name = new char[stock_name_size + 1/*\0*/];
            memset(stock_name, 0, stock_name_size + 1/*\0*/);
            if(!input_file.read(stock_name, sizeof(stock_name_size))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            date_time = new char[date_time_size + 1/*\0*/];
            memset(date_time, 0, date_time_size + 1/*\0*/);
            if(!input_file.read(date_time, sizeof(date_time_size))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&price, sizeof(price))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&vwap, sizeof(vwap))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&volume, sizeof(volume))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&f1, sizeof(f1))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&t1, sizeof(t1))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&f2, sizeof(f2))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&f4, sizeof(f4))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!input_file.read((char *)&f3, sizeof(f3))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            try {
                std::string s(date_time);
                boost::gregorian::date d(boost::gregorian::from_undelimited_string(s));
                boost::gregorian::date::ymd_type ymd = d.year_month_day();
                date_time_as_uint = (ymd.year-1) * 372 + (ymd.month - 1) * 31 + ymd.day;
            } catch(std::out_of_range e) {
                std::cerr << "DEBUG: date_time exception is " << e.what() << "\n";
                return StockMsg::ERROR_INVALID_DATETIME;
            }

            return StockMsg::ERROR_OK;
        }

        static bool is_debug;
        void static start_debugging() {
            StockMsg::is_debug = true;
        }
        void static stop_debugging() {
            StockMsg::is_debug = false;
        }

        void set_stock_name_size(const size_t new_size) {
            if (new_size > stock_name_size) {
                char *new_stock_name = new char[new_size + 1/*\0*/];
                memcpy(new_stock_name, stock_name, stock_name_size + 1/*\0*/);
                delete [] stock_name;
                stock_name = new_stock_name;
            } else if (new_size < stock_name_size) {
                char *new_stock_name = new char[new_size + 1/*\0*/];
                memcpy(new_stock_name, stock_name, new_size);
                new_stock_name[new_size-1] = '\0';
                delete [] stock_name;
                stock_name = new_stock_name;
            } else {
                //do nothing, new_size is equial to stock_name_size
            }
            stock_name_size = new_size;
        }

        void dump() const {
            if (!is_debug) 
                return;

            std::cerr << "DEBUG: ";
            const char *stock_name= "NULL";
            if (stock_name != NULL) {
                stock_name = this->stock_name;
            }
            std::cerr << "stock_name: " << stock_name<< ", ";

            const char *date_time = "NULL";
            if (date_time != NULL) {
                date_time = this->date_time;
            }
            std::cerr << "date_time: " << date_time << ", ";
            std::cerr << "date_time_as_uint: " << date_time_as_uint << ", ";
            std::cerr << "price: " << price << ", ";
            std::cerr << "vwap: " << vwap << ", ";
            std::cerr << "volume: " << volume << ", ";
            std::cerr << "f1: " << f1 << ", ";
            std::cerr << "t1: " << t1 << ", ";
            std::cerr << "f2: " << f2 << ", ";
            std::cerr << "f4: " << f4 << ", ";
            std::cerr << "f3: " << f3 << "\n";
        }

        StockMsg::StockError write_to_stream(std::ofstream &output_file) {
            if(!output_file.write(stock_name, stock_name_size)) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!output_file.write((char *)&date_time_as_uint, sizeof(date_time_as_uint))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!output_file.write((char *)&vwap, sizeof(vwap))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!output_file.write((char *)&volume, sizeof(volume))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            if(!output_file.write((char *)&f2, sizeof(f1))) {
                return StockMsg::ERROR_INVALID_STREAM;
            }
            return StockMsg::ERROR_OK;
        }


        ~StockMsg() {
            if (stock_name != NULL) {
                delete [] stock_name;
                stock_name = NULL;
            }
            if (date_time != NULL) {
                delete [] date_time;
                date_time = NULL;
            }
        };
};

bool StockMsg::is_debug = false;

static const size_t output_stock_name_size = 9;


int main(int argc, char **argv) {
    std::ifstream input;
    std::ofstream output;

    //switch debug mode on/off
    //StockMsg::start_debugging();

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
    while(input) {
        StockMsg msg; 
        StockMsg::StockError error_code = msg.read_from_stream(input);
        if (error_code != StockMsg::ERROR_OK) {
            if (input.eof()) {
                //all right, all file content has been read
                break;
            }
            std::cerr << "ERROR: Unable to read trade msg from stream. Error: " 
                      << error_code << "\n";
            msg.dump();
            return 1;
        }
        msg.dump();

        msg.set_stock_name_size(output_stock_name_size);
        error_code = msg.write_to_stream(output);
        if (error_code != StockMsg::ERROR_OK) {
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
