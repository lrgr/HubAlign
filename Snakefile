################################################################################
# SETTINGS, FILES, CONFIGURATION, AND DIRECTORIES
################################################################################
# Configuration/Settings/Modules
from os.path import join

# Directories
SRC_DIR = 'src'
BIN_DIR = 'bin'
EXAMPLE_DIR = 'example'
OUTPUT_DIR = 'output'

# Files
HUB_ALIGN = join('bin', 'HubAlign')
BLAST_BITSCORES_TXT = join(EXAMPLE_DIR, 'Test1_Test2.bitscore')
BLAST_BITSCORES_PKL = join(EXAMPLE_DIR, 'Test1_Test2.bitscore.pkl')

NAME1 = 'Test1'
NETWORK1 = join(EXAMPLE_DIR, '%s.tab' % NAME1)
NAME2 = 'Test2'
NETWORK2 = join(EXAMPLE_DIR, '%s.tab' % NAME2)

EXAMPLE_HUBALIGN_OUTPUT = join(OUTPUT_DIR, '%s-%s-hubalign-output.pkl' % (NAME1, NAME2))
################################################################################
# RULES
################################################################################
# General
rule all:
    input:
        EXAMPLE_HUBALIGN_OUTPUT

# Running on example data
rule run_example:
    input:
        bitscores=BLAST_BITSCORES_PKL,
        hubalign=HUB_ALIGN,
        networks=expand(join(EXAMPLE_DIR, '{name}.tab'), name=[NAME1, NAME2])
    params:
        names=' '.join([NAME1, NAME2])
    output:
        EXAMPLE_HUBALIGN_OUTPUT
    shell:
        'python hub_align.py -nf {input.networks} -nn {params.names} '\
        '-bbf {input.bitscores} -hab {input.hubalign} -o {output}'

rule wrap_bitscores:
    input:
        BLAST_BITSCORES_TXT
    output:
        BLAST_BITSCORES_PKL
    shell:
        'python wrap_bitscores.py -i {input} -o {output}'


# Compilation
rule compile_hubalign:
    input:
        hubalign=join(SRC_DIR, 'HubAlign.cpp'),
        alignment=join(SRC_DIR, 'Alignment.cpp'),
        network=join(SRC_DIR, 'Network.cpp')
    output:
        HUB_ALIGN
    shell:
        'g++ -o {output} {input.hubalign} {input.alignment} {input.network}'
