#!/usr/bin/env python

# Load required modules
import sys, os, argparse, numpy as np
from sklearn.externals import joblib
from collections import defaultdict

# Parse command-line arguments
parser = argparse.ArgumentParser()
parser.add_argument('-i', '--input_file', type=str, required=True)
parser.add_argument('-o', '--output_file', type=str, required=True)
args = parser.parse_args(sys.argv[1:])

# Load the input bitscores
with open(args.input_file, 'r') as IN:
    A_nodes = set()
    B_nodes = set()
    bitscores = defaultdict(int)
    nonzero, skipped = 0, 0
    for i, l in enumerate(IN):
        u, v, score = l.rstrip('\n').split('\t')
        if u == '' or v == '':
            skipped += 1
            continue
        A_nodes.add( u )
        B_nodes.add( v )
        bitscores[(u, v)] = float(score)
        nonzero += float(score) != 0
    print('Loaded scores: %s nodes x %s nodes' % (len(A_nodes), len(B_nodes)))
    print('Skipped:', skipped)
    print('Nonzero:', nonzero)

# Convert bitscores to numpy array
A_nodes = sorted(A_nodes)
B_nodes = sorted(B_nodes)
X = np.array([ [ bitscores[(u, v)] for v in B_nodes ] for u in A_nodes ])

# Dump to file
output = dict(params=vars(args), A_nodes=A_nodes, B_nodes=B_nodes, X=X)
joblib.dump( output, args.output_file )
