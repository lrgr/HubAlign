#!/usr/bin/env python

################################################################################
# SETUP
################################################################################
# Load required modules
import sys, os, argparse, networkx as nx, numpy as np, logging
from sklearn.externals import joblib

# Logging
FORMAT = '%(asctime)s %(filename)-15s %(levelname)-10s: %(message)s'
logging.basicConfig(format=FORMAT)
def getLogger(verbosity=logging.INFO):
    logger = logging.getLogger(__name__)
    logger.setLevel(verbosity)
    return logger

# I/O
def read_hubalign_scores(input_file):
    with open(input_file, 'r') as IN:
        # strips space in first column so we want all remaining columns
        nodes2 = IN.readline().rstrip('\n').split()
        nodes1 = []
        hub_align_scores = []
        for l in IN:
            arr = l.rstrip('\n').split()
            nodes1.append( arr[0] )
            hub_align_scores.append( [ float(f) for f in arr[1:] ] )

    hub_align_scores = np.array(hub_align_scores)
    return hub_align_scores, nodes1, nodes2

################################################################################
# MAIN
################################################################################
# Parse command-line arguments
def get_parser():
    parser = argparse.ArgumentParser()
    parser.add_argument('-nf', '--network_files', type=str, required=True, nargs=2)
    parser.add_argument('-nn', '--network_names', type=str, required=True, nargs=2)
    parser.add_argument('-bbf', '--blast_bitscore_file', type=str, required=True)
    parser.add_argument('-hab', '--hubalign_binary', type=str, required=False,
        default='./bin/HubAlign')
    parser.add_argument('-o', '--output_file', type=str, required=True)
    parser.add_argument('-v', '--verbosity', type=int, required=False, default=logging.INFO)
    parser.add_argument('-a', '--alpha', type=float, required=False, default=0.7)
    parser.add_argument('-l', '--lmbda', type=float, required=False, default=0.1)
    parser.add_argument('--d', type=int, required=False, default=0)
    return parser

# Main
def run( args ):
    ############################################################################
    # Input and setup
    ############################################################################
    # Set up logger
    logger = getLogger(args.verbosity)

    # Read the networks
    logger.info('[Reading in networks]')
    G_A, A_name = nx.read_edgelist(args.network_files[0]), args.network_names[0]
    G_B, B_name = nx.read_edgelist(args.network_files[1]), args.network_names[1]
    A_nodes, B_nodes = sorted(G_A.nodes()), sorted(G_B.nodes())
    from random import shuffle
    shuffle(A_nodes)
    shuffle(B_nodes)
    logger.info('- A: %s nodes x %s edges' % (len(A_nodes), G_A.number_of_edges()))
    logger.info('- B: %s nodes x %s edges' % (len(B_nodes), G_B.number_of_edges()))

    # Read the BLAST scores
    logger.info('[Reading in BLAST scores]')
    obj = joblib.load(args.blast_bitscore_file)
    sim = obj.get('X')
    sim_A_nodes, sim_B_nodes = obj.get('A_nodes'), obj.get('B_nodes')
    sim_A_index = dict(zip(sim_A_nodes, range(len(sim_A_nodes))))
    sim_B_index = dict(zip(sim_B_nodes, range(len(sim_B_nodes))))

    def bitscore(u, v):
        if u not in sim_A_index or v not in sim_B_index:
            return 0
        else:
            return sim[sim_A_index[u], sim_B_index[v]]

    logger.info('- A nodes: %s' % len(sim_A_nodes))
    logger.info('- B nodes: %s' % len(sim_B_nodes))

    ############################################################################
    # Generate HubAlign input files and execute
    ############################################################################
    # Output network files
    network_A_file = A_name + '.tab'
    nx.write_edgelist(G_A, network_A_file, delimiter='\t', data=False)
    network_B_file = B_name + '.tab'
    nx.write_edgelist(G_B, network_B_file, delimiter='\t', data=False)

    # Output BLAST scores file
    bitscore_file = '%s-%s.bitscore' % (A_name, B_name)
    with open(bitscore_file, 'w') as OUT:
        OUT.write( '\n'.join('%s\t%s\t%s' % (u, v, bitscore(u, v)) for u in A_nodes for v in B_nodes if bitscore(u, v) != 0 ) )

    # Create command for running HubAlign
    cmd_args = [ A_name, network_A_file, B_name, network_B_file, '-l',
                 args.lmbda, '-a', args.alpha, '-d', args.d,
                 '-b', bitscore_file ]
    cmd = '%s %s' % (args.hubalign_binary, ' '.join(map(str, cmd_args)))

    # Execute
    os.system(cmd)

    ############################################################################
    # Parse and re-save the output of HubAlign, and clean up
    ############################################################################
    # Load in the HubAlign scores
    logger.info('[Reading in HubAlign output]')
    hubalign_scores_file = '%s-%s.hubalign-scores.txt' % (A_name, B_name)
    hub_align_scores, nodes1, nodes2 = read_hubalign_scores(hubalign_scores_file)

    # Read in the alignment
    alignment_file = '%s-%s.alignment' % (A_name, B_name)
    with open(alignment_file, 'r') as IN:
        alignment = [ l.rstrip('\n').split() for l in IN ]

    # Read in the eval
    eval_file = '%s-%s.eval' % (A_name, B_name)
    with open(eval_file, 'r') as IN:
        evaluation = IN.read().rstrip('\n')

    # Print some summary statistics
    assert( hub_align_scores.shape == (len(nodes1), len(nodes2)) )
    logger.info('- A nodes: %s\n B nodes: %s' % (len(nodes1), len(nodes2)))
    for l in evaluation.split('\n'): logger.info(l)

    # Save to file
    output = dict(params=vars(args), A_nodes=nodes1, B_nodes=nodes2,
                  X=hub_align_scores, alignment=alignment, evaluation=evaluation)
    output['params']['cmd'] = cmd
    joblib.dump(output, args.output_file)

    # Clean up temporary files
    extra_files = [network_A_file, network_B_file, bitscore_file,
                   hubalign_scores_file, eval_file, alignment_file]
    for filename in extra_files:
        if os.path.isfile(filename):
            os.remove(filename)

if __name__ == '__main__': run( get_parser().parse_args(sys.argv[1:]) )
