#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np

# ------------------------------------------------------------
# Particle masses in eV (approximate, illustrative values)
# ------------------------------------------------------------
particles = [
    # 1st generation
    (r"$\nu_{1}$", 1e-3, "lepton", 1, 1, 0.1, 9.9e-4, 0.099),
    ("e", 0.511e6, "lepton", 1, 0.8, 0.1, 0, 0),
    ("u", 2.2e6, "quark", 1, 0.7, 0.1, 0, 0),
    ("d", 4.7e6, "quark", 1, 2.5, -0.25, 0, 0),

    # 2nd generation
    ("s", 94e6, "quark", 2, 0.4, 0.1, 0, 0),
    (r"$\nu_{2}$", 0.0086, "lepton", 2, 1.0, 0.1, 0.00774, 0.0914),
    (r"$\mu$", 105.7e6, "lepton", 2, 2.5, -0.25, 0, 0),
    ("c", 1.27e9, "quark", 2, 1, 0.1, 0, 0),

    # 3rd generation
    (r"$\nu_{3}$", 5e-2, "lepton", 3, 1.0, 0.1, 0.045, 0.05),
    (r"$\tau$", 1777e6, "lepton", 3, 0.5, 0.1, 0, 0),
    ("t", 173e9, "quark", 3, 1.0, 0.1, 0, 0),
    ("b", 4.18e9, "quark", 3, 2.5, -0.25, 0, 0),

    # Bosons (put them at x = 3.8 for spacing)
    # ("γ", 1e-3, "boson", 3.8),
    #("g", 1e-3, "boson", 3.8),
    ("W", 80.4e9, "boson", 3.8, 0.2, -0.12, 0, 0),
    ("Z", 91.2e9, "boson", 3.8, 1, 0.1, 0, 0),
    ("H", 125e9, "boson", 3.8, 2.4, -0.12, 0, 0),
]

# ------------------------------------------------------------
# Colors and markers by category
# ------------------------------------------------------------
category_style = {
    "lepton": dict(color="tab:blue", marker="o"),
    "quark": dict(color="tab:green", marker="s"),
    "boson": dict(color="tab:red", marker="^"),
}

# ------------------------------------------------------------
# Plot setup
# ------------------------------------------------------------
fig, ax = plt.subplots(figsize=(5, 8))

for name, mass, cat, gen, y, x, errlow, errup in particles:
    style = category_style[cat]
    #ax.scatter(gen, mass, s=80, edgecolor="k", linewidth=0.6, **style)
    #ax.text(gen+x, mass*y, name, va="center", fontsize=20)

    if "nu" in name:  # neutrino points
        # draw central marker
        ax.scatter(gen, mass, s=80, edgecolor="k", linewidth=0.6, zorder=3, **style)

        # fade error bar (log spacing for smoothness)
        ylow, yhigh = mass - errlow, mass + errup
        yline = np.geomspace(ylow, yhigh, 100)
        # alpha fades from 0.8 at center to 0.05 at ends
        center_idx = len(yline)//2
        alpha_up = np.linspace(0.8, 0.05, center_idx)
        alpha_down = np.linspace(0.05, 0.8, center_idx)
        alpha = np.concatenate([alpha_down, alpha_up])
        
        for y1, y2, a in zip(yline[:-1], yline[1:], alpha[:-1]):
            ax.vlines(gen, y1, y2, color=style["color"], linewidth=6, alpha=a, zorder=2)

    else:
        ax.scatter(gen, mass, s=80, edgecolor="k", linewidth=0.6, **style)
    ax.text(gen+x, mass*y, name, va="center", fontsize=20)
        
# Log scale and limits
ax.set_yscale("log")
ax.set_ylim(1e-5, 1e12)
ax.set_xlim(0.5, 4.3)

# Axis labels and ticks
ax.set_xlabel("Generation", fontsize=13)
ax.set_ylabel("Mass [eV]", fontsize=13)
ax.set_xticks([1, 2, 3])
ax.set_xticklabels(["1st", "2nd", "3rd"])
ax.tick_params(axis='both', which='major', labelsize=11)

# Grid and styling
ax.grid(True, which="both", linestyle="--", alpha=0.3)

# Legend
for cat, style in category_style.items():
    ax.scatter([], [], label=cat.capitalize(), s=200,
               edgecolor="k", linewidth=0.6, **style)
ax.legend(fontsize=16, loc="center", bbox_to_anchor=(0.5, 0.4))


# Title
ax.set_title("Fundamental Particle Masses", fontsize=15, pad=10)

plt.tight_layout()
plt.savefig("particle_masses.pdf", bbox_inches="tight")
plt.show()
