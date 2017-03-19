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

#include "util.h"

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

vector<Cable> cables;
vector<Table> tables;

void addCable(int r1, int i1, int r2, int i2, int cost) {
  Cable c(r1, i1, r2, i2, cost);
  cables.push_back(c);
}

void prntTable(int r) {
  int i;
  for (i = 0; i < N_ROUTERS; ++i) {
    if (tables[i].num == r) {
      break;
    }
  }
  Table t = tables[i];
  cout << "\n\t---- Table for Router " << r << ": ----\n";
  for (i = 0; i < N_ROUTERS; ++i) {
    int* l = t.getConn(i);
    cout << "To router " << i << ": use interface " << ((l[0] == -1) ? "(Local)\t" : ((l[0] == -2) ? "None\t\t" : ""));
    if (l[0] != -1 && l[0] != -2) {
      cout << l[0] << "\t\t";
    }
    cout << "at cost " << ((l[1] == 999) ? "(Infinite)" : "");
    if (l[1] != 999) {
      cout << l[1];
    }
    cout << endl;
  }
  cout << endl;
}

int* findCable(int r1, int r2) {
  int i;
  int* l;
  for (i = 0; i < N_CABLES; ++i) {
    if (l = cables[i].getLnk(r1, r2)) {
      break;
    }
  }
  return l;
}

// Display the error message and exit
void err(string msg) {
  cerr << msg;
  exit(1);
}

void sendTo(int r, char *msg) {
  int sock, port = 50007;
  struct sockaddr_in host;
  const char *server = ((r == 0) ? "128.235.208.201" : ((r == 1) ? "128.235.208.225" : ((r == 2) ? "128.235.209.204" : "128.235.209.205")));

  // Open the socket
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    err("Socket Opening Failed.\n");
  }
  // Attach the address and port
  host.sin_family = AF_INET;
  host.sin_addr.s_addr = inet_addr(server);
  host.sin_port = htons(port);

  // Connect to the server socket
  cout << "| Connecting to server: " << server << endl;

  if (connect(sock, (struct sockaddr *) &host, sizeof(host)) < 0) {
    err("Socket Connection Failed.\n");
  }
  cout << "| Server connected\n| Sending my Router Table to Router: " << r << endl;

  // Send the router info to server
  write(sock, msg, strlen(msg));

  // Close the socket connection
  close(sock);
  cout << "| Server Disconnected\n";
}

void sendUpdt(int r, int x) {
  int i;
  char msg[1024];
  stringstream buf;

  buf << r;
  for (i = 0; i < N_ROUTERS; ++i) {
    buf << "," << tables[r].table[i][2];
  }
  strcpy(msg, buf.str().c_str());

  for (i = 0; i < N_ROUTERS; ++i) {
    if (i == r || !findCable(r, i)) continue;
    sendTo(i, msg);
  }
}
