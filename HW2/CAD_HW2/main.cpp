#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<map>
using namespace std;
void printVector( vector<string> Text,bool writefile=false)
{
	for (auto i = Text.begin(); i != Text.end(); i++)
	{
		cout << *i << endl;
	}
	if (writefile)
	{
		fstream outFile("a.out", ios::out);
		for (auto i = Text.begin(); i != Text.end(); i++)
		{
			outFile << *i << endl;
		}
		outFile.close();
	}
	return;
}

class node
{
public:
	node()
	{
		name = "";
		booleanFunction.clear();
		booleanFunctionType.clear();
		predecessor.clear();
		predecessor_Index.clear();
		successor_Index.clear();
	}
	string name;
	vector<string> booleanFunction;
	vector<string> predecessor;
	vector<string> successor;
	vector<bool> booleanFunctionType;
	vector<int> predecessor_Index;
	vector<int> successor_Index;
	int Index=0;
	char aoiType=-1;	//AOI TYPE and=0 or=1 NOT=2
	int ASAPINT=0;
	int ALAPINT=0;
	int SLACK=0;
	int NowSlack = 0;
	bool used = false;
};

class graph
{
public:
	graph()
	{
		nodes.clear();
		modelName = "";
		inputNodes.clear();
		outputNodes.clear();
	}
	bool Process(const int& argc,const vector<string>& argv)
	{
		if (argc < 3)
		{
			return false;
		}
		readBlif(argv[2]);
		IndexAble();
		write_Output_File(argv[2]);
		
		if (argv[1] == "-l")
		{
			if (argc < 4)
			{
				cout << "invalid input\n";
				return false;
			}
			printVector(MR_LCS(stoi(argv[3])), true);
		}
		else if (argv[1] == "-r")
		{
			if (argc < 6)
			{
				cout << "invalid input\n";
				return false;
			}
			printVector(ML_RCS(stoi(argv[3]), stoi(argv[4]), stoi(argv[5])), true);
		}

		
		return true;
		
	}
	vector<string> ML_RCS(int AND, int OR, int NOT)
	{
		ASAP_Scheduling();
		ALAP_Scheduling();
		SLACK_Process();
		vector<string> processText;
		processText.push_back("Resource-constrained Scheduling");
		vector<int>resource;
		resource.resize(3, 0);
		*resource.begin() = AND;
		*(++resource.begin()) = OR;
		*(++++resource.begin()) = NOT;
		vector<int>ready[3];
		//vector<node>copyNodes = this->nodes;
		map<int, int>checkTable;
		for (auto i = inputNodes.begin(); i != inputNodes.end(); i++)
		{
			//checkTable[findNode(*i)] = 1;
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				if (*i == j->name)
				{
					checkTable[j->Index] = 1;
					j->used = true;
					break;
				}
			}
			//ready[nodes[findNode(*i)].aoiType].push_back(nodes[findNode(*i)]);
		}
		if (AND == 0)
		{
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				if (j->aoiType == 0)
				{
					processText.push_back("No feasible solution.\nEND");
					return processText;
				}
			}
		}
		if (OR == 0)
		{
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				if (j->aoiType == 1)
				{
					processText.push_back("No feasible solution.\nEND");
					return processText;
				}
			}
		}
		if (NOT == 0)
		{
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				if (j->aoiType == 2)
				{
					processText.push_back("No feasible solution.\nEND");
					return processText;
				}
			}
		}


		int l = 1;
		for (;;l++)
		{
			string Text = "";
			Text = to_string(l)+": ";
			for (auto i = nodes.begin(); i != nodes.end();i++)
			{
				if (i->used == true)
				{
					continue;
				}
				bool check = true;

				for (auto j = i->predecessor_Index.begin(); j != i->predecessor_Index.end(); j++)
				{
					if (checkTable[*j] != 1)
					{
						check = false;
						break;
					}
				}
				if (check)
				{
					ready[i->aoiType].push_back(i->Index);
					i->used = true;
					//i = copyNodes.erase(i);
					continue;
				}
				
			}

			

			if (ready[0].size() == 0 && ready[2].size() == 0 && ready[1].size() == 0)
				break;

			for (int i = 0; i <= 2; i++)
			{
				for (auto j = ready[i].begin(); j != ready[i].end(); j++)
				{
					this->nodes[*j].NowSlack = this->nodes[*j].ASAPINT + this->nodes[*j].SLACK - l;
				}
				sort(ready[i].begin(), ready[i].end(), [&](const int& a, const int& b) {return this->nodes[a].NowSlack < this->nodes[b].NowSlack; });
			}

			//And Or I 
			int type = 0;
			for (auto i=resource.begin();i!=resource.end();i++)
			{
				Text += "{";
				for (int j = 0; j < *i; j++)
				{
					//sort(ready[type].begin(), ready[type].end(), [&](const node& a, const node& b) {return a.SLACK < b.SLACK; });
					//to do the selection of the ready
					if (ready[type].size()> 0)
					{
						j == 0 ? Text += this->nodes[*ready[type].begin()].name : Text += " " + this->nodes[*ready[type].begin()].name;
						checkTable[*ready[type].begin()] = 1;

						ready[type].erase(ready[type].begin());
					}
				}
				type++;
				Text += "} ";
				
			}
			processText.push_back(Text);
			

		}

		
		processText.push_back("#AND: " + to_string(*resource.begin()));
		processText.push_back("#OR: " + to_string(*(++resource.begin())));
		processText.push_back("#NOT: " + to_string(*(++++resource.begin())));
		processText.push_back("END");
		return processText;
	}
	vector<string> MR_LCS(int time)
	{
		vector<string>processText;
		processText.push_back("Latency - constrained Scheduling");
		//cout << "Latency - constrained Scheduling";
		ASAP_Scheduling();
		bool valid = ALAP_Scheduling(time);
		SLACK_Process();
		if (!valid)
		{
			processText.push_back("No feasible solution.\nEND");
			return processText;
		}
		vector<int>resource;
		resource.resize(3, 0);
		
		map<int, int>checkTable;

		for (auto i = inputNodes.begin(); i != inputNodes.end(); i++)
		{
			//checkTable[findNode(*i)] = 1;
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				if (*i == j->name)
				{
					checkTable[j->Index] = 1;
					j->used = true;
					break;
				}
			}
		}

		//map<int, int>backup_checkTable = checkTable;
		for (;;)
		{
			int l = 1;
			string Text = "";
			//checkTable.clear();
			//checkTable = backup_checkTable;
			//copyNodes = backup_CopyNodes;
			vector<int>ready[3];
			for (;; l++)
			{
				
				for (auto i = nodes.begin(); i != nodes.end(); i++)
				{
					if (i->used)
					{
						continue;
					}

					bool check = true;
					for (auto j = i->predecessor_Index.begin(); j != i->predecessor_Index.end(); j++)
					{
						if (checkTable[*j] != 1)
						{
							check = false;
							break;
						}
					}
					if (check)
					{
						ready[i->aoiType].push_back(i->Index);
						i->used = true;
						//i = copyNodes.erase(i);
					}
					//i++;
				}
				//check done
				if (ready[0].size() == 0 && ready[2].size() == 0 && ready[1].size() == 0)
					break;

				//check if need resource
				int requireResource = -1;
				for (int i = 0; i <= 2; i++)
				{
					for (auto j = ready[i].begin(); j != ready[i].end(); j++)
					{
						this->nodes[*j].NowSlack = this->nodes[*j].ASAPINT + this->nodes[*j].SLACK - l;
						if (this->nodes[*j].NowSlack < 0)
						{
							//resource[i]++;
							requireResource = i;
							break;
						}
					}
				}
				Text += to_string(l) + ": ";
				//add resource
				if (requireResource != -1)
				{
					resource[requireResource]++;
					l = -1;
					break;
				}

				//And Or I 
				int type = 0;
				for (auto i = resource.begin(); i != resource.end(); i++)
				{
					Text += "{";
					for (int j = 0; j < *i; j++)
					{
						sort(ready[type].begin(), ready[type].end(), [&](const int& a, const int& b) {return nodes[a].NowSlack < nodes[b].NowSlack; });
						//to do the selection of the ready
						if (ready[type].size() > 0)
						{
							j == 0 ? Text += nodes[*ready[type].begin()].name : Text += " " + nodes[*ready[type].begin()].name;
							checkTable[nodes[*ready[type].begin()].Index] = 1;
							ready[type].erase(ready[type].begin());
						}
					}
					type++;
					Text += "} ";

				}
				Text += l==time?"":"\n";
				//Text = "";
			}
			if (l !=-1)
			{
				processText.push_back(Text);
				break;
			}

			checkTable.clear();
			for (auto jC = nodes.begin(); jC != nodes.end(); jC++)
			{
					jC->used = false;
			}
			for (auto iC = inputNodes.begin(); iC != inputNodes.end(); iC++)
			{
				//checkTable[findNode(*i)] = 1;
				for (auto jC = nodes.begin(); jC != nodes.end(); jC++)
				{
					if (*iC == jC->name)
					{
						checkTable[jC->Index] = 1;
						jC->used = true;
						break;
					}
				}
				//ready[nodes[findNode(*i)].aoiType].push_back(nodes[findNode(*i)]);
			}
		}

		processText.push_back("#AND: " + to_string(*resource.begin()));
		processText.push_back("#OR: " + to_string(*(++resource.begin())));
		processText.push_back("#NOT: " + to_string(*(++++resource.begin())));
		processText.push_back("END");
		return processText;
	}
	void SLACK_Process()
	{
		for (auto i = nodes.begin(); i != nodes.end(); i++)
		{
			i->SLACK = i->ALAPINT - i->ASAPINT;
		}
	}
	void ASAP_Scheduling()
	{
		map<int, int> checkTable;
		int count = 0;
		for (auto i = this->inputNodes.begin();i!=this->inputNodes.end();i++)
		{
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				if (j->name == *i)
				{
					int index = j->Index;
					nodes[index].ASAPINT = 0;
					checkTable[index] = 1;
					j->used=true;
					count++;
					break;
				}
			}
		}

		for (int i = 1;; i++)
		{
			vector<int> putToTable;
			for (auto j = nodes.begin(); j !=nodes.end();j++ )
			{
				
				if (j->used)
				{
					continue;
				}
				bool check = true;
				for (auto l = j->predecessor_Index.begin(); l != j->predecessor_Index.end(); l++)
				{
					if (checkTable[*l] != 1)
					{
						check = false;
						break;
					}
				}
				if (check)
				{
					//checkTable[j->Index] = 1;
					putToTable.push_back(j->Index);
					nodes[j->Index].ASAPINT = i;
					j->used = true;
					count++;
					continue;
				}
				
			}
			if (count == nodes.size())
			{
				for (auto it = nodes.begin(); it != nodes.end(); it++)
					it->used = false;
				break;
			}
			for (auto k = putToTable.begin(); k != putToTable.end(); k++)
				checkTable[*k] = 1;
		}
	}
	bool ALAP_Scheduling(int time = 0)
	{
		map<int, int> checkTable;
		int count = 0;
		for (auto i = this->outputNodes.begin(); i != this->outputNodes.end(); i++)
		{
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				if (j->name == *i)
				{
					int index = j->Index;
					nodes[index].ALAPINT = time;
					checkTable[index] = 1;
					j->used = true;
					count++;
					break;
				}
			}
		}
		int i = time;
		for (i = time-1;; i--)
		{
			vector<int>putToTable;
			for (auto j = nodes.begin(); j != nodes.end();j++)
			{

				if (j->used)
					continue;
				bool check = true;
				for (auto l = j->successor_Index.begin(); l != j->successor_Index.end(); l++)
				{
					if (checkTable[*l] != 1)
					{
						check = false;
						break;
					}
				}
				if (check)
				{
					//checkTable[j->Index] = 1;
					putToTable.push_back(j->Index);
					nodes[j->Index].ALAPINT = i;
					j->used = true;
					count++;
					//j=copyNodes.erase(j);
					continue;
				}
			}
			if (count == nodes.size())
			{
				for (auto it = nodes.begin(); it != nodes.end(); it++)
					it->used = false;
				break;
			}
			for (auto k = putToTable.begin(); k != putToTable.end(); k++)
				checkTable[*k] = 1;
		}

		if (i < 0)
		{
			for (auto j = nodes.begin(); j != nodes.end(); j++)
			{
				j->ALAPINT -= (i);
			}
			return false;
		}
		else
		{
			return true;
		}

	}
	void IndexAble()
	{
		sort(nodes.begin(), nodes.end(), [&](const node& a, const node& b) {return a.name < b.name; });
		int count = 0;
		for (vector<node>::iterator i = nodes.begin(); i != nodes.end(); i++)
		{
			i->predecessor_Index.clear();
			i->successor_Index.clear();
			i->Index = count++;
			for (vector<string>::iterator j = i->predecessor.begin(); j != i->predecessor.end(); j++)
			{
				i->predecessor_Index.push_back(findNode(*j));
			}
			for (vector<string>::iterator j = i->successor.begin(); j != i->successor.end(); j++)
			{
				i->successor_Index.push_back(findNode(*j));
			}
		}
	}
	bool write_Output_File(string argName)
	{
		fstream outFile("function.out", ios::out);
		outFile << "Node function:" << endl;
		for (vector<node>::iterator i = nodes.begin(); i != nodes.end(); i++)
		{
			if (i->booleanFunction.size() > 0)
			{
				i->aoiType = 0;
				outFile << i->name;
				if (!i->booleanFunctionType[0])
				{
					outFile << "'";
				}
				outFile  << " = ";
				for (int j = 0; j < i->booleanFunction.size(); j++)
				{
					outFile << i->booleanFunction[j];
					if (i->booleanFunction.size() == 1 && i->booleanFunction[0][i->booleanFunction[0].size()-1]=='\'')
					{
						i->aoiType = 2;
					}
					if (j + 1 < i->booleanFunction.size())
					{
						outFile << " +";
						i->aoiType = 1;
					}
				}
				outFile << endl;
			}
		}
		outFile << "END";
		outFile.close();

		fstream SlackFile("Slack.out", ios::out);
		ASAP_Scheduling();
		ALAP_Scheduling();
		SLACK_Process();
		for (auto i = nodes.begin(); i != nodes.end(); i++)
		{
			SlackFile << i->name << " Slack = " << i->SLACK <<" ASAP = "<<i->ASAPINT <<" ALAP = "<<i->ALAPINT<< endl;
			//i->successor.clear();
			//i->booleanFunctionType.clear();
			//i->booleanFunction.clear();
			//i->predecessor.clear();
		}
		SlackFile.close();
		
		return true;
	}
	int printPredecessor(string targetNode)
	{
		string temp = "predecessor:";
		int targetIndex = findNode(targetNode);
		if (nodes[targetIndex].predecessor.size() == 0)
		{
			temp += " -";
		}
		else
		{
			for (int i = 0; i < nodes[targetIndex].predecessor.size(); i++)
			{
				if (i == 0)
				{
					temp += " ";
					temp += nodes[targetIndex].predecessor[i];
					continue;
				}
				temp += ", " + nodes[targetIndex].predecessor[i];
			}
		}
		cout << temp << endl;
		return 0;
	}
	int printSuccessor(string targetNode)
	{
		string temp = "successor:";
		int targetIndex = findNode(targetNode);
		if (nodes[targetIndex].successor.size() == 0)
		{
			temp += " -";
		}
		else
		{
			for (int i = 0; i < nodes[targetIndex].successor.size(); i++)
			{
				if (i == 0)
				{
					temp += " ";
					temp += (nodes[targetIndex].successor[i]);
					continue;
				}
				temp += ", " + nodes[targetIndex].successor[i];
			}
		}
		cout << temp << endl;
		return 0;
	}
	int findNode(string boolName)
	{
		for (int i = 0; i != this->nodes.size(); i++)
		{
			if (this->nodes[i].name == boolName)
			{
				return i;
			}
		}
		return -1;
	}
	bool addSuccessor(node& addNode, string targetBoolName)
	{
		for (vector<string>::iterator i = addNode.successor.begin(); i != addNode.successor.end(); i++)
		{
			if ((*i) == targetBoolName)
			{
				return false;
			}
		}

		addNode.successor.push_back(targetBoolName);
		return true;

	}
	bool addPredecessor(node& addNode, string targetBoolName)
	{
		for (vector<string>::iterator i = addNode.predecessor.begin(); i != addNode.predecessor.end(); i++)
		{
			if ((*i) == targetBoolName)
			{
				return false;
			}
		}
		addNode.predecessor.push_back(targetBoolName);
		return true;
	}
	void createNode(string nodeName)
	{
		node tempNode;
		tempNode.name = nodeName;
		this->nodes.push_back(tempNode);
		//return &this->nodes[this->nodes.size() - 1];
	}
	void print_all_Address()
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			cout << "node: " << nodes[i].name << "  address: " << &nodes[i] << endl;
		}
	}
	bool readBlif(string fileName)
	{
		fstream inputFile;
		inputFile.open(fileName);
		if (!inputFile.is_open())
		{
			cout << "Fail In Open File" << endl;
			return false;
		}
		string command;

		while (inputFile >> command)
		{
			stringstream ss;
			string temp;
			if (command == ".names")
			{
			string stringtemp;
			while(true)
			{
				
				getline(inputFile, stringtemp);
				if(stringtemp[stringtemp.length()-1]!='\\')
				{
					temp+=stringtemp;
					break;
				}
				else if(stringtemp[stringtemp.length()-1]=='\\')
				{
					stringtemp[stringtemp.length()-1]=' ';
					temp+=stringtemp;
				}
					
			}
				ss << temp;
				vector<int> nodeIndex;
				//find boolean data in nodes;
				while (ss >> temp)
				{
					if (this->findNode(temp) == -1)
					{
						this->createNode(temp);
					}
					nodeIndex.push_back(findNode(temp));
				}
				//connect boolean node
				for (int j = 0; j < nodeIndex.size() - 1; j++)
				{
					this->addSuccessor(this->nodes[nodeIndex[j]], this->nodes[nodeIndex[nodeIndex.size() - 1]].name);
					this->addPredecessor(this->nodes[nodeIndex[nodeIndex.size() - 1]], this->nodes[nodeIndex[j]].name);
				}
				//bug fix cause when the vector expand the address will be difference
				//print_all_Address();

				for (; inputFile.peek() != '.';)
				{
					getline(inputFile, temp);
					string boolF = "";
					for (int i = 0; i < temp.length(); i++)
					{
						//output boolean process
						if (temp[i] == ' ')
						{
							if (temp[i + 1] == '1')
							{
								nodes[nodeIndex[nodeIndex.size() - 1]].booleanFunctionType.push_back(true);
								nodes[nodeIndex[nodeIndex.size() - 1]].booleanFunction.push_back(boolF);
							}
							else if (temp[i + 1] == '0')
							{
								nodes[nodeIndex[nodeIndex.size() - 1]].booleanFunctionType.push_back(false);
								nodes[nodeIndex[nodeIndex.size() - 1]].booleanFunction.push_back(boolF);
							}
							break;
						}
						//input boolean process
//cout<<"node_index "<<nodeIndex.size()<<" i: "<<i<<" temp "<<temp<<" temp length  "<<temp.length()<<endl;
/*if(i==temp.length()-1){
break;
}*/
						temp[i] == '0' ? boolF += " " + nodes[nodeIndex[i]].name + "'" : temp[i] == '1' ? boolF += " " + nodes[nodeIndex[i]].name : boolF += "";
					}
				}


			}
			else if (command == ".outputs")
			{
				getline(inputFile, temp);
				ss << temp;
				while (ss >> temp)
				{
					this->createNode(temp);
					this->outputNodes.push_back(temp);
				}
			}
			else if (command == ".inputs")
			{
				getline(inputFile, temp);
				ss << temp;
				while (ss >> temp)
				{
					this->createNode(temp);
					this->inputNodes.push_back(temp);
				}
			}
			else if (command == ".model")
			{
				inputFile >> this->modelName;
			}
			else if (command == ".end")
			{
				return true;
			}
		}
		inputFile.close();
		return true;
	}

private:
	vector<node> nodes;
	string modelName;
	vector<string> inputNodes;
	vector<string> outputNodes;
};

int main(int argc, char* argv[])
{
	graph boolGraph;
	vector<string>argvName;
	for (int i = 0; i < argc; i++)
	{
		argvName.push_back(argv[i]);
	}
	boolGraph.Process(argc,argvName);
	
	/*string command;
	while (true)
	{
		cout << "Please input a node: ";
		cin >> command;
		if (boolGraph.findNode(command) == -1)
		{
			if (command == "0")
			{
				break;
			}
			cout << "node " << command << " does not exist" << endl;
			continue;
		}

		boolGraph.printPredecessor(command);
		boolGraph.printSuccessor(command);
	}*/
	return 0;
}
