import matplotlib.pyplot as plt
import datetime
import numpy as np


# Function to calculate Pancake Day (Shrove Tuesday) for a given year
def get_pancake_day(year):
    easter = easter_sunday(year)
    return easter - datetime.timedelta(days=47)

# Function to calculate Easter Sunday using the Computus algorithm
# Returns numbered day of the year (e.g Jan 1st = 1) 
def easter_sunday(year):
    a = year % 19
    b = year // 100
    c = year % 100
    d = b // 4
    e = b % 4
    f = (b + 8) // 25
    g = (b - f + 1) // 3
    h = (19 * a + b - d - g + 15) % 30
    i = c // 4
    k = c % 4
    l = (32 + 2 * e + 2 * i - h - k) % 7
    m = (a + 11 * h + 22 * l) // 451
    month = (h + l - 7 * m + 114) // 31
    day = ((h + l - 7 * m + 114) % 31) + 1
    return datetime.date(year, month, day)

# Range we want to plot over
first_year = 1050
last_year = 2050
label_step = 100
tick_step = 25
linestyle = '-'

# Calculate pancake day for each year
years = list(range(first_year, last_year))
pancake_days = [get_pancake_day(year) for year in years]

# Convert pancake days to month/day format for y-axis labels
pancake_day_labels = [day.strftime("%b %d") for day in pancake_days]

# Convert those specific dates back to numerical format for plotting (day of the year)
pancake_day_nums = [day.timetuple().tm_yday for day in pancake_days]

# Generate range of y-axis ticks every 2 days
y_min = min(pancake_day_nums)
y_max = max(pancake_day_nums)
y_ticks = np.arange(y_min - (y_min % 2), y_max + 2, 2)  # Every 2 days

# Convert these y-ticks to formatted labels (month & day)
# Use 2023 as place holder. As long as we use the same (non-leap) year for all it's ok
y_labels = [(datetime.date(2023, 1, 1) + datetime.timedelta(days=int(y_tick - 1))).strftime("%b %d") for y_tick in y_ticks]

# Generate x ticks
x_min = min(years)
x_max = max(years)
x_ticks = np.arange(x_min - (x_min % label_step), x_max + label_step, label_step)
x_labels = [yr for yr in x_ticks]

# Plot
fig, ax = plt.subplots(figsize=(15, 6))
ax.plot(years, pancake_day_nums, marker='x', markersize=3, linestyle=linestyle, linewidth=0.5)
ax.set_xlabel("Year")
ax.set_ylabel("Pancake Day")
ax.set_title("Pancake Day Date vs Year (" + str(first_year) + "-" + str(last_year) + ")")
ax.grid(True, which='both', linestyle='--', linewidth=0.5)

# Set axis labels and grid
ax.set_xticks(x_ticks)
ax.set_xticklabels(x_labels)
ax.tick_params(axis="x", labelrotation=45)
ax.set_xticks(np.arange(x_min-label_step, x_max+label_step, tick_step), minor=True)
ax.tick_params(axis="x", which="minor", length=2)
ax.tick_params(axis="x", which="major", length=5)

ax.set_yticks(y_ticks)
ax.set_yticklabels(y_labels)


# Now make frequency histogram

# Generate range of x-axis ticks every 1 days
x_min2 = min(pancake_day_nums)
x_max2 = max(pancake_day_nums)
x_ticks2 = np.arange(x_min2 - 1, x_max2 + 2, 1)
x_labels2 = [(datetime.date(2023, 1, 1) + datetime.timedelta(days=int(x_tick2 - 1))).strftime("%b %d") for x_tick2 in x_ticks2]

fig2, ax2 = plt.subplots(figsize=(15, 8))
plt.hist(pancake_day_nums, histtype='bar', edgecolor='black', bins=x_ticks2, density=True)
ax2.set_xticks(x_ticks2)
ax2.set_xticklabels(x_labels2)
ax2.tick_params(axis="x", labelrotation=45)
ax2.set_yticks(np.arange(0, 0.05, 0.005))
ax2.set_yticks(np.arange(0, 0.05, 0.001), minor=True)
ax2.tick_params(axis="y", which="minor", length=2)
ax2.tick_params(axis="y", which="major", length=5)
ax2.set_ylabel("Frequency")
ax2.set_xlabel("Date")
ax2.set_title("Frequency of Pancake Day Date (" + str(first_year) + "-" + str(last_year) + ")")
ax2.grid(True, which='both', linestyle='--', linewidth=0.5)

plt.show()
