#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "m1.h"
#include "m2.h"
#include "streetDB.h"
using namespace std;

void input_map();
void select(int selection);

int main() {
    input_map();
    draw_map();
    std::cout << "Closing map\n";

    close_map();

    return 0;
}

void input_map() {
    bool load_success = true;
    string map_path;
    string selection;
    string map_name;
    cout<<"List of Maps: "<<endl;
    cout<<"1 toronto_canada"<<endl<<"2 cairo_egypt"<<endl<<"3 beijing china"<<endl<<"4 tehran_iran"<<endl<<
            "5 iceland"<<endl<<"6 rio-de-janeiro_brazil"<<endl<<"7 golden-horseshoe_canada"<<endl<<
            "8 new-delhi_india"<<endl<<"9 new-york_usa"<<endl<<"10 tokyo_japan"<<endl<<"11 london_england"<<endl;
    cout << "Please Enter the Number of Map: ";
    getline(cin,selection);
    map_select(selection);
}

