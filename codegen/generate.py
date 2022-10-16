#!/usr/bin/env python3

import os
import sys
from jinja2 import Environment, FileSystemLoader

TEMPLATE_DIR = 'codegen'

files = [
    'codegen.hpp',
    'sema.hpp',
    'dotgen.hpp',
]

ast_nodes = [
    'FloatExprNode',
    'IntegerExprNode',
    'BoolExprNode',
    'OrExprNode',
    'AndExprNode',
    'PrefixExprNode',
    'BinExprNode',
    'VarDeclNode',
    'FnDefNode',
    'FnParamNode',
    'BlockNode',
    'RootNode',
    'AssignStNode',
    'ReturnStNode',
    'IfStNode',
    'VarExprNode',
    'FnCallExprNode',
    'EmptyNode',
]


if not os.path.exists(TEMPLATE_DIR):
    sys.exit("template dir %s does not exist" % TEMPLATE_DIR)

if not os.path.exists('src'):
    sys.exit("src does not exist")

file_loader = FileSystemLoader(TEMPLATE_DIR)
env = Environment(loader=file_loader)
env.keep_trailing_newline = True

for f in files:
    env.get_template(f + '.j2').stream(ast_nodes=ast_nodes).dump('src/' + f)
