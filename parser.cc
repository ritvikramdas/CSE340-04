/*
 * Copyright (C) Sandeep Balaji, 2018
 *
 * Do not share this file with anyone
 */
#include "iostream"
#include "compiler.h"
#include "lexer.h"

using namespace std;
LexicalAnalyzer lexer;
struct StatementNode * parse_body();
vector<struct ValueNode*> valueNodes;
string reserve[] = { "END_OF_FILE",
    "VAR", "FOR", "IF", "WHILE", "SWITCH", "CASE", "DEFAULT", "PRINT", "ARRAY",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
    "NOTEQUAL", "GREATER", "LESS",
    "NUM", "ID", "ERROR"
};

struct ValueNode * getValueNode(string id){
    for (auto const& node: valueNodes){
        if (node->name == id){
            return node;
        }
    }
    return NULL;
}

struct ValueNode * getNum(string id){
    struct ValueNode * value = NULL;
    for (auto const& node: valueNodes){
        if (node->name == id){
            value = node;
        }
    }
    if(value == NULL){
        value = new ValueNode();
        value->name = id;
        value->value = stoi(id);
        valueNodes.push_back(value);
    }
    return value;
}

struct ValueNode* parse_primary(){
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        return getValueNode(t.lexeme);
    }
    else{
        return getNum(t.lexeme);
    }
}

void parse_case(){

}

void parse_caseList(){

}

struct IfStatement* parse_condition(){
    struct IfStatement* if_stmt = new IfStatement();
    struct ValueNode* v1 = parse_primary();
    Token t = lexer.GetToken();
    struct ValueNode* v2 = parse_primary();
    if_stmt->condition_operand1 = v1;
    if_stmt->condition_operand2 = v2;
    if (t.token_type == GREATER){
        if_stmt->condition_op = CONDITION_GREATER;
    }
    else if (t.token_type == LESS){
        if_stmt->condition_op = CONDITION_LESS;
    }
    else {
        if_stmt->condition_op = CONDITION_NOTEQUAL;
    }
    return if_stmt;
}

struct StatementNode * parse_forStmt(){
    struct StatementNode * s = new StatementNode();
    return s;
}

struct StatementNode * parse_switchStmt(){
    struct StatementNode * s = new StatementNode();
    return s;
}

struct StatementNode * parse_whileStmt(){
    struct StatementNode * s = new StatementNode();
    lexer.GetToken();
    struct IfStatement* ifStmt = parse_condition();
    return s;
}

struct StatementNode * parse_ifStmt(){
    struct StatementNode * s = new StatementNode();
    lexer.GetToken();
    struct IfStatement* ifStmt = parse_condition();
    s->type = IF_STMT;
    s->if_stmt = ifStmt;
    struct StatementNode * sb = parse_body();
    struct StatementNode * no = new StatementNode();
    no->type = NOOP_STMT;
    s->if_stmt->true_branch = sb;
    s->if_stmt->false_branch = no;
    s->next = no;
    while(sb->next != NULL){
        sb = sb->next;
    }
    sb->next = no;
    return s;
}

struct StatementNode * parse_printStmt(){
    lexer.GetToken();
    Token t = lexer.GetToken();
    struct StatementNode * s = new StatementNode();
    s->type = PRINT_STMT;
    s->print_stmt = new PrintStatement();
    s->print_stmt->id = getValueNode(t.lexeme);
    s->next = NULL;
    lexer.GetToken();
    return s;
}

struct StatementNode * parse_assignStmt(){
    Token t = lexer.GetToken();
    struct ValueNode* id = getValueNode(t.lexeme);
    struct StatementNode * s1 = new StatementNode();
    s1->type = ASSIGN_STMT;
    s1->assign_stmt = new AssignmentStatement();
    s1->assign_stmt->left_hand_side = id;
    s1->next = NULL;

    lexer.GetToken();
    struct ValueNode* value1 = parse_primary();
    Token t1 = lexer.GetToken();
    if (t1.token_type == SEMICOLON){
        s1->assign_stmt->op = OPERATOR_NONE;
        s1->assign_stmt->operand1 = value1;
        s1->assign_stmt->operand2 = NULL;
        lexer.UngetToken(t1);
    }
    else{
        struct ValueNode* value2 = parse_primary();
        if(t1.token_type == PLUS){
            s1->assign_stmt->op = OPERATOR_PLUS;
            s1->assign_stmt->operand1 = value1;
            s1->assign_stmt->operand2 = value2;
        }
        else if(t1.token_type == MINUS){
            s1->assign_stmt->op = OPERATOR_MINUS;
            s1->assign_stmt->operand1 = value1;
            s1->assign_stmt->operand2 = value2;
        }
        else if(t1.token_type == MULT){
            s1->assign_stmt->op = OPERATOR_MULT;
            s1->assign_stmt->operand1 = value1;
            s1->assign_stmt->operand2 = value2;
        }
        else{
            s1->assign_stmt->op = OPERATOR_DIV;
            s1->assign_stmt->operand1 = value1;
            s1->assign_stmt->operand2 = value2;
        }
    }
    lexer.GetToken();
    return s1;
}

struct StatementNode * parse_stmt(){
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == ID){
        return parse_assignStmt();
    }
    else if (t.token_type == IF){
        return parse_ifStmt();
    }
    else if (t.token_type == WHILE){
        return parse_whileStmt();
    }
    else if (t.token_type == SWITCH){
        return parse_switchStmt();
    }
    else if (t.token_type == FOR){
        return parse_forStmt();
    }
    else if (t.lexeme == "print"){
        return parse_printStmt();
    }
}

struct StatementNode * parse_stmtList(){
    struct StatementNode *stmt = parse_stmt();
    Token t = lexer.GetToken();
    if (t.token_type != RBRACE){
        lexer.UngetToken(t);
        struct StatementNode *next = parse_stmtList();
        if (stmt->type == IF_STMT){
            stmt->next->next = next;
        }
        else{
            stmt->next = next;
        }
    }
    return stmt;
}

struct StatementNode * parse_body(){
    lexer.GetToken();
    return parse_stmtList();
}

void parse_idList(){
    Token t = lexer.GetToken();
    struct ValueNode * A = new ValueNode();
    A->name = t.lexeme;
    A->value = 0;
    valueNodes.push_back(A);
    Token t1 = lexer.GetToken();
    if(t1.token_type ==COMMA){
        parse_idList();
    }
}

void parse_varSection(){
    parse_idList();
}

struct StatementNode * parse_program(){
    parse_varSection();
    return parse_body();
}

struct StatementNode * parse_generate_intermediate_representation(){
    return parse_program();
}
