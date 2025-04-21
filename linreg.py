import uproot
import numpy as np
import pandas as pd
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, r2_score
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt

# === 1. Load TTree data from ROOT file ===
file = uproot.open("/home/parkerw/Scripts/Mar26_quad_e_tspread_Tree.root")
tree = file["eveTree"]

branches = [
    "xFit", "yFit", "zFit", "xSeed", "ySeed", "zSeed",
    "xTrue", "yTrue", "zTrue",
    "xSeedPosErr", "xSeedNegErr", "ySeedPosErr", "ySeedNegErr", "zSeedPosErr", "zSeedNegErr",
    "xFitPosErr", "xFitNegErr", "yFitPosErr", "yFitNegErr", "zFitPosErr", "zFitNegErr",
    "scaledLLH", "itr", "neckHit", "nhits"
]

data = tree.arrays(branches, library="pd")
data = data.dropna()

# === 2. Compute derived variables ===

# True – Fit distance (target)
true_pos = data[["xTrue", "yTrue", "zTrue"]].to_numpy()
fit_pos = data[["xFit", "yFit", "zFit"]].to_numpy()
fit_true_dist = np.linalg.norm(true_pos - fit_pos, axis=1)

# Seed – Fit distance
seed_pos = data[["xSeed", "ySeed", "zSeed"]].to_numpy()
seed_fit_dist = np.linalg.norm(seed_pos - fit_pos, axis=1)

# Seed position uncertainty (average pos/neg errors)
seed_uncert = (
    data[["xSeedPosErr", "xSeedNegErr", "ySeedPosErr", "ySeedNegErr", "zSeedPosErr", "zSeedNegErr"]].abs().mean(axis=1)
)

# Fit position uncertainty
fit_uncert = (
    data[["xFitPosErr", "xFitNegErr", "yFitPosErr", "yFitNegErr", "zFitPosErr", "zFitNegErr"]].abs().mean(axis=1)
)

# Neck hit fraction
neck_frac = data["neckHit"] / data["nhits"]

# Build DataFrame
df = pd.DataFrame({
    "seed_uncert": seed_uncert,
    "fit_uncert": fit_uncert,
    "seed_fit_dist": seed_fit_dist,
    "scaledLLH": data["scaledLLH"],
    "itr": data["itr"],
    "neck_frac": neck_frac,
    "fit_true_dist": fit_true_dist
})

# Optional: remove outliers if needed
df = df[df["fit_uncert"] < 10000] 

# === 3. Regression ===
features = ["seed_uncert", "fit_uncert", "seed_fit_dist", "scaledLLH", "itr", "neck_frac"]
target = "fit_true_dist"

X = df[features]
y = df[target]

# Split the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Apply log transformation to features and add them to both X_train and X_test
X_train["log_fit_uncert"] = np.log1p(X_train["fit_uncert"])
X_train["log_seed_uncert"] = np.log1p(X_train["seed_uncert"])
X_train["log_scaledLLH"] = np.log1p(X_train["scaledLLH"])

X_test["log_fit_uncert"] = np.log1p(X_test["fit_uncert"])
X_test["log_seed_uncert"] = np.log1p(X_test["seed_uncert"])
X_test["log_scaledLLH"] = np.log1p(X_test["scaledLLH"])

# Drop the original columns from both training and testing sets
X_train = X_train.drop(columns=["fit_uncert", "seed_uncert", "scaledLLH"])
X_test = X_test.drop(columns=["fit_uncert", "seed_uncert", "scaledLLH"])

scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)

# Now, the model can use the transformed columns
model = LinearRegression()
model.fit(X_train_scaled, y_train)
y_pred = model.predict(X_test_scaled)

print("R² Score:", r2_score(y_test, y_pred))
mse = mean_squared_error(y_test, y_pred)
rmse = np.sqrt(mse)
print("RMSE:", rmse)

print("Intercept:", model.intercept_)
print("Coefficients:")
for name, coef in zip(X_train.columns, model.coef_):
    print(f"  {name}: {coef:.4f}")

# === 4. Plotting ===
plt.figure(figsize=(7, 6))
plt.scatter(y_test, y_pred, alpha=0.4)
plt.plot([0, y_test.max()], [0, y_test.max()], 'r--')
plt.xlabel("True Fit-True Distance [mm]")
plt.ylabel("Predicted Distance (Quality Index)")
plt.title("Linear Regression Model")
plt.grid(True)
plt.tight_layout()
plt.show()

# === 5. Save predicted index (optional) ===
df["quality_index"] = model.predict(X)
df.to_csv("reco_quality_index.csv", index=False)
