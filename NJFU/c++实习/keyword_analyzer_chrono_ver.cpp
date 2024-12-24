#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace std::chrono;

class KeywordAnalyzer {
private:
    vector<string> keywords;  // 保存保留字
    map<string, int> keywordCount;  // 保留字计数
    map<string, int> nonKeywordCount;  // 非保留字计数
    int scanCount;  // 扫描次数

    // 判断字符串是否为保留字
    bool isKeyword(const string& word) {
        return find(keywords.begin(), keywords.end(), word) != keywords.end();
    }

    // 从字符串中提取单词
    vector<string> extractWords(const string& line) {
        vector<string> words;
        string word;
        for (size_t i = 0; i < line.length(); i++) {
            char c = line[i];
            if (isalnum(c) || c == '_') {
                word += c;
            } else if (!word.empty()) {
                words.push_back(word);
                word = "";
            }
        }
        if (!word.empty()) {
            words.push_back(word);
        }
        return words;
    }

public:
    KeywordAnalyzer() {
        scanCount = 0;
    }

    // 从文件加载保留字
    void loadKeywords(const string& filename) {
        ifstream file(filename.c_str());
        string keyword;
        while (file >> keyword) {
            keywords.push_back(keyword);
            keywordCount[keyword] = 0;
        }
        file.close();
    }

    // 分析源文件
    void analyzeFile(const string& filename) {
        ifstream file(filename.c_str());
        string line;
        scanCount = 0;

        while (getline(file, line)) {
            scanCount++;
            vector<string> words = extractWords(line);
            
            for (size_t i = 0; i < words.size(); i++) {
                if (isKeyword(words[i])) {
                    keywordCount[words[i]]++;
                } else {
                    nonKeywordCount[words[i]]++;
                }
            }
        }
        file.close();
    }

    // 输出统计结果到文件
    void writeResults() {
        // 输出保留字统计
        ofstream keywordFile("keyword_data.txt");
        keywordFile << "保留字统计：\n";
        map<string, int>::iterator it;
        for (it = keywordCount.begin(); it != keywordCount.end(); ++it) {
            if (it->second > 0) {
                keywordFile << it->first << ": " << it->second << "\n";
            }
        }
        keywordFile.close();

        // 输出非保留字统计
        ofstream nonKeywordFile("non_keyword_data.txt");
        nonKeywordFile << "非保留字统计：\n";
        for (it = nonKeywordCount.begin(); it != nonKeywordCount.end(); ++it) {
            nonKeywordFile << it->first << ": " << it->second << "\n";
        }
        nonKeywordFile.close();
    }

    // 获取扫描次数
    int getScanCount() const {
        return scanCount;
    }
};

int main() {
    KeywordAnalyzer analyzer;
    
    // 记录开始时间
    auto start = high_resolution_clock::now();

    // 加载保留字文件
    analyzer.loadKeywords("keywords.txt");

    // 分析源文件
    analyzer.analyzeFile("source.cpp");

    // 输出结果
    analyzer.writeResults();

    // 计算耗时
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    // 输出扫描信息
    cout << "扫描次数: " << analyzer.getScanCount() << endl;
    cout << "扫描用时: " << duration.count() / 1000000.0 << " 秒" << endl;

    return 0;
}