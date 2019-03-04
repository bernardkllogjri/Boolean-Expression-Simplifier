#include <vector>
#include <set>
#include <utility>
#include <algorithm>
using namespace std;
struct ExpressionComparator;
class Expression{
public:

    static vector<string> inference_rules; 
    static bool suppress_output; 
    vector<Expression*> operands; 
    char symbol; 
    enum Expression_types{ 
        OR, AND, NOT, IMPLICATION, BICONDITIONAL, ATOMIC
    };
    int Expression_type;

    Expression(){ 
        if (Expression::inference_rules.size() == 0) {
            Expression::inference_rules.push_back("komplementi i dyfishte");
            Expression::inference_rules.push_back("identieti");
            Expression::inference_rules.push_back("dominanca");
            Expression::inference_rules.push_back("inversiAND");
            Expression::inference_rules.push_back("inversiOR");
            Expression::inference_rules.push_back("inversiBiteve");
            Expression::inference_rules.push_back("idepotence");
            Expression::inference_rules.push_back("deMorgan");
            Expression::inference_rules.push_back("perthithje");
            Expression::inference_rules.push_back("reduktim");
//	        Expression::inference_rules.push_back("shperndarja");

            Expression::suppress_output = false;
        }
        Expression_type = ATOMIC;
        symbol = '\0'; 
    }
    
    Expression(const Expression& existingExpression)
    {
        operands.clear();
        for(int i = 0; i < existingExpression.operands.size(); i++){
            Expression* new_exp = new Expression();
            *new_exp = *(existingExpression.operands[i]);
            operands.push_back(new_exp);
        }
        symbol = existingExpression.symbol;
        Expression_type = existingExpression.Expression_type;
    }
 
    Expression& operator= (const Expression& existingExpression){
        operands.clear();
        for(int i = 0; i < existingExpression.operands.size(); i++){
            Expression* new_exp = new Expression();
            *new_exp = *(existingExpression.operands[i]);
            operands.push_back(new_exp);
        }
        symbol = existingExpression.symbol;
        Expression_type = existingExpression.Expression_type;
        return *this;
    }
    bool is_atomic_symbol() const
    {
        return (operands.size()==0);
    }
    void clean();
    void simplifyCompletely();
    bool simplify();
    bool applyInferenceRule(int n); 
    bool directlyApplyInferenceRule(int i);
    pair<int,int> getOperandsWithMatches() const;
    string getExpressionHumanReadable() const;
    void printExpressionHumanReadable() const;
    string getExpressionMathematically() const;
    void printExpressionMathematically() const;
    bool is_tautology() const{ return(symbol == '1'); }
    bool is_contradiction() const{ return(symbol == '0'); }
    bool operator<(const Expression& other) const;
    bool operator>(const Expression& other){return !(*(this) < other);}
};

inline bool operator==(const Expression& lhs, const Expression& rhs)
{
    if(lhs.operands.size() != rhs.operands.size()){
        return false; 
    }
    if(lhs.operands.size() == 0){
        return (lhs.symbol == rhs.symbol);
    }
    if(lhs.Expression_type != rhs.Expression_type){
        return false; 
    }
    vector<Expression*> unused_rhs_ops = rhs.operands;
    bool has_match = false;
    for(int i = 0; i < lhs.operands.size(); i++){
        has_match = false;
        for(int j = 0; j < unused_rhs_ops.size(); j++){
            if(*(lhs.operands[i]) == *(unused_rhs_ops[j])){
                has_match = true;
                unused_rhs_ops.erase(unused_rhs_ops.begin()+j);
                j--;
            }
        }
        if(!has_match){
            return false;
        }
    }
    return true;
}
inline bool operator!=(const Expression& lhs, const Expression& rhs)
{
    return !(operator == (lhs,rhs));
}
