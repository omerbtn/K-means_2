import numpy as np
import pandas as pd
import sys
import math
import mykmeanssp as km

ITER_DEFAULT = 300


def main():
    argv = sys.argv
        
    if not checkLegal(argv):
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
    points.drop(columns=points.columns[0], inplace=True)
    n = points.shape[0]
    d = points.shape[1]
    points = points.to_numpy()

    np.random.seed(0)
    centroids_indices = [np.random.choice(n)]

    min_dists = np.asarray([0.0 for i in range(n)])
    for i in range(k - 1):
        for j in range(n):
            min_dists[j] = calc_min_dist(points[j], centroids_indices, points)
        total_sum = np.sum(min_dists)
        np.divide(min_dists, total_sum, out=min_dists, casting="unsafe")
        centroids_indices.append(np.random.choice(np.arange(n), size=1, p=min_dists)[0])

    indices_print_str = ""
    for i in range(k-1):
        indices_print_str += str(centroids_indices[i]) + ","
    indices_print_str += str(centroids_indices[k-1])
    print(indices_print_str, end="\r\n")

    points = points.tolist()
    centroids = km.fit(points, centroids_indices, k, n, d, iter, eps)
    if centroids is None:
        print("An Error Has Occurred")
        return

    printFormat(centroids, k, d)


def calc_min_dist(p, centroids_indices, points):
    min_dist = dist(p, points[0])
    for i in range(1, len(centroids_indices)):
        curr_dist = dist(p, points[centroids_indices[i]])
        if curr_dist < min_dist:
            min_dist = curr_dist
    return min_dist


def dist(point1, point2):
    squared_distances = [(p1 - p2) ** 2 for p1, p2 in zip(point1, point2)]
    sum_of_squared_distances = sum(squared_distances)
    distance = math.sqrt(sum_of_squared_distances)
    return distance


def printFormat(centroids, k, d):
    for i in range(k):
        res = ""
        for j in range(d-1):
            res += "{:.4f}".format(centroids[i][j]) + ","
        print(res + "{:.4f}".format(centroids[i][d-1]), end="\r\n")


def checkLegal(argv):
    if len(argv) > 6 or len(argv) < 5:
        print("An Error Has Occurred")
        return False
    try:
        k = int(argv[1])
        iter = int(argv[2]) if len(argv) == 6 else ITER_DEFAULT
        eps = float(argv[3]) if len(argv) == 6 else float(argv[2])
        path_1 = argv[4] if len(argv) == 6 else argv[3]
        df1 = pd.read_csv(path_1, header=None)
        n = df1.shape[0]
    except:
        print("An Error Has Occurred")
        return False

    if k <= 1 or k >= n:
        print("Invalid number of clusters!")
        return False
    if iter <= 1 or iter >= 1000:
        print("Invalid maximum iteration!")
        return False
    if eps < 0:
        print("Invalid epsilon!")
        return False
    
    return True


if __name__ == "__main__":
    main()
