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

#include "../util/util.h"

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
#define MY_NUM 0

extern vector<Cable> cables;
extern vector<Table> tables;

void startUp() {
  int i, j, k;

  cout << "| Initializing Router Tables\n";

  for (i = 0; i < N_ROUTERS; ++i) {
    Table t(i, N_ROUTERS);
    tables.push_back(t);
  }
  
  cout << "| Initializing Router Cables and Interfaces\n";

  addCable(0, 0, 1, 2, 1);
  addCable(0, 1, 2, 2, 3);
  addCable(0, 2, 3, 0, 7);
  addCable(1, 0, 2, 0, 1);
  addCable(2, 1, 3, 2, 2);

  cout << "| Initial Table for Router: " << MY_NUM << endl;

  prntTable(MY_NUM);

  cout << "| Calculating the links costs for Router: " << MY_NUM << endl;

  Table t = tables[MY_NUM];
  for (i = 0; i < N_ROUTERS; ++i) {
    if (i == MY_NUM) continue;
    int* c = findCable(MY_NUM, i);
    if (c == 0) continue;
    t.table[i][1] = c[0];
    t.table[i][2] = c[1];
  }
  t.updt = true;
  
  cout << "| Updated Table for Rotuer: " << MY_NUM << endl;

  prntTable(MY_NUM);

  cout << "| Startup done\n| Sending updates ..." << endl;
  sendUpdt(MY_NUM, 99);
}

void process(int next, int* lst) {
  cout << "| Checking for updates using Table info of Router: " << next << endl;

  vector<int*> t = tables[MY_NUM].table;
  int i, j, cost, toNext = t[next][2];
  int link = findCable(MY_NUM, next)[0];
  bool updt = false;

  for (i = 0; i < N_ROUTERS; ++i) {
    if (i == MY_NUM) continue;
    cost = toNext + lst[i];
    if (cost >= t[i][2]) continue;
    t[i][1] = link;
    t[i][2] = cost;
    tables[MY_NUM].updt = true;
    updt = true;
  }

  cout << "| " << ((!updt) ? "No Updates" : "Table Updated") << endl;;
  prntTable(MY_NUM);

  if (!updt) return;

  cout << "| Sending Updates ...\n";
  sendUpdt(MY_NUM, next);
}

int main() {
  string s;
  int c = 0, i, sock, newSock, port = 50007, next, res[N_ROUTERS];
  struct sockaddr_in host, client;
  socklen_t len;
  char buffer[1024];

  // Open the socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    err("Socket Opening Failed.\n");
  }
  // Attach the address and port
  host.sin_family = AF_INET;
  host.sin_addr.s_addr = INADDR_ANY;
  host.sin_port = htons(port);

  // Bind the socket
  if (bind(sock, (struct sockaddr *) &host, sizeof(host)) < 0) {
    err("Socket Binding Failed.\n");
  }
  cout << "| Server waiting ...\n";

  // Listen for connection
  listen(sock, 2);

  cin >> s;

  startUp();

  // Loop
  while (1) {
    len = sizeof(client);
    // Accept the new socket connection from client
    newSock = accept(sock, (struct sockaddr *) &client, &len);
    
    // Print the IP address of the client
    cout << "| Client connected: " << inet_ntoa(client.sin_addr) << "\n| Sending My Router Table: " << MY_NUM << endl;
    
    // Read the router info from client
    buffer[read(newSock, buffer, 1023)] = '\0';
    cout << "| Data from Client: " << buffer << endl;

    next = atoi(strtok(buffer, ","));
    for (i = 0; i < N_ROUTERS; ++i) {
      res[i] = atoi(strtok(0, ","));
    }

    process(next, res);

    // Close new socket connections
    close(newSock);
    cout << "| Client Disconnected\n";
    ++c;
    //cout << "| *************************** " << c << endl;
    //break;
  }

  // Close all socket connections
  close(sock);
  cout << "| Server Connection Closed\n";
  return 0;
}
