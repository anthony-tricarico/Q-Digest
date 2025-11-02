# Pseudocode for the serial implementation

The following pseudocode is taken directly from the original paper introducing
the Q-Digest algorithm and related data structure (Shrivastava et al., 2004).

## COMPRESS(Q,n,k)

### Inputs

- `Q`: An instance of the Q-Digest object
- `n`: The number of data values recorded in a (fictional) sensor
- `k`: The compression factor to be used

### Pseudocode

```pseudo
l = log(sigma) - 1;
while l > 0 do
    for all v in level l do
        if count(v) + count(v_s) + count(v_p) < (n/k) then
            count(v_p) += count(v) + count(v_s);
            delete v and v_s from Q;
        end if
    end for
    l = l - 1;
end while
```

The algorithm starts by indicating the number of levels `l` in the binary tree, as
measured by a base-2 logarithm of the maximum integer value available (represented
by `sigma`). Then, 1 is subtracted to always start 1 level above the leaves.

A `while` loop is started and executed until all levels (other than the leaves) are
explored.

A `for` loop is started to explore all the nodes `v` in a given level `l`.

For each node it is checked whether its count added to the count of its
siblings `v_s` and that of its parent (`v_p`) is less than `n/k` which
represents the desired degree of compression. If that is the case, then
the parent is updated to include the counts for the current node `v` and its
siblings `v_s`.

Finally, `l` is decremented by 1 to keep track of the next level to visit.

## MERGE(Q1(n1, k), Q2(n2, k))

### Inputs

- $Q_{1}$: The first Q-Digest having $n_{1}$ total data values and a compression
factor of $k$
- $Q_{2}$: The first Q-Digest having $n_{2}$ total data values and a compression
factor of $k$

### Pseudocode

```pseudo
Q = Q_1 union Q_2;
COMPRESS(Q, n1 + n2, k);
```

As explained in the paper, "the idea is to take the union of the two q-digest and
add the counts of buckets with the same range (\[min, max\]). Then, we compress the
resulting q-digest."

With union, it is meant a sum of counts of each node with its respective
counterpart in the other q-digest.

## Sources

Shrivastava, N., Buragohain, C., Agrawal, D., & Suri, S. (2004). Medians and beyond:
New aggregation techniques for sensor networks. Proceedings of the 2nd International
Conference on Embedded Networked Sensor Systems, 239–249. <https://doi.org/10.1145/1031495.1031524>
