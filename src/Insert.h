#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include "PatternList.h"
using namespace std;

//use to modify the source IR file.(insert the Pattern Block)
void write_pattern(string input_filename, PatternList patternlist)
{
	cout << "calling insert" << endl;
	string filename;
	filename = input_filename;
	ifstream infile(filename+".ll");		// the original file
	string line;    //每次读取的一行
	int line_count = 0;  //第几行

	ofstream fout(filename + "_add.ll", ios::out);			//the new modified IR file
	regex fun_begin("; Function Attrs:");			//insert the function definition in the begining
	int insert_fun_count = 0;
	smatch s;
	vector<Pattern> pList = patternlist.getPatterns();

	bool flag;				//use to judge whether to insert the original line;
	while (getline(infile, line)) {//每次读取一行 
		//cout << "line is" << line;
		if (regex_search(line, s, fun_begin)) { //插入gemm定义啥
			if (insert_fun_count == 0) {
				fout << "declare dso_local void @cblas_sgemm(i32, i32, i32, i32, i32, i32, float, float*, i32, float*, i32, float, float*, i32)\n";
				fout << "declare dso_local void @cblas_dgemm(i32, i32, i32, i32, i32, i32, double, double*, i32, double*, i32, double, double*, i32)\n";
				fout << "declare dso_local void @cblas_sgemv(i32, i32, i32, i32, float, float*, i32, float*, i32, float, float*, i32)\n";
				fout << "\n";
				insert_fun_count++;
				//line_count--;
			}
		}
		
		//insert the pattern
		//Pattern pattern = patternlist.getPattern();
		flag = true;
		for (int i = 0; i < pList.size(); i++) {
			Pattern pattern = pList[i];
			if (line_count == pattern.getmodLine()) {
				fout << pattern.getbrPatternLabel() << endl;
				flag = false;
				//pList.erase(pList.begin() + i);
			}
			else if (line_count == pattern.getInsertLine()) {
				fout << pattern.getcontent() << endl;
				flag = false;
				//pList.erase(pList.begin() + i);
			}
			//else {
				//fout << line << endl;
				//flag = true;
			//}
		}
		if (flag) {				// if the line is not the line that should insert
			fout << line << endl;
		}

		line_count++;
	}
	fout.close();
	infile.close();

	cout << "Complete!" << endl;
}
