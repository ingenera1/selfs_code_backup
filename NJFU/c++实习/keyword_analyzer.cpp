#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <sstream>
#include <algorithm>

using namespace std;

class KeywordAnalyzer {
private:
    vector<string> keywords;  // ���汣����
    map<string, int> keywordCount;  // �����ּ���
    map<string, int> nonKeywordCount;  // �Ǳ����ּ���
    int scanCount;  // ɨ�����

    // �ж��ַ����Ƿ�Ϊ������
    bool isKeyword(const string& word) {
        return find(keywords.begin(), keywords.end(), word) != keywords.end();
    }

    // ���ַ�������ȡ����
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

    // ���ļ����ر�����
    void loadKeywords(const string& filename) {
        ifstream file(filename.c_str());
        string keyword;
        while (file >> keyword) {
            keywords.push_back(keyword);
            keywordCount[keyword] = 0;
        }
        file.close();
    }

    // ����Դ�ļ�
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

    // ���ͳ�ƽ�����ļ�
    void writeResults() {
        // ���������ͳ��
        ofstream keywordFile("keyword_data.txt");
        keywordFile << "������ͳ�ƣ�\n";
        for (map<string, int>::iterator it = keywordCount.begin(); it != keywordCount.end(); ++it) {
            if (it->second > 0) {
                keywordFile << it->first << ": " << it->second << "\n";
            }
        }
        keywordFile.close();

        // ����Ǳ�����ͳ��
        ofstream nonKeywordFile("non_keyword_data.txt");
        nonKeywordFile << "�Ǳ�����ͳ�ƣ�\n";
        for (map<string, int>::iterator it = nonKeywordCount.begin(); it != nonKeywordCount.end(); ++it) {
            nonKeywordFile << it->first << ": " << it->second << "\n";
        }
        nonKeywordFile.close();
    }

    // ��ȡɨ�����
    int getScanCount() const {
        return scanCount;
    }
};

int main() {
    KeywordAnalyzer analyzer;
    
    // ��¼��ʼʱ��
    clock_t start = clock();

    // ���ر������ļ�
    analyzer.loadKeywords("keywords.txt");

    // ����Դ�ļ�
    analyzer.analyzeFile("source.cpp");

    // ������
    analyzer.writeResults();

    // �����ʱ
    clock_t end = clock();
    double duration = (double)(end - start) / CLOCKS_PER_SEC;

    // ���ɨ����Ϣ
    cout << "ɨ�����: " << analyzer.getScanCount() << endl;
    cout << "ɨ����ʱ: " << duration << " ��" << endl;

    return 0;
}
