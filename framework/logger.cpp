#include "logger.hpp"

namespace sfk {
    bool should_ignore_character(char c) {
        switch(c) {
            case ' ':
                return true;

            case SFK_RULING_TEXT_START:
                return true;

            default:
                return false;
        }
    }

    std::string ansi_rule(const std::string& rule) {
        // im tired

        if(rule == SFK_RULING_RESET) {
            return SFK_ANSI_RESET;
        } else if(rule == SFK_RULING_EMPHASIS) {
            return SFK_ANSI_EMPHASIS;
        } else if(rule == SFK_RULING_ITALIC) {
            return SFK_ANSI_ITALIC;
        } else if(rule == SFK_RULING_BLACK) {
            return SFK_ANSI_BLACK;
        } else if(rule == SFK_RULING_RED) {
            return SFK_ANSI_RED; 
        } else if(rule == SFK_RULING_GREEN) {
            return SFK_ANSI_GREEN;
        }  else if(rule == SFK_RULING_YELLOW) {
            return SFK_ANSI_YELLOW;
        } else if(rule == SFK_RULING_BLUE) {
            return SFK_ANSI_BLUE;
        } else if(rule == SFK_RULING_PURPLE) {
            return SFK_ANSI_PURPLE;
        } else if(rule == SFK_RULING_CYAN) {
            return SFK_ANSI_CYAN;
        } else if(rule == SFK_RULING_WHITE) {
            return SFK_ANSI_WHITE;
        } else if(rule == SFK_RULING_RESET) {
            return SFK_ANSI_RESET;
        } else if(rule == SFK_RULING_EPHASIS_TEXT) {
            return SFK_OUTPUT_EMPHASIS_TEXT;
        } else {
            return ("<![red]ukwn rule: " + rule + "[reset]!>").c_str();
        }
    }

    std::string parse_ruling(const std::string& str) {
        std::string ansi_rules = "";
        std::string rule = "";

        ansi_rules.reserve(10);
        rule.reserve(10);

        for(uint32_t i = 0; i < str.size(); i++) {
            if(!should_ignore_character(str[i])) {
                if(str[i] == SFK_RULING_SEPERATOR || str[i] == SFK_RULING_TEXT_END) {
                    ansi_rules += ansi_rule(rule);
                    rule = ""; 
                } else {
                    rule += str[i];
                }
            }
        } 

        return ansi_rules;
    }

    std::string parse_format(const std::string& format_) {
        std::string f = format_;

        bool rule = false;
        uint32_t rule_start = 0;

        for(uint32_t i = 0; i < f.size(); i++) {
            if(f[i] == SFK_RULING_TEXT_START) {
                rule = true;
                rule_start = i;
            }
            
            if(f[i] == SFK_RULING_TEXT_END && rule) {
                uint32_t    rule_remainder = (i - rule_start) + 1;    
                std::string parsed_rule = parse_ruling(f.substr(rule_start, rule_remainder));
            
                f.erase(rule_start, rule_remainder);
                f.insert(rule_start, parsed_rule);

                // go back to rule_start, may overstep boundries if erases replaces too much
                // doing this also allows recursive ruling, if a rule results in another rule 
                // it will be processed again.
                //
                // e.g.
                // rule foo => [red, em]
                // text: [foo] foo bar [reset]
                // >parse<
                // text: [red, em] foo bar !ANSI RESET CODE!
                // >parse<
                // text: !ANSI RED AND EMPHASIS CODE! food bar ...
                i = rule_start;
            } 
        }

        return f.c_str();
    }

    info_logger_t::info_logger_t() {
        buf.resize(4096);
        log_file.open("./log.txt");
        
        flags |= LOG_FLAGS_ENABLE_STD_PIPE;

        log(LOG_TYPE_INFO, "info_logger_init");
    }

    info_logger_t::~info_logger_t() {
        log_file.close();
    }

    const char* type_to_str(log_type_e type) {
        switch(type) {
            case LOG_TYPE_ERROR:
                return "[red] ERROR [reset]";

            case LOG_TYPE_ASSERT:
                return "[red, em] ASSERTION FAILED [reset]";

            case LOG_TYPE_INFO:
                return "[green] INFO [reset]";

            case LOG_TYPE_MISC:
                return "[yellow] MSIC [reset]";

            default:
                return "[red, em, it] UNKOWN [reset]";
        };
    }

    void info_logger_t::log(log_type_e type, const char* format_, ...) {
        size_t ptr = 0;
        char* msg_begin = buf.data(); 
        char* msg_ptr = msg_begin;
        va_list args;

        // parse for rulings
        std::string format = parse_format(
                strprintf("|SPOCK: %f> %s ", time.get_time(), type_to_str(type)) + std::string(format_));

        va_start(args, format_);
        ptr += vsprintf(msg_ptr, format.c_str(), args);
        msg_ptr = buf.data() + ptr;
        va_end(args);

        *msg_ptr = '\n';
        *(++msg_ptr) = '\0';
        ptr += 1; 

        if(flags & LOG_FLAGS_ENABLE_STD_PIPE) {
            FILE* pipe = nullptr;

            switch(type) {
            case LOG_TYPE_ASSERT: case LOG_TYPE_ERROR:
                pipe = stderr;
                break;

            default:
                pipe = stdout;
                break;
            };

            fwrite(msg_begin, sizeof(char), strlen(msg_begin) + 1, pipe);
        }

        if(type == LOG_TYPE_ERROR) {
            log(LOG_TYPE_INFO, "full unsafe exit due to error");
            exit(EXIT_FAILURE);
        }

        log_file.write(buf.data(), ptr);
    }

    void info_logger_t::spew() {
        printf("%s", buf.data());
    }

    void _assert(const char* file, const char* func, int line, const char* expr) {
        log.log(LOG_TYPE_ASSERT, "file(%s), func(%s), line(%i), expr(%s)\n", file, func, line, expr);
        abort();
    }
}