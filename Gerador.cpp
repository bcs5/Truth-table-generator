#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#define fr(a,b,c) for(int a = b, _ = c; a < _; a++)
#define X first
#define Y second
using namespace std;
typedef pair < int, string > pis;
typedef vector < pis > vpis;
typedef struct node{
	int key;
	char oper;
	struct node *l, *r;
} Node;

char varOrder[] = {'x', 'y', 'z', 't'};

bool variable[4];
int variableKey[4];
bool u[10000];
bool taken[10000];
int same[10000];
int key = 0;
vpis variables, sexpr;

Node *createNode(int key){
	Node *s = new Node;
	s->key = key;
	s->l = s->r = NULL;
	return s;
}

int getIdxByVar(char c){
	fr(i, 0, 4) if(varOrder[i] == c) return i;
}

char getVarByIdx(int idx){
	return varOrder[idx];
}

Node *subexpr(string s){
	if(s.length() == 1){
		Node *node = createNode(0);
		int v = getIdxByVar(s[0]);
		if(!variable[v]){
			variables.push_back(pis(key, s));
			variableKey[v] = key;
			variable[v] = true;
			node->key = key;
			key++;
		}else{
			node->key = variableKey[v];
		}
		return node;
	}
	Node *node = createNode(key++);
	sexpr.push_back(pis(node->key, s));
	if(s[0] == '(' && s[1] != '-' && s.back() == ')'){
		string a, b;
		char oper;
		int k = 0;
		fr(i, 1, s.length() - 1){
			if(s[i] == '(') k++;
			if(s[i] == ')') k--;
			if(k == 0){
				oper = s[i + 1];
				a = s.substr(1, i);
				b = s.substr(i + 2);
				b.pop_back();
				break;
			}
		}
		node->oper = oper;
		node->l = subexpr(a);
		node->r = subexpr(b);
	}else{
		if (s[0] == '('){
			s = s.substr(1);
			s.pop_back();
		}
		if (s[0] == '-'){
			node->oper = '-';
			node->l = subexpr(s.substr(1));
		}
	}
	return node;
}

bool val(Node * node){
	bool res;
	int sKey = same[node->key];
	if(taken[sKey]) return u[node->key] = u[sKey];
	if(node->oper != '-'){
		bool av = val(node->l);
		bool bv = val(node->r);
		if (node->oper == '.') res = (av && bv);
		if (node->oper == '+') res = (av || bv);
		if (node->oper == '>') res = (!av || bv);
	}else if(node->oper == '-'){
		res = !val(node->l);
	}
	u[sKey] = res;
	taken[sKey] = 1;
	return u[node->key] = res;
}

void destroy(Node *node)
{
	if (!node) return;
	destroy(node->l);
	destroy(node->r);
	free(node);
}

int partition(vpis &A, int start, int end){
	int i = start + 1;
	pis piv = A[start];
	fr(j, start + 1, end + 1){
		if(A[j].Y.length() < piv.Y.length() || (A[j].Y.length() == piv.Y.length() && A[j].Y < piv.Y)){
			swap(A[i], A[j]);
			i++;
		}
	}
	swap(A[start], A[i - 1]);
	return i - 1;
}

int rand_partition(vpis &A, int start, int end){
	srand (time(NULL));
	int random = start + rand() % (end - start + 1);
	swap(A[random], A[start]);
	return partition(A, start, end);
}

void quick_sort(vpis &A, int start, int end){
	if (start < end){
		int piv_pos = rand_partition(A, start, end);
		quick_sort(A, start, piv_pos - 1);
		quick_sort(A, piv_pos + 1, end);
	}
}

void normalize(){
	if(sexpr.empty()) return;
	vpis A;
	sexpr.swap(A);
	sexpr.push_back(A[0]);
	same[A[0].X] = A[0].X;
	fr(i, 1, A.size()){
		if(A[i].Y != A[i - 1].Y){
			sexpr.push_back(A[i]);
			same[A[i].X] = A[i].X;
		}else{
			same[A[i].X] = sexpr.back().X;
		}
	}
}

void sortVariables(){
	vpis A;
	variables.swap(A);
	fr(i, 0, 4){
		fr(j, 0, A.size()){
			if(A[j].Y[0] == varOrder[i]){
				variables.push_back(A[j]);
				same[A[j].X] = A[j].X;
			}
		}
	}
}

void ws(int n){
	while (n-- > 0) cout << ' ';
}

void nl(){
	cout << "-";
	fr(i, 0, variables.size()) cout << '-' << "-";
	fr(i, 0, sexpr.size()){
		fr(j, 0, sexpr[i].Y.length()) cout << '-';
		cout << "-";
	}
	cout << endl;
}

int main(){
	freopen("Expressoes.in", "r", stdin);
	freopen("Expressoes.out", "w", stdout);
	int n, t = 0;
	string expr;
	Node *tree = NULL;
	bool sat, tautology;
	cin >> n;
	while(n--){
		cin >> expr;
		key = 0;
		tree = subexpr(expr);
		quick_sort(sexpr, 0, sexpr.size() - 1);
		normalize();
		sortVariables();
		cout << "Tabela #" << ++t << endl;
		nl();
		cout << "|";
		fr(i, 0, variables.size()){
			cout << variables[i].Y << "|";
		}
		fr(i, 0, sexpr.size()){
			cout << sexpr[i].Y << "|";
		}
		cout << endl;
		nl();
		sat = false;
		tautology = true;
		fr(i, 0, 1 << variables.size()){
			cout << "|";
			memset(taken, 0, sizeof taken);
			fr(j, 0, variables.size()){
				int k = variables.size() - 1 - j;
				u[variables[k].X] = i & (1 << j);
				taken[variables[k].X]  = 1;
			}
			fr(j, 0, variables.size()){
				cout << u[variables[j].X] << "|";
			}
			bool res = val(tree);
			sat |= res;
			tautology &= res;
			fr(j, 0, sexpr.size()){
				ws(sexpr[j].Y.length() - 1);
				cout << u[sexpr[j].X] << "|";
			}
			cout << endl;
			nl();
		}
		if(sat){
			if(tautology){
				cout << "satisfativel e tautologia" << endl;
			}else{
				cout << "satisfativel e refutavel" << endl;
			}
		}else{
			cout << "insatisfativel e refutavel" << endl;
		}
		cout << endl;
		destroy(tree);
		tree = NULL;
		sexpr.clear();
		variables.clear();
		memset(variable, 0, sizeof variable);
	}
	return 0;
}
