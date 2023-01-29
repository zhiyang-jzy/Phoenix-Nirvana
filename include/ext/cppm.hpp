/* *
 *
 *   █████╗░██████╗░██████╗░███╗░░░███╗
 *  ██╔══██╗██╔══██╗██╔══██╗████╗░████║
 *  ██║░░╚═╝██████╔╝██████╔╝██╔████╔██║
 *  ██║░░██╗██╔═══╝░██╔═══╝░██║╚██╔╝██║
 *  ╚█████╔╝██║░░░░░██║░░░░░██║░╚═╝░██║
 *   ╚════╝░╚═╝░░░░░╚═╝░░░░░╚═╝░░░░░╚═╝
 *    C++ Program Progress Monitor
 *    https://github.com/soraxas/cppm
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 *
 * MIT License
 *
 * Copyright (c) 2020-2022 Tin Lai (@soraxas) <oscar@tinyiu.com>
 *
 * Permission is hereby  granted, free of charge, to any  person obtaining a copy
 * of this software and associated  documentation files (the "Software"), to deal
 * in the Software  without restriction, including without  limitation the rights
 * to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
 * copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
 * IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
 * FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
 * AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
 * LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * */
#pragma once
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <ios>
#include <iostream>
#include <math.h>
#include <numeric>
#include <signal.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
// for getting terminal size
#include <sys/ioctl.h>  //ioctl() and TIOCGWINSZ

#include <unistd.h>  // for STDOUT_FILENO

#define UNUSED(x) (void)(x)
#ifndef CPPM_MIN_PBAR_WIDTH
#define CPPM_MIN_PBAR_WIDTH 5
#endif

#ifdef CPPM_USE_MUTEX
#define CPPM_COUNT_T std::atomic<size_t>
#define CPPM_COUNT_LOAD(var) var.load()
#else
#define CPPM_COUNT_T size_t
#define CPPM_COUNT_LOAD(var) var
#endif

namespace cppm
{
    using fdtype = float;

    ////////////////////////////////////////////////////////////
    // tricks to initialise static variable in header across translational units
    // https://stackoverflow.com/questions/38043442/how-do-inline-variables-work
    template <class Dummy>
    struct StaticVariables_
    {
        static unsigned int terminal_width;
        static FILE *def_outfile;
    };
    // set default variables
    template <class Dummy>
    unsigned int StaticVariables_<Dummy>::terminal_width = 80;
    template <class Dummy>
    FILE *StaticVariables_<Dummy>::def_outfile = stderr;

    using StaticVariables = StaticVariables_<void>;  // easier naming
    ////////////////////////////////////////////////////////////

    // helpers and constants
    void flush_stdout(int sig);
    void update_terminal_width(int sig = -1);
    void hsv_to_rgb(float h, float s, float v, int &r, int &g, int &b);
    const char *const COLOR_RESET = "\033[0m\033[32m\033[0m\015";
    const char *const COLOR_RED = "\033[1m\033[31m";   // with bold
    const char *const COLOR_BLUE = "\033[1m\033[34m";  // with bold
    const char *const COLOR_LIME = "\033[32m";

    class pm
    {
    public:
        bool format_suffix_floating_pt = true;
        bool use_ema = true;
        float alpha_ema = 0.1;
        const float min_update_time = 0.15;

    protected:
        // time, iteration counters and deques for rate calculations
        std::chrono::time_point<std::chrono::system_clock> t_first =
                std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> t_old = std::chrono::system_clock::now();
        int n_old = 0;
        std::vector<fdtype> deq_t;
        std::vector<int> deq_n;
        size_t nupdates = 0;
        size_t total_ = 0;
        CPPM_COUNT_T cur_ = 0;
        size_t period = 1;
        unsigned int smoothing = 50;
        bool has_total_it = false;
        bool print_bar = false;
        bool finished_ = false;
        std::stringstream suffix_;
        std::string suffix;
        FILE *outfile_ = StaticVariables::def_outfile;

        // short terminal_width = 80;
        std::vector<const char *> bars = {" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};

        bool in_screen = (system("test $STY") == 0);
        bool in_tmux = (system("test $TMUX") == 0);
        bool is_tty = isatty(1);
        bool use_colors = true;
        bool color_transition = true;
        bool enable_speed_stats = true;
        int bar_width = 40;

        /////////////////////////////////////
        fdtype __tmp_pct = 0.;
        fdtype __tmp_avgrate = 0.;
        fdtype __tmp_remain_t = 0.;
        fdtype __tmp_dt_tot = 0.;
        /////////////////////////////////////

        std::string left_pad = "▕";
        std::string right_pad = "▏";
        std::string label = "";

    protected:
        /////////////////////////////////////
        // formatting
        /////////////////////////////////////
        inline void _print_color(const char *color)
        {
            if (use_colors)
                fprintf(outfile_, "%s", color);
        }

        inline void _print_bar()
        {
            fdtype pct = __tmp_pct > 1. ? 1. : __tmp_pct;  // clamp
            fdtype fills = (pct * bar_width);
            int ifills = fills;

            if (use_colors)
            {
                if (color_transition)
                {
                    // red (hue=0) to green (hue=1/3)
                    int r = 255, g = 255, b = 255;
                    hsv_to_rgb(0.0 + pct / 3, 0.65, 1.0, r, g, b);
                    fprintf(outfile_, "\033[38;2;%d;%d;%dm", r, g, b);
                }
                else
                    fprintf(outfile_, "%s", COLOR_LIME);
            }
            fprintf(outfile_, "%s", left_pad.c_str());
            for (int i = 0; i < ifills; i++)
                fprintf(outfile_, "%s", bars[8]);
            if (!in_screen and (pct < 1.0))
                fprintf(outfile_, "%s", bars[(int)(8.0 * (fills - ifills))]);
            for (int i = 0; i < bar_width - ifills - 1; i++)
                fprintf(outfile_, "%s", bars[0]);
            fprintf(outfile_, "%s", right_pad.c_str());
        }

        inline void _format_speed(std::ostringstream &oss, const fdtype &avgrate)
        {
            if (!enable_speed_stats)
                return;

            const char *unit = "Hz";
            fdtype div = 1.;
            if (avgrate > 1e6)
                unit = "MHz", div = 1.0e6;
            else if (avgrate > 1e3)
                unit = "kHz", div = 1.0e3;

            oss << std::fixed << std::setprecision(1) << avgrate / div << unit;
        }

        virtual inline void _sstream_progress_text(std::ostringstream &oss)
        {
            oss << cur_ << "/" << total_;
        }

        inline void _format_simplify_time_(std::ostringstream &oss, int seconds)
        {
            int hours, minutes, days;
            hours = minutes = days = -1;
            if (seconds >= 60)
            {
                minutes = seconds / 60;
                seconds = seconds % 60;
                if (minutes >= 60)
                {
                    hours = minutes / 60;
                    minutes = minutes % 60;
                    if (hours >= 24)
                    {
                        days = hours / 24;
                        hours = hours % 24;
                    }
                }
            }
            if (days >= 0)
                oss << std::setw(2) << std::setfill('0') << days << "d";
            if (hours >= 0)
                oss << std::setw(2) << std::setfill('0') << hours << "h";
            if (minutes >= 0)
                oss << std::setw(2) << std::setfill('0') << minutes << "m";
            oss << std::setw(2) << std::setfill('0') << seconds << "s";
        }

        /////////////////////////////////////
        // internal output and house keeping
        /////////////////////////////////////
        inline void _print_progress()
        {
            fprintf(outfile_, "\015");  // clear line
            // label and pct
            std::ostringstream pbar_pct;
            std::ostringstream pbar_suf;

            pbar_pct << label << std::fixed << std::setprecision(1) << std::setw(5)
                     << std::setfill(' ') << __tmp_pct * 100 << "%";
            std::string pbar_pct_str = pbar_pct.str();

            // always maintain the last non-empty suffix if the new one is empty
            std::string tmp;
            tmp = suffix_.str();
            if (tmp.length() > 0)
            {
                suffix = tmp.insert(0, " ");
            }
            if (has_total_it || print_bar)
            {
                // percentage
                _sstream_progress_text(pbar_suf);
                pbar_suf << " [", _format_speed(pbar_suf, __tmp_avgrate);
                pbar_suf << "|", _format_simplify_time_(pbar_suf, __tmp_dt_tot);
                pbar_suf << "<", _format_simplify_time_(pbar_suf, __tmp_remain_t);
                pbar_suf << "]";

                std::string pbar_suf_str = pbar_suf.str();

                compute_pbar_size(
                        pbar_pct_str.length() + pbar_suf_str.length() + suffix.length() + 2
                );
                _print_color(COLOR_RED);
                fprintf(outfile_, "%s", pbar_pct_str.c_str());
                _print_bar();
                _print_color(COLOR_BLUE);
                fprintf(outfile_, "%s", pbar_suf_str.c_str());
                fprintf(outfile_, "%s", COLOR_LIME);
                fprintf(outfile_, "%s", suffix.c_str());
            }
            else
            {
                _print_color(COLOR_BLUE);
                pbar_suf << "[", _format_speed(pbar_suf, __tmp_avgrate);
                pbar_suf << "|", _format_simplify_time_(pbar_suf, __tmp_dt_tot);
                pbar_suf << "]";

                std::string pbar_suf_str = pbar_suf.str();
                fprintf(outfile_, "%4ldit %s", CPPM_COUNT_LOAD(cur_), pbar_suf_str.c_str());
                fprintf(outfile_, "%s", COLOR_LIME);
                fprintf(outfile_, "%s", suffix.c_str());
            }

            // finish printing
            _print_color(COLOR_RESET);
            // if(!has_total_it || (total_ - cur_) > period) fflush(outfile_);
            fflush(outfile_);
        }

        inline void _internal_update_end()
        {
            suffix_.str("");
        }

        virtual inline void _compute_total()
        {
            if (has_total_it)
            {
                __tmp_remain_t = (total_ - cur_) / __tmp_avgrate;
                __tmp_pct = ((fdtype)cur_) / ((fdtype)total_);
                // // last small chunk of percentage.
                // if ((total_ - cur_) <= period)
                // {
                //     __tmp_pct = 1.;
                //     __tmp_avgrate = total_ / __tmp_dt_tot;
                //     // cur_ = total_;
                //     __tmp_remain_t = 0;
                // }
            }
        }

        inline bool _is_about_to_finish() const
        {
            bool about_to_finish = has_total_it && total_ - cur_ < 2;  // will finish loop soon

            return about_to_finish;
        }

        inline bool
        _should_update_display(std::chrono::system_clock::time_point &now, float &dt) const
        {
            if (cur_ % period == 0)
            {
                now = std::chrono::system_clock::now();
                dt = ((std::chrono::duration<fdtype>)(now - t_old)).count();

                // do nothing if last refresh time is too recent.
                if (dt >= min_update_time)
                    return true;
            }
            return false;
        }

        inline bool _should_update_display() const
        {
            std::chrono::system_clock::time_point now;
            float dt;
            return _should_update_display(now, dt);
        }

        inline bool _internal_update()
        {
            if (is_tty)
            {
                std::chrono::system_clock::time_point now;
                float dt;

                bool should_update_display = _should_update_display(now, dt);
                if (should_update_display)
                    nupdates++;

                if (!should_update_display && !_is_about_to_finish())
                    return false;

                __tmp_dt_tot = ((std::chrono::duration<fdtype>)(now - t_first)).count();
                int dn = cur_ - n_old;
                n_old = cur_;
                t_old = now;
                if (deq_n.size() >= smoothing)
                    deq_n.erase(deq_n.begin());
                if (deq_t.size() >= smoothing)
                    deq_t.erase(deq_t.begin());
                deq_t.push_back(dt);
                deq_n.push_back(dn);

                __tmp_avgrate = 0.;
                if (use_ema)
                {
                    __tmp_avgrate = deq_n[0] / deq_t[0];
                    for (unsigned int i = 1; i < deq_t.size(); i++)
                    {
                        fdtype r = 1.0 * deq_n[i] / deq_t[i];
                        __tmp_avgrate = alpha_ema * r + (1.0 - alpha_ema) * __tmp_avgrate;
                    }
                }
                else
                {
                    fdtype dtsum = std::accumulate(deq_t.begin(), deq_t.end(), 0.);
                    int dnsum = std::accumulate(deq_n.begin(), deq_n.end(), 0.);
                    __tmp_avgrate = dnsum / dtsum;
                }

                // learn an appropriate period length to avoid spamming outfile_
                // and slowing down the loop, shoot for ~25Hz and smooth over 3 seconds
                if (nupdates > 10)
                {
                    period = (int)(std::min(std::max((1.0 / 25) * cur_ / __tmp_dt_tot, 1.0), 5e5));
                    smoothing = 25 * 3;
                }
                _compute_total();
                return true;
            }
            return false;
        }

        inline void compute_pbar_size(const int &other_length)
        {
            bar_width = StaticVariables::terminal_width - other_length;
            if (bar_width < 0)                    // the available width is too small.
                bar_width = CPPM_MIN_PBAR_WIDTH;  // default width.
        }

    public:
        pm()
        {
            if (in_screen)
                set_theme_basic(), color_transition = false;
            else if (in_tmux)
                color_transition = false;
            update_terminal_width();
            signal(SIGINT,
                   flush_stdout);  // flush outfile_ when program is exiting
            signal(SIGWINCH,
                   update_terminal_width);  // update width when terminal is resizing
            show_console_cursor(false);     // default to not show cursor
            _print_progress();
        }

        pm(const ssize_t total) : pm()
        {
            total_ = total;
            has_total_it = true;
        }

#ifndef CPPM_USE_MUTEX
        pm(pm &&) = default;
#endif

        ~pm()
        {
            if (outfile_ != StaticVariables::def_outfile)
            {
                fclose(outfile_);
                outfile_ = StaticVariables::def_outfile;
            }
            finish();
        }

        /**
         * Compute where we should update the display in the next tick
         *
         * @param compute_dt if true, compute dt for more accurate (but slightly
         * computational expensive) prediction.
         * @return
         */
        inline bool willUpdateDisplay() const
        {
            return _is_about_to_finish() || _should_update_display();
        }

        inline size_t n()
        {
            return cur_;
        }

        void setOutFilename(const char *filename)
        {
            outfile_ = fopen(filename, "w");
        }

        inline void show_console_cursor(bool const show)
        {
            fprintf(outfile_, show ? "\033[?25h" : "\033[?25l");
        }

        // format fdtype nicely with some fixed percision
        template <class T>
        pm &operator<<(const T &t);

        void reset()
        {
            t_first = std::chrono::system_clock::now();
            t_old = std::chrono::system_clock::now();
            n_old = 0;
            deq_t.clear();
            deq_n.clear();
            period = 1;
            nupdates = 0;
            total_ = 0;
            cur_ = 0;
            has_total_it = false;
            finished_ = false;
            label = "";
            outfile_ = StaticVariables::def_outfile;
            update_terminal_width();
            _internal_update_end();
        }

        ///////////////////////////////////////////////////////////////
        inline void atomic_increment()
        {
#ifndef CPPM_USE_MUTEX
            throw std::runtime_error("CPPM had not define the flag '#define "
                                     "CPPM_COUNT_LOAD' to enable atomic counting.");
#endif
            /* Called to increment internal counter */
            ++cur_;
        }

        inline void print_progress()
        {
            if (finished_)
                return;
            _print_progress();
            _internal_update_end();
        }

        inline void try_print_progress()
        {
            if (finished_)
                return;
            if (_internal_update())
                _print_progress();
            _internal_update_end();
        }

        virtual void update()
        {
            if (finished_)
                return;
            ++cur_;
            try_print_progress();
        }

        void progress(int curr, int tot)
        {
            /* Called directly set current counter and total */
            cur_ = curr, total_ = tot;
            has_total_it = true;
            try_print_progress();
        }

        ///////////////////////////////////////////////////////////////

        void set_theme_line()
        {
            bars = {"─", "─", "─", "╾", "╾", "╾", "╾", "━", "═"};
        }

        void set_theme_circle()
        {
            bars = {" ", "◓", "◑", "◒", "◐", "◓", "◑", "◒", "#"};
        }

        void set_theme_braille()
        {
            bars = {" ", "⡀", "⡄", "⡆", "⡇", "⡏", "⡟", "⡿", "⣿"};
        }

        void set_theme_braille_spin()
        {
            bars = {" ", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠇", "⠿"};
        }

        void set_theme_vertical()
        {
            bars = {"▁", "▂", "▃", "▄", "▅", "▆", "▇", "█", "█"};
        }

        void set_theme_basic()
        {
            bars = {" ", " ", " ", " ", " ", " ", " ", " ", "#"};
            left_pad = "|";
            right_pad = "|";
        }

        void set_label(std::string label_)
        {
            label = label_.append(" ");
        }

        virtual void set_total(const int total)
        {
            total_ = total;
            has_total_it = true;
        }

        virtual void set_total(const double total)
        {
            set_total(static_cast<int>(total));
        }

        void disable_colors()
        {
            color_transition = use_colors = false;
        }

        void finish(bool set_to_total = false)
        {
            if (finished())
                return;
            finished_ = true;
            if (set_to_total)
                if (has_total_it)
                    cur_ = total_;
            _compute_total();
            _print_progress();
            fprintf(outfile_, "\n");
            fflush(outfile_);
        }

        inline bool finished() const
        {
            return finished_;
        }
    };

    class pm_timer : public pm
    {
    protected:
        fdtype total_seconds_ = 0.;

        inline void _compute_total() override
        {
            fdtype passed_time = elapsed();
            __tmp_remain_t = total_seconds_ - passed_time;
            __tmp_pct = passed_time / total_seconds_;
        }

        virtual inline void _sstream_progress_text(std::ostringstream &oss) override
        {
            oss << cur_ << "it";
        }

    public:
        pm_timer() = delete;

        void set_total(const int total) override
        {
            set_total(static_cast<double>(total));
        }

        void set_total(const double total) override
        {
            total_seconds_ = total;
        }

        pm_timer(const int seconds) : pm_timer((double)seconds)
        {
        }

        pm_timer(const double seconds) : pm(), total_seconds_(seconds)
        {
            print_bar = true;
        }

        fdtype elapsed() const
        {
            auto now = std::chrono::system_clock::now();

            fdtype passed_time = ((std::chrono::duration<fdtype>)(now - t_first)).count();
            return passed_time;
        }

        void progress(int curr, int tot) = delete;
    };

    template <class It>
    class IteratorProgressMonitor : public pm
    {
    protected:
        // update() should not be called directly on iterator wrapper
        void update()
        {
            pm::update();
        }

    public:
        IteratorProgressMonitor(It it, It it_end)
                : IteratorProgressMonitor(it, it_end, std::distance(it, it_end))
        {
        }

        IteratorProgressMonitor(It it, It it_end, int total)
                : pm(), iter_begin_(std::move(it)), iter_end_(std::move(it_end))
        {
            total_ = total;
            has_total_it = true;
            _print_progress();
        }

        IteratorProgressMonitor(IteratorProgressMonitor &&) = default;

        ~IteratorProgressMonitor()
        {
            // finish bar when iterator is done.
            //    finish();
        }

        struct iterator
        {
            // // using iterator_category = typename It::iterator_category;
            // using value_type = typename It::value_type;
            // // using difference_type = typename It::difference_type;
            // using pointer = typename It::pointer;
            using reference = typename It::reference;

        public:
            iterator(IteratorProgressMonitor<It> &parent, It &inner_iter)
                    : parent_(parent), iter_(inner_iter)
            {
            }

            inline iterator &operator++()
            {
                ++(iter_);
                i++;
                // std::cout << i << std::endl;
                parent_.update();
                return *this;
            }

            inline iterator operator++(int)
            {
                auto retval = *this;
                ++(*this);
                parent_.update();
                return retval;
            }

            inline bool operator==(const iterator &other) const
            {
                return iter_ == other.iter_;
            }

            inline bool operator!=(const iterator &other) const
            {
                return !(*this == other);
            }

            inline reference operator*() const
            {
                return *iter_;
            }

            int i = 0;

        private:
            IteratorProgressMonitor<It> &parent_;
            It &iter_;
        };

        inline iterator begin()
        {
            return IteratorProgressMonitor<It>::iterator(*this, iter_begin_);
        }

        inline iterator end()
        {
            return IteratorProgressMonitor<It>::iterator(*this, iter_end_);
        }

    protected:
        It iter_begin_;
        It iter_end_;

    };  // class iter

    // progress monitor for rvalue, needed to take ownership of container
    template <class Container>
    class IteratorProgressMonitorForRvalue
            : public IteratorProgressMonitor<typename Container::const_iterator>
    {
        using It = typename Container::const_iterator;

    public:
        IteratorProgressMonitorForRvalue(const Container &&container)
                : IteratorProgressMonitor<It>(container.begin(), container.end()), C(std::move(container))
        {
            // we need to reassign AGAIN because container is now invalid, and
            // we need to assign the begin and end from `C`.
            this->iter_begin_ = C.begin();
            this->iter_end_ = C.end();
        }

        Container C;
    };

    ///////////////////////////////////////////////////////////////
    // Builtin range iterator
    ///////////////////////////////////////////////////////////////

    template <class IntType>
    class RangeContainer
    {
        class RangeIterator
        {
        public:
            RangeIterator(IntType value_, IntType step_) : value(value_), step(step_)
            {
            }

            bool operator!=(RangeIterator const &other) const
            {
                return !(*this == other);
            }

            bool operator==(RangeIterator const &other) const
            {
                if (step > 0)
                    return value >= other.value;
                else
                    return value <= other.value;
            }

            IntType const &operator*() const
            {
                return value;
            }

            RangeIterator &operator++()
            {
                // ++value;
                value += step;
                return *this;
            }

            typedef const IntType &reference;

        private:
            IntType value;
            IntType step;
        };

    public:
        RangeContainer(IntType from_, IntType to_, IntType step_)
                : from(from_), to(to_), step(step_)
        {
            if (step_ == 0)
                throw std::invalid_argument("step size must be non-zero");
            if (from_ < to_ && step_ < 0)
                throw std::invalid_argument("step size must be positive if `from` is less than "
                                            "`to`!");
            if (from_ > to_ && step_ > 0)
                throw std::invalid_argument("step size must be negative if `from` is larger than "
                                            "`to`!");
        }

        RangeContainer(IntType from_, IntType to_) : RangeContainer(from_, to_, 1)
        {
        }

        RangeContainer(IntType to_) : RangeContainer(0, to_, 1)
        {
        }

        RangeIterator begin() const
        {
            return RangeIterator(from, step);
        }

        RangeIterator end() const
        {
            return RangeIterator(to, -step);
        }

    private:
        IntType const from;
        IntType const to;
        IntType const step;
    };

    ///////////////////////////////////////////////////////////////
    // public interface for accessing pm as a wrapper iterator
    ///////////////////////////////////////////////////////////////
    template <class It>
    auto iter(const It &first, const It &last)
    {
        return IteratorProgressMonitor<It>(first, last);
    }

    template <class It>
    auto iter(const It &first, const It &last, const size_t total)
    {
        return IteratorProgressMonitor<It>(first, last, total);
    }

    // lvalue container
    template <class Container>
    auto iter(const Container &C)
    {
        return iter(C.begin(), C.end());
    }

    // rvalue container
    template <class Container>
    auto iter(const Container &&C)
    {
        return IteratorProgressMonitorForRvalue<Container>(std::move(C));
    }

    // create an implicit iterator, similar to tqdm.trange
    template <class IntType>
    auto range(IntType start, IntType end, IntType step = 1)
    {
        RangeContainer<IntType> rc(start, end, step);
        IntType differences = (end - start);
        size_t it_len = differences / step;

        // add one if there are any reminder.
        IntType leap = it_len * step;
        if (start < end)
        {
            if (start + leap < end)
                it_len += 1;
        }
        else
        {
            if (start + leap > end)
                it_len += 1;
        }

        return iter(rc.begin(), rc.end(), it_len);
    }

    template <class IntType>
    auto range(IntType end)
    {
        return range((IntType)0, end, (IntType)1);
    }

#define FIXED_PREC(x) std::fixed << std::setprecision(x)
#define SCIEN_PREC(x) std::scientific << std::setprecision(x)

    // implementation of specialised formatting of float/fdtype
    // make it a fixed percision if it is float or double
    template <>
    inline pm &pm::operator<< <double>(const double &t)
    {
        if (format_suffix_floating_pt)
        {
            double abs_t = abs(t);

            if (abs_t < 1e-3 || abs_t > 1e3)
            {
                // format as x.xxe-08
                suffix_ << SCIEN_PREC(2)
                        << t;  // -1 because this mode includes a number before decimal place
            }
            else
            {
                unsigned short num_decimal_place;
                if (abs_t < 1)
                {
                    // format as 0.xxx
                    num_decimal_place = 3;
                }
                else if (abs_t < 100)
                {
                    // format as 12.34
                    num_decimal_place = 2;
                }
                else /* if (abs_t < 1000) */
                {
                    // format as 123.4
                    num_decimal_place = 1;
                }
                suffix_ << SCIEN_PREC(num_decimal_place)
                        << t;  // -1 because this mode includes a number before decimal place
            }
        }
        else
        {
            suffix_ << t;
        }

        return *this;
    }

    template <>
    inline pm &pm::operator<< <float>(const float &t)
    {
        return pm::operator<< <double>(t);
    }

    template <class T>
    inline pm &pm::operator<<(const T &t)
    {
        suffix_ << t;
        return *this;
    }

    ///////////////////////////////////////////////////////////////
    // Helpers
    ///////////////////////////////////////////////////////////////
    inline void flush_stdout(int sig)
    {
        fprintf(StaticVariables::def_outfile, "\n");
        fflush(StaticVariables::def_outfile);
        signal(sig, SIG_DFL);
        raise(sig);
    }

    inline void update_terminal_width(int sig)
    {
        UNUSED(sig);
        struct winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        StaticVariables::terminal_width = size.ws_col;
    }

    inline void hsv_to_rgb(float h, float s, float v, int &r, int &g, int &b)
    {
        if (s < 1e-6)
        {
            v *= 255., r = v, g = v, b = v;
        }
        int i = (int)(h * 6.0);
        float f = (h * 6.) - i;
        int p = (int)(255.0 * (v * (1. - s)));
        int q = (int)(255.0 * (v * (1. - s * f)));
        int t = (int)(255.0 * (v * (1. - s * (1. - f))));
        v *= 255;
        i %= 6;
        int vi = (int)v;
        switch (i)
        {
            case 0:
                r = vi, g = t, b = p;
                break;
            case 1:
                r = q, g = vi, b = p;
                break;
            case 2:
                r = p, g = vi, b = t;
                break;
            case 3:
                r = p, g = q, b = vi;
                break;
            case 4:
                r = t, g = p, b = vi;
                break;
            case 5:
                r = vi, g = p, b = q;
                break;
        }
    }
};  // end namespace cppm

#undef UNUSED
#undef CPPM_MIN_PBAR_WIDTH
#undef CPPM_USE_MUTEX
#undef CPPM_COUNT_T
#undef CPPM_COUNT_LOAD

// #endif