#include <iostream>
#include <cstdlib>
#include <vector>
#include <sys/time.h>
#include <algorithm>
#include <stdio.h>
#include <ctype.h>
#include "Expression.cpp"
using namespace std;
Expression* parseBooleanExpression(string booleanExpression){ 
    booleanExpression.erase(std::remove_if(booleanExpression.begin(), booleanExpression.end(), ::isspace), booleanExpression.end());
    Expression* mainConnective = new Expression();
    static const char disjunction_arr[] = {'|','+'};
    vector<char> disjunctions(disjunction_arr, disjunction_arr + sizeof(disjunction_arr) / sizeof(disjunction_arr[0]) );
    static const char conjunction_arr[] = {'^','&','*'};
    vector<char> conjunctions(conjunction_arr, conjunction_arr + sizeof(conjunction_arr) / sizeof(conjunction_arr[0]) );
    static const char negation_arr[] = {'~','!','-'};
    vector<char> negations(negation_arr, negation_arr + sizeof(negation_arr) / sizeof(negation_arr[0]) );
    
    int depth = 0; 
    bool depth_reached_zero = false;
    while(booleanExpression[0] == '(' && booleanExpression[booleanExpression.length()-1]==')' && !depth_reached_zero){
        depth = 1;
        depth_reached_zero = false;
        for(int i = 1; i < booleanExpression.length()-2;i++){
            if(booleanExpression[i] == '('){ 
                depth++;
            }
            if(booleanExpression[i] == ')'){ 
                depth--;
            }
            if(depth == 0){
                depth_reached_zero = true;
            }
        }
        if(!depth_reached_zero){
            booleanExpression = booleanExpression.substr(1,booleanExpression.length()-2);
        }
    }
    depth = 0;
    
    if(booleanExpression.length()==1){ 
        mainConnective->symbol = booleanExpression[0];
        return mainConnective;
    }
    for(int i = 0; i < booleanExpression.length();i++){
        if(depth == 0){
            if(find(conjunctions.begin(),conjunctions.end(),booleanExpression[i]) != conjunctions.end()){
                mainConnective->Expression_type = Expression::AND;
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(0,i))); 
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(i+1,booleanExpression.length())));
                return mainConnective;
            }
            if(find(disjunctions.begin(),disjunctions.end(),booleanExpression[i]) != disjunctions.end()){
                mainConnective->Expression_type = Expression::OR;
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(0,i))); 
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(i+1,booleanExpression.length())));
                return mainConnective;
            }
        }
        if(booleanExpression[i] == '('){ 
            depth++;
        }
        if(booleanExpression[i] == ')'){ 
            depth--;
        }
    }
    depth = 0;
    for(int i = 0; i< booleanExpression.length();i++){
        if(depth == 0){
            if(find(negations.begin(),negations.end(),booleanExpression[i]) != negations.end()){
                mainConnective->Expression_type = Expression::NOT;
                mainConnective->operands.push_back(parseBooleanExpression(booleanExpression.substr(i+1,booleanExpression.length())));
               
                return mainConnective;
            }
        }
        if(booleanExpression[i] == '('){ 
            depth++;
        }
        if(booleanExpression[i] == ')'){ 
            depth--;
        }
    }
    cerr << "Error: invalid function syntax" << endl;
    exit(1);
}
int main(int argc, char* argv[]){
    string booleanExpression;
    if(argc == 2){
        booleanExpression = argv[1];
    }
    else{
        cin >> booleanExpression;
    }
    Expression* outerExpression = parseBooleanExpression(booleanExpression);
    outerExpression->clean();
    cout << "Thjeshtimi i " << endl;
    outerExpression->printExpressionHumanReadable();
    while(outerExpression->simplify()){
        outerExpression->clean();
        outerExpression->printExpressionHumanReadable();
    }
    cout << "Shprehja perfundimtare: " << outerExpression->getExpressionHumanReadable() << endl;

    return 0;
}
