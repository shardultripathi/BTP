import networkx as nx
import sys

numNodes = int(sys.argv[1])
degree = int(sys.argv[2])
rewiring = float(sys.argv[3])

g = nx.connected_watts_strogatz_graph(n = numNodes, k = degree, p = rewiring, tries = 100, seed = None)
e = nx.edges(g)
# print(nx.diameter(g))
f1 = open('./edges','w+')
f1.write(str(e))
f1.close()