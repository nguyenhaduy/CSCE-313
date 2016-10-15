#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <vector>
using namespace std;

// Grabs all tokens
vector<string> split(string s) {
  int count = 0;
  string temp;
  vector<string> new_input;
  while(count < s.size()) {
    temp = "";
    while(count < s.size() && s[count] != ' ' && s[count] != '\"' && s[count] != '<' && s[count] != '>' && s[count] != '|' && s[count] != '&') {
      temp += s[count];
      count++;
    }
    if(s[count] == '\"') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      temp = s.substr(count, s.substr(count+1).find("\"")+2);
      count = count + 2 + (s.substr(count+1)).find("\"");
    } else if(s[count] == ' ') {
      count++;
    } else if(s[count] == '<') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = "<";
    } else if(s[count] == '>') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = ">";
    } else if(s[count] == '|') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = "|";
    } else if(s[count] == '&') {
      if(temp != "") {
        new_input.push_back(temp);
        temp = "";
      }
      count++;
      temp = "&";
    }
    if(temp != "") {
      new_input.push_back(temp);
    }
  }
  return new_input;
}

int main() {
  string input = "";
  while(true) {
    cout << "$ ";
    getline(cin, input);
    vector<string> temp = split(input);
    for(int i = 0; i < temp.size(); i++) {
      cout << "Item => " << temp[i] << endl;
    }
  }
  return 0;
}
