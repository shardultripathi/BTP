import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import animation

points = [[0.0]*3 for i in range(2200)]
regime = [0]*2200

with open('temp.dat') as file1:
    it = -1
    for line in file1:
        it = it + 1
        line = line.strip().split()
        for i in range(3):
            points[it][i] = float(line[i])

with open('results.txt') as file2:
    for line in file2:
        line = line.strip().split()
        regime[int(line[0])*8 + int(line[1])] = int(line[2])    

supv = [points[i] for i in range(2200) if regime[i] == 1]
vir = [points[i] for i in range(2200) if regime[i] == 2]
subv = [points[i] for i in range(2200) if regime[i] == 3]    

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.view_init(30,0)

ax.set_xlabel('A')
ax.set_ylabel('alpha')
ax.set_zlabel('beta')
plt.xlim([0,1100])
plt.ylim([0.0,0.5])
ax.set_zlim([0.0,0.5])

# init
def init():
    for c, m, l, la in [('y', 'o', supv, 'Super-viral'), ('b', '^', vir, 'Viral'), ('r', 'o', subv, 'Sub-viral')]:
        pointst = [[0.0]*2200 for i in range(3)]
        for i in range(3):
            pointst[i] = [row[i] for row in l]
        ax.scatter(pointst[0], pointst[1], pointst[2], c=c, marker=m, label=la, s=500)
    plt.legend(loc='upper right')
    return fig,

# animation function.  This is called sequentially
def animate(i):
    ax.view_init(elev=10., azim=i)
    return fig,

# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=360, interval=20, blit=True)
anim.save('../figures/3dplot/plots/total_animation1.mp4', fps=30, extra_args=['-vcodec', 'libx264'])

# plt.show()



