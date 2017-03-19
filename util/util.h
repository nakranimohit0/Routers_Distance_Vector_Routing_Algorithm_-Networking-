/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  Mohit Nakrani
  CS 356-004
  Prof. Blank
  Programming Assignment 3: Routers

  Souces used: My code is based on the client/server socket samples used in the site: http://www.linuxhowtos.org/C_C++/socket.htm
  To fetch/insert IP address, I used the manual: http://beej.us/guide/bgnet/output/html/multipage/inet_ntoaman.html
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

#define N_ROUTERS 4
#define N_INTERFACES 3
#define N_CABLES 5
//#define MY_NUM 1

class Cable {
 public:
  int r1, i1, r2, i2, cost;

  Cable(int rout1, int inter1, int rout2, int inter2, int c) {
    r1 = rout1;
    i1 = inter1;
    r2 = rout2;
    i2 = inter2;
    cost = c;
  }

  int* getLnk(int from, int to) {
    if (r1 == from && r2 == to) {
      return new int[2] {i1, cost};
    }
    if (r2 == from && r1 == to) {
      return new int[2] {i2, cost};
    }
    return 0;
  }
};

class Table {
 public:
  int num;
  bool updt;
  vector<int*> table;

  Table(int n, int total) {
    num = n;
    updt = false;
    
    for (int i = 0; i < total; ++i) {
      table.push_back(new int[3] {i, (i == n) ? -1 : -2, (i == n) ? 0 : 999});
    }
  }

  int* getConn(int dest) {
    for (int i = 0; i < N_ROUTERS; ++i) {
      if (table[i][0] == dest) {
        return new int[2] {table[i][1], table[i][2]};
      }
    }
    return 0;
  }
};

void addCable(int r1, int i1, int r2, int i2, int cost);
void prntTable(int r);
int* findCable(int r1, int r2);
void err(string msg);
void process(int next, int* lst);
void sendTo(int r, char *msg);
void sendUpdt(int r, int x);
