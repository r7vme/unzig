Root <- skip TopLevelDeclarations eof

# *** Top level ***
TopLevelDeclarations <- KEYWORD_pub? TopLevelDecl TopLevelDeclarations*
TopLevelDecl <- FnProto (SEMICOLON / Block)
             / VarDecl
FnProto <- KEYWORD_fn IDENTIFIER? LPAREN RPAREN TypeExpr
VarDecl <- KEYWORD_var IDENTIFIER COLON TypeExpr (EQUAL Expr)? SEMICOLON

# *** Block Level ***
Block <- LBRACE Statement* RBRACE
Statement <- VarDecl
          / AssignSt SEMICOLON
          / KEYWORD_return Expr? SEMICOLON
AssignSt <- Expr (AssignOp Expr)?

# *** Expression Level ***
Expr <- PrimaryExpr (BinaryOp PrimaryExpr)*
PrimaryExpr <- GroupedExpr
            / FnCallExpr
            / VarExpr
            / NumberExpr
GroupedExpr <- LPAREN Expr RPAREN
FnCallExpr <- IDENTIFIER LPAREN RPAREN
VarExpr <- IDENTIFIER
NumberExpr <- FLOAT
           / INTEGER
TypeExpr <- IDENTIFIER

# Operators
AssignOp <- EQUAL
BinaryOp <- PLUS / MINUS / ASTERISK / SLASH

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
