import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

points = [[0.0]*3 for i in range(1096)]
regime = [0]*1096

with open('temp.dat') as file1:
	it = -1
	for line in file1:
		it = it + 1
		line = line.strip().split()
		for i in range(3):
			points[it][i] = float(line[i])

with open('results.txt') as file2:
	it = -1
	for line in file2:
		it = it + 1
		line = line.strip().split()
		regime[int(line[0])*8 + int(line[1])] = int(line[2])	

supv = [points[i] for i in range(1096) if regime[i] == 1 and points[i][0] < 4000]
vir = [points[i] for i in range(1096) if regime[i] == 2 and points[i][0] < 4000]
subv = [points[i] for i in range(1096) if regime[i] == 3 and points[i][0] < 4000]	

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
for c, m, l, la in [('y', 'o', supv, 'Super-viral'), ('b', '^', vir, 'Viral'), ('r', 'o', subv, 'Sub-viral')]:
	points = [[0.0]*1096 for i in range(3)]
	for i in range(3):
		points[i] = [row[i] for row in l]
	ax.scatter(points[0], points[1], points[2], c=c, marker=m, label=la)

ax.set_xlabel('A')
ax.set_ylabel('alpha')
ax.set_zlabel('beta')
plt.legend(loc='upper right')
plt.savefig('../figures/3dplot1.png')
plt.show()