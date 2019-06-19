#!/usr/bin/env python
# encoding: utf-8
# Copyright (C) 2019 John Törnblom
#
# This file is part of jtmut, Johns tiny schemata mutation testing tool.
#
# jtmut is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# jtmut is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
# for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with jtmut; see the files COPYING and COPYING.LESSER. If not,
# see <http://www.gnu.org/licenses/>.
'''
Johns tiny schemata mutation testing tool for programs expressed in the 
xml-based srcml format.
'''

import optparse
import sys

from contextlib import contextmanager
from xml.dom import minidom


@contextmanager
def child(node, tag):
    yield node.appendChild(node.ownerDocument.createElement(tag))


class Builder(object):
    doc = None
    next_mid = None
    mutants = None
    
    def __init__(self, doc):
        self.doc = doc
        self.next_mid = 1
        self.mutants = set()
        
    def mid(self):
        mid = self.next_mid
        self.next_mid += 1
        return mid
    
    def element(self, tag):
        return self.doc.createElement(tag)
        
    def text(self, value):
        return self.doc.createTextNode(str(value))

    def clone(self, node):
        return node.cloneNode(deep=True)

    def call(self, fn_name, *arg_exprs):
        expr = self.element('expr')
        with child(expr, 'call') as call:
            with child(call, 'name') as name:
                name.appendChild(self.text(fn_name))

        with child(call, 'argument_list') as arg_list:
            arg_list.appendChild(self.text('('))
            for arg_expr in arg_exprs:
                with child(arg_list, 'argument') as arg:
                    arg_list.appendChild(arg_expr)
                        
            arg_list.appendChild(self.text(')'))

        return expr

    def binop(self, lhs, optext, rhs):
        expr = self.element('expr')

        expr.appendChild(lhs)
        with child(expr, 'operator') as op:
            op.appendChild(self.text(optext))
        expr.appendChild(rhs)

        return expr

    def literal(self, value):
        expr = self.element('expr')
        with child(expr, 'literal') as literal:
            literal.appendChild(self.text(value))

        return expr
    
    def cmp_mid(self, optext, value):
        lhs = self.call('jtmut_get_current_id')
        rhs = self.literal(value)
        return self.binop(lhs, optext, rhs)

    def mut_sdl(self, node, mid):
        if_ = self.element('if')
        if_.appendChild(self.text('if'))
        
        with child(if_, 'condition') as cond:
            cond.appendChild(self.text('('))
            cond.appendChild(self.cmp_mid('!=', mid))
            cond.appendChild(self.text(')'))
            
        with child(if_, 'then') as then:
            with child(then, 'block') as block:
                block.appendChild(self.text('{'))
                block.appendChild(self.clone(node))
                block.appendChild(self.text('}'))
    
        return if_

    def register_mutants(self):
        fn = self.element('function')
        
        with child(fn, 'type') as ty:
            with child(ty, 'name') as name:
                name.appendChild(self.text('JTMUT_CONSTRUCTOR'))

        fn.appendChild(self.text(' '))
        
        with child(fn, 'name') as name:
            name.appendChild(self.text('register_mutants'))
            
        with child(fn, 'parameter_list') as param_list:
            param_list.appendChild(self.text('('))
            param_list.appendChild(self.text(')'))

        with child(fn, 'block') as block:
            block.appendChild(self.text('{\n  '))
            for mid in self.mutants:
                mid = self.literal(mid)
                expr = self.element('expr')
                block.appendChild(self.call('jtmut_register_mutant', mid))
                block.appendChild(self.text(';\n  '))
            block.appendChild(self.text('\n}'))
            
        return fn
    
    def mutate(self, node, mut_op):
        ops = dict(sdl=self.mut_sdl)
        mid = self.mid()
        new_child = ops[mut_op](node, mid)
        node.parentNode.replaceChild(new_child, node)
        self.mutants.add(mid)


def mutate(doc, tag, mut_op):
    b = Builder(doc)
    for node in doc.getElementsByTagName(tag):
        b.mutate(node, mut_op)
        
    fn = b.register_mutants()
    doc.childNodes[0].appendChild(fn)



def main():
    '''
    Parse argv for options and arguments, and start C mutant schemata generator.
    '''
    parser = optparse.OptionParser(usage="%prog <source.xml>",
                                   formatter=optparse.TitledHelpFormatter())
                                   
    parser.set_description(__doc__.strip())
    (opts, args) = parser.parse_args()
    if len(args) == 0:
        parser.print_help()
        sys.exit(1)

    doc = minidom.parse(args[0])
    mutate(doc, 'expr_stmt', 'sdl')
    print(doc.toxml(encoding='utf-8'))

    
if __name__ == '__main__':
    main()
