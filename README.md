# Voronoi

A Voronoi diagram is a covering of a plane with a set of 'specifed' (my term) points. Each Voronoi cell has the property that all points in that cell are closer to that specific point than to any other specific point.

## Example
<img width="1002" alt="image" src="https://github.com/user-attachments/assets/d9db443e-a91c-4420-8c4c-272b86d78747">

The program can render the cells using various metrics; the three currently built in are:
 - Max Metric
 - Manhattan Distance
 - Euclidean Metric,
these being the $L_0$, $L_1$ and $L_2$ norms respectively.
 
## Running
``` console
$ ./voronoi
```

### Keys
M - Toggle Metric

Q - Quit
