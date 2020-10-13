#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<map>
using namespace std;


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
		successor.clear();
	}
	string name;
	vector<string> booleanFunction;
	vector<bool> booleanFunctionType;
	vector<string> predecessor;
	vector<string> successor;
	vector<int> predecessor_Index;
	vector<int> successor_Index;
	int Index=0;
	char aoiType=0;	//AOI TYPE and=1 or=2 NOT=3
	int ASAPINT;
	int ALAPINT;
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
	void ASAP_Scheduling()
	{
		map<int, int> checkTable;
		vector<node> copyNodes=this->nodes;
		for (auto i = this->inputNodes.begin();i!=this->inputNodes.end();i++)
		{
			int index = this->findNode(*i);
			nodes[index].ASAPINT = 0;
			checkTable[index] = 1;
			for (auto j = copyNodes.begin(); j != copyNodes.end(); j++)
			{
				if (j->name == *i)
				{
					copyNodes.erase(j);
					break;
				}
			}
		}

		for (int i = 1;; i++)
		{
			for (auto j = copyNodes.begin(); j != copyNodes.end(); )
			{
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
					checkTable[j->Index] = 1;
					j->ASAPINT = i;
					copyNodes.erase(j);
					continue;
				}
				j++;
			}
			if (copyNodes.size() == 0)
				break;
		}
	}
	void ALAP_Scheduling()
	{


	}
	void IndexAble()
	{
		sort(nodes.begin(), nodes.end(), [&](const node a, const node b) {return a.name < b.name; });
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
		sort(nodes.begin(), nodes.end(), [&](const node a, const node b) {return a.name < b.name; });
		for (vector<node>::iterator i = nodes.begin(); i != nodes.end(); i++)
		{
			if (i->booleanFunction.size() > 0)
			{
				i->aoiType = 1;
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
						i->aoiType = 3;
					}
					if (j + 1 < i->booleanFunction.size())
					{
						outFile << " +";
						i->aoiType = 2;
					}
				}
				outFile << endl;
			}
		}
		outFile << "END";
		outFile.close();
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
	node* createNode(string nodeName)
	{
		node tempNode;
		tempNode.name = nodeName;
		this->nodes.push_back(tempNode);
		return &this->nodes[this->nodes.size() - 1];
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
	if (argc < 2)
	{
		cout << "please input the Blif file";
	}
	if (!boolGraph.readBlif("aoi_sample01.blif"))
	{
		return -1;
	}
	boolGraph.write_Output_File("aoi_sample01.blif");
	string command;
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
	}
	return 0;
}
