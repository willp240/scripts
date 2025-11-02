#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt

# ------------------------------------------------------------
# Define x-axis: L/E (in km/GeV, roughly)
# ------------------------------------------------------------
LE = np.linspace(0, 2500, 2000)  # wide and smooth

# ------------------------------------------------------------
# Define "frequencies" from mass-squared differences
# (scaled so they produce visible beating)
# ------------------------------------------------------------
dm21 = 7.4e-5
dm31 = 2.5e-3
dm32 = dm31 - dm21

# relative phases (arbitrary scaling to make it pretty)
phi21 = 1.27 * dm21 * LE
phi31 = 1.27 * dm31 * LE
phi32 = 1.27 * dm32 * LE

# ------------------------------------------------------------
# Schematic survival probabilities (not physical precision)
# ------------------------------------------------------------
theta12, theta13 = 33 * np.pi/180, 8.6 * np.pi/180
P1 = 1 - np.sin(2*theta12)**2 * np.sin(phi21)**2
P2 = 1 - np.sin(2*theta13)**2 * np.sin(phi31)**2
P3 = 1 - 0.5*np.sin(phi32)**2  # approximate atmospheric term

# Combined "electron neutrino survival probability"
Pee = 0.33*P1 + 0.33*P2 + 0.34*P3

# ------------------------------------------------------------
# Plot
# ------------------------------------------------------------
fig, ax = plt.subplots(figsize=(10, 4))

ax.plot(LE, P1, color='tab:blue', lw=2, label=r"$\nu_1$ term")
ax.plot(LE, P2, color='tab:orange', lw=2, label=r"$\nu_2$")
ax.plot(LE, P3, color='tab:green', lw=2, label=r"$\nu_3$")
ax.plot(LE, Pee, color='k', lw=3, ls='--', label=r"Combined $P_{ee}$")

# Formatting
ax.set_ylim(0, 1.05)
ax.set_xlim(0, 2500)
ax.set_xlabel(r"$L/E$  [km / GeV]", fontsize=13)
ax.set_ylabel("Survival probability", fontsize=13)
ax.set_title("Schematic Neutrino Oscillation Probabilities", fontsize=14)
ax.legend(fontsize=11, ncol=2, loc="lower right")
ax.grid(True, linestyle="--", alpha=0.3)

plt.tight_layout()
plt.savefig("oscillation_probabilities.pdf", bbox_inches="tight")
plt.show()
