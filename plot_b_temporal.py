import numpy as np 
import matplotlib.pyplot as plt 
from scipy.stats import gaussian_kde
import matplotlib.colors as mcolors
import matplotlib.cm as cm

folder_out = "./"

fig = plt.figure(figsize=(6,4)) 
ax = fig.gca()
folder_in = "./"

step = 1000000
time_steps = list(range(15))

colorparams = [(t+1) * step for t in time_steps]
# Choose a colormap
colormap = cm.cividis
normalize = mcolors.Normalize(vmin=np.min(colorparams), vmax=np.max(colorparams))

for time_step in time_steps:
    name = "out" + str(time_step) + ".bin"
    b = np.fromfile(name, dtype=np.double)
    kde = gaussian_kde(b, bw_method=0.25)
    x_grid = np.linspace(-1,1,150)
    y = kde.evaluate(x_grid)
    color = colormap(normalize((time_step+1)*step),alpha=0.9)
    ax.plot(x_grid, y, color=color)

s_map = cm.ScalarMappable(norm=normalize, cmap=colormap)
s_map.set_array(colorparams)

halfdist = (colorparams[1] - colorparams[0])/2.0
boundaries = np.linspace(colorparams[0] - halfdist, colorparams[-1] + halfdist, len(colorparams) + 1)

cbar = fig.colorbar(s_map, spacing='proportional', ticks=colorparams, boundaries=boundaries, format='%2.2g')
cbarlabel = "Number of iterations"
cbar.set_label(cbarlabel, fontsize=12)

ax.set_xlabel("b")
ax.set_ylabel("density")
ax.set_xlim(-1,1)
ax.set_ylim(0,3.4)
plt.tight_layout()
plt.savefig(folder_out + "b_evolution.pdf")
plt.close()