#include "logger.hpp"
#include <signal.h>

namespace spk {
    bool should_ignore_character(char c) {
        switch(c) {
            case ' ':
                return true;

            case SFK_RULING_DEFAULT_TEXT_START:
                return true;

            default:
                return false;
        }
    }

    std::string parse_ruling(rule_map_t& map, const std::string& str) {
        std::string ansi_rules = "";
        std::string rule = "";

        ansi_rules.reserve(10);
        rule.reserve(10);

        for(uint32_t i = 0; i < str.size(); i++) {
            if(!should_ignore_character(str[i])) {
                if(str[i] == SFK_RULING_DEFAULT_SEPERATOR || str[i] == SFK_RULING_DEFAULT_TEXT_END) {
                    if(map.find(rule) != map.end()) {
                        ansi_rules += map[rule];
                    }

                    rule = ""; 
                } else {
                    rule += str[i];
                }
            }
        } 

        return ansi_rules;
    }

    std::string parse_format(rule_map_t& map, const std::string& format_) {
        std::string f = format_;

        bool rule = false;
        uint32_t rule_start = 0;

        for(uint32_t i = 0; i < f.size(); i++) {
            if(f[i] == SFK_RULING_DEFAULT_TEXT_START) {
                rule = true;
                rule_start = i;
            }
            
            if(f[i] == SFK_RULING_DEFAULT_TEXT_END && rule) {
                uint32_t    rule_remainder = (i - rule_start) + 1;    
                std::string parsed_rule = parse_ruling(map, f.substr(rule_start, rule_remainder));
            
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

    void log_sigv(int num) {
        printf("error\n");
        log.log(LOG_TYPE_ERROR, "seg fault");
    }

    info_logger_t::info_logger_t() {
        buf.resize(4096);
        log_file.open("./log.txt");
        
        flags |= LOG_FLAGS_ENABLE_STD_PIPE;

        // default rule mapping
        rule_map[SFK_RULING_DEFAULT_RESET]    = SFK_ANSI_RESET;
        rule_map[SFK_RULING_DEFAULT_EMPHASIS] = SFK_ANSI_EMPHASIS;
        rule_map[SFK_RULING_DEFAULT_ITALIC]   = SFK_ANSI_ITALIC;
        rule_map[SFK_RULING_DEFAULT_BLACK]    = SFK_ANSI_BLACK;
        rule_map[SFK_RULING_DEFAULT_RED]      = SFK_ANSI_RED; 
        rule_map[SFK_RULING_DEFAULT_GREEN]    = SFK_ANSI_GREEN;
        rule_map[SFK_RULING_DEFAULT_YELLOW]   = SFK_ANSI_YELLOW;
        rule_map[SFK_RULING_DEFAULT_BLUE]     = SFK_ANSI_BLUE;
        rule_map[SFK_RULING_DEFAULT_PURPLE]   = SFK_ANSI_PURPLE;
        rule_map[SFK_RULING_DEFAULT_CYAN]     = SFK_ANSI_CYAN;
        rule_map[SFK_RULING_DEFAULT_WHITE]    = SFK_ANSI_WHITE;
        rule_map[SFK_RULING_DEFAULT_RESET]    = SFK_ANSI_RESET;
        rule_map[SFK_RULING_DEFAULT_EPHASIS_TEXT] = SFK_OUTPUT_EMPHASIS_TEXT;
        
        // in the case of unexpected termination- output to the console
        signal(SIGSEGV, log_sigv);

        trace_list.reserve(500);
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

            case LOG_TYPE_TRACE:
                return "[yellow, em] TRACE [reset]";

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
        std::string format = parse_format(rule_map,
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

        if(type == LOG_TYPE_ERROR || type == LOG_TYPE_ASSERT) {
            log(LOG_TYPE_INFO, "[red] Aborting due to error [reset]");

            for(auto& str : trace_list) {
                log(LOG_TYPE_TRACE, str.c_str());
            }

            abort();
        }

        log_file.write(buf.data(), ptr);
    }

    void info_logger_t::spew() {
        printf("%s", buf.data());
    }

    bool info_logger_t::trace(const char* file, const char* func, int line) {
        const size_t max_trace_list_size = 49;
        
        if(trace_list.size() >= max_trace_list_size + 1) {
            return true;
        }

        // we dont want to add to the trace list more than once
        if(trace_list.size() == max_trace_list_size) {
            // to avoid overallocating on the heap, and causing wierd behaviour by allocating
            // we must limit the trace list size to 50

            trace_list.push_back(std::string("trace list max size reached"));
            log(LOG_TYPE_ERROR, "trace list max size reached, may be limit recursive-ness?");
            return true;
        }
        
        trace_list.push_back(
                "file(" + std::string(file) + ") - " + std::string(func) + " - line(" + std::to_string(line) + ")");
        return false;
    }

    void _assert(const char* file, const char* func, int line, const char* expr, const char* message) {
        log.log(LOG_TYPE_ASSERT, "\n\t# file(%s),\n \t| func(%s),\n \t| line(%i),\n \t| expr(%s)\n \t| %s", 
                                    file, func, line, expr, message);
    }

    trace_t::trace_t(const char* file, const char* func, int line) {
        error = log.trace(file, func, line);
    }

    trace_t::~trace_t() {
        if(!error)
            log.trace_list.pop_back();
    }
}