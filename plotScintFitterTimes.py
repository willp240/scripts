import re
import matplotlib.pyplot as plt
plt.rcParams['font.size'] = 15.0

# Define the log file path
log_file = "rat.pplxint13.3157683.log"

# Define the categories we are interested in
#categories = ["PMT Selector", "Muon Check", "Unseeded Classifiers", "Quad Seed", "MultiPDF", "Seeded Classifiers", "Energy"]
categories = ["Isotropy","ITR","XSite","QPDT","Mean Time","Iso Regions","Early Time","BiPo Cumul","BiPo LLH","AlphaBeta 212","AlphaBeta 214","BAB","Ext0Nu","Alpha N IBD"]
colors = ["Red", "Orange", "Purple", "Magenta", "Pink", "Yellow", "Beige", "White", "LightGreen", "Green", "Cyan", "Blue", "Grey", "Black" ]
#colors = ["Red", "Purple", "Pink", "Yellow", "Cyan", "Blue", "Green"]

values = {cat: [] for cat in categories}  # Store values for averaging

# Read the file and extract relevant values
with open(log_file, "r") as f:
    for line in f:
        match = re.match(r"([\w\s]+):\s+([-+]?[\d\.e-]+)", line)
        if match:
            key, value = match.groups()
            if key in categories:
                values[key].append(float(value))

# Compute averages
averages = {key: sum(vals) / len(vals) for key, vals in values.items() if vals}

for key, vals in averages.items():
    print(key, vals)

# Extract data for the pie chart
labels = list(averages.keys())
full_labels = [f"{key}: {value:.6f}" for key, value in averages.items()]
sizes = list(averages.values())  

# Normalize sizes to percentages
total = sum(sizes)
sizes = [s / total for s in sizes]  # Convert values to fractions of the whole

# Create pie chart without labels
fig, ax = plt.subplots()
wedges, texts = ax.pie(
    sizes, labels=[None] * len(labels),  # No direct labels
    autopct=None, startangle=0, 
    colors=colors, wedgeprops={'edgecolor': 'black'}
)

# Add a legend
#ax.legend(wedges, labels, title="Categories", loc="center left", bbox_to_anchor=(1, 0.5))
ax.legend(wedges, full_labels, bbox_to_anchor=(1,0.5), loc="center right", fontsize=12, 
           bbox_transform=plt.gcf().transFigure)

plt.subplots_adjust(left=0.0, bottom=0.1, right=0.45)

plt.title("ScintFitter Times (s)")
plt.savefig("RAT7.0.15_SeedClass.pdf", format="pdf")
#plt.savefig("RAT7.0.15_ScintFitter.pdf", format="pdf")
plt.show()
