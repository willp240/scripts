import glob
import re
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

files = sorted(glob.glob("Dec29_testGeoRatio_ratio*.output"))

ratios = []
llhs = []

ratio_re = re.compile(r"ratio([0-9]+)p([0-9]+)")
llh_re = re.compile(r"LLH:\s*([0-9.+-eE]+)")

for fname in files:
    # extract ratio from filename
    m = ratio_re.search(fname)
    if not m:
        continue

    ratio = float(f"{m.group(1)}.{m.group(2)}")

    # extract LLH from file
    llh = None
    with open(fname) as f:
        for line in f:
            m_llh = llh_re.search(line)
            if m_llh:
                llh = float(m_llh.group(1))
                break

    if llh is None:
        print(f"WARNING: No LLH found in {fname}")
        continue

    ratios.append(ratio)
    llhs.append(llh)

# sort by ratio
ratios, llhs = zip(*sorted(zip(ratios, llhs)))

# compute ΔLLH
best_llh = min(llhs)
dllhs = [llh - best_llh for llh in llhs]

print(f"Best LLH = {best_llh:.6f}")

# plot
plt.figure(figsize=(7,5))
plt.plot(
    ratios,
    dllhs,
    linestyle="",
    marker="+",
    markersize=4,
    markeredgecolor="midnightblue",
    markeredgewidth=1
)

ax = plt.gca()
ax.xaxis.set_major_locator(MultipleLocator(1.0))
ax.xaxis.set_minor_locator(MultipleLocator(0.1))
ax.tick_params(axis="x", which="major", length=6)
ax.tick_params(axis="x", which="minor", length=3)
ax.yaxis.set_minor_locator(MultipleLocator(0.2))
ax.set_xlim(0.0, 10.0)

# ΔLLH significance lines (1D scan)
for y, label in [(0.5, "1σ"), (2.0, "2σ"), (4.5, "3σ")]:
    plt.axhline(y, color="darkred", linestyle=":", linewidth=1.0)
    plt.text(
        plt.xlim()[1] * 0.95,
        y+0.05,
        label,
        fontsize=9,
        color="darkred"
    )

plt.xlabel("Geo Ratio: U/Th")
plt.ylabel(r"$\Delta$LLH")
plt.title(r"$\Delta$LLH vs Geo Ratio")
plt.grid(True)
plt.tight_layout()
plt.savefig("georatio_noconstraint.pdf")
plt.show()

