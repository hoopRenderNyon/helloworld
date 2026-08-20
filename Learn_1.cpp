#include <iostream>
#include <vector>
#include <string>
//导入必要的库文件

struct Point {
    int x, y;
};//声明坐标点

std::vector<std::string> clip(char separator, std::string str){
    int start = 0;//确认分隔段位置
    std::vector<std::string> clip_nums;//创建输出
    int n = 0;
    clip_nums.push_back("");//插入空字符串防止越界
    for (char ch : str){
        if (ch == separator){
            start++;
            clip_nums.push_back("");//检测到分隔符
        } else{
            clip_nums[start].push_back(ch);//正常加入分隔段
        }
    }
    return clip_nums;//返回分隔段
};

int size[2];

std::vector<std::string> string_clips;

int main(){
    char separator;
    std::string str_;
    std::cout<<"请输入分隔符:";
    std::cin >> separator;
    std::cout<<"\n请输入字符串:";
    std::cin >> str_;
    string_clips = clip(separator, str_);
    for (std::string strings : string_clips){
        std::cout<<strings<<"\n";
    }
    std::cout<<"输出完毕喵";
    std::cin.get();
    std::cin.get();
}