# Adapted from https://github.com/ziglang/zig-spec
# which is distributed under MIT License Copyright (c) 2018 Zig Programming Language
Root <- skip TopLevelDeclarations eof

# Top level
TopLevelDeclarations <- TopLevelDecl TopLevelDeclarations*
TopLevelDecl <- FnDef
             / VarDecl

FnDef <- KEYWORD_fn IDENTIFIER LPAREN FnParamList RPAREN TypeExpr Block

VarDecl <- KEYWORD_var IDENTIFIER COLON TypeExpr (AssignOp Expr)? SEMICOLON

# Block Level
Block <- LBRACE Statement* RBRACE
Statement <- VarDecl
          / IfSt
          / AssignSt
          / ReturnSt
          / Block
IfSt <- KEYWORD_if LPAREN Expr RPAREN Block ( KEYWORD_else Statement )?
AssignSt <- IDENTIFIER AssignOp Expr SEMICOLON
ReturnSt <- KEYWORD_return Expr? SEMICOLON

# Expression Level
Expr <- OrExpr
OrExpr <- AndExpr (KEYWORD_or AndExpr)*
AndExpr <- CompareExpr (KEYWORD_and CompareExpr)*
CompareExpr <- BinaryExpr (CompareOp BinaryExpr)?
BinaryExpr <- PrefixExpr BinOpRhsExpr
BinOpRhsExpr <- (BinOp PrefixExpr)*
PrefixExpr <- PrefixOp* PrimaryExpr
PrimaryExpr <- GroupedExpr
            / FnCallExpr
            / VarExpr
            / BoolExpr
            / NumberExpr

GroupedExpr <- LPAREN Expr RPAREN
FnCallExpr <- IDENTIFIER LPAREN ExprList RPAREN
VarExpr <- IDENTIFIER
NumberExpr <- FLOAT
           / INTEGER
BoolExpr <- KEYWORD_true
         / KEYWORD_false
TypeExpr <- IDENTIFIER

ExprList <- (Expr COMMA)* Expr?

# Function specific
FnParamList <- (FnParam COMMA)* FnParam?
FnParam <- IDENTIFIER COLON TypeExpr

# Operators
AssignOp <- EQUAL

BinOp <- PLUS / MINUS / ASTERISK / SLASH

CompareOp
    <- EQUALEQUAL
     / EXCLAMATIONMARKEQUAL
     / LARROW
     / RARROW
     / LARROWEQUAL
     / RARROWEQUAL

PrefixOp <- EXCLAMATIONMARK / MINUS

# Tokens
eof <- !.
dec <- [0-9]

dec_int <- dec*

comment <- '//' [^\n]*
skip <- ([ \n] / comment)*

FLOAT <- dec_int "." dec_int skip
INTEGER <- dec_int skip
IDENTIFIER
    <- !keyword [A-Za-z_] [A-Za-z0-9_]* skip


ASTERISK             <- '*'                skip
COLON                <- ':'                skip
COMMA                <- ','                skip
EQUAL                <- '='      ![=]      skip
EQUALEQUAL           <- '=='               skip
LBRACE               <- '{'                skip
LPAREN               <- '('                skip
MINUS                <- '-'                skip
PLUS                 <- '+'                skip
RBRACE               <- '}'                skip
RPAREN               <- ')'                skip
SEMICOLON            <- ';'                skip
SLASH                <- '/'                skip
RARROW               <- '>'      ![=]      skip
RARROWEQUAL          <- '>='               skip
LARROW               <- '<'      ![=]      skip
LARROWEQUAL          <- '<='               skip
EXCLAMATIONMARK      <- '!'      ![=]      skip
EXCLAMATIONMARKEQUAL <- '!='               skip

end_of_word <- ![a-zA-Z0-9_] skip
KEYWORD_fn          <- 'fn'          end_of_word
KEYWORD_pub         <- 'pub'         end_of_word
KEYWORD_return      <- 'return'      end_of_word
KEYWORD_var         <- 'var'         end_of_word
KEYWORD_if          <- 'if'          end_of_word
KEYWORD_else        <- 'else'        end_of_word
KEYWORD_true        <- 'true'        end_of_word
KEYWORD_false       <- 'false'       end_of_word
KEYWORD_and         <- 'and'         end_of_word
KEYWORD_or          <- 'or'          end_of_word

keyword <- KEYWORD_fn
        / KEYWORD_pub
        / KEYWORD_return
        / KEYWORD_var
        / KEYWORD_if
        / KEYWORD_else
