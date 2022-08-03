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
                 << "\t 0) 退出。\n"
                 << "\t 1) 复原至标准状态。\n"
                 << "\t 2) 复原至指定状态。\n"
                 << "\t 3) 随机打乱。\n"
                 << "\t 4) 在线输入旋转动作、输出转动状态。\n"
                 << "\t 5) 切换角块是否精确匹配。（当前为 " << boolalpha << exact_match << " ）\n"
                 << "\t 6) 输出初态。\n"
                 << "\t 7) 输出终态。\n"
                 << "\t 8) 交换初态与终态。\n"
                 << "\t 9) 设置映射表深度。（2~9，当前为 " << map_depth << " ）\n" << endl;

            do {
                cout << "输入你的选择：" << endl;
                cin >> choose;
            } while (choose < 0 || choose > 9);

            int input_num = 0, depth = 0;
            switch (choose) {
                case 0:
                    return;
                case 1:
                    arr_state[15] = zero_state;
                    if (!get_state(0, "【输入初态】"))break;
                    generate_map();
                    depth = search_all_path();
                    cout << "最少步骤数为 " << depth << "\n";
                    print_path();
                    break;
                case 2:
                    if (!get_state(15, "【输入终态】"))break;
                    if (!get_state(0, "【输入初态】"))break;
                    generate_map();
                    depth = search_all_path();
                    cout << "最少步骤数为 " << depth << "\n";
                    print_path();
                    break;
                case 3:
                    if (!get_state(0, "【输入初态】"))break;
                    do {
                        cout << "请输入随机打乱步数（1~1000）：" << endl;
                        cin >> input_num;
                    } while (input_num < 1 || input_num > 1000);
                    arr_state[15] = arr_state[0];
                    for (int i = 0, direction = 0; i < input_num; ++i) {
                        direction = random_gen(1, 6);
                        rotate(direction, 15);
                    }
                    generate_map();
                    depth = search_all_path();
                    cout << "最少步骤数为 " << depth << "\n";
                    print_path(1);
                    print_state(15);
                    break;
                case 4:
                    if (!get_state(0, "【输入初态】"))break;
                    arr_state[1] = arr_state[0];
                    for (string a_line; cout << "输入旋转编号 1) 右顺 2) 右逆 3) 上顺 4) 上逆 5) 前顺 6) 前逆，无效编号退出" << endl,
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
                case 6:
                    print_state(0);
                    break;
                case 7:
                    print_state(15);
                    break;
                case 8:
                    arr_state[1] = arr_state[0];
                    arr_state[0] = arr_state[15];
                    arr_state[15] = arr_state[1];
                    break;
                case 9:
                    do {
                        cout << "输入映射表深度。（2~9）" << endl;
                        cin >> map_depth;
                    } while (map_depth < 2 || map_depth > 9);
                    break;

                default:
                    break;
            }
        }
    }

    PocketCube() : PocketCube(6) {
    }

    explicit PocketCube(int map_depth) : map_depth(map_depth) {
        zero_state[0] = {0, 1};
        zero_state[1] = {1, 1};
        zero_state[2] = {2, 1};
        zero_state[3] = {3, 1};
        zero_state[4] = {4, 1};
        zero_state[5] = {5, 1};
        zero_state[6] = {6, 1};
    }

    static ostream &checkout_false(ostream &os, const vector<bool> &flag) {
        for (size_t i = 0; i != flag.size(); ++i) {
            if (!flag[i]) {
                os << i << " ";
            }
        }
        return os;
    }

    bool get_state(int depth, const char *prompt) {
        cout << "\n" << prompt << "\n"
             << "\t1) 保持状态。\n"
             << "\t2) 还原状态。\n"
             << "\t3) 输入状态。" << endl;

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
                     << "\t角块编号： 0) 红白蓝 1) 红白绿 2) 橙白蓝 3) 红黄蓝 4) 红黄绿 5) 橙黄绿 6) 橙黄蓝\n"
                     << "\t红橙色块朝向编号： 0) 前后 1) 左右 2) 上下\n" << endl;
                break;

            default:
                break;
        }

        vector<string> corner_block_prompt{"下前右", "下右后", "下左前", "上前右", "上右后", "上后左", "上左前"};
        vector<bool> input_check_flag(7, false);
        for (int i = 0; i < 7; ++i) {
            checkout_false(cout << setw(2) << i + 1 << ". " << corner_block_prompt[i] << "：角块(", input_check_flag) << ")？朝向？" << endl;
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
                    cout << setw(4) << all_path.size() << ".\n";
                }
                break;
            }
        }
        cout << endl;
    }

    void print_state(int depth) {
        array<string, 7> color{
                "红白蓝",
                "红白绿",
                "橙白蓝",
                "红黄蓝",
                "红黄绿",
                "橙黄绿",
                "橙黄蓝",
        };
        array<string, 3> direction{
                "前后",
                "左右",
                "上下",
        };
        for (array<int, 2> &a_corner: arr_state[depth]) {
            for (int &a_num: a_corner) {
                cout << a_num << " ";
            }
            cout << "\n";
        }
        for (array<int, 2> &a_corner: arr_state[depth]) {
            cout << color[a_corner[0]] << " " << direction[a_corner[1]] << "\n";
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

    int map_depth;
    array<array<int, 2>, 7> zero_state{};
    arr_16_7_2 arr_state{};
    map<int, vector<int>> state_to_path;
    vector<vector<int>> all_path;

    bool exact_match = true;
    steady_clock::time_point start_;

    void generate_map_sub_f1(const int &path_index, const int &path_depth) {
        static auto f_lambda = [](int x) {
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
        static vector<int> path_ = vector<int>(14, 0);
        static vector<int> transform_path = vector<int>(14, 0);
        for (path_[13 - path_index] = 1; path_[13 - path_index] < 7; ++path_[13 - path_index]) {
            if (skip(path_.end() - path_index - 1, path_.end(), false)) continue;
            arr_state[14 - path_index] = arr_state[15 - path_index];
            rotate(path_[13 - path_index], 14 - path_index);
            if (path_index + 1 == path_depth) {
                transform(path_.cend() - path_depth, path_.cend(), transform_path.begin(), f_lambda);
                state_to_path.insert({hash(14 - path_index), vector<int>(transform_path.cbegin(), transform_path.cbegin() + path_depth)});
            }
            if (path_index + 1 < path_depth) {
                generate_map_sub_f1(path_index + 1, path_depth);
            }
        }
        if (path_index == 0 && path_depth < map_depth) {
            generate_map_sub_f1(0, path_depth + 1);
        }
    }

    void generate_map() {
        state_to_path.clear();
        start();
        generate_map_sub_f1(0, 1);
        double second_time = getSecond();
        cout << __FUNCTION__ << "() 耗时 " << second_time << " 秒" << endl;
        cout << "state_to_path.size(): " << state_to_path.size() << endl;
    }

    void search_all_path_sub_f1(const int &path_index, const int &path_depth, const int &hash_goal) {
        static vector<int> path_ = vector<int>(14, 0);
        static int len_map_path = 0;
        for (path_[path_index] = 1; path_[path_index] < 7; ++path_[path_index]) {
            if (skip(path_.begin(), path_.begin() + path_index + 1)) continue;
            arr_state[path_index + 1] = arr_state[path_index];
            rotate(path_[path_index], path_index + 1);
            if (path_index + 1 == path_depth && hash_goal == hash(path_depth)) {
                all_path.emplace_back(path_.cbegin(), path_.cbegin() + path_depth);
            }
            if (path_depth <= 14 - map_depth) {
                if (path_index + 1 < path_depth) {
                    search_all_path_sub_f1(path_index + 1, path_depth, hash_goal);
                }
            } else if (path_index + 2 < path_depth) {
                search_all_path_sub_f1(path_index + 1, path_depth, hash_goal);
            } else {
                len_map_path = look_up_table(path_);
                if (len_map_path > 0) {
                    all_path.emplace_back(path_.cbegin(), path_.cbegin() + 14 - map_depth + len_map_path);
                }
            }
        }
        if (path_index == 0 && all_path.empty() && path_depth <= 14 - map_depth) {
            search_all_path_sub_f1(0, path_depth + 1, hash_goal);
        }
    }

    int search_all_path() {
        all_path.clear();

        int hash_goal = hash(15);
        if (hash_goal == hash(0)) {
            return 0;
        }

        start();
        search_all_path_sub_f1(0, 1, hash_goal);
        double second_time = getSecond();
        cout << __FUNCTION__ << "() 耗时 " << second_time << " 秒" << endl;

        if (all_path.empty()) {
            return -1;
        } else {
            int path_depth = (int) all_path[0].size();
            if (path_depth > 14 - map_depth) {
                path_depth = 14;
                for (vector<int> &a_path: all_path) {
                    if (a_path.size() < path_depth) {
                        path_depth = (int) a_path.size();
                    }
                }
                auto end_remove = remove_if(all_path.begin(), all_path.end(),
                                            [path_depth](vector<int> &a_path) { return a_path.size() > path_depth; });
                all_path.erase(end_remove, all_path.end());
            }
            return path_depth;
        }
    }

    int look_up_table(vector<int> &path_) {
        int hash_connect = hash(14 - map_depth);
        if (state_to_path.find(hash_connect) == state_to_path.end()) {
            return 0;
        } else {
            vector<int> path = state_to_path.at(hash_connect);
            std::copy(path.cbegin(), path.cend(), path_.begin() + 14 - map_depth);
            return (int) path.size();
        }
    }

    void right_clockwise(int n) {
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

    void right_counterclockwise(int n) {
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

    void up_clockwise(int n) {
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

    void up_counterclockwise(int n) {
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

    void front_clockwise(int n) {
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

    void front_counterclockwise(int n) {
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
                right_clockwise(depth);
                break;
            case 2:
                right_counterclockwise(depth);
                break;
            case 3:
                up_clockwise(depth);
                break;
            case 4:
                up_counterclockwise(depth);
                break;
            case 5:
                front_clockwise(depth);
                break;
            case 6:
                front_counterclockwise(depth);
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

    inline
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
