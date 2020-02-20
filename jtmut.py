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
    '''
    Create a with-context that yields a new child node with a given *tag*
    to an existing *node*.
    '''
    yield node.appendChild(node.ownerDocument.createElement(tag))


class MetaProgramBuilder(object):
    doc = None
    next_mid = None
    mutants = None
    
    def __init__(self, doc):
        self.doc = doc
        self.next_mid = 1
        self.mutants = set()
        
    def mid(self):
        '''
        Obtain a unique mutant id.
        '''
        mid = self.next_mid
        self.next_mid += 1
        return mid
    
    def element(self, tag):
        '''
        Create an element with a given *tag*.
        '''
        return self.doc.createElement(tag)
        
    def text(self, value):
        '''
        Create a text node with a given *value*.
        '''
        return self.doc.createTextNode(str(value))

    def clone(self, node):
        '''
        Create a deep clone of a given *node*.
        '''
        return node.cloneNode(deep=True)

    def call(self, fn_name, *arg_exprs):
        '''
        Create a call node that invokes a function named *fn_name*, with an
        optional list of arguments.
        '''
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
        '''
        Create a binary operation expression with the operand *optext*.
        '''
        expr = self.element('expr')

        expr.appendChild(lhs)
        with child(expr, 'operator') as op:
            op.appendChild(self.text(optext))
        expr.appendChild(rhs)

        return expr

    def literal(self, value):
        '''
        Create a literal expression node with a given *value*.
        '''
        expr = self.element('expr')
        with child(expr, 'literal') as literal:
            literal.appendChild(self.text(value))

        return expr

    def cmp_mid(self, optext, value):
        '''
        Create an comparison node that checks the equality between the active 
        mutant and *value*.
        '''
        lhs = self.call('jtmut_get_current_id')
        rhs = self.literal(value)
        return self.binop(lhs, optext, rhs)

    def mut_sdl(self, node):
        '''
        Clone and mutate a given *node* with the statement deletion (SDL) mutation 
        operator
        '''
        mid = self.mid()
        self.mutants.add(mid)
        
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
        '''
        Create a function node that registers all mutant instances when
        the metaprogram is initialized.
        '''
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
    

def mutate(document, tag, opid):
    '''
    Mutate all nodes in a *document* with a given *tag* using a mutation
    operator identified by *opid*. Supported mutation operator identifiers are:
       'sdl' - statement deletion.
    '''
    builder = MetaProgramBuilder(document)
    ops = dict(sdl=builder.mut_sdl)
    
    for node in document.getElementsByTagName(tag):
        new_child = ops[opid](node)
        node.parentNode.replaceChild(new_child, node)
        
    fn = builder.register_mutants()
    document.childNodes[0].appendChild(fn)


def main():
    '''
    Parse argv for options and arguments, and start C mutant schemata generator.
    '''
    parser = optparse.OptionParser(usage="%prog [source.xml]",
                                   formatter=optparse.TitledHelpFormatter())
    
    parser.add_option('-o', dest='output', action='store',
                      help='Save the metaprogram to PATH (defaults to stdout)',
                      metavar='PATH', default='/dev/stdout')
    
    parser.set_description(__doc__.strip())
    (opts, args) = parser.parse_args()
    if not args:
        args = ['/dev/stdin']

    doc = minidom.parse(args[0])
    mutate(doc, 'expr_stmt', 'sdl')

    with open(opts.output, 'w') as f:
        xml = doc.toxml(encoding='utf-8')
        f.write(xml)

    

    
if __name__ == '__main__':
    main()
