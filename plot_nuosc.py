#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np

# Create x values (distance or time)
x = np.linspace(0, 12*np.pi, 1200)

# Define two waves with a smaller frequency difference for clearer beating
y1 = np.sin(x) # nu_1 (purple)
y2 = np.sin(5 * x/6) # nu_2 (yellow)

# Plot setup
fig, ax = plt.subplots(figsize=(8, 3))

# Plot the two mass eigenstates
ax.plot(x, y1, '--', color='purple', lw=2, label=r"$\nu_1$")
ax.plot(x, y2, color='gold', lw=2, label=r"$\nu_2$")

# Highlight positions corresponding to flavor compositions
positions = [0.1, 6*np.pi, -0.1+12*np.pi]
textpos = [1.0, 6*np.pi, -1.0 + 12*np.pi]
labels = [r"$\nu_\mu$", r"$\nu_e$", r"$\nu_\mu$"]
colors = ['red', 'green', 'red']

for xpos, lab, col, textpos in zip(positions, labels, colors, textpos):
    ax.arrow(xpos, -0.95, 0.02, 0.95, head_width=0.4, head_length=0.3,
             fc=col, ec=col, lw=2, length_includes_head=True)
    ax.text(textpos, -1.1, lab, ha='center', va='top', color=col, fontsize=20)

# Aesthetic settings
ax.axhline(0, color='gray', lw=0.8)
ax.set_xlim(0, 12*np.pi)
ax.set_ylim(-1.5, 1.5)
ax.set_xticks([])
ax.set_yticks([])
ax.set_xlabel("Propagation distance (arbitrary units)", fontsize=12)
ax.legend(loc='upper center', ncol=2, frameon=False, fontsize=20, bbox_to_anchor=(0.4, 0.8 ))

plt.tight_layout()
plt.savefig("oscillation_mass_states.pdf", bbox_inches='tight')
plt.show()
