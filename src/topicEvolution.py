import random
import sys
import matplotlib.pyplot as plt

numProcesses = int(sys.argv[1])
fname = '../data/nodesVsTime'
TMAX = 1500

for iter in range(numProcesses):
    plt.clf()
    with open(fname+str(iter)+'.dat') as file:
        topics = 1000                   #topics is total number of topics
        size = [0]*topics               #size[i] is time for which topic i was talked about
        peak = [[0,i] for i in range(topics)]
        y = [[0]*TMAX for i in range(topics)]
        for line in file: 
            line = line.strip().split()
            for i,x in enumerate(line):
                x = int(x)
                y[i][size[i]] = x
                peak[i][0] = max(x,peak[i][0])
                size[i] = size[i] + 1
                topics = i

    size = size[:topics+1]
    y = y[:topics+1]
    for i in range(topics+1):
        y[i] = y[i][:size[i]]

    peak = peak[24:topics+1]
    peak.sort()
    topics = topics - 24

    plt_ids = set()

    while (len(plt_ids) < 3):
        plt_ids.add(peak[random.randint(0,19)][1])

    while (len(plt_ids) < 8):
        plt_ids.add(peak[random.randint(20,topics-20)][1])

    while (len(plt_ids) < 13):
        plt_ids.add(peak[random.randint(topics-19,topics)][1])

    plt_ids = list(plt_ids)
    for j in plt_ids:
        x = [i for i in range(size[j])]
        plt.plot(x, y[j], '-', label='Topic '+str(j))

    # plt.legend(loc='upper right')
    plt.xlabel('Time')
    plt.ylabel('Number of users')
    plt.savefig('../figures/evolution'+str(iter)+'.png')
    # plt.show()
