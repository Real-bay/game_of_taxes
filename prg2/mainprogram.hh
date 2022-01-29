#ifndef MAINPROGRAM_HH
#define MAINPROGRAM_HH


#ifdef QT_CORE_LIB
#ifndef NO_GRAPHICAL_GUI
#define GRAPHICAL_GUI
#endif
#endif


#include <string>
#include <random>
#include <regex>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <array>
#include <functional>
#include <utility>
#include <variant>
#include <bitset>
#include <cassert>

#include "datastructures.hh"

class MainWindow; // In case there's UI

class MainProgram
{
public:
    MainProgram();


    class Stopwatch;

    enum class PromptStyle { NORMAL, NO_ECHO, NO_NESTING };
    enum class TestStatus { NOT_RUN, NO_DIFFS, DIFFS_FOUND };

    bool command_parse_line(std::string input, std::ostream& output);
    void command_parser(std::istream& input, std::ostream& output, PromptStyle promptstyle);

    void setui(MainWindow* ui);

    void flush_output(std::ostream& output);
    bool check_stop() const;

    static int mainprogram(int argc, char* argv[]);

private:
    Datastructures ds_;
    MainWindow* ui_ = nullptr;

    static std::string const PROMPT;

    std::minstd_rand rand_engine_;

    static std::array<unsigned long int, 20> const primes1;
    static std::array<unsigned long int, 20> const primes2;
    unsigned long int prime1_ = 0; // Will be initialized to random value from above
    unsigned long int prime2_ = 0; // Will be initialized to random value from above
    unsigned long int random_towns_added_ = 0; // Counter for random towns added
    void init_primes();
    Name n_to_name(unsigned long int n);
    TownID n_to_townid(unsigned long int n);
    Coord n_to_coord(unsigned long int n);


    enum class StopwatchMode { OFF, ON, NEXT };
    StopwatchMode stopwatch_mode = StopwatchMode::OFF;

    enum class ResultType { NOTHING, LIST, HIERARCHY, ROUTE, CYCLE };
    using CmdResult = std::pair<ResultType, std::vector<TownID>>;
    CmdResult prev_result;
    bool view_dirty = true;

    TestStatus test_status_ = TestStatus::NOT_RUN;

    using MatchIter = std::smatch::const_iterator;
    struct CmdInfo
    {
        std::string cmd;
        std::string info;
        std::string param_regex_str;
        CmdResult(MainProgram::*func)(std::ostream& output, MatchIter begin, MatchIter end);
        void(MainProgram::*testfunc)();
        std::regex param_regex = {};
    };
    static std::vector<CmdInfo> cmds_;
    // Regex objects and their initialization
    std::regex cmds_regex_;
    std::regex coords_regex_;
    std::regex times_regex_;
    std::regex commands_regex_;
    std::regex sizes_regex_;
    void init_regexs();


    CmdResult cmd_add_town(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_print_town(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_change_town_name(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_add_vassalship(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_add_road(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_remove_road(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_taxer_path(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_longest_vassal_path(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_total_net_tax(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_towns_nearest(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_remove_town(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_town_count(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_all_towns(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_all_roads(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_town_vassals(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_roads_from(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_random_roads(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_random_road_network(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_any_route(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_shortest_route(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_least_towns_route(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_road_cycle_route(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_trim_road_network(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_clear_roads(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_clear_all(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_find_towns(std::ostream& output, MatchIter begin, MatchIter end);

    CmdResult help_command(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_random_add(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_randseed(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_read(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_testread(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_stopwatch(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_perftest(std::ostream& output, MatchIter begin, MatchIter end);
    CmdResult cmd_comment(std::ostream& output, MatchIter begin, MatchIter end);

    void test_print_town();
    void test_towns_nearest();
    void test_taxer_path();
    void test_longest_vassal_path();
    void test_total_net_tax();
    void test_remove_town();
    void test_remove_road();
    void test_change_town_name();
    void test_find_towns();
    void test_random_add();
    void test_all_towns();
    void test_all_roads();
    void test_town_vassals();
    void test_roads_from();
    void test_get_functions(TownID id);
    void test_random_roads();
    void test_any_route();
    void test_shortest_route();
    void test_least_towns_route();
    void test_road_cycle_route();
    void test_trim_road_network();

    void add_random_towns(unsigned int size, Coord min = {1,1}, Coord max = {10000, 10000});
    void add_random_roads(unsigned int n);
    Distance calc_distance(Coord c1, Coord c2);
    std::string print_town(TownID id, std::ostream& output, bool nl = true);
    std::string print_town_name(TownID id, std::ostream& output, bool nl = true);
    std::string print_coord(Coord coord, std::ostream& output, bool nl = true);

    template <typename Type>
    Type random(Type start, Type end);
    template <typename To>
    static To convert_string_to(std::string from);
    template <typename From>
    static std::string convert_to_string(From from);

    template<TownID(Datastructures::*MFUNC)()>
    CmdResult NoParTownCmd(std::ostream& output, MatchIter begin, MatchIter end);

    template<std::vector<TownID>(Datastructures::*MFUNC)()>
    CmdResult NoParListCmd(std::ostream& output, MatchIter begin, MatchIter end);

    template<TownID(Datastructures::*MFUNC)()>
    void NoParTownTestCmd();

    template<std::vector<TownID>(Datastructures::*MFUNC)()>
    void NoParListTestCmd();

    void create_road_network();
    void add_random_nonintersecting_roads(unsigned int random_roads);

    // Helper functions for create_road_network
    static bool doIntersect(Coord p1, Coord q1, Coord p2, Coord q2);
    static bool onSegment(Coord p, Coord q, Coord r);
    static int orientation(Coord p, Coord q, Coord r);

    friend class MainWindow;
};

template <typename Type>
Type MainProgram::random(Type start, Type end)
{
    auto range = end-start;
    assert(range != 0 && "random() with zero range!");

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine_);

    return static_cast<Type>(start+num);
}

template <typename To>
To MainProgram::convert_string_to(std::string from)
{
    std::istringstream istr(from);
    To result;
    istr >> std::noskipws >> result;
    if (istr.fail() || !istr.eof())
    {
        throw std::invalid_argument("Cannot convert string to required type");
    }
    return result;
}

template <typename From>
std::string MainProgram::convert_to_string(From from)
{
    std::ostringstream ostr;
    ostr << from;
    if (ostr.fail())
    {
        throw std::invalid_argument("Cannot convert type to string");
    }
    return ostr.str();
}

template<TownID(Datastructures::*MFUNC)()>
MainProgram::CmdResult MainProgram::NoParTownCmd(std::ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds_.*MFUNC)();
    return {ResultType::LIST, {result}};
}

template<std::vector<TownID>(Datastructures::*MFUNC)()>
MainProgram::CmdResult MainProgram::NoParListCmd(std::ostream& /*output*/, MatchIter /*begin*/, MatchIter /*end*/)
{
    auto result = (ds_.*MFUNC)();
    return {ResultType::LIST, result};
}

template<TownID(Datastructures::*MFUNC)()>
void MainProgram::NoParTownTestCmd()
{
    (ds_.*MFUNC)();
}

template<std::vector<TownID>(Datastructures::*MFUNC)()>
void MainProgram::NoParListTestCmd()
{
    (ds_.*MFUNC)();
}


#ifdef USE_PERF_EVENT
extern "C"
{
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags)
{
    int ret;

    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                   group_fd, flags);
    return ret;
}
}
#endif

class MainProgram::Stopwatch
{
public:
    using Clock = std::chrono::high_resolution_clock;

    Stopwatch(bool use_counter = false) : use_counter_(use_counter)
    {
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            memset(&pe_, 0, sizeof(pe_));
            pe_.type = PERF_TYPE_HARDWARE;
            pe_.size = sizeof(pe_);
            pe_.config = PERF_COUNT_HW_INSTRUCTIONS;
            pe_.disabled = 1;
            pe_.exclude_kernel = 1;
            pe_.exclude_hv = 1;

            fd_ = perf_event_open(&pe_, 0, -1, -1, 0);
            if (fd_ == -1) {
                throw "Couldn't open perf events!";
            }
        }
#endif
        reset();
    }

    ~Stopwatch()
    {
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            close(fd_);
        }
#endif
    }

    void start()
    {
        running_ = true;
        starttime_ = Clock::now();
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
            read(fd_, &startcount_, sizeof(startcount_));
            ioctl(fd_, PERF_EVENT_IOC_ENABLE, 0);
        }
#endif
    }

    void stop()
    {
        running_ = false;
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            ioctl(fd_, PERF_EVENT_IOC_DISABLE, 0);
            long long c; read(fd_, &c, sizeof(c));
            counter_ += (c - startcount_);
        }
#endif
        elapsed_ += (Clock::now() - starttime_);
    }

    void reset()
    {
        running_ = false;
#ifdef USE_PERF_EVENT
        if (use_counter_)
        {
            ioctl(fd_, PERF_EVENT_IOC_DISABLE, 0);
            ioctl(fd_, PERF_EVENT_IOC_RESET, 0);
            counter_ = 0;
        }
#endif
        elapsed_ = elapsed_.zero();
    }

    double elapsed()
    {
        if (!running_)
        {
            return static_cast<std::chrono::duration<double>>(elapsed_).count();
        }
        else
        {
            auto total = elapsed_ + (Clock::now() - starttime_);
            return static_cast<std::chrono::duration<double>>(total).count();
        }
    }

#ifdef USE_PERF_EVENT
    long long count()
    {
        if (use_counter_)
        {
            if (!running_)
            {
                return counter_;
            }
            else
            {
                long long c;
                read(fd_, &c, sizeof(c));
                return counter_ + (c - startcount_);
            }
        }
        else
        {
            assert(!"perf_event not enabled during StopWatch creation!");
        }
    }
#endif

private:
    std::chrono::time_point<Clock> starttime_;
    Clock::duration elapsed_ = Clock::duration::zero();
    bool running_ = false;

    bool use_counter_;
#ifdef USE_PERF_EVENT
    struct perf_event_attr pe_;
    long long startcount_ = 0;
    long long counter_ = 0;
    int fd_ = 0;
#endif
};


#endif // MAINPROGRAM_HH
