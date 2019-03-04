#include "Expression.h"
#include <vector>
#include <set>
#include <cmath>
#include <numeric>
#include <utility>
using namespace std;
void printTransformation(const Expression& from , const Expression& to , const string method);
vector<string> Expression::inference_rules;
bool Expression::suppress_output;

void Expression::simplifyCompletely(){
    while(simplify()){} 
}

bool Expression::simplify(){
    clean();
    
    for(int i = 0; i<inference_rules.size();i++){
        if(applyInferenceRule(i)){ 
            return true; 
        }
    }
    return false;
}
bool Expression::applyInferenceRule(int i){
    
    if(directlyApplyInferenceRule(i)){
        return i;
    }
    else{
        if(Expression_type == Expression::ATOMIC){ 
            return 0;
        }
        for(int j = 0; j<operands.size();j++){
            if(operands[j]->applyInferenceRule(i)){ 
                return i; 
            }
        }
    }
    return false;
}
bool Expression::directlyApplyInferenceRule(int i){
    if(inference_rules[i] == "komplementi i dyfishte"){ 
        if(Expression_type == Expression::NOT && operands[0]->Expression_type == Expression::NOT){
            printTransformation(*this, *(operands[i]->operands[i]), Expression::inference_rules[i]);
            *this = *(operands[i]->operands[i]);
            return true;
        }
    }
    if(inference_rules[i] == "inversiAND"){
        if(Expression_type == Expression::AND){
            vector<Expression> operand_representations;
            Expression* opposite_expression;
            for(int j = 0; j < operands.size(); j++){
                operand_representations.push_back(*(operands[j]));
            }
            for(int j = 0; j < operands.size(); j++){
                Expression* temp_expr_ptr = operands[j];
                opposite_expression = new Expression();
                opposite_expression->Expression_type = Expression::NOT;
                opposite_expression->operands.push_back(temp_expr_ptr);
                if(find(operand_representations.begin(),operand_representations.end(),*(opposite_expression)) != operand_representations.end()) {
                    Expression* new_contradiction = new Expression();
                    new_contradiction->Expression_type = Expression::ATOMIC;
                    new_contradiction->symbol = '0';
                    printTransformation(*this, *new_contradiction, Expression::inference_rules[i]);
                    *this = *new_contradiction;
                    return true;
                }
            }
        }
    }
    if(inference_rules[i] == "inversiOR"){
        if(Expression_type == Expression::OR){
            vector<Expression> operand_representations;
            Expression* opposite_expression;
            for(int j = 0; j < operands.size(); j++){
                operand_representations.push_back(*(operands[j]));
            }
            for(int j = 0; j < operands.size(); j++){
                Expression* temp_expr_ptr = operands[j];
                opposite_expression = new Expression();
                opposite_expression->Expression_type = Expression::NOT;
                opposite_expression->operands.push_back(temp_expr_ptr);
                if(find(operand_representations.begin(),operand_representations.end(),*(opposite_expression)) != operand_representations.end()) {
                    Expression* new_tautology = new Expression();
                    new_tautology->Expression_type = Expression::ATOMIC;
                    new_tautology->symbol = '1';
                    printTransformation(*this, *new_tautology, Expression::inference_rules[i]);
                    *this = *new_tautology;
                    return true;
                }
            }
        }
    }
    if(inference_rules[i] == "inversiBiteve" ){
        if(Expression_type == Expression::NOT){
            if(operands[0]->is_tautology()){
                Expression* new_contradiction = new Expression();
                new_contradiction->Expression_type = Expression::ATOMIC;
                new_contradiction->symbol = '0';
                printTransformation(*this, *new_contradiction, Expression::inference_rules[i]);
                *this = *new_contradiction;
                return true;
            }
            if(operands[0]->is_contradiction()){
                Expression* new_tautology = new Expression();
                new_tautology->Expression_type = Expression::ATOMIC;
                new_tautology->symbol = '1';
                printTransformation(*this, *new_tautology, Expression::inference_rules[i]);
                *this = *new_tautology;
                return true;
            }
        }
    }
    if(inference_rules[i] == "identiteti"){ // nese ke 0 dhe ben mbledhje e heqim 0 ose nese ke 1 dhe shumezim e heqim njeshin
        if(Expression_type == Expression::OR){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_contradiction()){
                    Expression* new_expression = new Expression();
                    *new_expression = *this;
                    new_expression->operands.erase(new_expression->operands.begin()+j);
                    printTransformation(*this, *new_expression, Expression::inference_rules[i]);
                    *this = *new_expression;
                    return true;
                }
            }
        }
        if(Expression_type == Expression::AND){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_tautology()){
                    Expression* new_expression = new Expression();
                    *new_expression = *this;
                    new_expression->operands.erase(new_expression->operands.begin()+j);
                    printTransformation(*this, *new_expression, Expression::inference_rules[i]);
                    *this = *new_expression;
                    return true;
                }
            }
        }
    }
    if(inference_rules[i] == "dominanca"){ //nese eshte shumezim dhe kam 0 eshte 0, nese eshte 1 eshte 1
        if(Expression_type == Expression::AND){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_contradiction()){
                    Expression* new_expression = new Expression();
                    new_expression->Expression_type = Expression::ATOMIC;
                    new_expression->symbol = '0';
                    printTransformation(*this, *new_expression, Expression::inference_rules[i]);
                    *this = *new_expression;
                    return true;
                }
            }
        }
        if(Expression_type == Expression::OR){
            for(int j = 0; j < operands.size(); j++){
                if(operands[j]->is_tautology()){
                    Expression* new_expression = new Expression();
                    new_expression->Expression_type = Expression::ATOMIC;
                    new_expression->symbol = '1';
                    printTransformation(*this, *new_expression, Expression::inference_rules[i]);
                    *this = *new_expression;
                    return true;
                }
            }
        }
    }
    if(inference_rules[i] == "idepotence"){
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ 
            vector<Expression> operand_representations;
            Expression* duplicate_expression = NULL;
            for(int j = 0; j<operands.size();j++){
                if(find(operand_representations.begin(),operand_representations.end(),*(operands[j])) != operand_representations.end()) {
                    duplicate_expression = operands[j];
                    break;
                }
                operand_representations.push_back(*(operands[j]));
            }
            if(duplicate_expression != NULL){
                vector<Expression*> temporary_operand_list;
                Expression* new_expression = new Expression();
                *new_expression = *this;
                for(int j = 0; j<new_expression->operands.size();j++){
                    if(*(new_expression->operands[j]) == *duplicate_expression){
                        new_expression->operands.erase(new_expression->operands.begin()+j);
                        j--;
                    }
                }
                if(new_expression->operands.size() == 0){
                    *new_expression = *(duplicate_expression); 
                }
                else{
                    Expression* new_atom = new Expression();
                    new_atom = duplicate_expression;
                    new_expression->operands.push_back(new_atom);
                }
                printTransformation(*this, *new_expression, Expression::inference_rules[i]);
                *this = *new_expression;
                return true;
            }
        }
    }
    if(inference_rules[i]=="deMorgan"){
        if(Expression_type == Expression::NOT){
            if(operands[0]->Expression_type == Expression::AND){
                Expression* old_expression = new Expression();
                *old_expression = *this;
                *this = *(operands[0]);
                Expression_type = Expression::OR;
                for(int j = 0; j < operands.size(); j++){
                    Expression* temp_expr_ptr = operands[j];
                    operands[j] = new Expression();
                    operands[j]->Expression_type = Expression::NOT;
                    operands[j]->operands.push_back(temp_expr_ptr);
                }
                printTransformation(*old_expression, *this, Expression::inference_rules[i]);
                return true;
            }
            if(operands[0]->Expression_type == Expression::OR){
                Expression* old_expression = new Expression();
                *old_expression = *this;
                *this = *(operands[0]);
                Expression_type = Expression::AND;
                for(int j = 0; j < operands.size(); j++){
                    Expression* temp_expr_ptr = operands[j];
                    operands[j] = new Expression();
                    operands[j]->Expression_type = Expression::NOT;
                    operands[j]->operands.push_back(temp_expr_ptr);
                }
                printTransformation(*old_expression, *this, Expression::inference_rules[i]);
                return true;
            }
        }
    }
    if(inference_rules[i]== "perthithje"){ //A*(A+B) = A; A+(A*B) = A
        bool found = false;
        Expression* old_expression = new Expression();
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){ 
            vector<Expression> operand_representations;
            for(int j = 0; j<operands.size();j++){
                operand_representations.push_back(*(operands[j])); 
            }
            for(int j = 0; j<operands.size();j++){
                if((Expression_type == Expression::AND && operands[j]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[j]->Expression_type == Expression::AND)){
                    
                    for(int k = 0; k < operands[j]->operands.size(); k++){
                        if(find(operand_representations.begin(),
                         operand_representations.end(), *(operands[j]->operands[k])) != operand_representations.end()) {
                            *old_expression = *this;
                          
                            operands.erase(operands.begin()+j); 
                            j--; 
                            found = true;
                            break;
                        }
                    }
                }
            }
            if(operands.size() == 1){
                *this = *(operands[0]); 
            }
            if(found){
                printTransformation(*old_expression, *this, Expression::inference_rules[i]);
                return true;
            }
        }
    }
    if(inference_rules[i]=="shperndarja"){ //TODO A*(A+B) = A*A+A*B; A+(B*C) = A+B * A+C
        bool accomplished_something = false;
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){
            vector<vector<Expression*> > outer_expressions; 
            for(int j = 0; j<operands.size();j++){
                vector<Expression*> inner_expressions; 
                if((Expression_type == Expression::AND && operands[j]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[j]->Expression_type == Expression::AND)){
                    for(int k = 0; k < operands[j]->operands.size(); k++){
                        inner_expressions.push_back(operands[j]->operands[k]);
                        accomplished_something = true;
                    }

                }
                else{
                    inner_expressions.push_back(operands[j]);
                }
                outer_expressions.push_back(inner_expressions);
            }
            if(accomplished_something){
                Expression* new_outer_expression = new Expression(); 
                if(Expression_type == Expression::AND){
                    new_outer_expression->Expression_type = Expression::OR;
                }
                else{
                    new_outer_expression->Expression_type = Expression::AND;
                }
                vector<Expression*> new_inner_expressions; // *a|c* and *b|c*
                int max_index = 1;
                for(int j = 0; j < outer_expressions.size(); j++){
                    max_index *= outer_expressions[j].size();
                }
                for(int j = 0; j < max_index; j++){
                    Expression* new_inner_expression = new Expression();
                    if(Expression_type == Expression::AND){
                        new_inner_expression->Expression_type = Expression::AND;
                    }
                    else{
                        new_inner_expression->Expression_type = Expression::OR;
                    }      
                    for(int k = 0; k < outer_expressions.size(); k++){
                        int num_choices = outer_expressions[k].size();
                        int change_interval = 1;
                        for(int l = k+1; l < outer_expressions.size(); l++){
                            change_interval *= outer_expressions[l].size();
                        }
                        Expression* new_exp = new Expression();
                        *new_exp = *(outer_expressions[k][(int)(floor(j/change_interval))%num_choices]);
                        new_inner_expression->operands.push_back(new_exp);
                    }
                    new_inner_expressions.push_back(new_inner_expression);
                }
                for(int k = 0; k < new_inner_expressions.size(); k++){
                    new_outer_expression->operands.push_back(new_inner_expressions[k]);
                }
                printTransformation(*this, *new_outer_expression, Expression::inference_rules[i]);
                *this = *new_outer_expression;
                return true;
            }
        }
    }
    if(inference_rules[i]=="reduktim"){ //TODO A+!A*B = A+B ; A*(!A+B) = A*B
        bool accomplished_something = false;
        if(Expression_type == Expression::AND || Expression_type == Expression::OR){
            vector<Expression> operand_representations;
            for(int j = 0; j<operands.size();j++){
                operand_representations.push_back(*(operands[j]));
            }
            Expression* old_expression = new Expression();
            for(int j = 0; j<operands.size();j++){
                if((Expression_type == Expression::AND && operands[j]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[j]->Expression_type == Expression::AND)){
                   
                    for(int k = 0; k < operands[j]->operands.size(); k++){ 
                        
                        Expression* negation_wrapper = new Expression();
                        if(operands[j]->operands[k]->Expression_type!= Expression::NOT){
                            negation_wrapper->Expression_type = Expression::NOT;
                            negation_wrapper->operands.push_back(operands[j]->operands[k]);
                        }
                        else{
                            negation_wrapper = operands[j]->operands[k]->operands[0];
                        }
                        if(find(operand_representations.begin(), operand_representations.end(), 
                            *(negation_wrapper)) != operand_representations.end()) {                            
                            *old_expression = *this;
                           
                            operands[j]->operands.erase(operands[j]->operands.begin()+k);
                            if(operands[j]->operands.size() == 1){
                                *(operands[j]) = *(operands[j]->operands[0]);
                            } 
                            accomplished_something = true;
                            goto finished;
                        }
                    }
                }
            }
            finished:
            if(accomplished_something){
                printTransformation(*old_expression, *this, Expression::inference_rules[i]);
                return true;
            }
        }
    }
    return false;
}
void Expression::clean(){
    for(int i = 0; i < operands.size(); i++){
        operands[i]->clean();
    }
    vector<Expression*> temporary_operand_list;
    if(Expression_type == Expression::AND || Expression_type == Expression::OR){
        for(int i = 0; i < operands.size(); i++){
            if(operands[i]->Expression_type == Expression_type){
                while(operands[i]->operands.size()>0){
                    temporary_operand_list.push_back(operands[i]->operands.front());
                    operands[i]->operands.erase(operands[i]->operands.begin());
                }
                operands.erase(operands.begin()+i);
                i--;
            }
        }
    }
    for(int i = 0; i < temporary_operand_list.size(); i++){
        operands.push_back(temporary_operand_list[i]);
    }
    if(Expression_type == Expression::AND || Expression_type == Expression::OR){
        if(operands.size()==1){
            *this = *(operands[0]);
        }
    }
}

pair<int,int> Expression::getOperandsWithMatches() const{
    if(Expression_type==Expression::AND || Expression_type==Expression::OR){
        for(int i = 0; i < operands.size(); i++){
            if((Expression_type == Expression::AND && operands[i]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[i]->Expression_type == Expression::AND)){
                for(int j = 0; j < operands[i]->operands.size(); j++){
                    for(int k = i+1; k < operands.size(); k++){
                        if((Expression_type == Expression::AND && operands[k]->Expression_type == Expression::OR)||
                    (Expression_type == Expression::OR && operands[k]->Expression_type == Expression::AND)){
                            for(int l = 0; l < operands[k]->operands.size(); l++){
                                if(*(operands[i]->operands[j]) == *(operands[k]->operands[l]) ){
                                    return make_pair(i,k);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return make_pair(-1,-1);
}


string Expression::getExpressionMathematically() const{
    string retStr = "";
    if(!is_atomic_symbol()){
        if(Expression_type == Expression::NOT){
            retStr += "NOT(";
        }
        if(Expression_type == Expression::AND){
            retStr += "AND(";
        }
        if(Expression_type == Expression::OR){
            retStr += "OR(";
        }
        retStr += operands[0]->getExpressionMathematically();
        for(int i = 1; i < operands.size(); i++){
            retStr += "," + operands[i]->getExpressionMathematically();
        }
        retStr += ")";
    }
    else{
        retStr += symbol;
    }
    return retStr;
}
void Expression::printExpressionMathematically() const{
    cout << getExpressionMathematically() << endl;
}
string Expression::getExpressionHumanReadable() const{
    string retStr = "";
    if(Expression_type == Expression::NOT){
        retStr += "!(";
        retStr += operands[0]->getExpressionHumanReadable();
        retStr += ")";
    }
    if(Expression_type == Expression::AND){
        retStr += "(";
        retStr += operands[0]->getExpressionHumanReadable();
        for(int i = 1; i<operands.size();i++){
            retStr += "* " + operands[i]->getExpressionHumanReadable();
        }
        retStr += ")";
    }
    if(Expression_type == Expression::OR){
        retStr += "(";
        retStr += operands[0]->getExpressionHumanReadable();
        for(int i = 1; i < operands.size(); i++){
            retStr += "+ " + operands[i]->getExpressionHumanReadable();
        }
        retStr += ")";
    }
    if(Expression_type == Expression::ATOMIC){
        if(is_contradiction()){
            retStr+= "0";
        }
        else if(is_tautology()){
            retStr+= "1";
        }
        else{
            retStr += symbol;
        }
    }
    return retStr;
}
void Expression::printExpressionHumanReadable() const{
    cout <<getExpressionHumanReadable() << endl;
}
void printTransformation(const Expression& from , const Expression& to , const string method){
    if(from.suppress_output){
        return;
    }
    string method_cpy = method;
    std::replace(method_cpy.begin(), method_cpy.end(), '_', ' ');
    cout << "Me "<< method_cpy << endl << endl;

}
