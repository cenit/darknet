# -*- coding: utf-8 -*-
"""
Created on Sun Apr  8 15:47:13 2018

@author: NICO
"""
import json
import scipy
import numpy as np
import matplotlib.pylab as plt

coords_file = "coords_small.json"
data = json.load(open(coords_file))
coords = [coord for name, coord in data.items()]

p1 = [coords[0][0]]
p2 = [coords[0][1]]
for i in range(1, len(coords)):
    D = scipy.spatial.distance.squareform(scipy.spatial.distance.pdist(np.concatenate(([p1[-1], p2[-1]], coords[i])), metric="euclidean"))[2:, :2]
    if len(coords[i]) == 1: 
        next_ = np.argmin(D)
        if next_ == 0: p1.append(coords[i][0])
        else: p2.append(coords[i][0])
    else:
        next_ = np.argmin(D, axis=0)
        if next_[0] == next_[1]: 
            if next_[0] == 0: p1.append(coords[i][0])
            else: p2.append(coords[i][0])
        else:
            p1.append(coords[i][next_[0]])
            p2.append(coords[i][next_[1]])
p1 = [(x, -y) for x,y in p1]
p2 = [(x, -y) for x,y in p2]
fig, ax = plt.subplots(nrows=1, ncols=1, figsize=(10,10))
ax.scatter(*zip(*p1), edgecolors="r", marker="o", s=80, alpha=.5, facecolors="none", lw=3, label="Nico")
ax.scatter(p1[0][0], p1[0][1], marker='o', s=70, color="r", label="start") # start p1
ax.scatter(*zip(*p2), edgecolors="b", marker="o", s=80, alpha=.5, facecolors="none", lw=3, label="Bazzani")
ax.scatter(p2[0][0], p2[0][1], marker='o', s=70, color="b", label="start") # start p2
plt.legend(loc="best", fontsize=14)
plt.savefig(coords_file.split(".")[0] + ".png")