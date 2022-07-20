/*
 * # 颜色
 *
 * 0：红色
 * 1：黄色
 * 2：蓝色
 * 3：绿色
 * 4：白色
 * 5：橙色
 *
 * # 角块顺序编号
 *
 * |5 4|
 * |6 3|
 *  ——
 *   |  1|
 *   |2 0|
 *
 *
 */

#include<ctime>

#include <algorithm>

using std::count_if;
using std::fill;
using std::remove_if;
using std::transform;

#include <array>

using std::array;

#include <chrono>

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::steady_clock;

#include <iomanip>

using std::setw;

#include <iostream>

using std::boolalpha;
using std::cin;
using std::cout;
using std::endl;
using std::ostream;

#include <limits>

using std::numeric_limits;

#include <map>

using std::map;

#include <numeric>

using std::accumulate;

#include <random>

using std::default_random_engine;
using std::random_device;
using std::uniform_int_distribution;

#include <sstream>

using std::istringstream;

#include <string>

using std::getline;
using std::string;

#include <vector>

using std::vector;


class PocketCube {
public:
    using arr_16_7_2 = array<array<array<int, 2>, 7>, 16>;

    void catalog_help() {
        int choose;
        while (true) {
            cout << "\n【选项目录】\n"
                 << "\t0：退出。\n"
                 << "\t1：复原至标准状态。\n"
                 << "\t2：复原至指定状态。\n"
                 << "\t3：随机打乱。\n"
                 << "\t4：在线输入旋转动作、输出转动状态。\n"
                 << "\t5：切换角块是否精确匹配。（当前：" << boolalpha << exact_match << "）" << endl;

            do {
                cout << "输入你的选择：" << endl;
                cin >> choose;
            } while (choose < 0 || choose > 5);

            int input_num = 0, depth = 0;
            switch (choose) {
                case 0:
                    return;
                case 1:
                    arr_state[14] = zero_state;
                    if (!get_state(0, "【输入初态】"))break;
                    generate_map();
                    depth = search_all_path();
                    cout << "最少步骤数为 " << depth << "\n";
                    print_path();
                    break;
                case 2:
                    if (!get_state(14, "【输入终态】"))break;
                    if (!get_state(0, "【输入初态】"))break;
                    generate_map();
                    depth = search_all_path();
                    cout << "最少步骤数为 " << depth << "\n";
                    print_path();
                    break;
                case 3:
                    if (!get_state(0, "【输入初态】"))break;
                    do {
                        cout << "请输入随机打乱步数（0~1000）：" << endl;
                        cin >> input_num;
                    } while (input_num < 0 || input_num > 1000);
                    arr_state[15] = arr_state[0];
                    for (int i = 0, direction = 0; i < input_num; ++i) {
                        direction = random_gen(1, 6);
                        rotate(direction, 15);
                    }
                    generate_map(15);
                    depth = search_all_path(15);
                    cout << "最少步骤数为 " << depth << "\n";
                    print_path(1);
                    break;
                case 4:
                    if (!get_state(0, "【输入初态】"))break;
                    arr_state[1] = arr_state[0];
                    for (string a_line; cout << "输入旋转编号（1：右顺 2：右逆 3：上顺 4：上逆 5：前顺 6：前逆），无效编号退出" << endl,
                            getline(cin, a_line);) {
                        bool exit = false;
                        istringstream line_stream(a_line);
                        for (int direction = 0; line_stream >> direction;) {
                            if (0 < direction && direction < 7) {
                                rotate(direction, 1);
                            } else {
                                exit = true;
                                break;
                            }
                        }
                        print_state(1);
                        if (exit) {
                            break;
                        }
                    }
                    break;
                case 5:
                    exact_match = !exact_match;
                    break;

                default:
                    break;
            }
        }
    }

    PocketCube() {
        zero_state[0] = {0, 1};
        zero_state[1] = {1, 1};
        zero_state[2] = {2, 1};
        zero_state[3] = {3, 1};
        zero_state[4] = {4, 1};
        zero_state[5] = {5, 1};
        zero_state[6] = {6, 1};
    }

    bool get_state(int depth, const char *prompt) {
        cout << "\n" << prompt << "\n"
             << "\t1：保持状态。\n"
             << "\t2：还原状态。\n"
             << "\t3：输入状态。" << endl;

        int choose = 0;
        do {
            cout << "输入你的选择：" << endl;
            cin >> choose;
        } while (choose < 1 || choose > 3);

        switch (choose) {
            case 1:
                return true;
            case 2:
                arr_state[depth] = zero_state;
                return true;
            case 3:
                cout << "\n【假设前提】\n\t面朝颜色：蓝色\n\t左后下方不动角块：左为橙色，后为绿色，下为白色\n"
                     << "\t角块编号： 0：红白蓝 1：红白绿 2：橙白蓝 3：红黄蓝 4：红黄绿 5：橙黄绿 6：橙黄蓝\n"
                     << "\t红橙色块朝向编号： 0：前后 1：左右 2：上下\n" << endl;
                break;

            default:
                break;
        }

        vector<string> corner_block_prompt{"下前右", "下右后", "下左前", "上前右", "上右后", "上后左", "上左前"};
        vector<bool> input_check_flag(7, false);
        for (int i = 0; i < 7; ++i) {
            cout << setw(2) << i + 1 << ". " << corner_block_prompt[i] << "：角块？朝向？" << endl;
            cin >> arr_state[depth][i][0] >> arr_state[depth][i][1];
            if (arr_state[depth][i][0] < 0 || arr_state[depth][i][0] > 6 || arr_state[depth][i][1] < 0 || arr_state[depth][i][1] > 2) {
                cout << "无效输入!" << endl;
                print_state(depth);
                return false;
            }
            int block_num = arr_state[depth][i][0];
            if (!input_check_flag[block_num]) {
                input_check_flag[block_num] = true;
            } else {
                cout << "输入角块重复，再次输入。" << endl;
                --i;
            }
        }
        return true;
    }

    void print_path(int max_path_num = 3) {
        array<string, 7> rotate_num{
                "",
                "右顺",
                "右逆",
                "上顺",
                "上逆",
                "前顺",
                "前逆"
        };
        int row = 0, col = 0;
        for (vector<int> &a_path: all_path) {
            ++row;
            cout << setw(4) << row << ". ";
            col = 0;
            for (int &a_do: a_path) {
                ++col;
                cout << rotate_num[a_do] << " ";
                if (col % 5 == 0) {
                    cout << "  ";
                }
            }
            cout << " |  ";
            col = 0;
            for (int &a_do: a_path) {
                ++col;
                cout << a_do << " ";
                if (col % 5 == 0) {
                    cout << "  ";
                }
            }
            cout << "\n";
            if (row >= max_path_num) {
                if (row < all_path.size()) {
                    cout << "    .\n" << setw(4) << all_path.size() << ".\n";
                }
                break;
            }
        }
        cout << endl;
    }

    void print_state(int depth) {
        for (array<int, 2> &a_corner: arr_state[depth]) {
            for (int &a_num: a_corner) {
                cout << a_num << " ";
            }
            cout << "\n";
        }
        cout << endl;
    }

    void start() {
        start_ = steady_clock::now();
    }

    double getSecond() const {
        auto end_ = steady_clock::now();
        return double(duration_cast<microseconds>(end_ - start_).count())
               * microseconds::period::num / microseconds::period::den;
    }


private:

    array<array<int, 2>, 7> zero_state{};
    arr_16_7_2 arr_state{};
    map<int, vector<int>> state_to_path;
    vector<vector<int>> all_path;

    bool exact_match = true;
    steady_clock::time_point start_;

    void generate_map(int goal_depth = 14) {
        state_to_path.clear();
        auto f_lambda = [](int x) {
            switch (x) {
                case 1:
                    return 2;
                case 2:
                    return 1;
                case 3:
                    return 4;
                case 4:
                    return 3;
                case 5:
                    return 6;
                case 6:
                    return 5;
                default:
                    return 0;
            }
        };
        vector<int> path_ = vector<int>(14, 0);
        vector<int> transform_path = vector<int>(5, 0);
        for (int depth_ = 13, length = 1; depth_ >= 9; --depth_, ++length)    //限制最深5层！
        {
            for (path_[13] = 1; path_[13] < 7; ++path_[13])//13
            {
                arr_state[13] = arr_state[goal_depth];
                rotate(path_[13], 13);
                if (depth_ == 13) {
                    transform(path_.cbegin() + depth_, path_.cend(), transform_path.begin(), f_lambda);
                    state_to_path.insert({hash(13), vector<int>(transform_path.cbegin(), transform_path.cbegin() + length)});
                }
                for (path_[12] = 1; depth_ < 13 && path_[12] < 7; ++path_[12])//12
                {
                    if (skip(path_.begin() + 12, path_.end(), false)) continue;
                    arr_state[12] = arr_state[13];
                    rotate(path_[12], 12);
                    if (depth_ == 12) {
                        transform(path_.cbegin() + depth_, path_.cend(), transform_path.begin(), f_lambda);
                        state_to_path.insert({hash(12), vector<int>(transform_path.cbegin(), transform_path.cbegin() + length)});
                    }
                    for (path_[11] = 1; depth_ < 12 && path_[11] < 7; ++path_[11])//11
                    {
                        if (skip(path_.begin() + 11, path_.end(), false)) continue;
                        arr_state[11] = arr_state[12];
                        rotate(path_[11], 11);
                        if (depth_ == 11) {
                            transform(path_.cbegin() + depth_, path_.cend(), transform_path.begin(), f_lambda);
                            state_to_path.insert({hash(11), vector<int>(transform_path.cbegin(), transform_path.cbegin() + length)});
                        }
                        for (path_[10] = 1; depth_ < 11 && path_[10] < 7; ++path_[10])//10
                        {
                            if (skip(path_.begin() + 10, path_.end(), false)) continue;
                            arr_state[10] = arr_state[11];
                            rotate(path_[10], 10);
                            if (depth_ == 10) {
                                transform(path_.cbegin() + depth_, path_.cend(), transform_path.begin(), f_lambda);
                                state_to_path.insert({hash(10), vector<int>(transform_path.cbegin(), transform_path.cbegin() + length)});
                            }
                            for (path_[9] = 1; depth_ < 10 && path_[9] < 7; ++path_[9])//9
                            {
                                if (skip(path_.begin() + 9, path_.end(), false)) continue;
                                arr_state[9] = arr_state[10];
                                rotate(path_[9], 9);
                                if (depth_ == 9) {
                                    transform(path_.cbegin() + depth_, path_.cend(), transform_path.begin(), f_lambda);
                                    state_to_path.insert({hash(9), vector<int>(transform_path.cbegin(), transform_path.cbegin() + length)});
                                }
                            }
                        }
                    }
                }
            }
        }
        cout << "state_to_path.size(): " << state_to_path.size() << endl;
    }

    int search_all_path(int goal_depth = 14) {
        all_path.clear();

        int hash_goal = hash(goal_depth);
        if (hash_goal == hash(0)) {
            return 0;
        }

        vector<int> path_ = vector<int>(14, 0);
        bool go_deep = true;
        int depth_ = 1, len_reverse = 0;
        start();
        for (; go_deep && depth_ <= 10; ++depth_)    //限制最深9层！
        {
            for (path_[0] = 1; path_[0] < 7; ++path_[0])//1
            {
                arr_state[1] = arr_state[0];
                rotate(path_[0], 1);
                if (depth_ == 1 && hash_goal == hash(1)) {
                    all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                    go_deep = false;
                }
                for (path_[1] = 1; depth_ > 1 && path_[1] < 7; ++path_[1])//2
                {
                    if (skip(path_.begin(), path_.begin() + 2)) continue;
                    arr_state[2] = arr_state[1];
                    rotate(path_[1], 2);
                    if (depth_ == 2 && hash_goal == hash(2)) {
                        all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                        go_deep = false;
                    }
                    for (path_[2] = 1; depth_ > 2 && path_[2] < 7; ++path_[2])//3
                    {
                        if (skip(path_.begin(), path_.begin() + 3)) continue;
                        arr_state[3] = arr_state[2];
                        rotate(path_[2], 3);
                        if (depth_ == 3 && hash_goal == hash(3)) {
                            all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                            go_deep = false;
                        }
                        for (path_[3] = 1; depth_ > 3 && path_[3] < 7; ++path_[3])//4
                        {
                            if (skip(path_.begin(), path_.begin() + 4)) continue;
                            arr_state[4] = arr_state[3];
                            rotate(path_[3], 4);
                            if (depth_ == 4 && hash_goal == hash(4)) {
                                all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                                go_deep = false;
                            }
                            for (path_[4] = 1; depth_ > 4 && path_[4] < 7; ++path_[4])//5
                            {
                                if (skip(path_.begin(), path_.begin() + 5)) continue;
                                arr_state[5] = arr_state[4];
                                rotate(path_[4], 5);
                                if (depth_ == 5 && hash_goal == hash(5)) {
                                    all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                                    go_deep = false;
                                }
                                for (path_[5] = 1; depth_ > 5 && path_[5] < 7; ++path_[5])//6
                                {
                                    if (skip(path_.begin(), path_.begin() + 6)) continue;
                                    arr_state[6] = arr_state[5];
                                    rotate(path_[5], 6);
                                    if (depth_ == 6 && hash_goal == hash(6)) {
                                        all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                                        go_deep = false;
                                    }
                                    for (path_[6] = 1; depth_ > 6 && path_[6] < 7; ++path_[6])//7
                                    {
                                        if (skip(path_.begin(), path_.begin() + 7)) continue;
                                        arr_state[7] = arr_state[6];
                                        rotate(path_[6], 7);
                                        if (depth_ == 7 && hash_goal == hash(7)) {
                                            all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                                            go_deep = false;
                                        }
                                        for (path_[7] = 1; depth_ > 7 && path_[7] < 7; ++path_[7])//8
                                        {
                                            if (skip(path_.begin(), path_.begin() + 8)) continue;
                                            arr_state[8] = arr_state[7];
                                            rotate(path_[7], 8);
                                            if (depth_ == 8 && hash_goal == hash(8)) {
                                                all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                                                go_deep = false;
                                            }
                                            for (path_[8] = 1; depth_ > 8 && path_[8] < 7; ++path_[8])//9
                                            {
                                                if (skip(path_.begin(), path_.begin() + 9)) continue;
                                                arr_state[9] = arr_state[8];
                                                rotate(path_[8], 9);
                                                if (depth_ == 9 && hash_goal == hash(9)) {
                                                    all_path.emplace_back(path_.cbegin(), path_.cbegin() + depth_);
                                                    go_deep = false;
                                                }
                                                if (depth_ > 9) {
                                                    len_reverse = look_up_table(path_);
                                                    if (len_reverse > 0) {
                                                        all_path.emplace_back(path_.cbegin(), path_.cbegin() + 9 + len_reverse);
                                                        go_deep = false;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        double second_time = getSecond();
        cout << "耗时 " << second_time << " 秒" << endl;

        if (go_deep) {
            return -1;
        } else {
            if (depth_ > 9) {
                depth_ = 14;
                for (vector<int> &a_path: all_path) {
                    if (a_path.size() < depth_) {
                        depth_ = (int) a_path.size();
                    }
                }
                auto end_remove = remove_if(all_path.begin(), all_path.end(),
                                            [depth_](vector<int> &a_path) { return a_path.size() > depth_; });
                all_path.erase(end_remove, all_path.end());
                return depth_;
            } else {
                return --depth_;
            }
        }
    }

    int look_up_table(vector<int> &path_) {
        int hash_depth_9 = hash(9);
        if (state_to_path.find(hash_depth_9) == state_to_path.end()) {
            return 0;
        } else {
            vector<int> path = state_to_path.at(hash_depth_9);
            std::copy(path.cbegin(), path.cend(), path_.begin() + 9);
            return (int) path.size();
        }
    }

    void youshun(int n) {
        int i = arr_state[n][0][0];
        arr_state[n][0][0] = arr_state[n][1][0];
        arr_state[n][1][0] = arr_state[n][4][0];
        arr_state[n][4][0] = arr_state[n][3][0];
        arr_state[n][3][0] = i;
        i = arr_state[n][0][1];
        arr_state[n][0][1] = 2 - arr_state[n][1][1];
        arr_state[n][1][1] = 2 - arr_state[n][4][1];
        arr_state[n][4][1] = 2 - arr_state[n][3][1];
        arr_state[n][3][1] = 2 - i;
    }

    void youni(int n) {
        int i = arr_state[n][0][0];
        arr_state[n][0][0] = arr_state[n][3][0];
        arr_state[n][3][0] = arr_state[n][4][0];
        arr_state[n][4][0] = arr_state[n][1][0];
        arr_state[n][1][0] = i;
        i = arr_state[n][0][1];
        arr_state[n][0][1] = 2 - arr_state[n][3][1];
        arr_state[n][3][1] = 2 - arr_state[n][4][1];
        arr_state[n][4][1] = 2 - arr_state[n][1][1];
        arr_state[n][1][1] = 2 - i;
    }

    void shangshun(int n) {
        int i = arr_state[n][3][0];
        arr_state[n][3][0] = arr_state[n][4][0];
        arr_state[n][4][0] = arr_state[n][5][0];
        arr_state[n][5][0] = arr_state[n][6][0];
        arr_state[n][6][0] = i;
        i = arr_state[n][3][1];
        if (arr_state[n][4][1] < 2) {
            arr_state[n][3][1] = 1 - arr_state[n][4][1];
        } else {
            arr_state[n][3][1] = 2;
        }
        if (arr_state[n][5][1] < 2) {
            arr_state[n][4][1] = 1 - arr_state[n][5][1];
        } else {
            arr_state[n][4][1] = 2;
        }
        if (arr_state[n][6][1] < 2) {
            arr_state[n][5][1] = 1 - arr_state[n][6][1];
        } else {
            arr_state[n][5][1] = 2;
        }
        if (i < 2) {
            arr_state[n][6][1] = 1 - i;
        } else {
            arr_state[n][6][1] = 2;
        }
    }

    void shangni(int n) {
        int i = arr_state[n][3][0];
        arr_state[n][3][0] = arr_state[n][6][0];
        arr_state[n][6][0] = arr_state[n][5][0];
        arr_state[n][5][0] = arr_state[n][4][0];
        arr_state[n][4][0] = i;
        i = arr_state[n][3][1];
        if (arr_state[n][6][1] < 2) {
            arr_state[n][3][1] = 1 - arr_state[n][6][1];
        } else {
            arr_state[n][3][1] = 2;
        }
        if (arr_state[n][5][1] < 2) {
            arr_state[n][6][1] = 1 - arr_state[n][5][1];
        } else {
            arr_state[n][6][1] = 2;
        }
        if (arr_state[n][4][1] < 2) {
            arr_state[n][5][1] = 1 - arr_state[n][4][1];
        } else {
            arr_state[n][5][1] = 2;
        }
        if (i < 2) {
            arr_state[n][4][1] = 1 - i;
        } else {
            arr_state[n][4][1] = 2;
        }
    }

    void qianshun(int n) {
        int i = arr_state[n][0][0];
        arr_state[n][0][0] = arr_state[n][3][0];
        arr_state[n][3][0] = arr_state[n][6][0];
        arr_state[n][6][0] = arr_state[n][2][0];
        arr_state[n][2][0] = i;
        i = arr_state[n][0][1];
        if (arr_state[n][3][1]) {
            arr_state[n][0][1] = 3 - arr_state[n][3][1];
        } else {
            arr_state[n][0][1] = 0;
        }
        if (arr_state[n][6][1]) {
            arr_state[n][3][1] = 3 - arr_state[n][6][1];
        } else {
            arr_state[n][3][1] = 0;
        }
        if (arr_state[n][2][1]) {
            arr_state[n][6][1] = 3 - arr_state[n][2][1];
        } else {
            arr_state[n][6][1] = 0;
        }
        if (i) {
            arr_state[n][2][1] = 3 - i;
        } else {
            arr_state[n][2][1] = 0;
        }
    }

    void qiani(int n) {
        int i = arr_state[n][0][0];
        arr_state[n][0][0] = arr_state[n][2][0];
        arr_state[n][2][0] = arr_state[n][6][0];
        arr_state[n][6][0] = arr_state[n][3][0];
        arr_state[n][3][0] = i;
        i = arr_state[n][0][1];
        if (arr_state[n][2][1]) {
            arr_state[n][0][1] = 3 - arr_state[n][2][1];
        } else {
            arr_state[n][0][1] = 0;
        }
        if (arr_state[n][6][1]) {
            arr_state[n][2][1] = 3 - arr_state[n][6][1];
        } else {
            arr_state[n][2][1] = 0;
        }
        if (arr_state[n][3][1]) {
            arr_state[n][6][1] = 3 - arr_state[n][3][1];
        } else {
            arr_state[n][6][1] = 0;
        }
        if (i) {
            arr_state[n][3][1] = 3 - i;
        } else {
            arr_state[n][3][1] = 0;
        }
    }

    void rotate(int direction, int depth) {
        switch (direction) {
            case 1:
                youshun(depth);
                break;
            case 2:
                youni(depth);
                break;
            case 3:
                shangshun(depth);
                break;
            case 4:
                shangni(depth);
                break;
            case 5:
                qianshun(depth);
                break;
            case 6:
                qiani(depth);
                break;

            default:
                break;
        }
    }

    static bool skip(vector<int>::iterator begin, vector<int>::iterator end, bool write = true) {
        for (auto begin_ = begin; end - begin_ > 1; ++begin_) {
            if (abs(*begin_ - *(begin_ + 1)) == 1 && (*begin_ + *(begin_ + 1) == 3 || *begin_ + *(begin_ + 1) == 7 || *begin_ + *(begin_ + 1) == 11)) {
                if (write) {
                    fill(begin_ + 2, end, 6);
                }
                return true;
            }
        }
        for (auto begin_ = begin; end - begin_ > 2; ++begin_) {
            if (*begin_ == *(begin_ + 1) && *(begin_ + 1) == *(begin_ + 2)) {
                if (write) {
                    fill(begin_ + 3, end, 6);
                }
                return true;
            }
        }
        return false;
    }

    int hash(int depth) {
        if (exact_match) { // 上界600362847*3-1=1801088540 < std::numeric_limits<int>::max()=2147483647
            return arr_state[depth][0][0] + arr_state[depth][0][1] * 7 +
                   arr_state[depth][1][0] * 21 + arr_state[depth][1][1] * 147 +
                   arr_state[depth][2][0] * 441 + arr_state[depth][2][1] * 3087 +
                   arr_state[depth][3][0] * 9261 + arr_state[depth][3][1] * 64827 +
                   arr_state[depth][4][0] * 194481 + arr_state[depth][4][1] * 1361367 +
                   arr_state[depth][5][0] * 4084101 + arr_state[depth][5][1] * 28588707 +
                   arr_state[depth][6][0] * 85766121 + arr_state[depth][6][1] * 600362847;
        } else {
            return arr_state[depth][0][0] +
                   arr_state[depth][1][0] * 7 +
                   arr_state[depth][2][0] * 49 +
                   arr_state[depth][3][0] * 343 +
                   arr_state[depth][4][0] * 2401 +
                   arr_state[depth][5][0] * 16807 +
                   arr_state[depth][6][0] * 117649;
        }
    }

    static int random_gen(int min = 0, int max = numeric_limits<int>::max(), unsigned seed = time(nullptr)) {
        static random_device seed_hardware;
        static default_random_engine e(seed ^ seed_hardware());
        static uniform_int_distribution<int> ud(min, max);
        ud.param(uniform_int_distribution<int>::param_type{min, max});
        return ud(e);
    }

};

int main() {
    PocketCube pocketCube = PocketCube();
    pocketCube.catalog_help();
}
