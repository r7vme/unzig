Root <- skip TopLevelDeclarations eof

# *** Top level ***
TopLevelDeclarations
    <- KEYWORD_pub? TopLevelDecl TopLevelDeclarations
     /

TopLevelDecl
    <- FnProto (SEMICOLON / Block)
     / VarDecl

FnProto <- KEYWORD_fn IDENTIFIER? LPAREN RPAREN TypeExpr

VarDecl <- KEYWORD_var IDENTIFIER (COLON TypeExpr)? (EQUAL Expr)? SEMICOLON

# *** Block Level ***
Statement
    <- VarDecl
     / AssignExpr SEMICOLON

# *** Expression Level ***
AssignExpr <- Expr (AssignOp Expr)?

Expr <- AdditionExpr

AdditionExpr <- MultiplyExpr (AdditionOp MultiplyExpr)*

MultiplyExpr <- PrimaryExpr (MultiplyOp PrimaryExpr)*

PrimaryExpr
    <- KEYWORD_return Expr?
     / Block
     / TypeExpr FnCallArguments*

Block <- LBRACE Statement* RBRACE

TypeExpr
    <- FLOAT
     / FnProto
     / GroupedExpr
     / IDENTIFIER
     / INTEGER

GroupedExpr <- LPAREN Expr RPAREN

# Operators
AssignOp <- EQUAL

AdditionOp
    <- PLUS
     / MINUS

MultiplyOp
    <- ASTERISK
     / SLASH

FnCallArguments <- LPAREN RPAREN

# *** Tokens ***
eof <- !.
dec <- [0-9]

dec_int <- dec*

skip <- [ \n]*

FLOAT <- dec_int "." dec_int skip
INTEGER <- dec_int skip
IDENTIFIER
    <- !keyword [A-Za-z_] [A-Za-z0-9_]* skip


ASTERISK             <- '*'      ![*%=]    skip
COLON                <- ':'                skip
EQUAL                <- '='      ![>=]     skip
LBRACE               <- '{'                skip
LPAREN               <- '('                skip
MINUS                <- '-'      ![%=>]    skip
PLUS                 <- '+'      ![%+=]    skip
RBRACE               <- '}'                skip
RPAREN               <- ')'                skip
SEMICOLON            <- ';'                skip
SLASH                <- '/'      ![=]      skip

end_of_word <- ![a-zA-Z0-9_] skip
KEYWORD_fn          <- 'fn'          end_of_word
KEYWORD_pub         <- 'pub'         end_of_word
KEYWORD_return      <- 'return'      end_of_word
KEYWORD_var         <- 'var'         end_of_word

keyword <- KEYWORD_fn / KEYWORD_pub / KEYWORD_return / KEYWORD_var