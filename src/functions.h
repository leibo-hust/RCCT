#pragma once
#include "Block.h"
#include "BlockList.h"
#include "BTree.h"
#include "VarList.h"
#include "Pattern.h"
#include "PatternList.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <cstring>
#include <vector>
#include <algorithm>
#include <limits.h>

bool isDecompile;
int flagcount = 0;

using namespace std;
PatternList patternlist;

//function declaer
vector <string> BlockNameAndPreds(string oneline);
vector <string> NextBlocks(string oneline);
string BlcokType(Block block);
string BodyKeywords(Block block);
void analysisLoops(BlockList list);
void findLoopsInTree(Node *root, Block& entry);
void judgePattern(BlockList looplist);
void decompileJudgePattern(BlockList looplist, Block& entry);
vector<string> findLoopVarAndRange(Block b);
vector<string> findOperands(string line);
//void SortBlockList(BlockList originlist);
bool isEqual(VarList list1, VarList list2);
string findDeRange(string var, vector<string> loopvar, vector<string> loopinitvar, vector<string> looprange);
vector<string> findDeMat(string line1, string line2, vector<string> loopvar, vector<string> loopinitvar, vector<string> looprange, vector<string> bodycontent);
vector<string> findDeMat(VarList& add1op1, VarList& add1op2, VarList& add2op1, VarList& add2op2, vector<string>& loopvar, vector<string>& loopinitvar, vector<string>& looprange);
vector<string> findDeVec(VarList& addop1, VarList& addop2, vector<string>& loopvar, vector<string>& loopinitvar, vector<string>& looprange);
string datatype(string line);
string DealDeLoopRange(vector<string> &looprange, int num);
vector<string> findAlphaAndBeta(string& loadalpha, VarList& destVarList, BlockList& looplist);
vector<string> defindAlphaAndBeta(string& loadalpha, VarList& destVarList, BlockList& looplist);

//create a BlockList for every function
void readFile(string filename) {

	ifstream ifs(filename);		//open file
	if (ifs) {	//if open success
		cout << "open the file successfully." << endl;

		string oneline;			//read one line everytime, it is the line that read evey time 
		int line_count = 0;		//line number of the line
		bool funcBegin = false;
		bool blockBegin = false;
		bool haveBlock = false;
		BlockList blocklist;		//one blocklist for one function
		Block block;		//define everyone block in the function

		while (getline(ifs, oneline) ) {   //read one line 
			if (oneline.find("define") != string::npos && oneline.find("{") != string::npos) {    //match "{", as the beginning of the function
				//cout << oneline << endl;
				//if(!oneline.find("@__remill_error") || !oneline.find("_remill_missing_block") || !oneline.find("_init") || !oneline.find("__start") || !oneline.find("dl_relocate_static_pie") || !oneline.find("register_tm_clones") || !oneline.find("_fini") || !oneline.find("frame_dummy") || !oneline.find("global_dtors_aux") || !oneline.find("mcsema"))
				getline(ifs, oneline);
				line_count++;
				if (oneline.find(":") != string::npos) {
					funcBegin = true;			//begin to record the content of the function
					haveBlock = true;
				}
			}

			if (funcBegin) {   //the beginning of the function, begin to record the blocks
				//cout << oneline << endl;
				if (oneline.find(":") != string::npos) { //the beginning of the block (this line is the title of the block)
					//block = new Block();
					//---ok vector <string> names = BlockNameAndPreds(oneline);		//record the block's name and the preds
					//---ok block.setTitle(names[0]);
					//---ok block.setPreds1(names[1]);
					//---ok block.setPreds2(names[2]);
					if (flagcount == 0) {
						cout << "flag title is: " << oneline << endl;
						if (oneline.find("block_") != string::npos) {		// if is decompilation
							isDecompile = true;
							cout << "is decompilation" << endl;
						}
						else {
							isDecompile = false;
							cout << "is not decompilation" << endl;		
						}
						flagcount++;
					}

					block.setTtileAndPreds(oneline);
					//cout << "|||||||" << endl;
					//cout << "title:_" << names[0] << endl;
					//cout << "title:_" << names[0] << " prdes1:_" << names[1] << "_" << " prdes2:_" << names[2] << "__" << endl;
					blockBegin = true;		//the block is begin
				}
				else if (oneline.empty() || oneline == "}") {  //empty line, means the end of the block
					//insert the block to the BlockList
					//cout << "-----" << endl;
					//cout << "title:_" << block.getTitle() << " prdes1:_" << block.getPreds1() << "_" << " prdes2:_" << block.getPreds2() << "__" << endl;
					//---ok  string brlabel = block.getContent().back();
					//---ok  vector <string> labels = NextBlocks(brlabel);			//the next two labels
					//---ok block.setNextTrue(labels[0]);
					//---ok block.setNextFalse(labels[1]);
					//cout << "testing..." << endl;
					block.setNextLabels(isDecompile);


					block.setLastLine(line_count);	//set the last line of the block
					//---string type = BlcokType(block);
					//---block.setType(type);			//determine the type of the block
					block.DecideType();
					blocklist.insert(block);		//when one block is end, insert it to the blocklist

					//Loop_cond_block condblock;
					//blocklist.insert(condblock);

					block.cleanContent();	//clean the content of the block
					blockBegin = false;		//block is end
					//cout << "testing2...." << endl;
				}

				if (blockBegin) {	//padding the content of the block;
					//cout << "\\\\\\\\\\\\" << endl;
					//cout << "title:_" << block.getTitle() << " prdes1:_" << block.getPreds1() << "_" << " prdes2:_" << block.getPreds2() << "__" << endl;
					block.setContent(oneline);	
				}

			}//end of funcBegin

			if (oneline == "}" && haveBlock) {	//the end of the function
				//cout << "!!!!!!!!!!!!!!!!!!!!!!!!       the line number is:_" << line_count << endl;
				//cout << oneline << endl;
				//cout << "is the end of function:_" << oneline << endl;
				//when one function is end, we need to deal the blocklist
				

				//only in the Decompile, need to do setBodyBlock();
				if (isDecompile) {
					blocklist.setBodyBlock();
				}

				//debuging: output all the blocks of the function
				/*for (int i = 0; i < blocklist.FuncBlockList.size(); i++) {
					Block b = blocklist.findById(i);
					if(b.getType() == "loop_body")
						b.showBlock();
				}*/
				cout << "================================ The beginning of a function ======================================" << endl;
				
				
				BTree btree = BTree();
				//int btree::callcount = 0;
				//cout << "count is:_" << btree.callcount << endl;
				//cout << "size is:_" << blocklist.FuncBlockList.size() << endl;
				//btree.Init(&blocklist.findById(0));

				Block b = blocklist.findById(0);
				// Block *initblock = &b;
				//cout << "the     title:_" << blocklist.findById(0).getTitle() << endl;
				//cout << "TTThe     title:_" << initblock->getTitle() << endl;
				Node *n = new Node;
				n->block = b;
				n->left = nullptr;
				n->right = nullptr;
				/*if (n != NULL) {
					cout << "hello" << endl;
				}
				if (n->left == NULL) {
					cout << "ttttttttt" << endl;
				}*/
				
				//test(n->left);
				btree.create(n, b, blocklist);
				//cout << "tree block title:_" << n->block->getTitle() << endl;
				std::cout << "Building the binary tree................" << std::endl;
				//btree.printTree(n);
				//------------btree.findLoops(n);
				findLoopsInTree(btree.getRoot(), b);
				//cout << "root block title:_" << n->block.getTitle() << endl;
				//Block bl = n->right->block;
				//cout << "b. title is:_" << bl.getTitle() << endl;
				//if (btree.root->left == nullptr)
					//cout << "the left is not null" << endl;
				//if (btree.root->left->block != nullptr)
					//cout << "not null block" << endl;

				//if (n->right->block->title[0] == 'f')
				
				//cout << "_" << btree.root->right->block->title[0] <<"_" << endl;
				//cout << "left block title:_" << t << endl;
			
				//SortBlockList(blocklist);

				blocklist.cleanList();	//clean the content of the blocklist	
				funcBegin = false;
				haveBlock = false;
			}

			line_count++;  //increase the line number
		}// end of the while(read file)

		cout << endl;
		cout << "------The identification process is complete!-------" << endl;
	}

	else {
		cout << "Your file is not avaliable." << endl;
	}

}



void findLoopsInTree(Node *root, Block& entry)
{
	BlockList looplist;
	if (root != NULL) {
		if (root->block.getType() == "loop_cond") {
			//cout << "in the loop find function:" << Tree->block.getTitle() << endl;
			looplist.insert(root->block);
			//cout << "right in the loop find function:" << Tree->right->block.getType() << endl;
			Node *cond = new Node;
			cond = root;
			while (cond->right->block.getType() != "other")
			{
				//string next_title = cond->right->block.getTitle();
				//cout << "next title is:_" << next_title << endl;
				//cout << "title and type of right:" << cond->right->block.getTitle() << "_ " << cond->right->block.getType() << endl;
				looplist.insert(cond->right->block);
				cond = cond->right;

				if (cond->right == nullptr) break;		// for decompile
			}
			cout << "Finding loops in the function......" << endl;
			//cout << "````````````````````````````````" << endl;
			for (int i = 0; i < looplist.getListSize(); i++) {
				cout << "  ->: " << looplist.FuncBlockList[i].getTitle() << endl;
				//cout << "2222222222222222222222222222222222222222222222222222222" << endl;
			}
			//cout << "------------------------------find loop end--------------------" << endl;
			//analysisLoops(looplist);
			if (isDecompile) {
				decompileJudgePattern(looplist, entry);
			}
			else {
				judgePattern(looplist);
			}

			findLoopsInTree(root->left, entry);
		}
		else {
			//cout << "others:" << Tree->block.getTitle() << endl;
			findLoopsInTree(root->right, entry);
			findLoopsInTree(root->left, entry);
		}
	}
}


//judge whether the block tree is the pattern and the type of the pattern
void judgePattern(BlockList looplist)
{
	int len = looplist.getListSize();
	vector<string> loopvar;
	vector<string> looprange;
	for (int i = 0; i < len; i += 2) {
		vector<string> tmp = findLoopVarAndRange(looplist.findById(i));
		loopvar.push_back(tmp[0]);
		looprange.push_back(tmp[1]);
		//cout << "var is:_" << tmp[0] << "_ ;range is:_" << tmp[1] << "_" << endl;
		//VarList varlist;
		//varlist.buildList(looplist, tmp[0]);
	}

	Block innerbody = looplist.findById(len - 1);
	vector<string> bodycontent = innerbody.getContent();
	//regex logic_op("= +[or|xor|and]+ ");      //Regular matching, identify whether it is a cond block
	regex logic_op(" = (xor|and|or) ");      //Regular matching, identify whether it is a cond block
	regex mul(" = [f]*mul ");      //whether it is a body block
	regex add(" = +[fadd|add]+ ");
	smatch s;
	string line;
	string goal, op1, op2;			//the operands in the operation
	int varcount = 0;
	vector<string> loopvarcp = loopvar;
	//vector<string> content = block.getContent();
	//for (int i = bodycontent.size() - 1; i >= 0; i--) {
	for (int i = 0; i < bodycontent.size(); i++) {
		line = bodycontent[i];
		//satitstic the count of the loop var occured in the loop body;
		for (int n = 0; n < loopvarcp.size(); n++) {
			//cout << "loovar here is:_" << loopvar[n];
			regex varreg(" " + loopvarcp[n]);
			if (regex_search(line, s, varreg) && loopvarcp.size() != 0)
			{
				varcount++;
				loopvarcp.erase(loopvarcp.begin() + n);
			}
		}
		//cout << "varcount is:_" << varcount << endl;

		//cout << "line is:_" << line << endl;
		if (regex_search(line, s, logic_op)) {		//if it's the bitmap
			//Finally, insert the command to determine if it is 0 or 1.
			//cout << "matching logic:_" << line << endl;
			cout << "Find the bitmap_logic pattern." << endl;
			goal = findOperands(line)[0];
			op1 = findOperands(line)[1];
			op2 = findOperands(line)[2];
			int e1 = line.find("=");
			int b1 = line.find(" ", e1 + 2);
			string op_code = line.substr(e1 + 2, b1 - e1 - 2);
			//cout << "logic op are:_" << goal << "_"  << op1 << "_" << op2 << "_" << op_code << "_" << endl;
			VarList op1list, op2list, goallist;
			op1list.buildList(looplist, op1);
			op2list.buildList(looplist, op2);
			goallist.buildList(looplist, goal);
			goallist.setRow(op_code);

			Pattern p = Pattern("bitmap");
			//PatternList patternlist;
			//cout << "looprange:_" << loopvar[0] << "_" << looprange[0] << endl;
			//cout << "looprange:_" << loopvar[1] << "_" <<looprange[1] << endl;
			int num = patternlist.size();
			p.setRangeIns(loopvar, looprange, num);
			p.setContent(op1list, op2list, goallist, num);
			p.setbrlabels(looplist);
			patternlist.add(p);

		}
		else if (regex_search(line, s, mul)) {
			//cout << "matching mul" << endl;
			//cout << "iiiiiiiiiiii is:_" << i << endl;
			//find the operands
			goal = findOperands(line)[0];
			op1 = findOperands(line)[1];
			op2 = findOperands(line)[2];
			//cout << "mul op are:_" << goal << "_" << op1 << "|_" << op2 << "|_" << endl;
			VarList mulist, mul1list, mul2list;
			mulist.buildList(looplist, goal);
			//cout << "-----------the init instruction of the two op of mul:_" << mul1list.getinitIns() << endl; 
			//cout << "_and_" << mul2list.getinitIns() << "_" << endl;
			string mulalphains = "";
			int mulalphalnum = INT_MAX;
			bool findAlpha = false;
			vector<string> alphabetaargs;
			string loadalpha = "";
			for (int j = i + 1; j < bodycontent.size(); j++) {
				// cout << "new line: " << j << endl;
				line = bodycontent[j];
				// bool hasAplha = false;
				if (regex_search(line, s, mul)) {		// mul alpha
					vector<string> p3 = findOperands(line);
					// cout << "------------p3 [1]-" << p3[0] << "-" << p3[2] << endl;
					if (p3[1] == goal || p3[2] == goal) {
						// cout << "mul alpha match........................\n";
						mulalphains = line;
						mulalphalnum = j;
						loadalpha = p3[1] == goal ? p3[2] : p3[1];
						mulist.addVar(p3[0]);
						findAlpha = true;
					}
					continue;
				}

				if (regex_search(line, s, add)) {
					// cout << "add line: " << line << endl;	
					//cout << "matching add" << endl;
					string addop1 = findOperands(line)[1];
					string addop2 = findOperands(line)[2];
					// cout << "add op are:_" << addop1 << "_" << addop2 << "_" << endl;
					if (mulist.findVar(addop1) || mulist.findVar(addop2)) {
						// cout << "in the list\n";
						mul1list.buildList(looplist, op1);
						mul1list.showList();
						// cout << "   the type of mul1: " << mul1list.getType() << endl; 
						mul2list.buildList(looplist, op2);
						// mul2list.showList();
						//cout << "   the type of mul2: " << mul2list.getType() << endl;
						if (mul1list.getType() == "matrix") {
							swap(mul1list, mul2list);
						} 
						VarList resultList;
						resultList.buildList(looplist, addop1);
						if (varcount == 2) {
							cout << "Find the MVM pattern." << endl;
							//cout << "-----------the init instruction of the goal:_" << resultList.getInitIns()[0] << endl;
							//cout << "-----------the init instruction of the two op of mul:_" << mul1list.getinitIns() << endl;
							Pattern p = Pattern("mvm");
							//PatternList patternlist;
							//cout << "looprange:_" << loopvar[0] << "_" << looprange[0] << endl;
							//cout << "looprange:_" << loopvar[1] << "_" <<looprange[1] << endl;
							int num = patternlist.size();
							if (findAlpha) {
								alphabetaargs = findAlphaAndBeta(loadalpha, resultList, looplist);
								p.setAlphaBeta(alphabetaargs);
							}
							p.setRangeIns(loopvar, looprange, num);
							p.setContent(mul1list, mul2list, resultList, num);
							p.setbrlabels(looplist);
							patternlist.add(p);
							//cout << "_and_" << mul2list.getinitIns() << "_" << endl;
						} else if (varcount == 3) {
							cout << "Find the MMM pattern." << endl;
							Pattern p = Pattern("mmm");
							//PatternList patternlist;
							//cout << "looprange:_" << loopvar[0] << "_" << looprange[0] << endl;
							//cout << "looprange:_" << loopvar[1] << "_" <<looprange[1] << endl;
							// vector<string> res = findAlphaAndBeta(mul1list, looplist);
							// p.setAlphaBeta(res);
							int num = patternlist.size();
							if (findAlpha) {
								alphabetaargs = findAlphaAndBeta(loadalpha, resultList, looplist);
								p.setAlphaBeta(alphabetaargs);
							}
							p.setRangeIns(loopvar, looprange, num);
							VarList alist, blist;
							p.setContent(mul1list, mul2list, resultList, num);
							p.setbrlabels(looplist);
							patternlist.add(p);
						}
					}
					i = j;
				}
			}
		}
	}

}

//Decompilation ------ judge whether the block tree is the pattern and the type of the pattern
void decompileJudgePattern(BlockList looplist, Block& entry)
{
	cout << "Judging if there is a pattern......" << endl;
	int len = looplist.getListSize();
	vector<string> loopinitvar, loopvar, looprange;// , loopblocks;
	for (int i = 0; i < len; i += 2) {
		vector<string> tmp = findLoopVarAndRange(looplist.findById(i));
		loopvar.push_back(tmp[0]);
		looprange.push_back(tmp[1]);
		//loopblocks.push_back(tmp[2]);
		loopinitvar.push_back(tmp[2]);
		//cout << "var is:_" << tmp[0] << "_ ;range is:_" << tmp[1] << "|_" << "_ ;init var is:_" << tmp[2] <<endl;
	}

	Block innerbody = looplist.findById(len - 1);
	vector<string> bodycontent = innerbody.getContent();
	//regex logic_op("= +[or|xor|and]+ ");      //Regular matching, identify whether it is a cond block
	regex logic_op(" = (xor|and|or) [0-9, a-z]* %[0-9]*, %[0-9]*");      //Regular matching, identify whether it is a cond block
	regex mul(" = [f]*mul [0-9, a-z]* %[0-9]*, %[0-9]*");      //whether it is a body block
	regex add(" = +[fadd|add]+ (i32|float) %[0-9]*, %[0-9]*");
	regex ptradd(" = add i64 %[0-9]*, %[0-9]*");			//address calculate: %189 = add i64 %188, %177
	vector <string> addIns;
	smatch s;
	string line, newline;
	string goal, op1, op2;			//the operands in the operation
	string data_type;				// the type of the data
	vector<string> needContent = entry.getContent();
	for (string& ins : bodycontent) {
		needContent.emplace_back(ins);
	}

	for (int i = 0; i < bodycontent.size(); i++) {
		line = bodycontent[i];
		// static the add instruction
		if (regex_search(line, s, ptradd)) {
			addIns.push_back(line);
		}

		if (regex_search(line, s, logic_op)) {		//if it's the bitmap
			//cout << "de matching logic:_" << line << endl;
			cout << "Find the bitmap_logic pattern." << endl;
			data_type = datatype(line);			//the data type of the logic op
			goal = findOperands(line)[0];
			op1 = findOperands(line)[1];
			op2 = findOperands(line)[2];
			//cout << "de logic op are:_" << goal << "_" << op1 << "_" << op2 << "_" << endl;

			int e1 = line.find("=");
			int b1 = line.find(" ", e1 + 2);
			string op_code = line.substr(e1 + 2, b1 - e1 - 2);
			//cout << "de the opcode is:_" << op_code << "_" << endl;

			VarList op1list, op2list, goallist;
			/*op1list.deBuilidVarList(innerbody, op1);
			op2list.deBuilidVarList(innerbody, op2);
			goallist.deBuilidVarList(innerbody, goal);*/
			op1list.deBuilidVarList(bodycontent, op1);
			op2list.deBuilidVarList(bodycontent, op2);
			goallist.deBuilidVarList(bodycontent, goal);

			// %127 = add i64 %126, % 88
			// %136 = add i64 %127, -144
			vector <string> Inss_1 = op1list.getInitIns();
			vector <string> Inss_2 = op2list.getInitIns();
			vector <string> Inss_3 = op2list.getInitIns();
			string lastIns = Inss_1[1];
			int e = lastIns.rfind(",");
			int b = lastIns.rfind("%", e);
			string op_var = lastIns.substr(b, e - b);
			string goaladdr;
			//cout << "de ooooop var is:_" << op_var << "_" << endl;
			regex varad(op_var + " = ");
			for (int j = i - 1; j >= 0; j--) {
				string newins = bodycontent[j];
				if (regex_search(newins, s, varad)) {
					int q = newins.rfind("%");
					goaladdr = newins.substr(q);
					//cout << "found goalvar is:_" << goaladdr << "_" << endl;
					break;
				}
			}
			Inss_1[1].replace(b, op_var.length(), goaladdr);
			//cout << "after replace:_" << Inss_1[1] << "_" << endl;
			Inss_2[1].replace(b, op_var.length(), goaladdr);
			Inss_3[1].replace(b, op_var.length(), goaladdr);
			op1list.setInitIns(Inss_1);
			op2list.setInitIns(Inss_2);
			goallist.setInitIns(Inss_3);
			// construct the pattern
			Pattern p = Pattern("bitmap");
			int num = patternlist.size();
			string add_ins = DealDeLoopRange(looprange, num);				//deal the looprange
			p.deSetContent(op1list, op2list, goallist, num);
			p.insertContent(add_ins);
			//p.deSetRangeIns(loopvar, looprange, looplist);
			p.deBitmapSetRangeIns(looprange.back(), op_code);
			p.setLibFunc();
			p.setbrlabels(looplist);			//maybe can merge with the deSetRangeIns
			patternlist.add(p);

		}
		else if (regex_search(line, s, mul)) {
			//cout << "de matching mul..." << endl;
			//cout << "iiiiiiiiiiii is:_" << i << endl;
			//find the operands
			int addcount = addIns.size();
			//cout << "the count of the add ins is:_" << addcount << "_" << endl;
			string mulalphains = "", loadalpha = "";
			int mulalphalnum = -1;
			bool findAlpha = false;
			VarList mulist;
			goal = findOperands(line)[0];
			mulist.deBuilidVarList(needContent, goal);
			// mulist.showList();

			for (int j = i + 1; j < bodycontent.size(); j++) {		//continue to find the add/fadd
				newline = bodycontent[j];
				if (regex_search(newline, s, ptradd)) {
					addIns.push_back(newline);
					//cout << "if new add ins" << endl;
				}

				if (regex_search(newline, s, mul)) {		// mul alpha
					vector<string> p3 = findOperands(newline);
					// cout << "------------p3 [1]-" << p3[0] << "-" << p3[2] << endl;
					VarList t1, t2;
					t1.deBuilidVarList(needContent, p3[1]);
					t2.deBuilidVarList(needContent, p3[2]);
					// t2.showList();
					if (t1 == mulist) {
						// cout << "mul alpha match........................\n";
						loadalpha = p3[1];
						findAlpha = true;
					} else if (t2 == mulist) {
						loadalpha = p3[2];
						findAlpha = true;
					}
					if (findAlpha) {
						mulalphains = newline;
						mulalphalnum = j;
						mulist.addVar(p3[0]);
					}
					continue;
				}

				if (regex_search(newline, s, add)) {			//when macthing the add ins
					//cout << "de matching add..." <<  newline << endl;
					data_type = datatype(newline);
					//cout << "the type of the mxm is:_" << data_type << "_" << endl;
					if (addcount == 3) {	//maybe the mvm
						//cout << "maybe theeeeeeeeeeeeeeeee mvm: " << addIns.size() << endl;
						cout << "Find the MVM pattern." << endl;
						// mulist.showList();
						// %204 = add i64 % 59, % 203
						// %208 = add i64 % 207, % 67
						// %213 = add i64 %212, %208
						int num = patternlist.size();
						string add_ins = DealDeLoopRange(looprange, num);				//deal the looprange
						//cout << "adddddddddddddddddddddddddddddd ins: " << addins << endl;
						/*1 for (int t = 0; t < looprange.size(); t++) {    // judge if the range is right
							cout << looprange[t] << endl;
						}*/
						// cout << "adddddddddddddddddddddddddddddd ins: " << newline << ", " << findAlpha << endl;
						string vec_addr, vec_col, res_addr, res_col;			// the addr and row of the vec and result
						vector<string> mat;					// the parameters of the matrix

						string add_1, op_2, or_2;
						add_1 = findOperands(addIns[0])[0];		// the first add res
						// the second add opts
						op_2 = findOperands(addIns[1])[1];
						or_2 = findOperands(addIns[1])[2];
						string addr2 = findOperands(addIns[1])[0];
						VarList var_2, add_temp;			// represent the second ins: %208
						VarList op_1;
						var_2.deBuilidVarList(bodycontent, op_2);
						add_temp.deBuilidVarList(bodycontent, or_2);
						/*for (string& s : addIns) {
							cout << s << endl;
							cout << "xxxxx" << endl;
						}*/
						bool find1 = var_2.findVar(add_1), find2 = add_temp.findVar(add_1);
						if (find1 || find2) {			//if the first two add is the mat;
							// add_temp.showList();
							string add1op1 = findOperands(addIns[0])[1], add1op2 = findOperands(addIns[0])[2];
							VarList a1o1, a1o2;
							a1o1.deBuilidVarList(bodycontent, add1op1);
							a1o2.deBuilidVarList(bodycontent, add1op2);
							if (find1) {
								mat = findDeMat(a1o1, a1o2, add_temp, var_2, loopvar, loopinitvar, looprange);
							} else {
								mat = findDeMat(a1o1, a1o2, var_2, add_temp, loopvar, loopinitvar, looprange);
							}
							// mat = findDeMat(addIns[0], addIns[1], loopvar, loopinitvar, looprange, bodycontent);
							// cout << addIns[2] << endl;
							/* be lazy
							int a0 = stoi(a[0].substr(1)), a1 = stoi(a[1].substr(1)), a2 = stoi(a[2].substr(1));
							// we should check weawher the var occur in loop.cond;
							vec_addr = a0 - a1 < 5 ? a[2] : a[1];
							vec_col = find
							// vec_col = findDeRange(findOperands(addIns[2])[1], loopvar, loopinitvar, looprange);*/
							vector<string> a = findOperands(addIns[2]);
							VarList inop1,inop2;
							inop1.deBuilidVarList(bodycontent, a[1]);
							inop2.deBuilidVarList(bodycontent, a[2]);
							vector<string> inr = findDeVec(inop1, inop2, loopvar, loopinitvar, looprange);
							vec_addr = inr[0];
							vec_col = inr[2]; 
						}
						else {	//if the last two 'add' is the mat;
							// the input vector
							vector<string> a = findOperands(addIns[0]);
							VarList inop1,inop2;
							inop1.deBuilidVarList(bodycontent, a[1]);
							inop2.deBuilidVarList(bodycontent, a[2]);
							vector<string> inr = findDeVec(inop1, inop2, loopvar, loopinitvar, looprange);
							vec_addr = inr[0];
							vec_col = inr[2];

							// the mat
							vector<string> add3 = findOperands(addIns[2]);
							VarList add3op1, add3op2;
							add3op1.deBuilidVarList(bodycontent, add3[1]);
							add3op2.deBuilidVarList(bodycontent, add3[2]);
							find1 = add3op1.findVar(addr2);
							find2 = add3op2.findVar(addr2);
							if (find1 || find2) {			//if the second two 'add' is the mat;
								if (find1) {
									mat = findDeMat(var_2, add_temp, add3op2, add3op1, loopvar, loopinitvar, looprange);
								} else {
									mat = findDeMat(var_2, add_temp, add3op1, add3op2, loopvar, loopinitvar, looprange);
								}
							}
						}
						// calculate the res;
						VarList op_3, temp_3;
						op_3.deBuilidVarList(bodycontent, findOperands(addIns[3])[1]);
						temp_3.deBuilidVarList(bodycontent, findOperands(addIns[3])[2]);
						vector<string> rr = findDeVec(op_3, temp_3, loopvar, loopinitvar, looprange);
						res_addr = rr[0];
						res_col = rr[2];
			
						vector<string> initins;
						initins.push_back("  %op1 = inttoptr i64 " + vec_addr +" to " + data_type + "*");
						//cout << "the build initins is:_" << initins[0] << "_" << endl;
						op_1.setInitIns(initins);

						initins[0] = "  %op1 = inttoptr i64 " + mat[0] + " to " + data_type + "*";
						var_2.setInitIns(initins);

						initins[0] = "  %op1 = inttoptr i64 " + res_addr + " to " + data_type + "*";
						op_3.setInitIns(initins);


						Pattern p = Pattern("mvm", data_type);
						p.deSetContent(op_1, var_2, op_3, num);
						p.insertContent(add_ins);
						p.deSetRange(0, "1", vec_col);
						p.deSetRange(1, mat[1], mat[2]);
						p.deSetRange(2, "1", res_col);
						p.setLibFunc();
						p.setbrlabels(looplist);
						patternlist.add(p);
						//p.setRangeIns(loopvar, looprange, num);
						//p.setContent(mul1list, mul2list, resultList, num);

					}
					// if there are 4 add ins before the "add/fadd", it may be the mmm
					else if (addcount == 4) {	//maybe the mmm	
						// %166 = add i64 %165, %67
						// %169 = add i64 %166, %168          : %67 is the address of the mat; %165 and %168 is the loop var(like i, j)
						//cout << "maybe the mmmmmm" << endl;
						cout << "Find the MMM pattern." << endl;

						Pattern p = Pattern("mmm");
						int num = patternlist.size();
						string add_ins = DealDeLoopRange(looprange, num);				//deal the looprange
						VarList op_1, op_2, op_3;
						vector<string> initins, matres, matres1, matres2;
						VarList add1op1, add1op2, add2op1, add2op2;
						vector<string> addr1 = findOperands(addIns[0]), addr2 = findOperands(addIns[1]);
						bool f1 = false, f2 = false;
						
						// first input mat
						add2op1.deBuilidVarList(bodycontent, addr2[1]);
						add2op2.deBuilidVarList(bodycontent, addr2[2]);
						f1 = add2op1.findVar(addr1[0]);
						f2 = add2op2.findVar(addr1[0]);
						if (f1 ||  f2) {
							add1op1.deBuilidVarList(bodycontent, addr1[1]);
							add1op2.deBuilidVarList(bodycontent, addr1[2]);
							if (f1) {	// second 'add', first op is the add res of first 'add'
								matres = findDeMat(add1op1, add1op2, add2op1, add2op2, loopvar, loopinitvar, looprange);
							} else if (f2) {
								matres = findDeMat(add1op1, add1op2, add2op2, add2op1, loopvar, loopinitvar, looprange);
							}
							//cout << "mat 1 : " << matres[0] << "_ "<< matres[1] << "_ " << matres[2] << endl;
							initins.push_back("  %op1 = inttoptr i64 " + matres[0] + " to " + data_type + "*");
							op_1.setInitIns(initins);
						}

						// second input mat
						addr1 = findOperands(addIns[2]);
						addr2 = findOperands(addIns[3]);
						add2op1.deBuilidVarList(bodycontent, addr2[1]);
						add2op2.deBuilidVarList(bodycontent, addr2[2]);
						f1 = add2op1.findVar(addr1[0]);
						f2 = add2op2.findVar(addr1[0]);
						if (f1 ||  f2) {
							add1op1.deBuilidVarList(bodycontent, addr1[1]);
							add1op2.deBuilidVarList(bodycontent, addr1[2]);
							if (f1) {	// second 'add', first op is the add res of first 'add'
								matres1 = findDeMat(add1op1, add1op2, add2op1, add2op2, loopvar, loopinitvar, looprange);
							} else if (f2) {
								matres1 = findDeMat(add1op1, add1op2, add2op2, add2op1, loopvar, loopinitvar, looprange);
							}
							//cout << "mat 2 : " << matres1[0] << "_ " << matres1[1] << "_ " << matres1[2] << endl;
							initins[0] = "  %op2 = inttoptr i64 " + matres1[0] + " to " + data_type + "*";
							op_2.setInitIns(initins);
						}

						// res mat
						addr1 = findOperands(addIns[4]);
						addr2 = findOperands(addIns[5]);
						add2op1.deBuilidVarList(bodycontent, addr2[1]);
						add2op2.deBuilidVarList(bodycontent, addr2[2]);
						f1 = add2op1.findVar(addr1[0]);
						f2 = add2op2.findVar(addr1[0]);
						if (f1 ||  f2) {
							add1op1.deBuilidVarList(bodycontent, addr1[1]);
							add1op2.deBuilidVarList(bodycontent, addr1[2]);
							if (f1) {	// second 'add', first op is the add res of first 'add'
								matres2 = findDeMat(add1op1, add1op2, add2op1, add2op2, loopvar, loopinitvar, looprange);
							} else if (f2) {
								matres2 = findDeMat(add1op1, add1op2, add2op2, add2op1, loopvar, loopinitvar, looprange);
							}
							//cout << "mat 3 : " << matres2[0] << "_ " << matres2[1] << "_ " << matres2[2] << endl;
							initins[0] = "  %op3 = inttoptr i64 " + matres2[0] + " to " + data_type + "*";
							op_3.setInitIns(initins);
						}
						
						//p.setRangeIns(loopvar, looprange, num);
						//p.setContent(mul1list, mul2list, resultList, num);
						p.deSetContent(op_1, op_2, op_3, num);
						p.insertContent(add_ins);

						p.deSetRange(0, matres[1], matres[2]);
						p.deSetRange(1, matres1[1], matres1[2]);
						p.deSetRange(2, matres2[1], matres2[2]);


						//PatternList patternlist;
						//cout << "looprange:_" << loopvar[0] << "_" << looprange[0] << endl;
						//cout << "looprange:_" << loopvar[1] << "_" <<looprange[1] << endl;
						
						p.setLibFunc();
						p.setbrlabels(looplist);
						patternlist.add(p);
					}
					break;
				}
			}

			break;
		}
	}

}

vector<string> findAlphaAndBeta(string& loadalpha, VarList& resVarList, BlockList& looplist) {
	vector<string> res = {"float", "1.0", "", "float", "0.0", "-1"};		// default value: alpha type, alpha value, beta type, beta value.
	int len = looplist.getListSize();
	vector<string> innermostbody = looplist.findById(len - 1).getContent();
	Block penultimateBlock = looplist.findById(len - 3);
	vector<string> penultimatebody = penultimateBlock.getContent();
	// find beta first
	regex mul(" = [f]*mul ");
	regex load(loadalpha + " = load");
	smatch s;
	string betagoal = "";
	regex store("store (float|i32|i64|i8|double) %");
	int i = 0;
	bool findbeta = false;
	for (string& ins : penultimatebody) {	
		i++;
		if (regex_search(ins, s, mul)) {		// 如果存在mul语句
			cout << "in findbeta: " << ins << endl;
			vector<string> opset = findOperands(ins);
			string op1 = opset[1], op2 = opset[2];
			betagoal = opset[0];
			string ty = datatype(ins);
			// resVarList.showList();
			BlockList penultimatelist;
			penultimatelist.insert(penultimateBlock);
			VarList op1list, op2list;
			op1list.buildList(penultimatelist, op1);
			op2list.buildList(penultimatelist, op2);
			if (resVarList == op1list) {
				res[4] = op2;
				res[3] = ty;
				resVarList.mergeList(op1list);
				findbeta = true;
				// cout << "lalallalal: " << op2 << endl; 
				// break;
			} else if (resVarList == op2list) {
				res[4] = op1;
				res[3] = ty;
				findbeta = true;
				resVarList.mergeList(op2list);
				// cout << "lalallalal: " << op2 << endl; 
				// break;
			} else {
				// cout << "aaaaaa\n";
			}
		} else if (regex_search(ins, s, store) && findbeta == true) {		// the store ins
			int p = ins.find("*") + 2;
			int x = ins.find(",", p);
			string aim = ins.substr(p, x - p);
			// cout << "22222222222222222222222222222222 try to find: " << aim << endl;
			if (resVarList.findVar(aim)) {
				res[5] = to_string(penultimateBlock.getLastLine() - (penultimatebody.size() - i));
				// cout << "333333333333333333333333333333 try to find: " << res[5] << endl;
				break;
			}
		}
	}

	// find alpha second
	for (string& ins :innermostbody) {	
		if (regex_search(ins, s, load)) {		// 如果存在load语句
			// cout << "--load ins: " << ins << endl;
			string ty = datatype(ins);
			res[1] = ty;
			int e = ins.find("=");
			res[2] = ins.substr(e - 1) + "\n";			// -> " = load ......."
			// cout << "-" << res[2] << "-" << endl;
		}
	}

	return res;
}

vector<string> defindAlphaAndBeta(string& loadalpha, VarList& resVarList, BlockList& looplist) {
	vector<string> res = {"float", "1.0", "", "float", "0.0", "-1"};		// default value: alpha type, alpha value, beta type, beta value.
	int len = looplist.getListSize();
	vector<string> innermostbody = looplist.findById(len - 1).getContent();
	Block penultimateBlock = looplist.findById(len - 3);
	vector<string> penultimatebody = penultimateBlock.getContent();
	// find beta first
	regex mul(" = [f]*mul ");
	regex load(loadalpha + " = load");
	smatch s;
	string betagoal = "";
	regex store("store (float|i32|i64|i8|double) %");
	int i = 0;
	bool findbeta = false;
	for (string& ins : penultimatebody) {	
		i++;
		if (regex_search(ins, s, mul)) {		// 如果存在mul语句
			cout << "in findbeta: " << ins << endl;
			vector<string> opset = findOperands(ins);
			string op1 = opset[1], op2 = opset[2];
			betagoal = opset[0];
			string ty = datatype(ins);
			// resVarList.showList();
			BlockList penultimatelist;
			penultimatelist.insert(penultimateBlock);
			VarList op1list, op2list;
			op1list.buildList(penultimatelist, op1);
			op2list.buildList(penultimatelist, op2);
			if (resVarList == op1list) {
				res[4] = op2;
				res[3] = ty;
				resVarList.mergeList(op1list);
				findbeta = true;
				// cout << "lalallalal: " << op2 << endl; 
				// break;
			} else if (resVarList == op2list) {
				res[4] = op1;
				res[3] = ty;
				findbeta = true;
				resVarList.mergeList(op2list);
				// cout << "lalallalal: " << op2 << endl; 
				// break;
			} else {
				// cout << "aaaaaa\n";
			}
		} else if (regex_search(ins, s, store) && findbeta == true) {		// the store ins
			int p = ins.find("*") + 2;
			int x = ins.find(",", p);
			string aim = ins.substr(p, x - p);
			// cout << "22222222222222222222222222222222 try to find: " << aim << endl;
			if (resVarList.findVar(aim)) {
				res[5] = to_string(penultimateBlock.getLastLine() - (penultimatebody.size() - i));
				// cout << "333333333333333333333333333333 try to find: " << res[5] << endl;
				break;
			}
		}
	}

	// find alpha second
	for (string& ins :innermostbody) {	
		if (regex_search(ins, s, load)) {		// 如果存在load语句
			// cout << "--load ins: " << ins << endl;
			string ty = datatype(ins);
			res[1] = ty;
			int e = ins.find("=");
			res[2] = ins.substr(e - 1) + "\n";			// -> " = load ......."
			// cout << "-" << res[2] << "-" << endl;
		}
	}

	return res;
}

vector<string> findOperands(string line)		//find the operands of the operation(or, fadd, fmul)
{
	//%or = or i32 %4, %6
	//%121 = or i32 %120, %117
	//%add = fadd float %23, %mul46
	//%mul46 = fmul float %18, %21
	vector<string> res;
	string val0, val1, val2;
	int e = line.find("=");
	int b = line.find("%", e);
	int c = line.find(",", b);
	int l = line.size();
	val0 = line.substr(2, e - 3);
	val1 = line.substr(b, c - b);
	val2 = line.substr(c + 2, l - c - 2);
	res.push_back(val0);
	res.push_back(val1);
	res.push_back(val2);
	return res;
}


//find the loops' var and range
vector<string> findLoopVarAndRange(Block block)
{
	vector<string> bcontent = block.getContent();
	vector<string> res;
	string var, range, initvar;

	if (isDecompile == false) {		//Compile
		regex load(" = load +[i32|i16]+, +[i32|i16]+\\* %[a-z, 0-9]*");
		//regex load("= load i32, i32\\* %i6, align 4");
		regex icmp(" = icmp [a-z]* +[i32|i16]+ %[a-z, 0-9]*, [0-9]*");
		smatch s;
		string line;
		int b, e, loadcount = 0;
		for (int i = 0; i < bcontent.size(); i++) {
			line = bcontent[i];
			if (regex_search(line, s, load) && loadcount == 0) {				//Find the loop variable(like %i, %j)
				///cout << "load line found." << endl;
				b = line.find("*");
				e = line.find(",", b);
				var = line.substr(b + 2, e - b - 2);
				loadcount++;
				//cout << "var is:_" << var << "_" << endl;
			}
		
			if (regex_search(line, s, icmp)) {				//Find the true loop range: slt, sle, sgt, sge
				if ((b = line.find("slt")) != -1) {		// if is the slt, then the range = last var: slt i32 %5, 100(%4)
					e = line.find(",", b);
					range = line.substr(e + 2);
					//std::cout << "slt range is:_" << range << "_" << endl;
				}
				else {										// others, the range = last var + 1: slt i32 %5, 99(%4) [99 + 1,(%4 + 1)]
					if ((b = line.find("sle")) != -1) {		// if is the slt, then the range = last var: slt i32 %5, 100(%4)
						e = line.find(",", b);
						range = line.substr(e + 2);
						//cout << "sle range is:_" << range << "_" << endl;
					}
					else if (line.find("sg") != string::npos) {
						e = line.rfind(",");
						b = line.rfind("%", e);
						range = line.substr(b, e - b);
						//cout << "sg range is:_" << range << "_" << endl;
					}
					// if is the number
					if (range[0] != '%') {
						//int num = stoi(range);
						int num = 0;
						num += 1;
						range = to_string(num);
					}
					else {		// else is the var
						range = range + "+";
					}
				}
				//cout << "loop variable:_" << var << "_ ; loop range:_" << range << "_" << endl;
			}
		}
	}

	else {				//Decompilation    --- rule is in note.txt
		regex load(" = load (i32|i16), (i32|i16)\\* %[a-z, 0-9]*");		//%176 = load i32, i32* %175
		regex load2(" = load (i32|i16), (i32|i16)\\* ");		//%176 = load i32, i32* %175
		regex add_(" = add (i32|i16) %[a-z, 0-9]*, -[0-9]*");		// %177 = add i32 %176, -3
		regex add("= add (i32|i16) %[a-z, 0-9]*, [0-9]*");			// % 177 = add i32 % 176, 3
		regex icmp_2(" = icmp eq (i32|i16) %[a-z, 0-9]*, 2");		// %182 = icmp eq i32 %181, 2
		regex icmp_0(" = icmp [a-z]* (i32|i16) %[a-z, 0-9]*, 0");		// %183 = icmp eq i32 %178, 0  %183 = icmp ne i32 %178, 0  %183 = icmp slt i32 %178, 0,
		smatch s;
		string line;
		bool slx = false;
		bool is_var = false;

		int len = bcontent.size();
		int l;
		//find load ins
		for (l = 0; l < len; l++) {
			line = bcontent[l];
			if (regex_search(line, s, load)) {
				break;
			}
		}
		for (int k = l + 1; k < len; k++) {
			string new_line = bcontent[k];
			if (regex_search(new_line, s, load2)) {
				int b = line.find("%");
				int e = line.find("=", b);
				range = new_line.substr(b, e - b - 1);
				//1 cout << "is var : load line is:_" << new_line << "_" << endl;
				//cout << "is_var : var is: _" << range << "_" << endl;
				is_var = true;
				break;
			}
		}

		// find the loop var
		int b = line.find("%");
		int e = line.find("=", b);
		var = line.substr(b, e - b - 1);
		//1 cout << "load line is:_" << line << "_" << endl;
		//cout << "de var is: _" << var << "_" << endl;

		line = bcontent[l - 1];
		b = line.rfind("%");
		e = line.find(" ", b);
		initvar = line.substr(b, e - b);
		//1 cout << "inttoptr line is:_" << line << "_" << endl;
		//cout << "de origin var is: _" << initvar << "_" << endl;
		//o res.push_back(line.substr(b, e - b - 1));

		for (int i = l; i < len; i++) {
			line = bcontent[i];
			if (regex_search(line, s, add_)) {
				slx = true;
				l = i;
				break;
			}
		}

		if (slx || is_var) {		// add - || is_var, slt or sle
			line = bcontent[l];
			b = line.find("-");
			string num = line.substr(b + 1);
			//cout << "de num is:_" << num << "_" << endl;

			for (int j = l + 1; j < len; j++) {
				line = bcontent[j];
				if (regex_search(line, s, icmp_0)) {	//sle
					//1 cout << "mmmmmmmmmm line: " << line << endl;
					for (int k = j + 1; k < len; k++) {
						//1 cout << "k is: " << k << " len is: " << len << endl;
						line = bcontent[k];
						//1 cout << bcontent[k] << endl;
						if (regex_search(line, s, icmp_2)) {	//sle
							//1 cout << "mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm 2" << endl;
							for (int h = k + 1; h < len; h++) {
								line = bcontent[h];
								if (regex_search(line, s, icmp_0)) {	//sle
									if (is_var) {			// the range should be the var + 1;
										range = range + "+1";
										//cout << "sssssssssssssssssssssssssssle range : de sle range is:" << range << endl;
									}
									else {
										int value = stoi(num);
										value += 1;
										//cout << "de value is:_" << value << endl;
										range = to_string(value);
									}
									//o res.push_back(range);
									break;
								}
							}
							break;
						}
					}
					break;
				}

				else if (regex_search(line, s, icmp_2)) {		//slt
					for (int k = j + 1; k < len; k++) {
						line = bcontent[k];
						if (regex_search(line, s, icmp_0)) {	//slt
							//o res.push_back(num);
							//new
							if (is_var) {			// the range should be the var + 1;
								range = range;
								//cout << "iiiiiiiiiiiiiiiiiiiiiiiiiiiis var : de slt range is:" << range << endl;
							}
							else {
								//cout << "de slt range is:" << num << endl;
								range = num;
							}
							break;
						}
					}
					break;
				}
			}


		}

		else {			//add +,sge or sgt			(In fact, 'else' only need one line: res.push_back(var); there is no need to distinguish sge and sgt. 
			bool sgt = false;
			for (int j = l + 1; j < len; j++) {
				line = bcontent[j];
				if (regex_search(line, s, icmp_0)) {
					for (int k = j; k < len; k++) {
						line = bcontent[k];
						if (regex_search(line, s, icmp_2)) {		//sgt
							sgt = true;
							break;
						}
					}
					break;
				}
			}

			if (sgt) {
				//o res.push_back(var);
				range = var + "+1";
			}
			else {
				//o res.push_back(var);
				range = var + "+1";
			}
		}
	}	//end of decompile

	cout << "loop variable:_" << var << "_ ; loop range:_" << range << "_" << endl;
	res.push_back(var);
	res.push_back(range);
	//res.push_back(block.getTitle());
	res.push_back(initvar);

	return res;
}



vector<string> oldfindLoopVarAndRange(Block block)
{
	vector<string> bcontent = block.getContent();
	vector<string> res;
	string var, range, initvar;
	//compilation
	//if(i = 0; i <=  5)     %2 = load i32, i32* %j, align 4
	                       //%cmp2 = icmp slt i32 % 2, 3
	//if (j = 5; j > 0)    //%1 = load i32, i32* %i, align 4
		                   //%cmp = icmp sge i32 %1, 0

	if (isDecompile == false) {		//Compile
		regex load(" = load +[i32|i16]+, +[i32|i16]+\\* %[a-z, 0-9]*");
		//regex load("= load i32, i32\\* %i6, align 4");
		regex icmp(" = icmp [a-z]* +[i32|i16]+ %[a-z, 0-9]*, [0-9]*");
		smatch s;
		string line;
		int b, e;
		for (int i = 0; i < bcontent.size(); i++) {
			line = bcontent[i];
			if (regex_search(line, s, load)) {
				///cout << "load line found." << endl;
				b = line.find("*");
				e = line.find(",", b);
				var = line.substr(b + 2, e - b - 2);
				//cout << "var is:_" << var << "_" << endl;
			}
			if (regex_search(line, s, icmp)) {
				e = line.find(",");
				string r = line.substr(e + 1);
				int val = stoi(r);
				if (val == 0) {		//if the val is 0, then the value of the loop-var is in the "var"
					range = var + "+ 1";	//must add 1;
					//cout << "trrrrright value is:_" << range << "_" << endl;
				}
				else {
					val = abs(val);
					int m = line.find("icmp");
					int n = line.find(" ", m + 5);
					string cond = line.substr(m + 5, n - m - 5);
					//cout << "cond is:_" << cond << "_" << endl;
					if (cond[2] == 'e') {
						val += 1;			//add 1 becasue : i <= 5, then the range is actually 6 = 5 + 1;
					}
					range = to_string(val);
					//cout << "right value is:_" << val << "_" << endl;
				}
				//cout << "var is:_" << var << "_ ;range is:_" << range << "_" << endl;
			}
		}

		//---ok  res.push_back(var);
		//---ok  res.push_back(range);
		//---ok  return res;
	}

	else {				//Decompilation    --- rule is in note.txt
		regex load(" = load (i32|i16), (i32|i16)\\* %[a-z, 0-9]*");		//%176 = load i32, i32* %175
		regex add_(" = add (i32|i16) %[a-z, 0-9]*, -[0-9]*");		// %177 = add i32 %176, -3
		regex add("= add (i32|i16) %[a-z, 0-9]*, [0-9]*");			// % 177 = add i32 % 176, 3
		regex icmp_2(" = icmp eq (i32|i16) %[a-z, 0-9]*, 2");		// %182 = icmp eq i32 %181, 2
		regex icmp_0(" = icmp [a-z]* (i32|i16) %[a-z, 0-9]*, 0");		// %183 = icmp eq i32 %178, 0  %183 = icmp ne i32 %178, 0  %183 = icmp slt i32 %178, 0,
		smatch s;
		string line;
		bool slx = false;

		int len = bcontent.size();
		int l;
		//find load ins
		for (l = 0; l < len; l++) {
			line = bcontent[l];
			if (regex_search(line, s, load)) {
				break;
			}
		}
		
		// find the loop var
		int b = line.find("%");
		int e = line.find("=", b);
		var = line.substr(b, e - b - 1);
		cout << "load line is:_" << line << "_" << endl;
		cout << "de var is: _" << var << "_" << endl;

		line = bcontent[l - 1];
		b = line.rfind("%");
		e = line.find(" ", b);
		initvar = line.substr(b, e - b);
		cout << "inttoptr line is:_" << line << "_" << endl;
		cout << "de origin var is: _" << initvar << "_" << endl;
		//o res.push_back(line.substr(b, e - b - 1));

		for (int i = l; i < len; i++) {
			line = bcontent[i];
			if (regex_search(line, s, add_)) {
				slx = true;
				l = i;
				break;
			}
		}

		if (slx) {		// add -, slt or sle
			line = bcontent[l];
			b = line.find("-");
			string num = line.substr(b + 1);
			cout << "de num is:_" << num << "_" << endl;

			for (int j = l + 1; j < len; j++) {
				line = bcontent[j];
				if (regex_search(line, s, icmp_0)) {	//sle
					for (int k = j + 1; k < len; k++) {
						line = bcontent[k];
						if (regex_search(line, s, icmp_2)) {	//sle
							for (int h = k + 1; h < len; h++) {
								line = bcontent[h];
								if (regex_search(line, s, icmp_0)) {	//sle
									int value = stoi(num);
									value += 1;
									cout << "de value is:_" << value << endl;
									range = to_string(value);
									//o res.push_back(range);
									break;
								}
							}
						}
						break;
					}
					break;
				}

				else if (regex_search(line, s, icmp_2)) {		//slt
					for (int k = j + 1; k < len; k++) {
						line = bcontent[k];
						if (regex_search(line, s, icmp_0)) {	//slt
							//o res.push_back(num);
							//new
							cout << "de slt range is:" << num << endl;
							range = num;
							break;
						}
					}
					break;
				}
			}

		
		}

		else {			//add +,sge or sgt			(!!!!��ʵelseֻ��ֱ�ӣ�һ�и㶨����������sge��sgt��res.push_back(var);
			bool sgt = false;
			for (int j = l + 1; j < len; j++) {
				line = bcontent[j];
				if (regex_search(line, s, icmp_0)) {	
					for (int k = j; k < len; k++) {
						line = bcontent[k];
						if (regex_search(line, s, icmp_2)) {		//sgt
							sgt = true;
							break;
						}
					}
					break;
				}
			}

			if (sgt) {
				//o res.push_back(var);
				range = var;
			}
			else {
				//o res.push_back(var);
				range = var;
			}
		}
		/*for (int i = l; i < len; i++) {
			line = bcontent[i];
			if (regex_search(line, s, add_)) {				//
				b = line.find("-");
				string num = line.substr(b);
				cout << "de num is:_" << num << "_" << endl;
				for (int j = i + 1; j < len; j++) {
					if (regex_search(line, s, icmp_0)) {	//sle
						for (int k = j + 1; k < len; k++) {
							if (regex_search(line, s, icmp_2)) {	//sle
								for (int h = k + 1; h < len; h++) {
									if (regex_search(line, s, icmp_0)) {	//sle
										int value = stoi(num);
										value += 1;
										res.push_back(to_string(value));
										break;
									}
								}
							}
						}

					}

					else if (regex_search(line, s, icmp_2)) {		//slt
						for (int k = j + 1; k < len; k++) {
							if (regex_search(line, s, icmp_0)) {	//slt
								res.push_back(num);
								break;
							}
						}
					}
				}
			}

			else {		//add +,sge or sgt



			}
			else if (regex_search(line, s, add)) {			//add +,sge or sgt


			}

			return res;

		}*/
	}	//end of decompile
	
	
	res.push_back(var);
	res.push_back(range);
	res.push_back(block.getTitle());
	res.push_back(initvar);

	return res;
}




string findDeRange(string var, vector<string> loopvar, vector<string> loopinitvar, vector<string> looprange)
{
	//cout << "var findRANGE is:_" << var << "_" << endl;
	string res;
	for (int j = 0; j < loopvar.size(); j++) {
		//cout << "findDErange, loopvar and loopinitvar:_" << loopvar[j] << " " << loopinitvar[j] << "_" << endl;
		if (var == loopvar[j] || var == loopinitvar[j]) {
			//cout << "FIND!" << endl;
			//is ok: return looprange[j];
			res = looprange[j];
			break;
		}
		/*else {
			cout << "Didn't find the var" << endl;
		}*/
	}

	return res;
}

// two ops of first add ins and one op of second add ins;
vector<string> findDeMat(VarList& add1op1, VarList& add1op2, VarList& add2op1, VarList& add2op2, vector<string>& loopvar, vector<string>& loopinitvar, vector<string>& looprange) {
	// return the {address, row, col} of matrix
	// if add2op is the address
	/*cout << "////////////////in finddemat\n";
	for (string& s : loopinitvar) {
		cout << "_" << s << endl;
	}
	add1op1.showList();
	add1op2.showList();
	add2op1.showList();
	add2op2.showList();*/
	string mataddr = "", matrow = "", matcol = "";
	// string t1, t2, t3, t4;
	bool f1 = true, f2 = true, f3 = true, f4 = true;
	int p1 = -1, p2 = -1, p3 = -1, p4 = -1;
	// find the correspond loop var
	for (int j = 0; j < loopvar.size(); j++) {
		if (f1) {
			for (int i = 0; i < add1op1.Size(); i++) {
				if (loopvar[j] == add1op1.getVar(i) || loopinitvar[j] == add1op1.getVar(i)) {
					// t1 = loopinitvar[j];
					f1 = false;
					p1 = j;
					break;
				}
			}
		}
		if (f2) {
			for (int i = 0; i < add1op2.Size(); i++) {
				if (loopvar[j] == add1op1.getVar(i) || loopinitvar[j] == add1op1.getVar(i)) {
					// t2 = loopinitvar[j];
					f2 = false;
					p2 = j;
					break;
				}
			}
		}
		if (f3) {
			for (int i = 0; i < add2op1.Size(); i++) {
				if (loopvar[j] == add2op1.getVar(i) || loopinitvar[j] == add2op1.getVar(i)) {
					// t3 = loopinitvar[j];
					f3 = false;
					p3 = j;
					break;
				}
			}
		}
		if (f4) {
			for (int i = 0; i < add2op2.Size(); i++) {
				if (loopvar[j] == add2op2.getVar(i) || loopinitvar[j] == add2op2.getVar(i)) {
					// t4 = loopinitvar[j];
					f4 = false;
					p4 = j;
					break;
				}
			}
		}
	}
	// cout << "-_-_-_-_-_-_" << t1 << "-" << t2  << "-" << t3 << endl;
	if (f3) {			// address in second 'add' ins
		mataddr = add2op1.getVar(0);
		matrow = looprange[p4];
		matcol = p4 == p1 ? looprange[p2] : looprange[p1];
	} else if (f1) { // address in 1st 'add' ins, and the 1st op
		mataddr = add1op1.getVar(0);
		matrow = looprange[p2];
		matcol = p2 == p3 ? looprange[p4] : looprange[p3];
	} else if (f2) { // address in 1st 'add' ins, and the 1st op
		mataddr = add1op2.getVar(0);
		matrow = looprange[p1];
		matcol = p1 == p3 ? looprange[p4] : looprange[p3];
	}
	// cout << "------------ find de mat: " << mataddr << "----" << matrow << "----" << matcol << endl;
	return {mataddr, matrow, matcol};
}

vector<string> findDeVec(VarList& addop1, VarList& addop2, vector<string>& loopvar, vector<string>& loopinitvar, vector<string>& looprange) {
	bool f1 = true, f2 = true;
	int p1 = -1, p2 = -1;
	string mataddr = "", matrow = "1", matcol = "";
	// find the correspond loop var
	for (int j = 0; j < loopvar.size(); j++) {
		if (f1) {
			for (int i = 0; i < addop1.Size(); i++) {
				if (loopvar[j] == addop1.getVar(i) || loopinitvar[j] == addop1.getVar(i)) {
					// t1 = loopinitvar[j];
					f1 = false;
					p1 = j;
					break;
				}
			}
		}
		if (f2) {
			for (int i = 0; i < addop2.Size(); i++) {
				if (loopvar[j] == addop2.getVar(i) || loopinitvar[j] == addop2.getVar(i)) {
					// t2 = loopinitvar[j];
					f2 = false;
					p2 = j;
					break;
				}
			}
		}
	}
	if (f1) { // address in 1st 'add' ins, and the 1st op
		mataddr = addop1.getVar(0);
		matcol = looprange[p2];
	} else if (f2) { // address in 1st 'add' ins, and the 1st op
		mataddr = addop2.getVar(0);
		matcol = looprange[p1];
	}

	return {mataddr, matrow, matcol};
}


vector<string> findDeMat(string line1, string line2 , vector<string> loopvar, vector<string> loopinitvar, vector<string> looprange, vector<string> bodycontent)
{
	// goal: return the mat address, row and col size
	string addr, row, col;
	cout << "////////////////in finddemat\n";
	for (string& s : loopvar) {
		cout << "_" << s << endl;
	}
	cout << "////////////////in finddemat\n";
	for (string& s : loopinitvar) {
		cout << "_" << s << endl;
	}
	// the example of the line1 and line2
	// %166 = add i64 %165, %67
	// %169 = add i64 %166, %168          : %67 is the address of the mat; %165 and %168 is the loop var(like i, j)
	string add_1, op_1, or_1;
	string op_2, or_2;
	VarList dest_1, var_1, address_1;		// represent the first ins: %166
	VarList dest_2, var_2, add_temp;			// represent the second ins: %169
	add_1 = findOperands(line1)[0];
	op_1 = findOperands(line1)[1];
	or_1 = findOperands(line1)[2];			//or_1 is the mat address
	addr = or_1;

	var_1.deBuilidVarList(bodycontent, op_1);
	//cout << "the var_1.getVar(0) is:_" << var_1.getVar(var_1.Size() - 1) << "_" << endl;

	row = findDeRange(var_1.getVar(var_1.Size() - 1), loopvar, loopinitvar, looprange);			// the value of the row

	//add_2 = findOperands(line2)[0];
	op_2 = findOperands(line2)[1];
	or_2 = findOperands(line2)[2];

	var_2.deBuilidVarList(bodycontent, op_2);
	add_temp.deBuilidVarList(bodycontent, or_2);

	// judge which is the middle var
	if (var_2.findVar(add_1)) {			//if is the first opr
		col = findDeRange(add_temp.getVar(add_temp.Size() - 1), loopvar, loopinitvar, looprange);
	}
	else if (add_temp.findVar(add_1)) {			// if is the second opr
		col = findDeRange(var_2.getVar(var_2.Size() - 1), loopvar, loopinitvar, looprange);
	}
	
	vector<string> res;
	res.push_back(addr);
	res.push_back(row);
	res.push_back(col);

	return res;
	//return { addr, row, col };
}

string datatype(string line) {
	//%219 = fmul float %218, % 217
	//%166 = mul i32 %165, %152
	//%134 = and i32 %133, %130
	int e = line.rfind(",");
	int o = line.rfind(" ", e);
	int t = line.rfind(" ", o - 1);
	//string type = line.substr(t + 1, o - t - 1);
	//return type;
	return line.substr(t + 1, o - t - 1);
}


vector<string> FindAddrAndRange(vector<VarList> list, vector<string> loopvar, vector<string> loopinitvar, vector<string> looprange)
{
	vector<string> res;
	string addr, row, col;
	int len = list.size();
	if (len == 2) {			//mvm
		row = 1;
		for (int i = 0; i < len; i++) {
			if (list[i].Size() == 1) {		//address var
				addr = list[i].getVar(0);
			}
			else {
				// find the loop var
				string var = list[i].getVar(list[i].Size() - 1);
				for (int j = 0; j < loopvar.size(); j++) {
					if (var == loopvar[j] || var == loopinitvar[j]) {
						col = looprange[j];
						break;
					}
				}
			}
		}
	}

	else if (len == 3) {			// mvm
		for (int i = 0; i < len; i++) {
			if (list[i].Size() == 1) {		//address var
				addr = list[i].getVar(0);
			}
			else {
				// find the loop var
				string var = list[i].getVar(list[i].Size() - 1);
				for (int j = 0; j < loopvar.size(); j++) {
					if (var == loopvar[j] || var == loopinitvar[j]) {
						if (i == 0) {			// the first one is the row
							row = looprange[j];
						}
						else {					// the second one is the col
							col = looprange[j];
						}
						break;
					}
				}
			}
		}
	}

	res.push_back(addr);
	res.push_back(row);
	res.push_back(col);
	
	return res;
}

string DealDeLoopRange(vector<string> &looprange, int num)
{
	string InitIns;
	for (int i = 0; i < looprange.size(); i++) {
		string range = looprange[i];
		if (range.find("+") != string::npos) {		//if find +
			//string size = looprange[i].substr(0, looprange[i].size() - 2) + "_size";// +to_string(num);
			string size = "%size_" + looprange[i].substr(1, looprange[i].size() - 3);// +to_string(num);
			InitIns += ("  " + size + " = add i32 " + looprange[i].substr(0, looprange[i].size() - 2) + ", 1\n");
			//cout << "add new ins:_" << InitIns << endl;
			looprange[i] = size;
			//cout << "Insert here ins:_" << looprange[i] << "_and_" << size << endl;
			//cout << "range init ins:_" << rangeInit << endl;
			//cout << "now Init ins:_" << InitIns << endl;
		}
	}
	return InitIns;
}

void DeFindAddressAndRange(vector<string> addIns)
{
	int len = addIns.size();

	if (len == 4) {
		//vec + mat + vec
		VarList add_1, op1_1, op2_1, add_2, op1_2, op2_2;
		string goal, op1, op2;
		for (int i = 1; i < 2; i++) {

		}


	}
	else if (len == 6) {

	}
}



















void analysisLoops(BlockList list)
{
	//cout << "calling the analysisiLoops" << endl;
	int len = list.getListSize();		//get the loops' depth
	//cout << "the size is:_" << len << endl;
	Block keyBodyBlock = list.findById(len - 1);
	if (len == 2) {
		if (BodyKeywords(keyBodyBlock) == "logic") {
			cout << "may be the bitmap logical operation" << endl;
		}
	}
	else if (len == 4) {
		if (BodyKeywords(keyBodyBlock) == "mmm") {
			cout << "may be the mvm operation" << endl;
		}
	}
	else {
		if (BodyKeywords(keyBodyBlock) == "mmm") {
			cout << "may mmm operation" << endl;
		}
	}
}

string BodyKeywords(Block block)
{
	//logic operation: or, xor, and
	//mvm, mmm: mul/fmul, add/fadd
	//string type = "other";
	regex logic_op("= [or|xor|and] ");      //Regular matching, identify whether it is a cond block
	regex mul(" = [f]*mul ");      //whether it is a body block
	regex add(" = +[fadd|add]+ ");
	smatch s;
	string line;
	vector<string> content = block.getContent();
	for (int i = content.size() - 1; i >= 0; i--) {
		line = content[i];
		//cout << "line is:_" << line << endl;
		if (regex_search(line, s, logic_op)) {
			//type = "logic";
			return "logic";
		}
		else if (regex_search(line, s, add)) {
			cout << "matching add" << endl;
			for (int j = i; j >= 0; j--) {
				line = content[j];
				if (regex_search(line, s, mul)) {
					//type = "mmm";
					//break;
					cout << "matching mul" << endl;
					return "mmm";
				}
			}
		}
	}
	return "none";
}


//find the name of the next two(one) block
vector <string> NextBlocks(string oneline)
{
	//cout << "input brlabel is:_" << oneline << endl;
	vector <string> names;
	int q, r;   //positions of the key chars

	string truelabel, falselabel;		//the two next labels
	//br i1 %cmp, label %for.body, label %for.end           br label %for.inc52
	if (oneline.find("ret") != string::npos) {			//ret is the last block
		truelabel = falselabel;
	}
	else {
		q = oneline.find("label");

		if ((r = oneline.find(",", q)) != string::npos) {		//if there are two preds
			truelabel = oneline.substr(q + 7, r - q - 7);
			falselabel = oneline.substr(r + 9);
		}
		else {
			truelabel = oneline.substr(q + 7);
		}
	}
	//cout << "true_label:_" << truelabel << "_" << "                  false_label:_" << falselabel << "_" << endl;

	names.push_back(truelabel);
	names.push_back(falselabel);
	return names;

}

//find the name and the preds block's name
vector <string> BlockNameAndPreds(string oneline)
{
	vector <string> names;
	int p, q, r;   //positions of the key chars
	string title, preds_1, preds_2;;   //the block name of current block and the two preds

	p = oneline.find(":");
	title = oneline.substr(0, p);
	//cout << "titl:_" << title << endl;

	if ((q = oneline.find("preds")) != string::npos) {
		if ((r = oneline.find(",")) != string::npos) {		//if there are two preds
			preds_1 = oneline.substr(q + 9, r - q - 9);
			preds_2 = oneline.substr(r + 3);
			//cout << "prdes1:_" << preds_1 << "_" << " prdes2:_" << preds_2 << "__" << endl;
		}
		else {
			preds_1 = oneline.substr(q + 9);
			//cout << "prdes1:_" << preds_1 << "_" << endl;
		}
	}

	names.push_back(title);
	names.push_back(preds_1);
	names.push_back(preds_2);
	return names;
}

//Determine the type of block(juedge if it's the loop condition block)
string BlcokType(Block block)
{
	string type;
	string title = block.getTitle();
	//compile-according to the name of the block
	regex loop_cond("[for|while]+.cond[0-9]*");      //Regular matching, identify whether it is a cond block
	regex loop_body("[for|while]+.body[0-9]*");      //whether it is a body block
	smatch s;
	if (regex_search(title, s, loop_cond)) {
		type = "loop_cond";
	}
	else if (regex_search(title, s, loop_body)) {
		type = "loop_body";
	}
	else
		type = "other";

	//Decompile-according to the key instrctions of the block
	string preds_1 = block.getPreds1();
	string preds_2 = block.getPreds2();
	vector <string> blockcontent = block.getContent();
	regex phi_1("phi i64"), phi_2("phi %struct.Memory\\* ");
	if (!preds_1.empty() && !preds_2.empty()) { //two preds are not null 
		cout << "the first :" << blockcontent[1] << endl;
		cout << "the second :" << blockcontent[2] << endl;
		if (regex_search(blockcontent[1], s, phi_1) && regex_search(blockcontent[2], s, phi_2)) {
			type = "loop_cond";
		}
	}

	return type;
}

//find all the variables that equal to the goal variable 


bool isEqual(VarList list1, VarList list2)
{
	vector<string> varlist1 = list1.getList();
	for (int i = 0; i < varlist1.size(); i++) {
		if (list2.findVar(varlist1[i])) {
			return true;
			break;
		}
	}
	return false;
}

vector<string> findMAC() {

}

vector<pair<string, string>> findVectorAndMatrix() {
	// vector: loop var in add ins/getelementptr
	// mat: two add, two loop var, relay;  / getelementptr
	
}
