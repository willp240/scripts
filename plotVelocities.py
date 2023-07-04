import matplotlib.pyplot as plt

data = {'0.5% PPO': 212.220, '2.2% PPO': 186.488, '6% PPO 1.5% Te': 188.020, '6% PPO 2.5% Te': 187.113}
names = list(data.keys())
values = list(data.values())

fig = plt.figure()
ax = fig.gca()
ax.set_xlabel('Material')
ax.set_ylabel('Quad Velocity, mm/ns')
plt.ylim([0,300])
ax.scatter(names, values, marker="x")
plt.grid()
plt.show()
