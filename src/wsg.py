import networkx as nx
import sys

numNodes = int(sys.argv[1])
degree = int(sys.argv[2])
rewiring = int(sys.argv[3])

g = connected_watts_strogatz_graph(n = numNodes, k = degree, p = rewiring, tries = 100, seed = None)
e = nx.edges(g)
f1 = open('./edges','w+')
f1.write(str(e))
f1.close()