import re
import matplotlib.pyplot as plt
plt.rcParams['font.size'] = 15.0

# Define the log file path
log_file = "/data/snoplus/parkerw/ratSimulations/May23_SFTime_updated/output/merged.output"

# Define the categories we are interested in
categories = ["PMT Selector", "Muon Check", "Unseeded Classifiers", "Quad Seed", "MultiPDF", "Seeded Classifiers", "Energy", "Energy Seeded Classifiers", "Total"]
#categories = ["Isotropy","ITR","XSite","QPDT","Mean Time","Iso Regions","Early Time","BiPo Cumul","BiPo LLH","AlphaBeta 212","AlphaBeta 214","BAB","Ext0Nu","Alpha N IBD"]
#colors = ["Red", "Orange", "Purple", "Magenta", "Pink", "Yellow", "Beige", "White", "LightGreen", "Green", "Cyan", "Blue", "Grey", "Black" ]
colors = ["Red", "Purple", "Pink", "Yellow", "Cyan", "Blue", "Green", "Black"]

category_set = set(categories)  # O(1) lookup
#values = {cat: [] for cat in categories}  # Store values for averaging
values = {cat: {"sum": 0.0, "count": 0} for cat in categories}

# Read the file and extract relevant values
linecount = 0
skipped = 0
with open(log_file, "r") as f:
    for line in f:
        linecount += 1
        if linecount % 1_000_000 == 0:
            print(f"Processed {linecount:,} lines...")

        #if linecount > 600000000:
         #   break
            
        if ":" not in line:
            skipped += 1
            continue

        key, val = line.split(":", 1)
        key = key.strip()
        if key not in category_set:
            continue

        try:
            #values[key].append(float(val.strip()))
            v = float(val.strip())
            values[key]["sum"] += v
            values[key]["count"] += 1
        except ValueError:
            skipped += 1
# Compute averages
#averages = {key: sum(vals) / len(vals) for key, vals in values.items() if vals}
averages = {key: data["sum"] / data["count"] for key, data in values.items() if data["count"] > 0}

for key, vals in averages.items():
    print(key, vals)

del averages["Total"]
    
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
#plt.savefig("RAT8.0.1_SeedClassBisMSB.pdf", format="pdf")
plt.savefig("RAT8.0.1_ScintFitter_updated.pdf", format="pdf")
plt.show()
