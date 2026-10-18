#!/usr/bin/env python

from __future__ import absolute_import, division, print_function

import argparse
import os
import tempfile

matplotlib_config_dir = os.path.join(
    tempfile.gettempdir(), "matplotlib-{}".format(os.getuid())
)
os.makedirs(matplotlib_config_dir, exist_ok=True)
os.environ.setdefault("MPLCONFIGDIR", matplotlib_config_dir)

import matplotlib.pyplot as plt
import numpy as np


DATA = np.array(
    [
        [0.0, 0.0],
        [0.06, 30.22],
        [0.08, 39.29],
        [0.1, 49.39],
        [0.15, 74.65],
        [0.2, 99.07],
        [0.3, 150.05],
        [0.4, 201.24],
        [0.5, 253.53],
        [0.8, 407.97],
    ],
    dtype=float,
)


def fit_through_origin(x, y):
    denominator = np.sum(x * x)
    if denominator == 0.0:
        raise ValueError("Cannot fit a line through the origin when all x values are zero")
    return np.sum(x * y) / denominator


def main():
    parser = argparse.ArgumentParser(
        description="Plot H(0) versus energy and fit a line constrained through (0, 0)."
    )
    parser.add_argument("-o", "--output", default="h0_fit.png", help="Output image file")
    parser.add_argument("--show", action="store_true", help="Display the plot window")
    args = parser.parse_args()

    x = DATA[:, 0]
    y = DATA[:, 1]
    slope = fit_through_origin(x, y)

    x_fit = np.linspace(0.0, x.max(), 200)
    y_fit = slope * x_fit

    fig, ax = plt.subplots(figsize=(8, 5.5))
    ax.scatter(x, y, color="tab:blue", marker="+", linewidths=1.8, label="Coordinated Points", zorder=3)
    ax.plot(
        x_fit,
        y_fit,
        color="tab:red",
        linestyle=(0, (5, 4)),
        linewidth=0.6,
        alpha=0.65,
        label="Fit: H(0) = {:.3f} E".format(slope),
    )

    ax.set_xlabel("Energy, MeV")
    ax.set_ylabel("H(0)")
    ax.grid(True, alpha=0.3)
    ax.legend()
    fig.tight_layout()

    fig.savefig(args.output, dpi=200)
    print("Fit constrained through (0, 0): H(0) = {:.6f} * Energy".format(slope))
    print("Wrote {}".format(os.path.abspath(args.output)))

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()
