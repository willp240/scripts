import matplotlib.pyplot as plt
import numpy as np
def RayPlot(phases,angle,R_vec): # Creates Rayleigh Plot
    r=[0.975]*len(phases) # Radial positions of points
    fig, ax = plt.subplots(figsize=(5, 5), subplot_kw={'projection': 'polar'})
    ax.scatter(phases,r,color='black') # Plot data
    ax.annotate("", xytext=(0, 0), xy=(mu,R), # Plot Rayleigh arrow
                arrowprops=dict(arrowstyle="->"),color='black')
    ax.set_theta_zero_location('N') # ’N’ for North (top)
    ax.set_theta_direction(-1) # Clockwise direction
    ax.set_xticklabels([]) # Disable axis labels
    ax.set_rticks([]) # Disables radial ticks
    ax.grid(False) # Disables grid
    ax.set_rmax(1) # Set maximum radius
    p=np.exp(-len(phases)*R_vec**2) # p-value estimate (without corrections)
    ax.text(3.5,1.3,f"p-value={p:.3}",fontsize=20) # Display p-value
    plt.show()
    
seed=1234 # Initial random seed
ran=np.random.default_rng(seed) # Define random number generator
n=10 # Number of dat points
theta=2*np.pi*ran.random(n) # Generate random angles
C=np.sum(np.cos(theta)) # ’x’ vector addition
S=np.sum(np.sin(theta)) # ’y’ vector addition
mu=np.arctan2(S,C) # Estimate average direction
R=np.sqrt(S**2 + C**2)/n # Normalised Rayleigh vector
p=np.exp(-n*R*R) # p-value (without corrections)
plt.rc('font',family='Times New Roman') # Specify font
RayPlot(theta,mu,R)
