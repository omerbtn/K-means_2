import numpy as np
import pandas as pd
import sys
import math

ITER_DEFAULT = 300


def main():
    argv = sys.argv
    if len(argv) > 6 or len(argv) < 5:
        # print error
        return
    k = int(argv[1])
    iter = int(argv[2]) if len(argv) == 6 else ITER_DEFAULT
    eps = float(argv[3]) if len(argv) == 6 else float(argv[2])
    path_1 = argv[4] if len(argv) == 6 else argv[3]
    path_2 = argv[5] if len(argv) == 6 else argv[4]

    df1 = pd.read_csv(path_1, header=None)
    df2 = pd.read_csv(path_2, header=None)
    points = df1.join(df2.set_index(0), on=0, how='inner', rsuffix='_df2', lsuffix='_df1')
    points = points.sort_values(by=0)
    n = points.shape[0]
    d = points.shape[1] - 1
    # points = points.to_numpy()
    # print(points)
    np.random.seed(0)
    # assert input is valid

    centroids_indices = [np.random.choice(n)]
    min_dists = np.asarray([0.0 for i in range(n)])
    for i in range(k - 1):
        for j in range(n):
            min_dists[j] = calc_min_dist(points.iloc[j, 1:].to_numpy(), centroids_indices, points)
        # print(min_dists)
        total_sum = np.sum(min_dists)
        np.divide(min_dists, total_sum, out=min_dists, casting="unsafe")
        centroids_indices.append(np.random.choice(np.arange(n), size=1, p=min_dists)[0])

    print(centroids_indices)


def calc_min_dist(p, centroids_indices, points):
    # print(p)
    min_dist = dist(p, points.iloc[centroids_indices[0], 1:])
    for i in range(1, len(centroids_indices)):
        curr_dist = dist(p, points.iloc[centroids_indices[i], 1:])
        if curr_dist < min_dist:
            min_dist = curr_dist
    return min_dist


def dist(point1, point2):
    squared_distances = [(p1 - p2) ** 2 for p1, p2 in zip(point1, point2)]
    sum_of_squared_distances = sum(squared_distances)
    distance = math.sqrt(sum_of_squared_distances)
    return distance


if __name__ == "__main__":
    main()
